#include "../h/port.h"
#include "../h/systimer.h"                                                                                         ////////main.c main.c main.c////////////////////
#include "../h/serial.h"
#include "../h/alarm.h"

unsigned int volatile g_sys_10ms_ticket = 0;
void Time0(void) interrupt 1 using 1  //“interrupt”声明函数为中断服务函数
//其后的1 为定时器T0 的中断编号；0 表示使用第0 组工作
{
    g_sys_10ms_ticket++;
}
 
 void init_timer(void)
{

    //AUXR &= 0x7F;//    定时器0T
	AUXR |=0X80;
    //start up a 1ms timer
    TMOD &= 0xF0; //使用          定时器T0            的模式2
 #ifdef SYS_HZ_4M     //4M 1ms
    TL0=0xC0;//0x30;//(65536-56083)%256; //定时器T0 的高8 位赋初值
    TH0=0x63;//0xf8;//(65536-56083)/256; //定时器T0 的高8 位赋初值
 #endif    //27M  1ms
#ifdef SYS_HZ_6M     //6M 1ms
    TL0=0x0c;//0x30;//(65536-56083)%256; //定时器T0 的高8 位赋初值
    TH0=0xFe;//0xf8;//(65536-56083)/256; //定时器T0 的高8 位赋初值
 #endif    //27M  1ms
 #ifdef SYS_HZ_27M
    TL0= 0x36;//(65536-56083)%256; //定时器T0 的高8 位赋初值
    TH0= 0xf7;//(65536-56083)/256; //定时器T0 的高8 位赋初值
 #endif
    EA=1; //开总中断
    TF0 = 0;
    ET0=1; //定时器T0 中断允许    
    TR0=1; //启动定时器T0
}
void stop_sys_timer(void)
{
TR0= 0 ;
}
