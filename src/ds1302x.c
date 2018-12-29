#include "stc8.h"
#include "intrins.h" 
void ds1302_delay( void )
{
    int i;
    //for(i = 0; i < 10; i++)
    {
        _nop_();
    }
}
sbit g_SCL  = P4^7;

sbit g_data = P5^4;
sbit g_RST  = P5^5;
//sbit g_SCL  = P1^0;

//sbit g_data = P1^1;
//sbit g_RST  = P1^2;

void ds1302_writeByte(unsigned char mCommand, unsigned char mData);
void ds1302_init()
{	 
   // g_SCL=0; g_RST=0	 ;g_data=0;
    ds1302_writeByte(0x8e,0x00); 
     
} 
/************************************************************************
**函数名：void ds1302_writeByte(unsigned char mCommand, unsigned char mData)
**  功能：往DS1302写入一个字节
**  参数: unsigned char mCommand, unsigned char mData
*************************************************************************/
void ds1302_writeByte(unsigned char mCommand, unsigned char mData)
{
    int loop;
    g_RST = 0;
    g_SCL = 0;
    ds1302_delay();
    g_RST = 1;
    
    for(loop = 0; loop < 8; loop++)
    {
        g_data = mCommand &0x01;
        ds1302_delay();
        g_SCL = 1;
        ds1302_delay();
		mCommand>>=1;
        g_SCL = 0; 
	

    }
    for(loop = 0; loop < 8; loop++)
    {
        g_data = (mData>>loop)&0x01;
      	
        g_SCL = 1;
       	ds1302_delay();
        g_SCL = 0;
		ds1302_delay();
                              
    }
	g_SCL=0;
    g_RST = 0; //拉低停止数据传输                                    

} 
/************************************************************************
**函数名：unsigned char ds1302_readByte(unsigned char mCommand)
**  功能：从DS1302读取一个字节
**  参数: unsigned char mCommand
************************************************************************ */
unsigned char ds1302_readByte(unsigned char address)
{
    unsigned char loop = 0;
    unsigned char readData = 0;
    unsigned char temp = 0;
	g_RST = 0; 
	g_SCL = 0; 
	ds1302_delay();
    g_RST = 1;
	ds1302_delay();
  
	for(loop = 0; loop < 8; loop++)
    {
        g_data = address &0x01;
       	  ds1302_delay();
        g_SCL = 1;
       ds1302_delay();
	   g_data = 0;
		address>>=1;
        g_SCL = 0; 
	
       
    }
    for(loop = 0; loop < 8; loop++)
    {	
        g_SCL = 1;
		ds1302_delay();
        g_SCL = 0;
		if(g_data)
            readData|=(1>>loop); 
		 ds1302_delay();

    }
	g_SCL=0;
    g_RST = 0; //拉低停止数据传输
    return readData;                                   
}
/* bit7   6   5   4   3   2   1   0             */
/*  | -- 1  pause  0 start					    */
/*        |------|-->   10s					    */
/*	                  |------------|-> 1s       */
 
unsigned char ds1302_read_sec()
{
    unsigned char sec;
	sec =  ds1302_readByte(0x81); 
	return (sec & 0xFF);
}
void ds1302_stop()
{
    ds1302_writeByte(0x80, 0x80);  //sec
}
char g_ds1302_1s = 0;
void ds1302_start()
{
    g_ds1302_1s = 0;
    ds1302_writeByte(0x80, 0x03);  //sec
}

char ds1302_1s_trigger()
{
    unsigned char newsec;
	newsec =  ds1302_readByte(0x81); 
	newsec = /*((newsec & 0x70)>>4) * 10 +*/ newsec & 0xF;
	if(g_ds1302_1s != newsec)
	{
	    return 1;
	}
	else return 0;
}

#if 0
void ds1302_setTime(struct ALLDATE allDate)
{
    unsigned char loop;
    unsigned char *pallDate = (unsigned char *)&allDate;
    allDate = convertToSetTime(allDate);
    ds1302_writeByte(0x8e, 0x00);  //control为的最高位wp
    for(loop = 0; loop <= 6; loop++)
    {
        ds1302_writeByte(0x8c-(loop<<1), *(pallDate+loop));  //sec                  

    } 
    /*ds1302_writeByte(0x8e, 0x00);  //control为的最高位wp
    ds1302_writeByte(0x8c, allDate.yd.year);  //year
    ds1302_writeByte(0x8a, allDate.yd.day);  //day
    ds1302_writeByte(0x88, allDate.md.month);  //month
    ds1302_writeByte(0x86, allDate.md.date);  //date
    ds1302_writeByte(0x84, allDate.hms.hour);  //hour
    ds1302_writeByte(0x82, allDate.hms.min);  //min
    ds1302_writeByte(0x80, allDate.hms.sec);  //sec            */
}
    allDate.hms.min =  ds1302_readByte(0x83);  //min
    allDate.hms.sec =  ds1302_readByte(0x81);  //sec  */
#endif
