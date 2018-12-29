#define CPU_HZ 11059200;
#define g_duoji_c P1_0;
char g_arch = 0; //0-180;
char g_runing_arch = -1;
int g_us_ticks=0;
void set_arch(char arch_)
{
    if(arch_>180 || arch_<0)return;
    g_arch = arch_;
}
void init_duoji()
{
    us_ticks = CPU_HZ/12;
    g_duoji_c = 0;
    TMOD=0x01; //���ö�ʱ��0������ʽ1��������TRλ����
    TH0=(65536-1)/256; 
    TL0=(65536-1)%256; 
    EA=1; //�����ж� 
    ET0=1; //����ʱ��0�ж� 
    TR0=1; //������ʱ��0 
}
int g_pwm_hi_ticks = 0;
void PWM1() interrupt 1 //��ʱ��0 �жϣ��������� 
{ 
    int c_20ms_ticks= g_us_ticks*20000;
    if(0 == g_duoji_c && g_runing_arch != g_arch)
    {
        g_pwm_hi_ticks = 500 + g_arch*1000/90��
        g_pwm_hi_ticks = 65535 - g_pwm_hi_ticks;
  g_runing_arch = g_arch;
    }
 
    if(g_duoji_c == 0) //����ϸ���ƽΪ0�����¸��ߵ�ƽ��ʱ��Ϊ  
    { 
        TH0=(g_pwm_hi_ticks)/256; 
        TL0=(g_pwm_hi_ticks)%256; 
    } 
    else //����ϸ���ƽΪ0�����¸��ߵ�ƽΪ  
    { 
        TH0=(65535-(c_20ms_ticks-65535+g_pwm_hi_ticks))/256; 
        TL0=(65535-(c_20ms_ticks-65535+g_pwm_hi_ticks))%256; 
    } 
    g_duoji_c=~g_duoji_c; 
} 