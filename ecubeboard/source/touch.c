#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // for open/close
#include <fcntl.h>      // for O_RDWR
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <string.h>
#include <linux/input.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO

#include "dipswitch.h"
#include "keypad.h"
#include "bitmap.h"
#include "touch.h"
#include "camera.h"

#define	 FBDEV_FILE "/dev/fb0"
#define  INPUT_DEVICE_LIST	"/proc/bus/input/devices"
#define  EVENT_STR	"/dev/input/event"

#define  MAX_BUFF	200
#define BIT_VALUE_24BIT   24

static int		screen_width = 0;
static int		screen_height = 0;
static int		bits_per_pixel = 0;
static int		line_length = 0;

#define MAX_TOUCH_X	0x740
#define MAX_TOUCH_Y	0x540

void readFirstCoordinate(int fd, int* cx , int* cy)
{
	struct input_event event;
	int readSize;
	while(1)
	{
		readSize = read(fd, &event, sizeof(event));
		
		if(isCleaningOn() && isUserOn() == 0) break;

		if ( readSize == sizeof(event) )
		{
			//			printf("type :%04X \n",event.type);
			//			printf("code :%04X \n",event.code);
			//			printf("value:%08X \n",event.value);
			//when event type is 3
			if( event.type == EV_ABS )
			{
				//when event code is center touch x position
				if (event.code == ABS_MT_POSITION_X )
				{
					*cx = event.value*screen_width/MAX_TOUCH_X; 
				}
				//when event code is center touch y position
				else if ( event.code == ABS_MT_POSITION_Y )
				{
					*cy = event.value*screen_height/MAX_TOUCH_Y;
				}
			}
			//when event type is 0
			else if ((event.type == EV_SYN) && (event.code == SYN_REPORT ))
			{
				break;
			}

		}
	}
}

//initialize screen black color
void initScreen(unsigned char *fb_mem )
{
	int		coor_y;
	int		coor_x;
	unsigned long *ptr;

	for(coor_y = 0; coor_y < screen_height; coor_y++)
	{
		ptr =   (unsigned long *)fb_mem + screen_width * coor_y ;
		for(coor_x = 0; coor_x < screen_width; coor_x++)
		{
			//fill screen black
			*ptr++  =   0x000000;
		}
	}
}

//read bmp file
void read_bmp(char *filename, char **pDib, char **data, int *cols, int *rows)
{
	BITMAPFILEHEADER    bmpHeader;
	BITMAPINFOHEADER    *bmpInfoHeader;
	unsigned int    size;
	unsigned char   magicNum[2];
	int     nread;
	FILE    *fp;

	//open file binary read only
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("ERROR\n");
		return;
	}

	// identify bmp file
	magicNum[0] = fgetc(fp);
	magicNum[1] = fgetc(fp);
	//printf("magicNum : %c%c\n", magicNum[0], magicNum[1]);

	if (magicNum[0] != 'B' && magicNum[1] != 'M') {
		printf("It's not a bmp file!\n");
		fclose(fp);
		return;
	}

	//read bmp header and save data
	nread = fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
	size = bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
	*pDib = (unsigned char *)malloc(size);      // DIB Header(Image Header)
	fread(*pDib, 1, size, fp);
	bmpInfoHeader = (BITMAPINFOHEADER *)*pDib;

	//printf("nread : %d\n", nread);
	//printf("size : %d\n", size);

	// check 24bit
	if (BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount))     // bit value
	{
		printf("It supports only 24bit bmp!\n");
		fclose(fp);
		return;
	}

	*cols = bmpInfoHeader->biWidth;
	*rows = bmpInfoHeader->biHeight;
	*data = (char *)(*pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);
	fclose(fp);
}

//close bmp file
void close_bmp(char **pDib)     // DIB(Device Independent Bitmap)
{
	free(*pDib);
}

