#ifndef _LOW_VOLTAGE_H__H
#define _LOW_VOLTAGE_H__H
extern bit volatile g_low_voltage; 
void init_low_power_inter();
char get_voltage();
void enable_voltage_read_init();
void disable_voltage_read();
#endif