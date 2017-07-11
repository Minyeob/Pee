#include<stdio.h>
#include<unistd.h> 
#include<fcntl.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include"keypad.h"
#include"serial.h"
#include"textlcd.h"
#include"http.h"
#include"buzzer.h"
#include"dipswitch.h"
#include "dotmatrix.h"
#include "sevensegment.h"
#include "touch.h"
#include "cled.h"
#include "oled.h"
#include "eightled.h"
#define FALSE 0
#define TRUE 1



static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//user is connection status.
static int userOn = FALSE;

void userStatus(int b)
{
	// Critical section by user's connection status.
	pthread_mutex_lock(&mutex);
	userOn = b;
	pthread_mutex_unlock(&mutex);
}

int isUserOn()
{
	return userOn;
}

void* keypadFunc(void* data)
{
	int number = 0, pressCount = 0, rdata;
	int push = FALSE;
	char* content;
	char* parsing;
	int peeCount = 0;
	char str[32];
	int i ;
	pthread_t pthread;
	int pthreadId;
	fd[0] = open(KEYPAD_DEV, O_RDWR);
	if ( fd[0] < 0 )
	{
		perror("keypad open error.\n");
		return NULL;
	}
	
	while(read(fd[0], &rdata, 4) != -1)
	{
		// When inputting keypad4 while user is connected.
		if(isUserOn())
		{
			if(rdata == 4 && isCleaningOn() == FALSE)
			{

				cledWrite(RED_LED);
				push = TRUE;
				serialWrite("7/@",3);
				dotMatrixOff();
				userStatus(FALSE);
				textInit();
				closeDoorImg();
				normalScreen();
				eightledWrite(0,0);
			}
			// sevensegmentWrite in connected count.
			else
			{
				sevensegmentWrite(getSigalrmCount());
				dotMatrixOn(peeCount);
			}
			continue;
		}
		if(isCleaningOn())
		{
			continue;
		}
		
		usleep(1000);

		if(rdata == 0)	{ push = FALSE;	continue; }
		
		else if(rdata == 1)		rdata = 1;
		else if(rdata == 2)		rdata = 2;
		else if(rdata == 3)		rdata = 3;
		
		else if(rdata == 5)		rdata = 4;
		else if(rdata == 6)		rdata = 5;
		else if(rdata == 7)		rdata = 6;

		else if(rdata == 9)		rdata = 7;
		else if(rdata == 10)	rdata = 8;
		else if(rdata == 11)	rdata = 9;
		
		else if(rdata == 14)	rdata = 0;
		
		else if(rdata == 16)	rdata = 16;
		else continue;

		if(push == TRUE)	continue;

		printf("keypad : button NO:%d\n",rdata);
		if(rdata < 10)
		{
			push = TRUE;
			
			if(pressCount == 0 && rdata == 0) continue;
			if(pressCount > 8) continue;
			
			pressCount++;
			number = number*10 + rdata;
			

			textlcdWrite(rdata);
		}
		else if(rdata == 16)
		{
			push = TRUE;

			if(pressCount != 9)
			{
				buzzerOn(20);
				sleep(1);
				buzzerOff();
				textInit();
				pressCount = 0;
				number = 0;

				continue;
			}
			sprintf(str, "/users/%d", number);
			// Http communication to check user information.
			content = request("GET", str, 0);

			// User authentication succeeded. 
			if(strcmp(content,"")) 
			{
				userStatus(TRUE);
				peeCount = 0;
				parsing = strstr(content, "\"count\":");
				parsing +=8;
				for(i = 0; i < strlen(parsing) ; ++i)
				{
					if(parsing[i] >= '0' && parsing[i] <= '9')
					{
						peeCount *= 10;
						peeCount += (parsing[i] - '0');
					}
				}
				pthread_create(&pthread, NULL, studentScreenFunc, (void*)&number);
				serialWrite("1/@", 3);
				textlcdWrite(number);
			}
			// User authentication failed.
			else
			{
				buzzerOn(7);
				sleep(1);
				buzzerOff();
				textInit();
			}

			free(content);
			content = NULL;
			parsing = NULL;
			pressCount = 0;
			number = 0;
		}

	}
	printf("keypad close()\n");
	close(fd[0]);


	return NULL;
}
