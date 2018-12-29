#include "../h/utility.h"
#include "../h/82c55.h"
#include "../h/serial.h"
#include "reg51.h"
#include "intrins.h"                             //_nop_();��ʱ������

 sbit level_water= P0^0;
 sbit selfpower  = P0^2;
 sbit beeppower  = P0^1;
   static int Countor=0;
   sbit selfpower2 = P1^0;
 
void main()
{   
       int loop =1;	int count=0;
	   int signal=0,old_signal=0;
	   selfpower=1;
				  selfpower2=1;
   EA=1; //�����ж�
ET0=1; //��ʱ��T0 �ж�����
TMOD=0x01; //ʹ�ö�ʱ��T0 ��ģʽ2
TH0=(65536-46083)/256; //��ʱ��T0 �ĸ�8 λ����ֵ
TL0=(65536-46083)%256; //��ʱ��T0 �ĸ�8 λ����ֵ
TR0=1; //������ʱ��T0



	   beeppower=0;
	 //  beeppower=1;Delay(200000);beeppower=0;	   Delay(300000);
       serial_port_init();	 //���ڳ�ʼ��
			send_UART('p');	


       // nec82c55_init();	 //IO��չ��ʼ��
	while(loop)
	{	   		   
			signal=	 level_water; 			
            if(signal==1)
			{	 	
			    count++;
                send_UART('h');	  
 			 	if(Countor >=20*60*25||count >=4)
				{
				    beeppower=1;
					Countor=0;
					while(Countor<20*35)
					Delay(10000);
					count = 0;
					selfpower = 0;
					PCON=1;
	                while(1);//PCON=1;`
					loop =0;
				}
			}
			else if(signal ==0)
			{
				
			    send_UART('l');
				count=0;
			  
				  // 	if(count >=3) {count = 0;pumponoff = 1;selfpower = 1; loop =0; }
			}
			Delay(2000);
	}	 
}
 
 void Time0(void) interrupt 1 using 0 //��interrupt����������Ϊ�жϷ�����
//����1 Ϊ��ʱ��T0 ���жϱ�ţ�0 ��ʾʹ�õ�0 �鹤��
{

Countor++; //�жϴ����Լ�1
//if(Countor==20*6) //���ۼ���20 �Σ�����ʱ��1s
//{		send_UART('x');
//	   selfpower=0;Delay(20);   while(1);
// Countor=0; //��Countor ��0�����´�0 ��ʼ����
//}
TH0=(65536-46083)/256; //��ʱ��T0 �ĸ�8 λ���¸���ֵ
TL0=(65536-46083)%256; //��ʱ��T0 �ĸ�8 λ���¸���ֵ
//	 TR0=1; //������ʱ��T0
}