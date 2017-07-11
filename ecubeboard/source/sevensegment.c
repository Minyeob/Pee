#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "sevensegment.h"

#define MAX_FND_NUM     6

#define  DOT_OR_DATA    0x80

const unsigned short segNum[10] =
{
	0x3F, // 0
	0x06,
	0x5B,
	0x4F,
	0x66,
	0x6D,
	0x7D,
	0x27,
	0x7F,
	0x6F  // 9
};
const unsigned short segSelMask[MAX_FND_NUM] = 
{
	0xFE00,
	0xFD00,
	0xFB00,
	0xF700,
	0xEF00,
	0xDF00
};

static struct termios oldt, newt;
void changemodeS(int dir)
{
	if( dir == 1)
	{
		tcgetattr(STDIN_FILENO , &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO );
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	}
	else
	{
		tcsetattr(STDIN_FILENO , TCSANOW, &oldt);

	}
}


// sevensegmentWrite...
void sevensegmentWrite(int num)
{
	int cSelCounter = 0;
	int temp,i; 
	static unsigned short wdata;
	int fndChar[MAX_FND_NUM];
	
	temp = num % 1000000;
	fndChar[0]= temp /100000;

	temp = num % 100000;
	fndChar[1]= temp /10000;

	temp = num % 10000;
	fndChar[2] = temp /1000;

	temp = num %1000;
	fndChar[3] = temp /100;

	temp = num %100;
	fndChar[4] = temp /10;

	fndChar[5] = num %10;

	for(cSelCounter = 0 ; cSelCounter < MAX_FND_NUM; ++cSelCounter)
	{
		wdata = segNum[fndChar[cSelCounter]] | segSelMask[cSelCounter] ;
		write(fd[2],&wdata,2);
	}
}

void sevensegmentOpen()
{
	int rdata = 0;
	fd[2] = open(SEVENSEGMENT_DEV, O_RDWR);
	if ( fd[2] < 0 )
	{
		perror("driver open error.\n");
		return exit(-1);
	}
	changemodeS(1);
	sevensegmentWrite(rdata);
}

void sevensegmentClose()
{
	changemodeS(0);
	printf("sevensegmentClose()\n");
	close(fd[2]);
}