int paint(char *filename)
{
	int i, j, k, t;
	int fbfd;
	int screen_width;
	int screen_height;
	int bits_per_pixel;
	int line_length;
	int coor_x, coor_y;
	int cols = 0, rows = 0;
	int mem_size;

	char    *pData, *data;
	char    r, g, b;
	//maximam resolution is 1280 * 800
	unsigned long   bmpdata[1280 * 800];
	unsigned long   pixel;
	unsigned char   *pfbmap;
	unsigned long   *ptr;
	struct  fb_var_screeninfo fbvar;
	struct  fb_fix_screeninfo fbfix;

	read_bmp(filename, &pData, &data, &cols, &rows);

	for (j = 0; j < rows; j++)
	{
		k = j * cols * 3;
		t = (rows - 1 - j) * cols;

		for (i = 0; i < cols; i++)
		{
			b = *(data + (k + i * 3));
			g = *(data + (k + i * 3 + 1));
			r = *(data + (k + i * 3 + 2));

			pixel = ((r << 16) | (g << 8) | b);
			bmpdata[t + i] = pixel;          // save bitmap data bottom up
		}
	}
	close_bmp(&pData);

	if ((fbfd = open(FBDEV_FILE, O_RDWR)) < 0)
	{
		printf("%s: open error\n", FBDEV_FILE);
		exit(1);
	}

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar))
	{
		printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
		exit(1);
	}

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix))
	{
		printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
		exit(1);
	}

	if (fbvar.bits_per_pixel != 32)
	{
		fprintf(stderr, "bpp is not 32\n");
		exit(1);
	}

	screen_width = fbvar.xres;
	screen_height = fbvar.yres;
	bits_per_pixel = fbvar.bits_per_pixel;
	line_length = fbfix.line_length;
	mem_size = line_length * screen_height;

	pfbmap = (unsigned char *)
		mmap(0, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

	if ((unsigned)pfbmap == (unsigned)-1)
	{
		perror("fbdev mmap\n");
		exit(1);
	}

	// fb clear - black
	for (coor_y = 0; coor_y < screen_height; coor_y++) {
		ptr = (unsigned long *)pfbmap + (screen_width * coor_y);
		for (coor_x = 0; coor_x < screen_width; coor_x++)
		{
			*ptr++ = 0x000000;
		}
	}

	// direction for image generating : (0,0)-> (1,0)-> (2,0)-> ...-> (row, column)
	for (coor_y = 0; coor_y < rows; coor_y++) {
		ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
		for (coor_x = 0; coor_x < cols; coor_x++) {
			*ptr++ = bmpdata[coor_x + coor_y*cols];
		}
	}

	munmap(pfbmap, mem_size);
	close(fbfd);
	return 0;
}

//show main screen
void firstpaint()
{
	paint("Main.bmp");
}

void* studentScreenFunc(void* data)
{
	char	eventFullPathName[100];
	int	eventnum;
	int	x, y = 0;
	int	fb_fd,fp;
	struct  fb_var_screeninfo fbvar;
	struct  fb_fix_screeninfo fbfix;
	unsigned char   *fb_mapped;
	int		mem_size;
	int student_number = *(int*)data;

	//print to address for defined format
	sprintf(eventFullPathName,"%s%d",EVENT_STR,2);

	printf("touch input event name:%s\n", eventFullPathName);

	//open event only read
	fp = open( eventFullPathName, O_RDONLY);
	if (-1 == fp)
	{
		printf("%s open fail\n",eventFullPathName);
		return NULL;
	}

	if( access(FBDEV_FILE, F_OK) )
	{
		printf("%s: access error\n", FBDEV_FILE);
		close(fp);
		return NULL;
	}

	if( (fb_fd = open(FBDEV_FILE, O_RDWR)) < 0)
	{
		printf("%s: open error\n", FBDEV_FILE);
		close(fp);
		return NULL;
	}

	if( ioctl(fb_fd, FBIOGET_VSCREENINFO, &fbvar) )
	{
		printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
		goto fb_err;
	}

	if( ioctl(fb_fd, FBIOGET_FSCREENINFO, &fbfix) )
	{
		printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
		goto fb_err;
	}

	//fbvar is lcd xres is size of row and yres is size of column
	screen_width    =   fbvar.xres;
	screen_height   =   fbvar.yres;
	bits_per_pixel  =   fbvar.bits_per_pixel;
	line_length     =   fbfix.line_length;

//	printf("screen_width : %d\n", screen_width);
//	printf("screen_height : %d\n", screen_height);
//	printf("bits_per_pixel : %d\n", bits_per_pixel);
//	printf("line_length : %d\n", line_length);

	mem_size    =   screen_width * screen_height * 4;
	//mmap is to define value of memory to file or device
	//mmap(start, length, prot, flags, fd, offset)
	//mmap define values start ~ start+length of memory
	//PROT_READ can read and PROT_WRITE can write and PROT_EXEC can execute
	//map_shared can share file via processes and map_private can't share file
	//mmap return address of memory
	fb_mapped   =   (unsigned char *)mmap(0, mem_size,
			PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);
	//when can't mapping
	if (fb_mapped < 0)
	{
		printf("mmap error!\n");
		goto fb_err;
	}

	firstpaint();

	//student imagefile name is imagefile
	char imagefile[13];
	sprintf(imagefile, "%d", student_number);
	sprintf(imagefile + 9, "%s", ".bmp");

	while(1)
	{
		readFirstCoordinate(fp,&x, &y);

		if(isUserOn() == 0) break;
		if (525 < x && x < 645)
		{
			if (135 < y && y < 700)
			{
				usleep(500);
				paint(imagefile);
				break;
			}
		}
		else if (150 < x && x < 400)
		{
			if (135 < y && y < 700)
			{
				printf("cameraOn accept\n");
				cameraOn();
				usleep(500);
				break;
			}
		}

	}

fb_err:
	close(fb_fd);
	close(fp);

	return 0;

}

