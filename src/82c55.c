#include "reg52.h"
#include "../h/utility.h"
sbit    nec8255cs  =  P2^2;
sbit    nec8255rd  =  P2^7;
sbit    nec8255A1  =  P2^4;
sbit    nec8255A0  =  P2^5;
sbit    nec8255wr  =  P2^6; 	
sbit    nec8255rst =  P2^3; 
//#define dateread  //(P1^7<<7+P1^6<<6+P1^5<<5+P1^4<<4+P1^3<<3+P1^2<<2+P1^1<<1+P1^0)

void nec82c55_init()
{
    nec8255rd=1;
	nec8255wr=1;
	nec8255cs=0;
    nec8255rst=0;
	nec8255rst=1;
	Delay(4);
	nec8255rst=0;
    //nec8255cs=1;	
}

void nec82c55_read(unsigned char port,unsigned char * value)
{		//nec8255cs=0;
 	   #if 0
	//д0�����п�  ��ʼ������״̬
	   	nec8255A0=1;
		nec8255A1=1;
		P0=0x80;	  //���п����
		
		nec8255wr=0;
	 	Delay(1);
		nec8255wr=1;
	//	Delay(10);
		 //���ӹ������ý���
	    
		nec8255A0=0x1&port;
		nec8255A1=(0x2&port)>>1;
	    P0=0;	  
		nec8255wr=0;
	 	Delay(1);
		nec8255wr=1;
	 	Delay(10);

	//��ʼ��  �ѿ��ӹ������û���������Ϊ����
       	nec8255A0=1;
		nec8255A1=1;
		P0=0x9B;	  //���п�����
		nec8255wr=0;
	 	Delay(1);
		nec8255wr=1;
	 	Delay(10);
				 //���ӹ������ý���
 #endif
    	nec8255A0=0x1&port;
		nec8255A1=(0x2&port)>>1;	
	 	Delay(1);
   		nec8255rd=0;
		Delay(10);
		*value=	P0;//date read;
		nec8255rd=1;
	//	nec8255cs=1;
}
//inlines	 void relay_on(unsigned char arry,unsigned char on_off)
//	 {
//	 	P0^array=on_off;
	 
//	 }