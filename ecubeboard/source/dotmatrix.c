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
#include <termios.h>
#include "sensor.h"
#include "dotmatrix.h"



#define MAX_COLUMN_NUM	5
// 0 ~ 9
const unsigned short NumData[10][MAX_COLUMN_NUM]=
{
	{0xfe00,0xfd7F,0xfb41,0xf77F,0xef00}, // 0
	{0xfe00,0xfd42,0xfb7F,0xf740,0xef00}, // 1
	{0xfe00,0xfd79,0xfb49,0xf74F,0xef00}, // 2
	{0xfe00,0xfd49,0xfb49,0xf77F,0xef00}, // 3
	{0xfe00,0xfd0F,0xfb08,0xf77F,0xef00}, // 4
	{0xfe00,0xfd4F,0xfb49,0xf779,0xef00}, // 5
	{0xfe00,0xfd7F,0xfb49,0xf779,0xef00}, // 6
	{0xfe00,0xfd07,0xfb01,0xf77F,0xef00}, // 7
	{0xfe00,0xfd7F,0xfb49,0xf77F,0xef00}, // 8
	{0xfe00,0xfd4F,0xfb49,0xf77F,0xef00}  // 9
};

static struct termios oldt, newt;
void changemode(int dir)
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


int kbhit(void)
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO , &rdfs);

	select(STDIN_FILENO + 1 , &rdfs , NULL, NULL, &tv);

	return FD_ISSET(STDIN_FILENO , &rdfs);
}



#define ONE_LINE_TIME_U 	1000

// dotMaxtrix write
void dotMatrixOn(int num)
{
	int cSelCounter;
	int highChar , lowChar;
	int temp ;
	unsigned short wdata[2];

	temp = num % 100;

	highChar = temp / 10;
	lowChar = temp % 10;

	cSelCounter = 0;

	for(cSelCounter = 0 ; cSelCounter < MAX_COLUMN_NUM; ++cSelCounter)
	{
		wdata[0] = NumData[highChar][cSelCounter];

		wdata[1] = NumData[lowChar][cSelCounter];

		write(fd[8],(unsigned char*)wdata,4);

	}

}

void dotMatrixOff()
{
	unsigned short wdata[2];
	wdata[0] = 0;
	wdata[1] = 0;
	write(fd[8],(unsigned char*)wdata, 4);

}

void dotMatrixOpen()
{
	changemode(1);
	// open  driver 
	fd[8] = open(DOTMATRIX_DEV, O_RDWR);
	if ( fd[8] < 0 )
	{
		perror("driver  open error.\n");
		exit(1);
	}
	
	changemode(0);
}

void dotMatrixClose()
{
	printf("dotMatrixClose()");
	close(fd[8]);
}

