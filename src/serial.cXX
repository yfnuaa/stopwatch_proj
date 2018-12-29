#include "string.h" 
#include "stdio.h"
#include "stc8.h"
#include "../h/serial.h"
volatile unsigned char g_cmdrecv=0;
volatile unsigned char g_cmdmode=0;

volatile unsigned char g_cmddata[16];
volatile unsigned char g_cmddatalen=0;
volatile unsigned char echo =0;
volatile unsigned char g_cmdindex=0;
volatile unsigned char g_version=0;
//******serial_port_initial*********************************---------------------------
   bit busy;
char wptr;
char rptr;
char buffer[16];
void serial_port_init(void)    	//115200bps@27.000MHz
{
    SCON = 0x50;
    T2L = 0xFB;
    T2H = 0xFF;
    AUXR |= 0x01;
	AUXR &=0xFB;
    wptr = 0x00;
    rptr = 0x00;
    busy = 0;
	AUXR|=0X10;
    ES = 1;
    EA = 1;

    
}
 
//*********UART_Interrupt_Receive************************************------------------------------

void UART_Interrupt_Receive(void) interrupt 4
{
    
    if(RI==1 )
    {
    	if(g_cmdindex>0)
    	{
    	    if(g_cmddata[0]!=0x5a&&SBUF==0x5A)
    	    g_cmdindex=0;
        }
    	g_cmddata[g_cmdindex++] = SBUF;
    	
    	if(SBUF == 0xA5)
    	{
    	    if( g_cmdindex==12)
            {
    	        if(g_cmdrecv == 0)
    	        {
    	            g_cmdrecv=1; //告诉主程序，有新命令了
    	            g_cmddatalen = g_cmdindex;
    	         
            	    send_char('o');
            	    send_char('k');
    	        }
    	        else
    	        {  
 
    	            send_char('k');
    	        	send_char('o');
            	}
        	}
    	    #if 0
            else if(g_cmdindex == 3)    	  
            {
    	        if(g_cmddata[1]==0xAA)  ////告诉主程序，进入cmdmode
                {
                     g_cmdmode=1; //告诉主程序，进入cmdmode
                }
    	        else if(g_cmddata[1]==0xBB)
    	        {
    	        	g_cmdmode=0; //告诉主程序，退出CMDmode      进入 ready 状态
            	}
             
            }
    	    #endif
            else if( g_cmdindex==7)
            {   
                if(    g_cmddata[1]=='e' &&   g_cmddata[2]=='c'&&g_cmddata[3]=='h'&&g_cmddata[4]=='o')
                {
    	            if(    g_cmddata[5]=='1')echo=1;
    	            else echo = 0;
    	        }
    	     
            } 
    	    #if 0
    	    else if( g_cmdindex == 5)
    	    {
    	        if(    g_cmddata[1]=='v' &&   g_cmddata[2]=='e'&&g_cmddata[3]=='r')
    	    	g_version=1;

    	    
    	    }     
    	    #endif
            else if(    g_cmdindex <= 1)
            {
    	        
    	    	send_char('k');
    	    	send_char('o');
            }
    	    g_cmdindex = 0;
    	}

    	RI  =   0;
        // send_char(k);
    }
    else
    {
        TI  =  0;
    }
}
#endif

//******send_char********************-------------------------------
void send_char( char dat)
{
    while (busy);
    busy = 1;
    SBUF = dat;
}

void send_buffer(unsigned char * pbuff, int len)
{
    int i = 0;
    while(i<len)
    {
        send_char(pbuff[i++]);
    }
}


void send_integ(int datat)
{
    char buff[10]={0};
    memset(buff,0,10);
   // sprintf(buff,"%d",datat);
	send_char('[');
    //send_buffer(buff,strlen(buff));
	send_char(']');
}