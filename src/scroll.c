#include "../h/port.h"
#include "../h/scroll.h"
#include "../h/utility.h"
#include "../h/serial.h"

sbit PLUS_0_A_IN = P1^1;
sbit PLUS_0_B_IN = P1^2;
char g_plus_status  [e_roll_max] = {e_stop};
char g_plus_sub_status  [e_roll_max] = {e_stop};

char g_plus_a_in    [e_roll_max] = {high};
char g_plus_a_in_old[e_roll_max] = {high};
char g_plus_b_in    [e_roll_max] = {high};
char g_plus_b_in_old[e_roll_max] = {high};
char g_plus_count[ e_roll_max] = {0};    
#if 1                                                                            
uchar g_juggle_delay = 0;
void init_scroll()
{
    g_plus_a_in[0] = high;
    g_plus_b_in[0] = high;
    g_plus_a_in_old[0] =high;
    g_plus_b_in_old[0] = high;
    g_plus_status  [0] = e_stop;
	g_juggle_delay = 0;
	g_plus_count[0] = 0;
}
void scan_roll(char roll_index)
{
    if(g_juggle_delay == 0)
    {   
        if(e_roll_a == roll_index)
        {
            g_plus_a_in[e_roll_a] = PLUS_0_A_IN;
            g_plus_b_in[e_roll_a] = PLUS_0_B_IN;
        }

        if( g_plus_a_in_old[roll_index] != g_plus_a_in[roll_index] || g_plus_b_in_old[roll_index] != g_plus_b_in[roll_index])
        {
            g_juggle_delay = 2;// filter juggle
        }
		return;
    }
    else if(g_juggle_delay>1)
    {
        g_juggle_delay -- ;
        return;
    }
    g_juggle_delay = 0;
    if(e_roll_a == roll_index)
    {
        g_plus_a_in[e_roll_a] = PLUS_0_A_IN;
        g_plus_b_in[e_roll_a] = PLUS_0_B_IN;
    }

    if(high == g_plus_b_in[roll_index] && high == g_plus_a_in[roll_index])
    {
        g_plus_status[roll_index] = e_stop;
    } 
    switch(g_plus_status[roll_index])
    {
        case e_stop:
            if(low == g_plus_a_in[roll_index] && high == g_plus_a_in_old[roll_index] && high == g_plus_b_in[roll_index])
            {
                g_plus_status[roll_index] = e_clockwise;
            }       
            else if(low == g_plus_b_in[roll_index] && high == g_plus_b_in_old[roll_index] && high == g_plus_a_in[roll_index])
            {
                g_plus_status[roll_index] = e_anti_clockwise;
            }
        
        break;
        case e_clockwise:
            if(high == g_plus_a_in[roll_index] && low == g_plus_a_in_old[roll_index] && low == g_plus_b_in[roll_index])
            {
                g_plus_count[roll_index]++;
                g_plus_status[roll_index] = e_wait_stop; 
            }       
        break;
        case e_anti_clockwise:
            if(high == g_plus_b_in[roll_index] && low == g_plus_b_in_old[roll_index] && low == g_plus_a_in[roll_index])
            {
                g_plus_count[roll_index]--;
                g_plus_status[roll_index] = e_wait_stop; 
            } 
        break;
        case e_wait_stop:
            if(high == g_plus_b_in[roll_index] && high == g_plus_a_in[roll_index])
            {
                g_plus_status[roll_index] = e_stop;
            }
        break;
    }
    g_plus_a_in_old[roll_index] = g_plus_a_in[roll_index];
    g_plus_b_in_old[roll_index] = g_plus_b_in[roll_index];
}
#else
void init_scroll()
{
     g_plus_a_in[0] = low;
        g_plus_b_in[0] = low;
		g_plus_a_in_old[0] =low;
    g_plus_b_in_old[0] = low;
	g_plus_status  [0] = e_stop;
}
void scan_roll(char roll_index)
{
     if(e_roll_a == roll_index)
    {
        g_plus_a_in[e_roll_a] = PLUS_0_A_IN;
        g_plus_b_in[e_roll_a] = PLUS_0_B_IN;
    }
    if( g_plus_a_in_old[roll_index] != g_plus_a_in[roll_index] && g_plus_b_in_old[roll_index] != g_plus_b_in[roll_index])
    {
        _nop_();
        if(e_roll_a == roll_index)
        {
            g_plus_a_in[e_roll_a] = PLUS_0_A_IN;
            g_plus_b_in[e_roll_a] = PLUS_0_B_IN;
        }
 
  
    }
    switch(g_plus_status[roll_index])
    {
        case e_stop:
            if(high == g_plus_a_in[roll_index] && low == g_plus_a_in_old[roll_index])
            {
                if(low == g_plus_b_in[roll_index])
                {
                    g_plus_status[roll_index] = e_clockwise;
                     
                    g_plus_sub_status[roll_index] = e_pre_count;
                }
                else
                {
                    g_plus_status[roll_index] = e_anti_clockwise;
                    
                    g_plus_sub_status[roll_index] = e_pre_count;
                }
            }
			#if 1
			else if(high == g_plus_b_in[roll_index] && high == g_plus_a_in[roll_index])
			{
                g_plus_status[roll_index] = e_anti_clockwise;
                g_plus_sub_status[roll_index] = e_pre_count;
            }
			#endif
        break;
        case e_clockwise:
            switch(g_plus_sub_status[roll_index])
            {
                case e_pre_count:
                if(high == g_plus_a_in[roll_index] && high == g_plus_b_in[roll_index] )
                {
                    g_plus_count[roll_index]++;     
                    g_plus_sub_status[roll_index] = e_count;  
                }
                else if(high == g_plus_a_in[roll_index] && low == g_plus_b_in[roll_index] )
                {							   //wait...
                }
                else
                    g_plus_status[roll_index] = e_stop;        
                break;
                
                case e_count:
			//	send_buffer("{");send_integ(g_plus_a_in[roll_index]); send_integ(g_plus_b_in[roll_index]); send_buffer("}");
                //if(low == g_plus_a_in[roll_index] && low == g_plus_b_in[roll_index] )
                {
				    
                    g_plus_status[roll_index] = e_stop; 
                }
                 
                 
                break;
              }
            break;
        case e_anti_clockwise:
            switch(g_plus_sub_status[roll_index])
            {
               
                case e_pre_count:
                    if(high == g_plus_a_in[roll_index] && low == g_plus_b_in[roll_index] )
                    {
                        g_plus_count[roll_index] -- ;
                        g_plus_sub_status[roll_index] = e_count;  
                    }
                    else if(high == g_plus_a_in[roll_index] && high == g_plus_b_in[roll_index] )
                    {
					    //wait...
                    }
                    else
                    {
                        g_plus_status[roll_index] = e_stop;     
                    }                    
                    break;
                    
                case e_count:
				 //   send_buffer("{");send_integ(g_plus_a_in[roll_index]); send_integ(g_plus_b_in[roll_index]); send_buffer("}");
                    if(low == g_plus_a_in[roll_index] && low == g_plus_b_in[roll_index] )
                    {
                         g_plus_status[roll_index] = e_stop; 
                    }
                                    
                    break;
            }
            break;
        default:
        break;
    }    
    g_plus_a_in_old[roll_index] = g_plus_a_in[roll_index];
    g_plus_b_in_old[roll_index] = g_plus_b_in[roll_index];

}
#endif