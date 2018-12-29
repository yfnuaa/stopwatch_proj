#include "../h/port.h"
#include "../h/led.h"
#include "../h/stopwatch.h"
#include "../h/utility.h"
#include "../h/serial.h"
sbit g_LED4 = P0^7;
sbit g_LED3 = P2^6;
sbit g_LED2 = P2^5;
sbit g_LED1 = P2^4;

sbit g_seg_a  = P0^4;
sbit g_seg_b  = P0^6;
sbit g_seg_c  = P0^2;
sbit g_seg_d  = P0^0;
sbit g_seg_e  = P2^7;
sbit g_seg_f  = P0^5;
sbit g_seg_g  = P0^3;
sbit g_seg_dp = P0^1;
uchar       BCD_table[] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                            0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
                            0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
                            0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
                            0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
                            0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
                            0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
                            0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
                            0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
                            0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99 
						  };
uchar DIG_CODE[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //��������ֱ�
                              
void flash_display(void)
{                            
    static char sig = 1;
    unsigned char cdata=0;      
    if(g_blink_off_flag){g_LED4 = g_LED3 = g_LED2 = g_LED1 = 1; return;}

    switch(sig)
    {
        case 1:
            cdata = BCD_table[g_timer_to_count_min]>>4;//g_timer_to_count_min/10;
            g_LED4 = 1; //g_LED1 = 0;     
        break;
        case 2:
            cdata = BCD_table[g_timer_to_count_min]&0xF;//g_timer_to_count_min%10;
            g_LED1 = 1; //g_LED2 = 0;
        break;
        case 3:
            cdata = BCD_table[g_timer_to_count_sec]>>4;//g_timer_to_count_sec/10;
            g_LED2 = 1; //g_LED3 = 0;
        break;
        case 4:
            cdata = BCD_table[g_timer_to_count_sec]&0xF;//g_timer_to_count_sec%10; 
            g_LED3 = 1; //g_LED4 = 0;
        break;
    }    
    // send_buffer("ledd",4);
    // P0 = (~DIG_CODE[cdata]);
    g_seg_a = (~ DIG_CODE[cdata]    )&1;
    g_seg_b = (~(DIG_CODE[cdata]>>1))&1;
    g_seg_c = (~(DIG_CODE[cdata]>>2))&1;
    g_seg_d = (~(DIG_CODE[cdata]>>3))&1;
    g_seg_e = (~(DIG_CODE[cdata]>>4))&1;
    g_seg_f = (~(DIG_CODE[cdata]>>5))&1;
    g_seg_g = (~(DIG_CODE[cdata]>>6))&1;
    g_seg_dp= (~(DIG_CODE[cdata]>>7))&1;
    if(1 == sig)
    {
        g_LED1 = 0;
    }
    else if(2 == sig)
    {
        g_LED2 = 0;
        g_seg_dp = 0;
    }
    else if(3 == sig)
    {
        g_LED3 = 0;
    }
    else
    {
        g_LED4 = 0;
    }
    if(++sig == 5)sig = 1;
}
void update_sec(uchar x)
{}
void update_min(uchar x)
{}
void close_display()
{
}
void init_display(){}
void black_display(void)
{
    g_LED4 = g_LED3 = g_LED2 = g_LED1=1;  g_seg_dp = 1;
}
