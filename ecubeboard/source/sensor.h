#ifndef SENSOR_H

#define KEYPAD_DEV			"/dev/cnkey"
#define TEXTLCD_DEV			"/dev/cntlcd"
#define SEVENSEGMENT_DEV	"/dev/cnfnd"
#define OLED_DEV			"/dev/cnoled"
#define EIGHTLED_DEV		"/dev/cnled"


#define FB_DEV				"/dev/fb0"
#define INPUTEVENT_DEV		"/dev/input/event2"
#define DOTMATRIX_DEV		"/dev/cnmled"
#define DIPSWITCH_DEV		"/dev/cndipsw"
#define BUZZER_DEV			"/dev/cnbuzzer"
#define EVENT_STR			"/dev/input/event"
#define FBDEV_FILE			"/dev/fb0"
#define SERIAL_DEV			"/dev/ttyACM0"
#define CLED_DEV			"/dev/cncled"

#define SENSOR_COUNT 12

static int fd[SENSOR_COUNT];	//0  : keypad, 
								//1  : textlcd.
								//2  : sevensegment
								//3  : oled
								//4  : eightled
								//5  : buzzer
								//6  : event
								//7  : fb
								//8  : dot matrix
								//9  : serial
								//10 : socket
								//11 : dipswitch
#endif
