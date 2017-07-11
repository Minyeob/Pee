#include<stdio.h>
#include<unistd.h> 
#include<fcntl.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include "dipswitch.h"
#include "touch.h"
#include "keypad.h"
#include "serial.h"
#include "buzzer.h"
#define FALSE 0
#define TRUE 1

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int cleaning = FALSE;

void cleaningStatus(int s)
{
	 pthread_mutex_lock(&mutex);
	 cleaning = s;
	 pthread_mutex_unlock(&mutex);
}

int isCleaningOn()
{
	return cleaning;
}


void* dipswitchFunc(void* data)
{
	int rdata;
	int admin = 0;
	int status = 0;

	fd[11] = open(DIPSWITCH_DEV, O_RDWR);
	
	if ( fd[11] < 0 )
	{
		perror("dipswitch open error.\n");
		return NULL;
	}
	
	// dipswitch initialization
	if(read(fd[11], &rdata, 4) != -1)
	{
		rdata &= 0xFFFF;

		if(rdata == 0xFFFF)
		{
			cleaningStatus(TRUE);
			status = 1;
			serialWrite("8/@",3);
			cleaningScreen();
			cleaningSound();
		}
		else				
		{
			cleaningStatus(FALSE);
			status = 0;
		}
	}	

	while(read(fd[11], &rdata, 4) != -1)
	{
		if(isCleaningOn())
		{
			cleaningSound();
		}
		
		
		sleep(2);
		
		
		rdata &= 0xFFFF;
		if(rdata == 0xFFFF && status == 0 && 
		   isUserOn() == 0 && isCleaningOn() == 0)
		{
			openDoorImg();
			status = 1;
			cleaningStatus(TRUE);
			serialWrite("8/@",3);
			cleaningScreen();
			cleaningSound();
		}
		else if(rdata != 0xFFFF && status != 0 
				&& isUserOn() == 0  && isCleaningOn() != 0)
		{
			closeDoorImg();
			cleaningStatus(FALSE);
			status = 0;
			serialWrite("9/@", 3);
			normalScreen();
		}
	}
	printf("dipswitch close()\n");
	close(fd[11]);


	return NULL;
}
