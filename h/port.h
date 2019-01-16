#ifndef _PORT_H__H
#define _PORT_H__H	
#include "stc8.h"
#include "intrins.h" 		 
typedef  unsigned char uchar;
typedef  unsigned char u8;
typedef  unsigned int  u16;
typedef  unsigned long u32;
typedef  char          i8;
 typedef  int           i16;
typedef  long          i32;

//#define TEST

//#define ENABLE_SERIAL


#define SYS_HZ_4M
//#define SYS_HZ_6M
//#define SYS_HZ_27M
#endif

/*-------------------
timer 0  system timer

timer 1  serial port baud rate

timer 2  alarm buzzer

timer 3  lcd

timer 4  alarm blink rgb
--------------------*/