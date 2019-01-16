#include "../h/port.h"
#include "../h/alarm.h"    
#include "../h/serial.h"
#include "../h/systimer.h"
uchar volatile g_HZ_H_duty_h = 0;
uchar volatile g_HZ_L_duty_h = 0;
uchar volatile g_HZ_H_duty_l = 0;
uchar volatile g_HZ_L_duty_l = 0; 
sbit g_buzzer = P3^2;
char g_duty = 50;
#define ON 1
#define OFF 0
void Time1(void) interrupt 12  //“interrupt”声明函数为中断服务函数
//其后的1 为定时器T0 的中断编号；0 表示使用第0 组工作
{       
    AUXINTIF &= ~T2IF;                          //清中断标志
    g_buzzer = ~g_buzzer;
    if(ON == g_buzzer)
    {
        T2H = g_HZ_H_duty_h;
        T2L = g_HZ_L_duty_h; 
    }
    else
    {
        T2H = g_HZ_H_duty_l;
        T2L = g_HZ_L_duty_l; 
    }
    //T2H= g_HZ_H;         //(65536-56083)/256; //定时器T2 的高8 位重新赋初值
    //T2L= g_HZ_L;         //(65536-56083)%256; //定时器T0 的高8 位重新赋初值
}
void init_timer2(void)
{
    T2L = g_HZ_L_duty_l;   //65536-11.0592M/12/1000
    T2H = g_HZ_H_duty_l;   
    // AUXR &= 0xFB;       //1/12 T
    AUXR |= 0x04;           //Timer clock is 1T mode
    AUXR |= 0x10;          //启动定时器
    IE2  |= ET2;           //使能定时器中断
    EA    = 1;             
}     
#if 0 
void PWM_Isr() interrupt 22 using 1
{
    if (PWMCFG & 0x80)
    {
        PWMCFG &= ~0x80;                        //清中断标志
        P14    = !P14;                             //测试端口
    }
}
void init_pwm()
{
    P_SW2   = 0x80;
    PWMCKS  = 0x0f;                              // PWM时钟为系统时钟/16
    PWMC    = 0x0100;                              //设置PWM周期为256个PWM时钟
    P_SW2   = 0x00;
    PWMFDCR = 0x7a;                             //能IO口异常检测中断
    PWMCR   = 0xc0;                               //启动PWM模块并使能PWM计数器中断
    EA      = 1;
}
#endif
void set_duty(char duty)
{
   g_duty = duty; 
}
#ifdef SYS_HZ_4M
#define SYS_HZ 4000000
#elif define SYS_HZ_6M
#define SYS_HZ 6000000
#elif define SYS_HZ_27M
#define SYS_HZ 27000000
#endif
//#define SYS_HZ_6M
//#define SYS_HZ_27M
#if 0
void set_HZ(float HZ)
{
    unsigned int temp;
    unsigned int duty;
    temp = 65536 - SYS_HZ/2/HZ;       //2000000/(HZ>>1);
       
    g_HZ_L_duty_l = g_HZ_L_duty_h = temp&0xFF;
    g_HZ_H_duty_l = g_HZ_H_duty_h = temp>>8;     
}
#else
void set_HZ(float HZ)
{
    u32 temp = 0;
    u32  duty = 0;
    temp = SYS_HZ/HZ;

    duty = temp*g_duty/100;        //duty high part
    duty = 65536 - duty; 
    
    g_HZ_L_duty_h = duty&0xFF;
    g_HZ_H_duty_h = duty>>8;     

    duty = temp - duty;            // duty low part
    duty = 65536 - duty; 
    
    g_HZ_L_duty_l = duty&0xFF;
    g_HZ_H_duty_l = duty>>8;    
}
#endif
#define buzzer_pin_normal  P3M1 &= ~0x04;   P3M0 &= ~0x04; //P16 //P2M1 &= ~0x80;   P2M0 &= ~0x80;  //P27  //0 0 准双向口
#define buzzer_pin_high_output  P3M1 &= ~0x04; P3M0 |= 0x04; //P16  //P2M1 &= ~0x80; P2M0 |= 0x80; //P27   //0 1 推挽输出

void start_alarm()
{	
   // buzzer_pin_high_output;
    init_timer2();
}

void stop_alarm()
{
    AUXR &= 0xEF;                    //停止定时器
    //IE2 = IE2&(~ET2);
//	buzzer_pin_normal;
    g_buzzer = OFF;
}

//----------------------------blink LED ----------------------------------
#define ON  1
#define OFF 0
sbit g_led_R_port = P0^3;
sbit g_led_G_port = P0^2;
sbit g_led_B_port = P0^1;
u8   g_ledR_counter, g_ledG_counter, g_ledB_counter;
u8   g_ledR_duty, g_ledG_duty, g_ledB_duty;

