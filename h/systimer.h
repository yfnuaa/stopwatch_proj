#ifndef _SYSTIMER_H__H_
#define _SYSTIMER_H__H_
extern unsigned int volatile g_sys_10ms_ticket;
void init_timer(void);
void stop_sys_timer(void);
#endif