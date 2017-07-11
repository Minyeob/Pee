#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sensor.h"
#include "buzzer.h"
#include "dipswitch.h"

void buzzerOpen()
{
	int buzzerNumber = 7;
		
	printf("buzzer number :%d \n",buzzerNumber);	
	// open  driver 
	fd[5] = open(BUZZER_DEV, O_RDWR);
	if ( fd < 0 )
	{
		perror("driver (//dev//cnbuzzer) open error.\n");
		exit(-1);
	}	
}

void buzzerClose()
{
	printf("buzzerClose()");
	close(fd[5]);
}

void buzzerOn(int buzzerNumber)
{
	 write(fd[5],&buzzerNumber,4);
}

void buzzerOff()
{
	int off = 0;
	write(fd[5],&off,0);
}


// The sound of cleaning the urinal. 
void cleaningSound()
{
	int i;
	int melody[18] = {17, 16, 17, 16, 17, 12, 15, 13, 10, 1, 5, 10, 12, 5, 9, 12, 13, 5};

	for(i = 0; i < 16; i++)
	{
		buzzerOn(melody[i]);
		usleep(300000);
		buzzerOff();
	}
}

