#include "../h/port.h"
#include "../h/serial.h"
#include "../h/stopwatch.h"
#include "../h/keyprocess.h"
sbit g_key_state = P1^0;

#define PUSHED 0
#define POPED 1
bit   volatile g_key_released_inter = 0,g_key_released;
bit   volatile g_key_pressed_inter  = 0,g_key_pressed;
uchar volatile g_key_pressing_1s_inter = 0,g_key_pressing_1s;

void process_key(void)
{
    static int delay = 0;
//	send_buffer("PUSHED");
    if(delay > 1)
    {
        delay--;
        return;
    }
    if(PUSHED == g_key_state && 0 == g_key_pressed_inter && 0 == g_key_pressing_1s_inter && 0 == delay)
    {
        delay = 100;
        return;
    }
    else if(0 == g_key_pressed_inter && 0 == g_key_pressing_1s_inter && delay == 1)
    {
        delay = 0;
    }

    if(PUSHED == g_key_state)
    {
       if(0 == g_key_pressing_1s_inter)
       {
           g_key_pressed_inter = 1;send_buffer("PUSHED");
           g_key_pressing_1s_inter = 1;
           g_1s_key_timer = e_timer_start;
       }
       else
       {
           g_key_pressed_inter = 0;
           if(g_1s_key_timer == e_timer_finished)
           {
               g_1s_key_timer = e_timer_start;
               g_key_pressing_1s_inter++;
           }
       }
       g_key_released_inter = 0;
    }
    else
    {
       if(g_key_pressing_1s_inter)
       {
           g_key_released_inter = 1; send_buffer("key releasexxd");
           g_key_pressed_inter  = 0;
           g_key_pressing_1s_inter = 0;
       }
       else
       {
           g_key_released_inter = 0;
       }  
    }
}
bit g_key_juggle_start =0;
void INT0_Isr() interrupt 0
{
//	if(INT0)
	{
	}
//	else
	{
	}
//	process_key();
}
#ifdef KEY_INTERRUPT_
void init_key(void)
{
    IT0 = 0;//enable INT0 raise and fall interrupt
//	EX0 = 1;  //enable INT0 interrupt
	EA = 1;
    
}
#endif
void get_key_state()
{
	g_key_released = g_key_released_inter;
	g_key_pressed = g_key_pressed_inter;
	g_key_pressing_1s = g_key_pressing_1s_inter;
}