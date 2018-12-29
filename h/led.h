#ifndef _LED_H_HH_
#define _LED_H_HH_
void init_display(void);
void close_display(void);
void flash_display(void);
void black_display(void);
void update_min(uchar min);
void update_sec(uchar sec);
extern bit g_blink_flag;
extern uchar g_battery_display ;
#endif