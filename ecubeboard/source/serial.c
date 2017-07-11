#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "oled.h"
#include "textlcd.h"
#include "serial.h"
#include "keypad.h"
#include "cled.h"
#include "eightled.h"
#include "buzzer.h"
#include "http.h"
#include "dotmatrix.h"
#include "touch.h"
#define BAUDRATE B9600

#define FALSE 0
#define TRUE 1



static unsigned int sigalrmCount = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void setSigalrmCount(int b)
{
	// Critical section for usage time.
	pthread_mutex_lock(&mutex);
	sigalrmCount = b;
	pthread_mutex_unlock(&mutex);
}

int getSigalrmCount()
{
	    return sigalrmCount;
}


void init()
{
	struct termios newtio;
	fd[9] = open( SERIAL_DEV, O_RDWR | O_NOCTTY );
	
	if (fd[9] < 0) 
	{
		perror(SERIAL_DEV);
		exit(-1); 
	}
	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
	
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	tcflush(fd[9], TCIFLUSH);
	tcsetattr(fd[9],TCSANOW,&newtio);
}

void serialWrite(char* buf, int size)
{
	write(fd[9],buf, size);	

}

// The signal function to determine the usage time.
void sevensegmentSigalrmHandler(int signo)
{
	int peeTime;
	char data[128];
	char page[30];
	char* ret;
	printf("sigalrmCount : %d\n", sigalrmCount);
	peeTime = getSigalrmCount();
	if(isUserOn())
	{
		setSigalrmCount(++peeTime);
		alarm(1);
	}
	else
	{
		//user information send to DB [HTTP]
		sprintf(data, "{\"id\":%d,\"time\":%d}", getStudentNumber(), peeTime);
		sprintf(page,"/users/%d",getStudentNumber());	
		ret = request("POST", page, data);
		free(ret);

		setSigalrmCount(0);
		setStudentNumber(0);
	}
}


void parsing(char* buf, int size)
{
	if(buf[0] == '2')
	{
		dotMatrixOff();
		userStatus(FALSE);
		textInit();
		normalScreen();
	}
	else if(buf[0] == '3')
	{
		// opendoor

		cledWrite(BLUE_LED);
		eightledWrite(0,1);		
		openDoorImg();
		alarm(1);

	}
	else if(buf[0] == '4')
	{
		//closeDoor
		cledWrite(RED_LED);
		dotMatrixOff();
		userStatus(FALSE);
		textInit();
		closeDoorImg();
		normalScreen();
	}
	else if(buf[0] == '5')
	{
		cledWrite(GREEN_LED);
		buzzerOn(buf[2]-'0');
		usleep(500);
		buzzerOff();
		eightledWrite(buf[2]-'0', 0);	
	}
	else if(buf[0] == '6')
	{
		cledWrite(BLUE_LED);
		eightledWrite(0,1);		
	}
	/*
	else if(buf[0] == '7')
	{
		//closeDoor
		dotMatrixOff();
		userStatus(FALSE);
		textInit();
		cledWrite(RED_LED);
		closeDoorImg();
		normalScreen();
	}
*/
}
// serial thread function
void* serialFunc(void* data)
{
	int c, res;
	char buf[1024];
	
	init();
	signal( SIGALRM, sevensegmentSigalrmHandler);

	while ( (res = read(fd[9], buf, 1024)) != -1) 
	{
		buf[res]=0;             /* set end of string, so we can printf */
		printf("serial : %s", buf);
		parsing(buf, res);
	}
	printf("serial Close() %d\n",res);
	
	close(fd[9]);
	return 0;
}
