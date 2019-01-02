#ifndef ___ALARM_H____
#define ___ALARM_H____
void start_alarm(void);
void stop_alarm(void);
void set_HZ(float HZ);
void set_duty(char duty);
 
void start_breath_blink();
void start_alarm_blink(); 
void close_rgb_led();
extern u8   g_led1_counter, g_led2_counter, g_led3_counter;
extern u8   g_led1_duty,    g_led2_duty   , g_led3_duty;

#endif
