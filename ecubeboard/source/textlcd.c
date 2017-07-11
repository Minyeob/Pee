#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "textlcd.h"

#define FALSE 0
#define TRUE 1


 
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Student number in use.
static int studentNumber = 0;

void setStudentNumber(int s)
{
		// Critical section
	    pthread_mutex_lock(&mutex);
		studentNumber = s;
		pthread_mutex_unlock(&mutex);
}

int getStudentNumber()
{
	return studentNumber;
}



int IsBusy(void)
{
	unsigned short wdata, rdata;

	wdata = SIG_BIT_RW;
	write(fd[1] ,&wdata,2);

	wdata = SIG_BIT_RW | SIG_BIT_E;
	write(fd[1] ,&wdata,2);

	read(fd[1],&rdata ,2);

	wdata = SIG_BIT_RW;
	write(fd[1],&wdata,2);

	if (rdata &  BUSY_BIT)
		return TRUE;

	return FALSE;
}

int writeCh(unsigned short ch)
{
	unsigned short wdata =0;

	if ( IsBusy())
		return FALSE;

	wdata = SIG_BIT_RS | ch;
	write(fd[1] ,&wdata,2);

	wdata = SIG_BIT_RS | ch | SIG_BIT_E;
	write(fd[1] ,&wdata,2);

	wdata = SIG_BIT_RS | ch;
	write(fd[1] ,&wdata,2);
	usleep(1000);
	return TRUE;

}

int writeCmd(unsigned short cmd)
{
	unsigned short wdata ;

	if ( IsBusy())
		return FALSE;

	wdata = cmd;
	write(fd[1] ,&wdata,2);

	wdata = cmd | SIG_BIT_E;
	write(fd[1] ,&wdata,2);

	wdata = cmd ;
	write(fd[1] ,&wdata,2);

	return TRUE;
}




int functionSet(void)
{
	unsigned short cmd = 0x0038; // 5*8 dot charater , 8bit interface , 2 line

	if (!writeCmd(cmd))
		return FALSE;
	return TRUE;
}

int setDDRAMAddr(int x , int y)
{
	unsigned short cmd = 0;
	//  printf("x :%d , y:%d \n",x,y);
	if(IsBusy())
	{
		perror("setDDRAMAddr busy error.\n");
		return FALSE;

	}

	if ( y == 1 )
	{
		cmd = DDRAM_ADDR_LINE_1 +x;
	}
	else if(y == 2 )
	{
		cmd = DDRAM_ADDR_LINE_2 +x;
	}
	else
		return FALSE;

	if ( cmd >= 0x80)
		return FALSE;


	//  printf("setDDRAMAddr w1 :0x%X\n",cmd);

	if (!writeCmd(cmd | SET_DDRAM_ADD_DEF))
	{
		perror("setDDRAMAddr error\n");
		return FALSE;
	}
	//  printf("setDDRAMAddr w :0x%X\n",cmd|SET_DDRAM_ADD_DEF);
	usleep(1000);
	return TRUE;
}


#define LINE_NUM            2
#define COLUMN_NUM          16          

// clearScreen 
int clearScreen(int nline)
{
	int i;
	if (nline == 0)
	{   
		if(IsBusy())
		{   
			perror("clearScreen error\n");
			return FALSE;
		}   
		if (!writeCmd(CLEAR_DISPLAY))
			return FALSE;
		return TRUE;
	}   
	else if (nline == 1)
	{   
		setDDRAMAddr(0,1);
		for(i = 0; i <= COLUMN_NUM ;i++ )
		{   
			writeCh((unsigned char)' ');
		}   
		setDDRAMAddr(0,1);

	}   
	else if (nline == 2)
	{   
		setDDRAMAddr(0,2);
		for(i = 0; i <= COLUMN_NUM ;i++ )
		{   
			writeCh((unsigned char)' ');
		}   
		setDDRAMAddr(0,2);
	}   
	return TRUE;
}

int displayMode(int bCursor, int bCursorblink, int blcd  )
{
	unsigned short cmd  = 0;

	if ( bCursor)
	{   
		cmd = DIS_CURSOR;
	}   

	if (bCursorblink )
	{   
		cmd |= DIS_CUR_BLINK;
	}   

	if ( blcd )
	{   
		cmd |= DIS_LCD;
	}   

	if (!writeCmd(cmd | DIS_DEF))
		return FALSE;

	return TRUE;
}



void textInit()
{
	char c[24];
	int i;
	displayMode(1,1,12);
	clearScreen(1);
	clearScreen(2);
	setDDRAMAddr(0,1);
	sprintf(c, " Student number ?");
	for(i = 0 ; i < strlen(c) ; ++i)
	{   
		writeCh((unsigned char)c[i]);
	}   
	setDDRAMAddr(0,2);
}



void textlcdOpen()
{
	char c[24];
	int i;
	fd[1] = open(TEXTLCD_DEV, O_RDWR);
	if(fd[1] < 0)
	{
		perror("driver open error.\n");
		exit(-1);
	}
	functionSet();
	textInit();
}

void textlcdClose()
{
	printf("textlcdClose()");
	close(fd[1]);
}

// textlcdWrite() plays two roles.
// 1. Output the input on value from the keypad.
// 2. Verified student number output.
void textlcdWrite(int data)
{
	char c[20];
	int i;

	// Output the input on value from the keypad.
	if(data < 10)
	{
		printf("textlcdWrite : %d\n",data);
		writeCh((unsigned char)(data + '0'));
	}
	// Verified student number output.
	else
	{
		setStudentNumber(data);

		clearScreen(1);
		clearScreen(2);
		setDDRAMAddr(0,1);
		
		printf("textlcdWrite : %d\n",data);
		sprintf(c, " Hello %d",data);
		
		for(i = 0 ; i < strlen(c) ; ++i)
		{
			writeCh((unsigned char)c[i]);
		}
		
		setDDRAMAddr(0,2);
		sprintf(c, " Welcome !!!");
		for(i = 0 ; i < strlen(c) ; ++i)
		{
			writeCh((unsigned char)c[i]);
		}
	}
}

