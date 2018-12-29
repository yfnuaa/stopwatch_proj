#include "../h/utility.h"
#include "../h/82c55.h"
#include "../h/serial.h"
#include "reg51.h"
#include "intrins.h"                             //_nop_();延时函数用

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
   EA=1; //开总中断
ET0=1; //定时器T0 中断允许
TMOD=0x01; //使用定时器T0 的模式2
TH0=(65536-46083)/256; //定时器T0 的高8 位赋初值
TL0=(65536-46083)%256; //定时器T0 的高8 位赋初值
TR0=1; //启动定时器T0



	   beeppower=0;
	 //  beeppower=1;Delay(200000);beeppower=0;	   Delay(300000);
       serial_port_init();	 //串口初始化
			send_UART('p');	


       // nec82c55_init();	 //IO扩展初始化
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
 
 void Time0(void) interrupt 1 using 0 //“interrupt”声明函数为中断服务函数
//其后的1 为定时器T0 的中断编号；0 表示使用第0 组工作
{

Countor++; //中断次数自加1
//if(Countor==20*6) //若累计满20 次，即计时满1s
//{		send_UART('x');
//	   selfpower=0;Delay(20);   while(1);
// Countor=0; //将Countor 清0，重新从0 开始计数
//}
TH0=(65536-46083)/256; //定时器T0 的高8 位重新赋初值
TL0=(65536-46083)%256; //定时器T0 的高8 位重新赋初值
//	 TR0=1; //启动定时器T0
}