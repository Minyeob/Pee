#ifndef SERIAL_H
#include "sensor.h"


void setSigalrmCount(int);
int getSigalrmCount();
void serialWrite(char*,int);
void init();
void* serialFunc(void* data);

#endif
