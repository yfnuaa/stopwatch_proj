#include "../h/port.h"
#include "string.h" 
#include "stdio.h"

#if 0
volatile uchar g_cmdrecv=0;
volatile uchar g_cmdmode=0;

volatile uchar g_cmddata[16];
volatile uchar g_cmddatalen=0;
volatile uchar echo =0;
volatile uchar g_cmdindex=0;
volatile uchar g_version=0;
#endif
#ifdef ENABLE_SERIAL
//******serial_port_initial*********************************---------------------------

void serial_port_init(void)        //115200bps@27.000MHz
{
    //P3M1 &= 0xFC;
//	P3M0 |=0x3 ;
 	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xBF;		//��ʱ��1ʱ��ΪFosc/12,��12T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
#ifdef SYS_HZ_4M
	TL1 = 0xF7;		//�趨��ʱ��ֵ
	TH1 = 0xFF;		//�趨��ʱ��ֵ
#endif
#ifdef SYS_HZ_6M
	TL1 = 0xF3;		//�趨��ʱ��ֵ
	TH1 = 0xFF;		//�趨��ʱ��ֵ
#endif
#ifdef SYS_HZ_27M
	TL1 = 0xC5;		//�趨��ʱ��ֵ
	TH1 = 0xFF;		//�趨��ʱ��ֵ
#endif
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
}
  
#if 0
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
                    g_cmdrecv=1; //��������������������
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
                if(g_cmddata[1]==0xAA)  ////���������򣬽���cmdmode
                {
                     g_cmdmode=1; //���������򣬽���cmdmode
                }
                else if(g_cmddata[1]==0xBB)
                {
                    g_cmdmode=0; //�����������˳�CMDmode      ���� ready ״̬
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
void send_char(uchar i)
{
    ES       =   0;  //�ش����ж�
    TI       =   0;  //���㴮�ڷ�������ж������־
    SBUF     =   i;
    while(TI ==0);    //�ȴ��������
    TI       =   0;  //���㴮�ڷ�������ж������־
    ES       =   1;  //�������ж�
}

void send_buffer(uchar * pbuff)
{
    int i = 0;
    int len = strlen(pbuff);
    while(i<len)
    {
        send_char(pbuff[i++]);
    }
}


void send_integ(int datat)
{
    char buff[10]={0};
    //memset(buff,0,10);
    sprintf(buff,"%d",datat);
    send_char('[');
    send_buffer(buff);
    send_char(']');
}
#else
void serial_port_init(void) {}
	 void send_integ(int datat){} 
	 void send_buffer(uchar * pbuff){}	
	  void send_char(uchar i){}
#endif