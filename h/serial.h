#ifndef __SERIAL_H__
#define __SERIAL_H__
#include "../h/port.h"
extern void serial_port_init();
extern void send_buffer(unsigned char * pbuff);
extern void send_char(char i);
extern void send_integ(int i);
extern volatile uchar g_cmdrecv;
extern volatile uchar g_cmdmode;

extern volatile uchar g_cmddata[16];
extern volatile uchar g_cmddatalen;
extern volatile uchar echo;
extern volatile uchar g_version;

#endif