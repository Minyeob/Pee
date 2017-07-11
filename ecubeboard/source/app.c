#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "sensor.h"
#include "app.h"



// static thread three (keypad, serial, dipwsitch)
#define THREAD_COUNT 3


typedef struct ThreadSet
{
	void* (*h)(void*);
}ThreadSet;

int main(int argc, char **argv)
{
	int i;
	int threadId[THREAD_COUNT];
	pthread_t pthread[THREAD_COUNT];
	ThreadSet t[THREAD_COUNT]= 
	{
		keypadFunc,
		serialFunc,
		dipswitchFunc,
	};
	int status;

	for(i = 0 ; i < THREAD_COUNT; ++i)
	{
		threadId[i] = pthread_create(&pthread[i], NULL, t[i].h, NULL);
		if (threadId[i] < 0)
		{
			perror("thread create error : ");
			 exit(0);
		}
	}
	
	// sensor device open
	textlcdOpen();
	oledOpen();
	sevensegmentOpen();
	eightledOpen();
	cledOpen();
	buzzerOpen();
	dotMatrixOpen();
	normalScreen();
	
	openDoorImg();
	for(i = 0 ; i < THREAD_COUNT ; ++i)
	{
		pthread_join(pthread[i], (void *)&status);
	}
	

	// sensor device close
	textlcdClose();
	oledClose();
	sevensegmentClose();
	eightledClose();
	cledClose();
	buzzerClose();
	dotMatrixClose();
	return 0;
}
