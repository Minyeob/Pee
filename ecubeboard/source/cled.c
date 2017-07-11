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
#include "cled.h"

#define LED_ON  1
#define LED_OFF 0
#define MAX_LED_NO      8

#define INDEX_LED       0
#define INDEX_RED_LED       1
#define INDEX_GREEN_LED     2
#define INDEX_BLUE_LED      3
#define INDEX_MAX           4


void cledWrite(int color)
{
	unsigned short colorArray[INDEX_MAX];
	

	// RED color 
	if(color == RED_LED)
	{
		colorArray[INDEX_RED_LED] =(unsigned short)255;
		colorArray[INDEX_GREEN_LED] =(unsigned short)0;
		colorArray[INDEX_BLUE_LED] =(unsigned short)0;
	}
	// blue color
	else if(color == BLUE_LED)
	{
		colorArray[INDEX_RED_LED] =(unsigned short)0;
		colorArray[INDEX_GREEN_LED] =(unsigned short)0;
		colorArray[INDEX_BLUE_LED] =(unsigned short)255;
	}
	// green color
	else if(color == GREEN_LED)
	{
		colorArray[INDEX_RED_LED] =(unsigned short)0;
		colorArray[INDEX_GREEN_LED] =(unsigned short)255;
		colorArray[INDEX_BLUE_LED] =(unsigned short)0;
	}
	else
	{
		printf("cled : color error %d",color);
		return;
	}
	
	 write(fd[5],&colorArray,6);

}

void cledOpen()
{
	fd[5] = open(CLED_DEV,O_RDWR);
	if(fd[5] < 0)
	{
		perror("cled open error\n");		exit(-1);
	}
	cledWrite(RED_LED);
}

void cledClose()
{
	printf("cled close()\n");
	close(fd[5]);
	
}
