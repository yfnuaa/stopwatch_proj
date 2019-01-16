#include "../h/port.h"
#include "../h/utility.h"
#include "string.h"
#include "../h/82c55.h"
#include "../h/serial.h"

#include "../h/scroll.h"
#include "../h/led.h" 
#include "../h/stopwatch.h"
#include "../h/alarm.h"
#include "../h/ds1302.h"
#include "../h/eeprom.h"
#include "../h/systimer.h"
#include "../h/keyprocess.h"
#include "../h/voltage.h"
//volatile bit g_1ms_triggered = 0;
static   bit g_paused = 0;       
 
//7 seg led refresh timer---------------------
uchar g_refresh_battery_icon_start = e_timer_stop; 
#define BATTERY_ICON_FLASH 50      //0.5s flash ， 10ms for one tick
//--------------------------------------------
uchar g_saved_count_time   = 0;
char  g_timer_to_count_min = 0;
char  g_timer_to_count_sec = 0;
//some system used timer---------------------------------------------------------------
#ifdef TEST
uchar volatile g_1s_test_timer       = e_timer_stop;
#endif
uchar volatile g_100ms_timer         = e_timer_stop;      //used for scroll 
uchar volatile g_alarm_timeout_timer = e_timer_stop;      //used for alarm timer out time
uchar volatile g_1s_key_timer        = e_timer_stop;      //used for key press time

uchar volatile g_1min_sys_idle_timer = e_timer_stop;
uchar volatile g_sys_idle_time_1min  = 0; 

uchar volatile g_1min_poll_battery_timer   =  e_timer_finished;// used for poll battery voltage
unsigned int g_poll_voltage_timer_interval = 0;


//-------------------------------------------------------------------------------------

//ds1302 poll timer-------------------------------------------s
uchar volatile g_poll_ds1302_timer_start  = e_timer_stop;
unsigned int volatile g_poll_ds1302_timer = 0;
//------------------------------------------------------------

//alarm led blink timer---------------------------------------
char g_blink_function_start      = e_timer_stop;
unsigned int g_blink_led_counter = 0;
bit g_blink_off_flag             = 0;
#define BLINK_OFF_MS 20  //200ms
#define BLINK_ON_MS  30  //300ms
//------------------------------------------------------------

#define check_timer(g_poll_start, old_poll_ticket, COUNT_TICKET)       \
{                                                                      \
    if(e_timer_start == g_poll_start)                                  \
    {                                                                  \
        old_poll_ticket = g_sys_10ms_ticket;                           \
        g_poll_start    = e_timer_counting;                            \
    }                                                                  \                       
    else if(e_timer_counting == g_poll_start)                          \
    {                                                                  \
        if(old_poll_ticket < g_sys_10ms_ticket)                        \
        {                                                              \
            temp = g_sys_10ms_ticket - old_poll_ticket;                \
        }                                                              \
        else                                                           \
        {                                                              \ 
            temp = 0xffff - old_poll_ticket + 1 + g_sys_10ms_ticket;   \
        }                                                              \
        if(temp >= COUNT_TICKET) g_poll_start = e_timer_finished;      \
    }                                                                  \
}                                                                    
                                              
