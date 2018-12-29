#ifndef _STOPWATCH__H__
#define _STOPWATCH__H__
#include "../h/port.h"

extern char g_timer_to_count_min;
extern char g_timer_to_count_sec;
extern bit g_blink_off_flag; 
extern  uchar volatile g_1s_key_timer; 

extern bit g_blink_flag;
enum sys_status
{
    e_idle,
    e_count_up,
    e_count_down,
    e_finished_alarm,
    e_low_power,
    e_temp,
	e_test
};
enum timer_state
{
    e_timer_stop,       //0
    e_timer_start,      //1
    e_timer_counting,   //2
    e_timer_finished,   //3
};

#endif