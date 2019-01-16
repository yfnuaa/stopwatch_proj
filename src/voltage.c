#include "../h/port.h"  
#if 0      
#define ENLVR           0x40                    //RSTCFG.6
#define LVD2V2          0x00                    //LVD@2.0V
#define LVD2V4          0x01                    //LVD@2.4V
#define LVD2V7          0x02                    //LVD@2.7V
#define LVD3V0          0x03                    //LVD@3.0V
#define LVDF            0x20                    //PCON.5
int volatile g_low_voltage = 0;
void Lvd_Isr() interrupt 6 using 1
{
    PCON &= ~LVDF;                              //清中断标志
    //P32 = ~P32;                               //测试端口
    g_low_voltage ++;
    ELVD = 1;
    if(g_low_voltage>=100)ELVD = 0;
}
void init_low_power_inter()
{
    return;
    PCON &= ~LVDF;                              //测试端口
//  RSTCFG = ENLVR | LVD3V0;                    //使能3.0V时低压复位,不产生LVD中断
    RSTCFG = LVD2V7;                            //使能3.0V时低压中断
    ELVD = 1;                                   //使能LVD中断
    EA = 1;
}    
#endif
sbit g_3_6v_sw1  = P3^4;
sbit g_3_8v_sw2  = P3^3;
sbit g_4_0v_sw3  = P3^5;
sbit g_ad_enable = P3^6;     
void enable_voltage_read_init()
{
    //IO 初始化为开漏模式
    P3M1 |= 0x38; P3M0 |= 0x38;
    //IO 输出高
    g_3_6v_sw1 = g_3_8v_sw2 = g_4_0v_sw3 = 1;
	
    CMPCR2 = 0x10;    //比较起结果经过16个去抖动始终后输出 
    CMPCR1 = 0;
    CMPCR1 &= ~0x08;  //p37为CMP+输入脚
    CMPCR1 &= ~0x04;  //内部参考电压为CMP- 输入脚
    CMPCR1 &= ~0x02;  //禁止比较器结果输出
//    CMPCR1 |= 0x80;   //使能比较起模块
//    g_ad_enable = 0;
}
void disable_voltage_read()
{
    CMPCR1 &= ~0x80;  //禁止比较起模块
	g_ad_enable = 1;
}
 
char get_voltage()
{
    static vol = 0;
    static comp_stage = 0;
    switch(comp_stage)
    {
        case 0:
        vol = 0;
		CMPCR1 |= 0x80;   //使能比较起模块
        g_ad_enable = 0;
        //P20// P2.0 输出 0     3.6
        g_3_6v_sw1 = 0;
        comp_stage++;
        break;
        case 1:
        if(!(CMPCR1 & 0x1)){ g_3_6v_sw1 = 1;comp_stage = 6; break;} 
        comp_stage++;
        g_3_6v_sw1 = 1;
        break;

        case 2:
        vol = 1;
        //P21//  P21 输出0         3.8
        g_3_8v_sw2 = 0;
        comp_stage++;
        break;
        case 3:
        if(!(CMPCR1 & 0x1)){g_3_8v_sw2 = 1; comp_stage = 6; break;} 
        comp_stage++;
        g_3_8v_sw2 = 1;
        break;

        case 4:
        vol = 2;     //      4.0
        //P21//  P21 输出0
        g_4_0v_sw3 = 0;
        comp_stage++;
        break;
        case 5:
        if(!(CMPCR1 & 0x1)){g_4_0v_sw3 = 1; comp_stage = 6; break;} 
        comp_stage++; vol = 3; 
        g_4_0v_sw3 = 1;
        break;
     }
    if(comp_stage == 6)
    {
        comp_stage = 0;
        disable_voltage_read();
        return vol;
    }
    else return -1;
}