void calc_time()
{
    unsigned int temp =0;
    static unsigned int old_5s_timer_ticket     = 0;
    static unsigned int old_100ms_timer_ticket  = 0;
    static unsigned int old_poll_ds1302_ticket  = 0;
    static unsigned int old_blink_ticket        = 0;
    static unsigned int olg_battery_icon_flash_ticket = 0;
    static unsigned int old_key_pressing_ticket = 0;
    static unsigned int old_1min_timeout_ticket = 0;
    static unsigned int old_poll_battery_timer_ticket = 0;

    #ifdef TEST 
    static unsigned int old_1s_testtimer_ticket  = 0;
    check_timer(g_1s_test_timer, old_1s_testtimer_ticket, 100);
    #endif

    //100ms timer check        //100ms timer check//100ms timer check
    check_timer(g_100ms_timer, old_100ms_timer_ticket, 10)
    check_timer(g_1min_sys_idle_timer, old_1min_timeout_ticket, 6000);   
    check_timer(g_1min_poll_battery_timer, old_poll_battery_timer_ticket,100);//6000);                              
    check_timer(g_1s_key_timer, old_key_pressing_ticket, 100);
    //5s timer check  
    check_timer(g_alarm_timeout_timer, old_5s_timer_ticket, 600);//6s
 
    //ds1302s check          //used for poll ds1302 sec; //first timer ==960, then 10ms one time until 1s trigger from ds1302
    check_timer(g_poll_ds1302_timer_start, old_poll_ds1302_ticket, g_poll_ds1302_timer);//g_poll_ds1302_timer);
 
    //flash battery timer
    check_timer(g_refresh_battery_icon_start, olg_battery_icon_flash_ticket, BATTERY_ICON_FLASH);

    // for blink led
    check_timer(g_blink_function_start, old_blink_ticket, g_blink_led_counter);
    if(e_timer_finished == g_blink_function_start)
    {
        g_blink_off_flag = !g_blink_off_flag;    //g_blink_flag ==1 led off ,
        if(g_blink_off_flag)
        {
            g_blink_led_counter = BLINK_OFF_MS;
            black_display();
            stop_alarm();
        }
        else
        {    
            start_alarm();
            g_blink_led_counter = BLINK_ON_MS;
            update_min(g_timer_to_count_min);
            update_sec(g_timer_to_count_sec);
        }
        g_blink_function_start  = e_timer_start;
   }
}
 

char  volatile g_sys_state = e_idle;
sbit g_key_state = P1^0;
void init_hardware(void)
{
    //init_low_power_inter();// low voltage detect...
	#ifdef KEY_INTERRUPT_
	init_key();
	#endif
	enable_voltage_read_init();
    serial_port_init        ();  
    init_timer ();
    ds1302_init();
    ds1302_stop();
    init_display ();
    black_display();
}

#define init_timer_and_state()                    \
{   g_sys_state = e_idle;  stop_alarm();          \
    init_scroll();                                \
    g_1min_sys_idle_timer = e_timer_stop;         \
    g_sys_idle_time_1min  = 0;                    \
    g_blink_function_start = e_timer_stop;        \
    g_blink_off_flag = 0;                         \
    g_poll_ds1302_timer_start = e_timer_stop;     \
    g_100ms_timer  = e_timer_stop;                \
    g_1s_key_timer = e_timer_stop;                \
    g_alarm_timeout_timer    = e_timer_stop;      \
    g_timer_to_count_min = 0;                     \
    g_timer_to_count_sec = 0;                     \
    g_key_released = 0; g_key_pressed = 0;        \ 
    g_key_pressing_1s = 0; g_paused = 0;          \
    g_poll_voltage_timer_interval = 10;            \
    g_1min_poll_battery_timer = e_timer_finished; \
	g_refresh_battery_icon_start = e_timer_stop; g_battery = g_battery_display = 0; \
}

bit g_wakeup_from_sleep_flag = 0;
 
uchar g_battery = 0;
uchar g_battery_display = 0;
void poll_voltage()                                                 
{   
    if(g_battery == 0)                                              
    {                                                               
        if(g_refresh_battery_icon_start == e_timer_stop)            
        {                                                           
            g_refresh_battery_icon_start = e_timer_start;           
        }                                                           
        else if( e_timer_finished == g_refresh_battery_icon_start)  
        {                                                           
            g_battery_display = !g_battery_display;      
            update_sec(g_timer_to_count_sec);						
            update_min(g_timer_to_count_min);			
            g_refresh_battery_icon_start = e_timer_start;           
        }                                                           
    }                                                                
    if(e_timer_finished == g_1min_poll_battery_timer)               
    {                                                               
        g_poll_voltage_timer_interval++;                            
        g_1min_poll_battery_timer= e_timer_start;                   
    }                                                               
    else if(e_timer_stop == g_1min_poll_battery_timer)              
    {                                                               
        g_1min_poll_battery_timer = e_timer_start;                  
    }      
	                                                         
    if(g_poll_voltage_timer_interval <10) return;                    
    g_battery = get_voltage();                                  
    if(-1 == g_battery){ return; }                                  
    g_poll_voltage_timer_interval = 0;                              
	send_buffer("vol");send_integ(g_battery);
                                                               
    if(g_battery > 0)                                          
    {                                                               
        if(g_battery_display!= g_battery)							
		{
			g_battery_display = g_battery;                             
            update_sec(g_timer_to_count_sec);						
            update_min(g_timer_to_count_min);
		}					
    }                                                               
}                                                                       

