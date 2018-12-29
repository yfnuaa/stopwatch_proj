/**************************************
--- STC MCU Limited -------------------
--- �꾧�Ƽ�        ��� 2010-6-2 -----
--- Mobile: 13922805190 ---------------
--- Fax: 0755-82944243 ----------------
--- Tel: 0755-82948412 ----------------
--- Web: www.STCMCU.com ---------------
--- ��ʾSTC 12TMCU����DS1302 ----------
--- ����Ƶ��: 12MHz -------------------
���Ҫ�ڳ�����ʹ�û������������øó���
��������ע��ʹ���˺꾧�Ƽ������ϼ�����
**************************************/
#include "../h/port.h"
#include "../h/serial.h"
//sbit g_1302_vcc = P3^1;
sbit SCLK = P1^3;                   //DS1302ʱ�ӿ�P1.0
sbit IO   = P1^4;                     //DS1302���ݿ�P1.1
sbit RST   =P1^5;                    //DS1302Ƭѡ��P1.2
  
void Delayxx()
{
    _nop_();  //  _nop_(); _nop_();
}

/**************************************
��DS1302��1�ֽ�����
**************************************/
uchar DS1302_Readuchar()
{
    uchar i = 0;
    uchar dat = 0;
    for (i=0; i<8; i++)             //8λ������
    {
        SCLK = 0;                   //ʱ��������
        Delayxx();                    //��ʱ�ȴ�

        dat >>= 1;                  //��������һλ
        if (IO) dat |= 0x80;        //��ȡ����
        SCLK = 1;                   //ʱ��������
        Delayxx();                    //?�������̨���y
    }
    return dat;
}

/**************************************
��DS1302д1�ֽ�����
**************************************/
void DS1302_Writeuchar(uchar dat)
{
    char i = 0;
    for (i=0; i<8; i++)             //8λ������
    {
        SCLK = 0;                   //ʱ��������
        Delayxx();                    //��ʱ�ȴ�
        dat >>= 1;                  //�Ƴ�����
        IO = CY;                    //�ͳ����˿�
        SCLK = 1;                   //ʱ��������
        Delayxx();                    //��ʱ�ȴ�
    }
}

/**************************************
��DS1302ĳ��ַ�ĵ�����
**************************************/
uchar DS1302_ReadData(uchar addr)
{
    uchar dat;
    RST = 0;
    Delayxx();                        //��ʱ�ȴ�
    SCLK = 0;
    Delayxx();                        //��ʱ�ȴ�
    //_nop_();
    RST = 1;
    Delayxx();                        //��ʱ�ȴ�
    //_nop_();
    DS1302_Writeuchar(addr);         //д��ַ
    dat = DS1302_Readuchar();        //������
    SCLK = 1;
    RST = 0;
    return dat;
}

/**************************************
��DS1302��ĳ����ַд������
**************************************/
void DS1302_WriteData(uchar addr, uchar dat)
{
    RST = 0;
    Delayxx();                        //��ʱ�ȴ�
    //_nop_();
    SCLK = 0;
    Delayxx();                        //��ʱ�ȴ�
    //_nop_();
    RST = 1;
    Delayxx();                        //��ʱ�ȴ�
    //_nop_();
    DS1302_Writeuchar(addr);         //д��ַ
    DS1302_Writeuchar(dat);          //д����
    SCLK = 1;
    RST = 0;
}

/**************************************
д���ʼʱ��
**************************************/

/**************************************
��ȡ��ǰsʱ��
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
    //DS1302_WriteData(0x8e, 0x00);   //����д����
    //DS1302_WriteData(0x80, 0x80);
    //DS1302_WriteData(0x8e, 0x80);   //д����
    //g_1302_vcc = 0;
}  
uchar volatile g_ds1302_1s = 0;
void ds1302_start()
{   
  //  g_1302_vcc = 1;
    g_ds1302_1s = 0;
    DS1302_WriteData(0x8e, 0x00);   //����д����
    DS1302_WriteData(0x80, 0x01);
    DS1302_WriteData(0x8e, 0x80);   //д����
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
                    //��    ��    ʱ    ��    ��  ����    ��
uchar code init[] = {0x80, 0x00, 0x20, 0x01, 0x01, 0x05, 0x10};

void DS1302_SetTime(uchar *p)
{
    uchar addr = 0x80;
    uchar n = 7;

    DS1302_WriteData(0x8e, 0x00);   //����д����
    while (n--)
    {
        DS1302_WriteData(addr, *p++);
        addr += 2;    
    }
    DS1302_WriteData(0x8e, 0x80);   //д����
}
#endif
/**************************************
��ʼ��DS1302
**************************************/
void ds1302_init()
{
    RST = 0;
    SCLK = 0;                      
    //DS1302_WriteData(0x8e, 0x00);   //����д����
    //DS1302_WriteData(0x80, 55);     //ʱ������
    //DS1302_WriteData(0x90, 0xa6);   //һ�������ܣ�4K������
    //DS1302_WriteData(0x8e, 0x80);   //д����
    //DS1302_Initial();               //��ʼ��DS1302
#ifdef TEST
    DS1302_SetTime(init);           //���ó�ʼʱ��
#endif
}