void normalScreen()
{
	char	eventFullPathName[100];
	int	fb_fd, fp;

	struct  fb_var_screeninfo fbvar;
	struct  fb_fix_screeninfo fbfix;
	unsigned char   *fb_mapped;
	int		mem_size;

	//print to address for defined format
	sprintf(eventFullPathName, "%s%d", EVENT_STR, 2);

	printf("touch input event name:%s\n", eventFullPathName);

	//open event only read
	fp = open(eventFullPathName, O_RDONLY);
	if (-1 == fp)
	{
		printf("%s open fail\n", eventFullPathName);
		return ;
	}

	if (access(FBDEV_FILE, F_OK))
	{
		printf("%s: access error\n", FBDEV_FILE);
		close(fp);
		return ;
	}

	if ((fb_fd = open(FBDEV_FILE, O_RDWR)) < 0)
	{
		printf("%s: open error\n", FBDEV_FILE);
		close(fp);
		return ;
	}

	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fbvar))
	{
		printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
		goto fb_err;
	}

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fbfix))
	{
		printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
		goto fb_err;
	}

	//fbvar is lcd xres is size of row and yres is size of column
	screen_width = fbvar.xres;
	screen_height = fbvar.yres;
	bits_per_pixel = fbvar.bits_per_pixel;
	line_length = fbfix.line_length;

//	printf("screen_width : %d\n", screen_width);
//	printf("screen_height : %d\n", screen_height);
//	printf("bits_per_pixel : %d\n", bits_per_pixel);
//	printf("line_length : %d\n", line_length);

	mem_size = screen_width * screen_height * 4;
	//mmap is to define value of memory to file or device
	//mmap(start, length, prot, flags, fd, offset)
	//mmap define values start ~ start+length of memory
	//PROT_READ can read and PROT_WRITE can write and PROT_EXEC can execute
	//map_shared can share file via processes and map_private can't share file
	//mmap return address of memory
	fb_mapped = (unsigned char *)mmap(0, mem_size,
			PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	//when can't mapping
	if (fb_mapped < 0)
	{
		printf("mmap error!\n");
		goto fb_err;
	}
	initScreen(fb_mapped);
	paint("urinals.bmp");

fb_err:
	close(fb_fd);
	close(fp);

}

void cleaningScreen()
{
	char	eventFullPathName[100];
	int	fb_fd, fp;

	struct  fb_var_screeninfo fbvar;
	struct  fb_fix_screeninfo fbfix;
	unsigned char   *fb_mapped;
	int		mem_size;

	//print event type to eventFullPathName
	sprintf(eventFullPathName, "%s%d", EVENT_STR, 2);

	printf("touch input event name:%s\n", eventFullPathName);

	//open event only read
	fp = open(eventFullPathName, O_RDONLY);

	//when open file is failed
	if (-1 == fp)
	{
		printf("%s open fail\n", eventFullPathName);
		return ;
	}

	//when access file is failed
	if (access(FBDEV_FILE, F_OK))
	{
		printf("%s: access error\n", FBDEV_FILE);
		close(fp);
		return ;
	}

	//when open error
	if ((fb_fd = open(FBDEV_FILE, O_RDWR)) < 0)
	{
		printf("%s: open error\n", FBDEV_FILE);
		close(fp);
		return ;
	}

	//ioctl error
	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fbvar))
	{
		printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
		goto fb_err;
	}

	//fbvar is lcd, xres is size of row and yres is size of column
	screen_width = fbvar.xres;
	screen_height = fbvar.yres;
	bits_per_pixel = fbvar.bits_per_pixel;
	line_length = fbfix.line_length;

//	printf("screen_width : %d\n", screen_width);
//	printf("screen_height : %d\n", screen_height);
//	printf("bits_per_pixel : %d\n", bits_per_pixel);
//	printf("line_length : %d\n", line_length);

	mem_size = screen_width * screen_height * 4;
	//mmap is to define value of memory to file or device
	//mmap(start, length, prot, flags, fd, offset)
	//mmap define values start ~ start+length of memory
	//PROT_READ can read and PROT_WRITE can write and PROT_EXEC can execute
	//map_shared can share file via processes and map_private can't share file
	//mmap return address of memory
	fb_mapped = (unsigned char *)mmap(0, mem_size,
			PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	//when can't mapping
	if (fb_mapped < 0)
	{
		printf("mmap error!\n");
		goto fb_err;
	}
	//screen rollback
	initScreen(fb_mapped);
	//show waiting display
	paint("cleaning.bmp");

	//when error is oucccred, close file.
fb_err:
	close(fb_fd);
	close(fp);

}