void main()
{
    char scroll_count = 0;
	close_rgb_led();
INIT_START:
    //P0M0 = 0;
    //P0M1 = 0;
    close_rgb_led();
    init_timer_and_state();
    init_hardware();

    g_saved_count_time = IapRead(0x400);

    if(g_saved_count_time > 99) g_saved_count_time = 0;
    g_timer_to_count_min = g_saved_count_time;
    send_buffer("saved time:"); send_integ(g_saved_count_time);
    update_sec(g_timer_to_count_sec);
    update_min(g_timer_to_count_min);
    g_refresh_battery_icon_start = e_timer_start; 
    #ifdef TEST
    g_1s_test_timer = e_timer_start;     ds1302_start();      // test only 
    #endif
    while(1)
    {   
        calc_time();
		
        #ifdef TEST
            if(e_timer_finished == g_1s_test_timer){ ds1302_read_sec(); g_1s_test_timer = e_timer_start;}     //test only
        #endif
        {       
            scan_roll(e_roll_a); 	 process_key();  
            get_key_state();
            {
                scroll_count = 0;
                if(g_plus_count[e_roll_a] && e_timer_stop == g_100ms_timer)
                {
                    g_100ms_timer = e_timer_start;         //trigger 100ms timer
                }
                if(e_timer_finished == g_100ms_timer)       //1 means 100ms count up 
                {     
                    g_100ms_timer = e_timer_stop;
                    scroll_count  = g_plus_count[e_roll_a];
                    g_plus_count[e_roll_a] = 0;
                }
            } 
        }              
        if(0 == g_key_pressing_1s && 0 == g_key_released && 0 == g_key_pressed && e_idle == g_sys_state && 0 == g_plus_count[e_roll_a] && 0 == scroll_count)//没有案件按下,也没有按键释放. 开始计时.5分钟
        {
            if(e_timer_finished == g_1min_sys_idle_timer)
            {
                g_sys_idle_time_1min++; 
                g_1min_sys_idle_timer= e_timer_start;
            }
            else if(e_timer_stop == g_1min_sys_idle_timer)
            {
                g_1min_sys_idle_timer = e_timer_start;
            }
        }
        else
        {    
            g_sys_idle_time_1min = 0;
            g_1min_sys_idle_timer = e_timer_stop;
        }                                        
      
        if(e_idle == g_sys_state)poll_voltage();

        if(g_key_pressing_1s >= 3 || g_sys_idle_time_1min >= 1)
        {     
           
            if(IapRead(0x400) != g_saved_count_time) 
            {
                IapWrite(0x400, g_saved_count_time);
            }
            VOCTRL = 0;            // 选择内部静态保持电流控制线路,静态电流 1.5uA
            // VOCTRL = 0x80;           // 选择外部静态保持电流控制线路,电流小于 0.1uA
            P_SW2 = 0; //IT2 = 0;    //下降沿中断 
            IE2 = ES2;  //EX0 = 1;   //使能中断 
            black_display();
            close_display();
            close_rgb_led();
            disable_voltage_read();
				  stop_sys_timer();
           		     stop_scroll();
			     ds1302_shutdown();
					  stop_alarm();
            //pcon |=0x1; //MCU idle
            while(0 == g_key_state)Delay(799);
            send_buffer("goto sleep, keypressing"); send_integ(g_key_state);
            PCON = 0x02; //MCU sleep
            g_wakeup_from_sleep_flag=1; 
		    while(0 == g_key_state)Delay(799);
            goto INIT_START;
        }//g_sys_state=      e_test ;
        switch(g_sys_state)
        {
            case e_idle:  
                if     (scroll_count> 10){scroll_count += 12;}
                else if(scroll_count<-10){scroll_count -= 12;}                     
                else if(scroll_count> 9 ){scroll_count += 10;}
                else if(scroll_count<-9 ){scroll_count -= 10;}
                else if(scroll_count> 6 ){scroll_count += 4 ;}
                else if(scroll_count<-6 ){scroll_count -= 4 ;}
                g_timer_to_count_min += scroll_count;
                if(scroll_count!=0)
                {

                    if(g_timer_to_count_min > 99) g_timer_to_count_min -= 100;
                    if(g_timer_to_count_min < 0) g_timer_to_count_min = 100 + g_timer_to_count_min;
                    if(g_saved_count_time != g_timer_to_count_min)
                    {
                        send_integ(g_timer_to_count_min); 
                        g_saved_count_time = g_timer_to_count_min;
                    }
                    update_min(g_timer_to_count_min);
                }
                if(g_key_pressed)
                { 
                    ds1302_start();        
                    //send_buffer("rxn!");   
                    if(IapRead(0x400) != g_saved_count_time)
                    {
                        IapWrite(0x400, g_timer_to_count_min); g_saved_count_time = g_timer_to_count_min;
                    }
                    g_100ms_timer = e_timer_stop;// cancel poll scroll
                    //if(g_timer_to_count_min>99) g_timer_to_count_min -= 99;
                    //if(g_timer_to_count_min<0) g_timer_to_count_min = 100 +    g_timer_to_count_min;
                    
                    g_sys_state = e_count_down;
                    if(0 == g_timer_to_count_min)   
                    {            
                        //g_poll_ds1302_timer = 900;//begin to poll ds1302 for 1s
                        g_sys_state = e_count_up;
                    }
 
                    //send_int(g_timer_to_count_min);//debug use  
                    g_poll_ds1302_timer = 98;   //begin to poll ds1302 for 1s
                    g_poll_ds1302_timer_start = e_timer_start;
                    start_breath_blink();// info user counter is running
                }
            break;
            case e_count_up:
                if(e_timer_finished == g_poll_ds1302_timer_start)
                {
                    if( ds1302_1s_trigger())
                    {
                        g_poll_ds1302_timer_start = e_timer_start;
                        g_poll_ds1302_timer = 99; 
                        if(1 == g_paused)
                        break;
                        g_timer_to_count_sec++;
                        if(g_timer_to_count_sec == 60)
                        {
                            g_timer_to_count_min++;
                            g_timer_to_count_sec = 0;  
                            if(g_timer_to_count_min ==  100)
                                g_timer_to_count_min = 0; 
                            update_min(g_timer_to_count_min);
                        }
                        update_sec(g_timer_to_count_sec);
                        {
                            send_integ(g_timer_to_count_min);    //debug use
                            send_integ(g_timer_to_count_sec);    //debug use
                        }
                    }
                    else
                    {
                        g_poll_ds1302_timer_start = e_timer_start;
                        g_poll_ds1302_timer = 1; 
                    }

                }
                if(g_key_pressed)//pause
                {
                    g_paused = !g_paused;
                    //if(!g_paused)start_breath_blink();// info user counter is runnin
                    //else close_rgb_led();
                } 
                if(scroll_count >=5 || scroll_count<=-5)
                {
                    g_poll_ds1302_timer_start = e_timer_stop;
                    g_sys_state = e_temp;
                    ds1302_stop();close_rgb_led();
                    g_timer_to_count_min = 0;
                    g_timer_to_count_sec = 0;  update_min(g_timer_to_count_min); update_sec(g_timer_to_count_sec);
                }
            break;
            case e_count_down:
            {         
                if(g_poll_ds1302_timer_start == e_timer_finished)
                {        
                    if(ds1302_1s_trigger())
                    {
                         g_poll_ds1302_timer_start = e_timer_start;
                         g_poll_ds1302_timer = 99; 
                         if(1 == g_paused)
                         break;
                         if(g_timer_to_count_sec)
                         {
                             g_timer_to_count_sec --;
                             
                         }
                         else                              
                         {
                             g_timer_to_count_sec = 59;
                             g_timer_to_count_min --;
                             update_min(g_timer_to_count_min);
                         }
                         update_sec(g_timer_to_count_sec);
                         {
                            send_integ(g_timer_to_count_min);    //debug use
                            send_integ(g_timer_to_count_sec);    //debug use
                         }
                         if((g_timer_to_count_sec+g_timer_to_count_min) == 0)
                         {
                            g_poll_ds1302_timer = e_stop; ds1302_stop();
                            g_sys_state = e_finished_alarm;
                            //set_HZ(17.23);
                            set_HZ(5000);
                            set_duty(50);
                            //start_alarm();
                            start_alarm_blink();
                            g_alarm_timeout_timer = e_timer_start; 
                             
                            g_blink_led_counter =  BLINK_OFF_MS;
                            g_blink_off_flag = 1; 
                            g_blink_function_start = e_timer_start;
                        }    
                    }
                    else
                    {
                        g_poll_ds1302_timer_start = e_timer_start;
                        g_poll_ds1302_timer = 1;       //every 5ms goto check wether the 1s timer is triggered
                        break;
                    }
                }    
                if(g_key_pressed)//pause
                {
                    g_paused = !g_paused;
                    //if(!g_paused)start_breath_blink();// info user counter is runnin
                    //else close_rgb_led();
                }  
                if(scroll_count >= 5 || scroll_count <= -5)
                {	
			        close_rgb_led();
                    g_poll_ds1302_timer_start = e_timer_stop;
                    g_timer_to_count_min = g_saved_count_time;
                    g_timer_to_count_sec = 0;
                    g_sys_state = e_temp;     update_min(g_timer_to_count_min); update_sec(g_timer_to_count_sec);
                }      
            }
            break;
            case e_finished_alarm:
            {  
                //set_HZ(16.39);
                if(g_alarm_timeout_timer == e_timer_finished ||g_key_released||scroll_count||g_plus_count[e_roll_a])
                {
                    stop_alarm();close_rgb_led();
                    g_blink_function_start = e_timer_stop; g_blink_off_flag = 0;
                    g_alarm_timeout_timer = e_timer_stop;
                    send_buffer("goto idle");
                    g_timer_to_count_min = g_saved_count_time;
                    g_timer_to_count_sec = 0;
                    g_sys_state = e_idle;
                    g_plus_count[e_roll_a] = 0;
                    update_min(g_timer_to_count_min); update_sec(g_timer_to_count_sec);
                }
             }
            break;
            case e_temp:
            if(0 == scroll_count)
            g_sys_state = e_idle;
            break;
            #if 0
            case e_test:
            {
            static bit stat = 0;
            static int hz = 17.23;
            if(stat ==0 && g_key_pressed)
            {
                //set_HZ(17.23);
                stat =1;
                set_HZ(hz);
                //set_duty(1000);
                start_alarm();
                update_min(hz/100);
                update_sec(hz%100);
            }
            else if(1 == stat && g_key_pressed)
            {
                stop_alarm();close_rgb_led();
            }
            if(1 == stat)
            {
                update_blink_flash_led();
            }
            if(scroll_count)
            {
                if     (scroll_count> 10){scroll_count += 12;}
                else if(scroll_count<-10){scroll_count -= 12;}                     
                else if(scroll_count> 9 ){scroll_count += 10;}
                else if(scroll_count<-9 ){scroll_count -= 10;}
                else if(scroll_count> 6 ){scroll_count += 4 ;}
                else if(scroll_count<-6 ){scroll_count -= 4 ;}
                hz+=scroll_count*100;
                update_min(hz/100);
                update_sec(hz%100);
                set_HZ(hz);
            }
            
             }
            break;
            #endif
        }
        scroll_count = 0;
      }
}
