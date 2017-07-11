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
#include "eightled.h"

#define LED_ON  1
#define LED_OFF 0
#define MAX_LED_NO      8



void eightledWrite(int ledNo, int control)
{
	// 0 0 ; all led off
	// 1 0 ; 1th led off
	// 2 0 ; 2th led off



	// 0 1 ;  all led on
	// 1 1 ; 1th led on
	// 2 1 ; 2th led on
	// 3 1 ; 3th led on

	int wdata = 0;
	int rdata = 0;
	int temp ;
	if(ledNo == 0)
	{
		if(control == 1) wdata = 0xff;
		else wdata = 0;
	}
	else
	{
		read(fd[4], &rdata,4);
		temp = 1;

		if(control == 1)
		{
			temp <<= (ledNo - 1);
			wdata = rdata | temp;
		}
		else
		{
			temp = ~(temp<<(ledNo-1));
			wdata = rdata & temp;
		}
	}

	write(fd[4], &wdata, 4);
}

void eightledOpen()
{
	fd[4] = open(EIGHTLED_DEV,O_RDWR);
	if(fd[4] < 0)
	{
		perror("eightled open error\n");		exit(-1);
	}
	eightledWrite(0,0);
}

void eightledClose()
{
	printf("eightled close()\n");
	close(fd[4]);
	
}
