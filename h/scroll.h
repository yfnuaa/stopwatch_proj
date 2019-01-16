#ifndef __SCROLL_H__
#define __SCROLL_H__
//timer.h
enum roll_index
{
    e_roll_a,
    e_roll_max,
    e_roll_b,
};

#define low  0
#define high 1

typedef enum phase_status
{
    e_stop,
    e_clockwise,
    e_anti_clockwise,
	e_wait_stop,    
	e_judge_clockwise,
    e_judge_anti_clockwise,
};
enum phase_sub_status
{
    e_all_low,
    e_pre_count,
    e_count,
    e_countend,
};
extern void stop_scroll(void); 
extern char g_plus_count[e_roll_max];
extern void init_scroll();
extern void scan_roll(char);
#endif
