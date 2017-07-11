#ifndef TEXTLCD_H
#include"sensor.h"
//int writeCmd(unsigned short);


void setStudentNumber(int);
int getStudentNumber();

void textInit();
int writeCh(unsigned short);
int setDDRAMAddr(int,int);
int clearScreen(int);
void textlcdOpen();
void textlcdClose();
void textlcdWrite(int);
#endif
/******************************************************************************
  *
  *      TEXT LCD FUNCTION
  *
  ******************************************************************************/

#define CLEAR_DISPLAY       0x0001
#define CURSOR_AT_HOME      0x0002

// Entry Mode set 
#define MODE_SET_DEF        0x0004
#define MODE_SET_DIR_RIGHT  0x0002
#define MODE_SET_SHIFT      0x0001

// Display on off
#define DIS_DEF             0x0008
#define DIS_LCD             0x0004
#define DIS_CURSOR          0x0002
#define DIS_CUR_BLINK       0x0001

// shift
#define CUR_DIS_DEF         0x0010
#define CUR_DIS_SHIFT       0x0008
#define CUR_DIS_DIR         0x0004

// set DDRAM  address 
#define SET_DDRAM_ADD_DEF   0x0080

// read bit
#define BUSY_BIT            0x0080
#define DDRAM_ADD_MASK      0x007F


#define DDRAM_ADDR_LINE_1   0x0000
#define DDRAM_ADDR_LINE_2   0x0040


#define SIG_BIT_E           0x0400
#define SIG_BIT_RW          0x0200
#define SIG_BIT_RS          0x0100

