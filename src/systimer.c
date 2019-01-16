#include "../h/port.h"
#include "../h/systimer.h"                                                                                         ////////main.c main.c main.c////////////////////
#include "../h/serial.h"
#include "../h/alarm.h"

unsigned int volatile g_sys_10ms_ticket = 0;
void Time0(void) interrupt 1 using 1  //��interrupt����������Ϊ�жϷ�����
//����1 Ϊ��ʱ��T0 ���жϱ�ţ�0 ��ʾʹ�õ�0 �鹤��
{
    g_sys_10ms_ticket++;
}
 
 void init_timer(void)
{

    //AUXR &= 0x7F;//    ��ʱ��0T
	AUXR |=0X80;
    //start up a 1ms timer
    TMOD &= 0xF0; //ʹ��          ��ʱ��T0            ��ģʽ2
 #ifdef SYS_HZ_4M     //4M 1ms
    TL0=0xC0;//0x30;//(65536-56083)%256; //��ʱ��T0 �ĸ�8 λ����ֵ
    TH0=0x63;//0xf8;//(65536-56083)/256; //��ʱ��T0 �ĸ�8 λ����ֵ
 #endif    //27M  1ms
#ifdef SYS_HZ_6M     //6M 1ms
    TL0=0x0c;//0x30;//(65536-56083)%256; //��ʱ��T0 �ĸ�8 λ����ֵ
    TH0=0xFe;//0xf8;//(65536-56083)/256; //��ʱ��T0 �ĸ�8 λ����ֵ
 #endif    //27M  1ms
 #ifdef SYS_HZ_27M
    TL0= 0x36;//(65536-56083)%256; //��ʱ��T0 �ĸ�8 λ����ֵ
    TH0= 0xf7;//(65536-56083)/256; //��ʱ��T0 �ĸ�8 λ����ֵ
 #endif
    EA=1; //�����ж�
    TF0 = 0;
    ET0=1; //��ʱ��T0 �ж�����    
    TR0=1; //������ʱ��T0
}
void stop_sys_timer(void)
{
TR0= 0 ;
}
