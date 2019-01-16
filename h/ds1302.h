#ifndef __DS1302_H___
#define __DS1302_H___
    unsigned char ds1302_read_sec();
    void ds1302_stop();
	void ds1302_shutdown();

    void ds1302_start();
    char ds1302_1s_trigger();
    void ds1302_init();
#endif