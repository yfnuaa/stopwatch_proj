#ifndef __KEYPROCESS_HH_H
#define __KEYPROCESS_HH_H

extern bit g_key_released;
extern bit g_key_pressed;
extern uchar volatile g_key_pressing_1s;

#ifdef KEY_INTERRUPT_
void init_key();
#endif

void get_key_state(void);
void process_key();

#endif