/**************************************
--- STC MCU Limited -------------------
--- 宏晶科技        设计 2010-6-2 -----
--- Mobile: 13922805190 ---------------
--- Fax: 0755-82944243 ----------------
--- Tel: 0755-82948412 ----------------
--- Web: www.STCMCU.com ---------------
--- 演示STC 12TMCU控制DS1302 ----------
--- 工作频率: 12MHz -------------------
如果要在程序中使用或在文章中引用该程序
请在其中注明使用了宏晶科技的资料及程序
**************************************/
#include "../h/port.h"
#include "../h/serial.h"
//sbit g_1302_vcc = P3^1;
sbit SCLK = P1^3;                   //DS1302时钟口P1.0
sbit IO   = P1^4;                     //DS1302数据口P1.1
sbit RST   =P1^5;                    //DS1302片选口P1.2
  
void Delayxx()
{
    _nop_();  //  _nop_(); _nop_();
}

/**************************************
从DS1302读1字节数据
**************************************/
uchar DS1302_Readuchar()
{
    uchar i = 0;
    uchar dat = 0;
    for (i=0; i<8; i++)             //8位计数器
    {
        SCLK = 0;                   //时钟线拉低
        Delayxx();                    //延时等待

        dat >>= 1;                  //数据右移一位
        if (IO) dat |= 0x80;        //读取数据
        SCLK = 1;                   //时钟线拉高
        Delayxx();                    //?óê±μè′y
    }
    return dat;
}

/**************************************
向DS1302写1字节数据
**************************************/
void DS1302_Writeuchar(uchar dat)
{
    char i = 0;
    for (i=0; i<8; i++)             //8位计数器
    {
        SCLK = 0;                   //时钟线拉低
        Delayxx();                    //延时等待
        dat >>= 1;                  //移出数据
        IO = CY;                    //送出到端口
        SCLK = 1;                   //时钟线拉高
        Delayxx();                    //延时等待
    }
}

/**************************************
读DS1302某地址的的数据
**************************************/
uchar DS1302_ReadData(uchar addr)
{
    uchar dat;
    RST = 0;
    Delayxx();                        //延时等待
    SCLK = 0;
    Delayxx();                        //延时等待
    //_nop_();
    RST = 1;
    Delayxx();                        //延时等待
    //_nop_();
    DS1302_Writeuchar(addr);         //写地址
    dat = DS1302_Readuchar();        //读数据
    SCLK = 1;
    RST = 0;
    return dat;
}

/**************************************
往DS1302的某个地址写入数据
**************************************/
void DS1302_WriteData(uchar addr, uchar dat)
{
    RST = 0;
    Delayxx();                        //延时等待
    //_nop_();
    SCLK = 0;
    Delayxx();                        //延时等待
    //_nop_();
    RST = 1;
    Delayxx();                        //延时等待
    //_nop_();
    DS1302_Writeuchar(addr);         //写地址
    DS1302_Writeuchar(dat);          //写数据
    SCLK = 1;
    RST = 0;
}

/**************************************
写入初始时间
**************************************/

/**************************************
读取当前s时间
**************************************/
#ifdef TEST
uchar ds1302_read_sec( )
{
    uchar sec;// = 0;// 
    sec =  DS1302_ReadData(0x83);
    send_integ((sec>>4)*10+  (sec&0xF));
    send_char(':');     
    sec = DS1302_ReadData(0x81);
    send_integ((sec>>4)*10+  (sec&0xF));
    return sec;//( DS1302_ReadData(0x81));
}
#endif
void ds1302_stop()
{
    //DS1302_WriteData(0x8e, 0x00);   //允许写操作
    //DS1302_WriteData(0x80, 0x80);
    //DS1302_WriteData(0x8e, 0x80);   //写保护
    //g_1302_vcc = 0;
}  
uchar volatile g_ds1302_1s = 0;
void ds1302_start()
{   
  //  g_1302_vcc = 1;
    g_ds1302_1s = 0;
    DS1302_WriteData(0x8e, 0x00);   //允许写操作
    DS1302_WriteData(0x80, 0x01);
    DS1302_WriteData(0x8e, 0x80);   //写保护
}

char ds1302_1s_trigger()
{
    uchar x=0;
    x =  DS1302_ReadData(0x81); 
    x =  /*((x)>>4)*10 + */(x&0xF);
    if(g_ds1302_1s != x)
    {    
        g_ds1302_1s = x;
        return 1;
    }
    else
    {
        send_char('x');//send_integ(x);
        return 0;
    }
}

#ifdef TEST
                    //秒    分    时    日    月  星期    年
uchar code init[] = {0x80, 0x00, 0x20, 0x01, 0x01, 0x05, 0x10};

void DS1302_SetTime(uchar *p)
{
    uchar addr = 0x80;
    uchar n = 7;

    DS1302_WriteData(0x8e, 0x00);   //允许写操作
    while (n--)
    {
        DS1302_WriteData(addr, *p++);
        addr += 2;    
    }
    DS1302_WriteData(0x8e, 0x80);   //写保护
}
#endif
/**************************************
初始化DS1302
**************************************/
void ds1302_init()
{
    RST = 0;
    SCLK = 0;                      
    //DS1302_WriteData(0x8e, 0x00);   //允许写操作
    //DS1302_WriteData(0x80, 55);     //时钟启动
    //DS1302_WriteData(0x90, 0xa6);   //一个二极管＋4K电阻充电
    //DS1302_WriteData(0x8e, 0x80);   //写保护
    //DS1302_Initial();               //初始化DS1302
#ifdef TEST
    DS1302_SetTime(init);           //设置初始时间
#endif
}
