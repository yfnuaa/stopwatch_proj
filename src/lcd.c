#include "../h/port.h"
#include "../h/led.h"
#include "../h/stopwatch.h"
#include "../h/utility.h"
#include "../h/serial.h"
sbit g_com1 = P1^6;//P2^7;
sbit g_com2 = P1^7;//P2^6;
sbit g_com3 = P5^4;//P2^5;
sbit g_com4 = P5^5;//P2^4;
sbit g_seg_a  = P2^7;//P0^0;
sbit g_seg_b  = P2^6;//P0^1;
sbit g_seg_c  = P2^5;//P0^2;
sbit g_seg_d  = P2^4;//P0^3;
sbit g_seg_e  = P2^3;//P0^4;
sbit g_seg_f  = P2^2;//P0^5;
sbit g_seg_g  = P2^0;//P0^6;
sbit g_seg_dp = P2^1;//P0^7;
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
uchar DIG_CODE[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //数码管数字表
void init_display (void);
void close_display(void);
void black_display(void);
extern bit g_blink_flag;
static char volatile g_pwm_scan_index = 0;// 0 1--com1    2 3--com2   4 5--com3    6 7--com4 
   
static volatile uchar  g_com1_data = 0;
static volatile uchar  g_com2_data = 0;
static volatile uchar  g_com3_data = 0;
static volatile uchar  g_com4_data = 0;
//uchar g_com1_data_, g_com2_data_,g_com3_data_,g_com4_data_;

//PnM1.x  PnM0.x
//  0       0       准双向口
//  0       1       推挽输出
//  1       0       高阻输入
//  1       1       开漏输出
#define com1_hight_input   P1M1 |= 0x40; P1M0 &= ~0x40;//P16  //P2M1 |= 0x80; P2M0 &= ~0x80; //P27 //1 0 高阻输入
#define com2_hight_input   P1M1 |= 0x80; P1M0 &= ~0x80;//P17  //P2M1 |= 0x40; P2M0 &= ~0x40; //P26
#define com3_hight_input   P5M1 |= 0x10; P5M0 &= ~0x10;//P54  //P2M1 |= 0x20; P2M0 &= ~0x20; //P25
#define com4_hight_input   P5M1 |= 0x20; P5M0 &= ~0x20;//P55  //P2M1 |= 0x10; P2M0 &= ~0x10; //P24 

#define com1_normal  P1M1 &= ~0x40;   P1M0 &= ~0x40; //P16 //P2M1 &= ~0x80;   P2M0 &= ~0x80;  //P27  //0 0 准双向口
#define com2_normal  P1M1 &= ~0x80;   P1M0 &= ~0x80; //P17 //P2M1 &= ~0x40;   P2M0 &= ~0x40;  //P26
#define com3_normal  P5M1 &= ~0x10;   P5M0 &= ~0x10; //P54 //P2M1 &= ~0x20;   P2M0 &= ~0x20;  //P25
#define com4_normal  P5M1 &= ~0x20;   P5M0 &= ~0x20; //P55 //P2M1 &= ~0x10;   P2M0 &= ~0x10;  //P24

#define com1_push_pull  P1M1 &= ~0x40; P1M0 |= 0x40; //P16  //P2M1 &= ~0x80; P2M0 |= 0x80; //P27   //0 1 推挽输出
#define com2_push_pull  P1M1 &= ~0x80; P1M0 |= 0x80; //P17  //P2M1 &= ~0x40; P2M0 |= 0x40; //P26 
#define com3_push_pull  P5M1 &= ~0x10; P5M0 |= 0x10; //P54  //P2M1 &= ~0x20; P2M0 |= 0x20; //P25
#define com4_push_pull  P5M1 &= ~0x20; P5M0 |= 0x20; //P55  //P2M1 &= ~0x10; P2M0 |= 0x10; //P24
void TM3_Isr() interrupt 19 using 1
{    
    //getdata();
    AUXINTIF &= ~T3IF;                 //清中断标志
    switch(g_pwm_scan_index)
    {
        //COM 1//COM 1//COM 1//COM 1//COM 1//COM 1//COM 1//COM 1
        case 0:    
        com4_hight_input;
        P2 = ~g_com1_data;g_com1 = 1; 
        //P2 = 0xFF;g_com1 = 1;       
        com1_push_pull; 
        break;
        case 1:
        com1_hight_input;
        P2 = g_com1_data; g_com1 = 0;
        //P2 = 0;g_com1 = 0; 
        com1_push_pull; 
        break;

        //COM2//COM2//COM2//COM2//COM2//COM2//COM2//COM2//COM2
        case 2:
        com1_hight_input;
        P2 = ~g_com2_data;    g_com2 =  1;
        //  P2 = 0xFF;    g_com2 =  1;
        com2_push_pull;
        break;
        case 3:
        com2_hight_input
        P2 = g_com2_data; g_com2 = 0;
        //P2 = 0;    g_com2 = 0;
        com2_push_pull;
        break;

        //COM3//COM3//COM3//COM3//COM3//COM3//COM3//COM3//COM3//COM3//COM3
        case 4:
        com2_hight_input;
        P2 = ~g_com3_data; g_com3 = 1; 
        //P2 = 0xFF; g_com3 = 1; 
        com3_push_pull;
        break;
        case 5:
        com3_hight_input
        P2 = g_com3_data; g_com3 = 0;
        //P2 = 0; g_com3 = 0;
        com3_push_pull;
        break;

        //COM4 //COM4 //COM4 //COM4 //COM4 //COM4 //COM4 //COM4 //COM4
        case 6:
        com3_hight_input;
        P2 = ~g_com4_data; g_com4 = 1;    
        //P2 = 0xFF; g_com4 = 0;    
        com4_push_pull;
        break; 
        case 7:
        com4_hight_input;
        P2 = g_com4_data; g_com4 = 0;
        //P2 = 0; g_com4 = 1;
        com4_push_pull;
        break;
    }
    g_pwm_scan_index++;
    if(g_pwm_scan_index >= 8)g_pwm_scan_index = 0;
}
  
void init_display(void)
{
    P0M1 = 0; P0M0 =0;
    g_com1_data = g_com2_data= g_com3_data = g_com4_data = 0;
    g_com4_data = g_com4_data|0x04; //S4
    com1_hight_input;com2_hight_input;com3_hight_input;com4_hight_input;//set all com high rissor

    T4T3M &= 0xFD;   //定时器3时钟12T模式
#ifdef SYS_HZ_4M //4MHz
    T3L = 0xBF;      //设置定时初值    //2.5ms
    T3H = 0xFC;      //设置定时初值
#endif    
#ifdef SYS_HZ_27M //27MHz
    T3L = 0x07;      //设置定时初值    //2.5ms
    T3H = 0xEA;      //设置定时初值
#endif    
#ifdef SYS_HZ_6M  //6MHz
    T3L = 0x1E;      //设置定时初值    //2.5ms
    T3H = 0xFB;      //设置定时初值
#endif
    IE2 = IE2|ET3;   //使能定时器中断
    T4T3M |= 0x08;   //启动定时器
    EA = 1;
}
void close_display()
{
    T4T3M &= 0xF7;  //停止定时器3
    P2=0;g_com1 = g_com2 = g_com3 = g_com4 =0;
    com1_hight_input;com2_hight_input;com3_hight_input;com4_hight_input;//set all com high rissor
}
extern bit g_blink_off_flag;
void update_min(uchar min)
{
    uchar min_code=0;
 
    if(g_blink_off_flag)return;
    min = BCD_table[min];
    min_code =  DIG_CODE[min >> 4];
    g_com1_data &= 0x3F;//0xFC;
	
    g_com1_data |= (min_code&0x20)?0x80:0; //1F
    g_com1_data |= (min_code&0x01)?0x40:0; //1A
    g_com2_data &= 0x3F;
    g_com2_data |= (min_code&0x40)?0x80:0; //1G
    g_com2_data |= (min_code&0x02)?0x40:0; //1B
    g_com3_data &= 0x3F;
    g_com3_data |= (min_code&0x10)?0x80:0; //1E
    g_com3_data |= (min_code&0x04)?0x40:0; //1C
    g_com4_data &= 0x3F;
    g_com4_data |= (min_code&0x08)?0x80:0; //1D
    if(3 == g_battery_display)g_com4_data |= 0x40; //S3
    min_code =  DIG_CODE[(min&0xF)];
    g_com1_data &= 0xCF;
    g_com1_data |= (min_code&0x20)?0x20:0; //2F
    g_com1_data |= (min_code&0x01)?0x10:0; //2A
    g_com2_data &= 0xCF;
    g_com2_data |= (min_code&0x40)?0x20:0; //2G
    g_com2_data |= (min_code&0x02)?0x10:0; //2B
    g_com3_data &= 0xCF;
    g_com3_data |= (min_code&0x10)?0x20:0; //2E
    g_com3_data |= (min_code&0x04)?0x10:0; //2C
    g_com4_data &= 0xCF;
    g_com4_data |= (min_code&0x08)?0x20:0; //2D
    if(g_battery_display >= 2)g_com4_data |= 0x10; //S2
#if 0
    g_com2_data = 0xff;
    g_com3_data = 0;
    g_com1_data = 0;
    g_com4_data = 0;
#endif    
     
}

void update_sec(uchar sec)
{    
    uchar sec_code=0;//  return;
    //  sec=11;
    if(g_blink_off_flag)return;
    sec = BCD_table[sec];

    sec_code =  DIG_CODE[sec >> 4];
    g_com1_data &= 0xF3;
    g_com1_data |= (sec_code&0x20)?0x08:0;  //3F
    g_com1_data |= (sec_code&0x01)?0x04:0;  //3A
    g_com2_data &= 0xF3;
    g_com2_data |= (sec_code&0x40)?0x08:0;  //3G
    g_com2_data |= (sec_code&0x02)?0x04:0;  //3B
    g_com3_data &= 0xF3;
    g_com3_data |= (sec_code&0x10)?0x08:0;  //3E
    g_com3_data |= (sec_code&0x04)?0x04:0;  //3C 
    g_com4_data &= 0xF3;
    g_com4_data |= (sec_code&0x08)?0x08:0;  //3D
    g_com4_data |=0x04;
    //if(OPEN S4 )  g_com4_data = g_com4_data|(0x04);//S4

    sec_code = DIG_CODE[(sec&0xf)];
    g_com1_data &= 0xFC;
    g_com1_data |= (sec_code&0x20)?0x01:0;   //4F
    g_com1_data |= (sec_code&0x01)?0x02:0;   //4A
    g_com2_data &= 0xFC;
    g_com2_data |= (sec_code&0x40)?0x01:0;   //4G
    g_com2_data |= (sec_code&0x02)?0x02:0;   //4B
    g_com3_data &= 0xFC;
    g_com3_data |= (sec_code&0x10)?0x01:0;   //4E
    g_com3_data |= (sec_code&0x04)?0x02:0;   //4C
    g_com4_data &= 0xFC;
    g_com4_data |= (sec_code&0x08)?0x01:0;   //4D
    if(g_battery_display >= 1) g_com4_data |= 0x02;  //S1 
}
void black_display(void)
{
    //set all com high rissor
    com1_hight_input;com2_hight_input;com3_hight_input;com4_hight_input;//set all com high rissor
}
