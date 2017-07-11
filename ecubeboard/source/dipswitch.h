#ifndef KEYPAD_H
#include "sensor.h"

void cleaningStatus(int);
int isCleaningOn();
void* dipswitchFunc(void* data);
void cleaningSound();
#endif