void update_breath_blink()
{
    if(g_led_R_port != ON && g_ledR_counter <= g_ledR_duty)
    {
        g_led_R_port = ON;
    }
    else if(g_led_R_port == ON && g_ledR_counter> g_ledR_duty)
    {
        g_led_R_port = OFF;
    }


    if(g_led_G_port != ON && g_ledG_counter <= g_ledG_duty)
    {
        g_led_G_port = ON;
    }
    else if(g_led_G_port != OFF && g_ledG_counter > g_ledG_duty)
    {
        g_led_G_port = OFF;
    }


    if(g_led_B_port != ON  && g_ledB_counter <= g_ledB_duty)
    {
        g_led_B_port = ON;
    }
    else if(g_led_B_port != OFF && g_ledB_counter > g_ledB_duty)
    {
        g_led_B_port = OFF;
    }
}
bit g_breath_led_on = 0;
char g_breath_led_r_direction, g_breath_led_g_direction, g_breath_led_b_direction;
void close_rgb_led()
{
    g_led_R_port = g_led_G_port = g_led_B_port = OFF;
    g_breath_led_on = 0;     IE2 &= ~ET4;  // stop blink 

}
void poll_led_breath_1700us()
{
    g_ledR_counter++;
    g_ledG_counter++;
    g_ledB_counter++;
    
    if(g_ledR_counter == 11)g_ledR_counter = 1;
    if(g_ledG_counter == 11)g_ledG_counter = 1;
    if(g_ledB_counter == 11)g_ledB_counter = 1;
	
    update_breath_blink();
    {
        static long duty_counter = 0;
        duty_counter++;
        //if(duty_counter == 400)//every 1/10 s duty ++ or duty --  ,accomplish breath light    at 10us  1000hz
        //if(duty_counter == 20000)//every 1/10 s duty ++ or duty --  ,accomplish breath light   at20us  500HZ
        if(duty_counter == 120/*1.7ms * 150 = 250ms*/)//every 1/5 s duty ++ or duty --  ,accomplish breath light   at40us  250HZ
        {
            duty_counter = 0;
            
            if(g_ledR_duty >= 30) { g_breath_led_r_direction=-3; }
            else if (g_ledR_duty < 2) { g_breath_led_r_direction=3; }
            g_ledR_duty += g_breath_led_r_direction;
            
            if(g_ledG_duty >= 30) { g_breath_led_g_direction=-3; }
            else if (g_ledG_duty < 2) { g_breath_led_g_direction=3; }
            g_ledG_duty += g_breath_led_g_direction;
            
            if(g_ledB_duty >= 30) { g_breath_led_b_direction=-3; }
            else if (g_ledB_duty < 2) { g_breath_led_b_direction=3; }
            g_ledB_duty += g_breath_led_b_direction; 
        }
    }
}

void poll_led_alarm_100ms()
{
    g_ledR_counter++;
    g_ledG_counter++;
    g_ledB_counter++;
    if(g_ledR_counter == 101)g_ledR_counter = 1;
    if(g_ledG_counter == 101)g_ledG_counter = 1;
    if(g_ledB_counter == 101)g_ledB_counter = 1;
    g_led_R_port = g_led_G_port = g_led_B_port = OFF;
    if(g_ledR_counter == 1) g_led_R_port = ON;
    if(g_ledG_counter == 1) g_led_G_port = ON;
    if(g_ledB_counter == 1) g_led_B_port = ON;
}

#if 1
void TM4_Isr() interrupt 20 using 1
{
    AUXINTIF &= ~T4IF;           //clear interrupt flag
    if(g_breath_led_on)
    {
        poll_led_breath_1700us();
    }
    else   // alarm led
    {
        poll_led_alarm_100ms();    
    }
}
 
void Timer4Init_breath_1700us(void)    //1ms@4MHz
{
    T4T3M |= 0x20;       //Timer clock is 1T mode    
	//T4T3M &= 0xDF;		//Timer clock is 12T mode
    T4L = 0x70 ;         //Initial timer value     //1000HZ
    T4H = 0xE5;            //Initial timer value
    T4T3M |= 0x80;         //Timer4 start run
    IE2 |= ET4;
    EA = 1;
}
void Timer4Init_alarm_100ms(void)    //1ms@4MHz
{
	T4T3M &= 0xDF;		//Timer clock is 12T mode
    T4L = 0xCB;         //Initial timer value     //1000HZ
    T4H = 0x7D;            //Initial timer value
    T4T3M |= 0x80;         //Timer4 start run
    IE2 |= ET4;
    EA = 1;
}
#endif 

void start_breath_blink()
{
    g_ledR_duty = 50;//g_sys_1ms_ticket%100;
    g_ledG_duty = 25;//(g_sys_1ms_ticket>>1)%100;
    g_ledB_duty = 0;//(g_ledR_duty+10)%100;
    g_ledR_counter  = 0;// g_sys_1ms_ticket%100;
    g_ledG_counter  = 0;//(g_sys_1ms_ticket>>1)%100;
    g_ledB_counter  = 0;//(g_ledR_duty+10)%100;
    g_breath_led_on = 1;
    g_breath_led_r_direction = 3;
    g_breath_led_g_direction = 3;
    g_breath_led_b_direction = 3;
//  g_led_timer_HL = 0xFFD8;  //10us  //0xFFD8  Tled= 1ms  1000 HZ
//  g_led_timer_HL = 0xFFB0;  //20us  //0xFFB0  Tled= 2ms  500  HZ
//    g_led_timer_HL = 0xE570;// 0xE570;  //1.7ms  // Tled= 17ms  at 60HZ
    Timer4Init_breath_1700us();
}
void start_alarm_blink()
{
    g_ledR_counter = g_sys_10ms_ticket%10;
    g_ledG_counter = (g_sys_10ms_ticket>>1)%10;
    g_ledB_counter = (g_ledR_counter+3)%10;
    g_breath_led_on = 0;
    Timer4Init_alarm_100ms();
}
