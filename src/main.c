#include "headfile.h"

void Oled_refresh(void);
void PrintData(void);

extern uint8 show_flag, change_flag, change_done_flag, clear_flag;
extern Road_Mode roadmode;
uint16 test_ad_max = 1500, test_ad_min = 1500, test_ad_now;
uint8 ADdata[23]={0xfe,0xfd,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xfc};

//values i wanna change
uint16 high_speed_time_cnt = 100;
uint8  road_mode_switch;

int main(void)
{   
    Bsp_Init();		
    chassis_param_init();
    
    while(1)
    {    
        if(show_flag == 1)
            Oled_refresh();
        else
        {
            if(clear_flag == 1)
            {
                clear_flag = 0;
                OLED_Fill(0x00);
            }
        }
        
        switch(change_flag)
        {
            case 1:
                break;
            case 2:
                break;
            default:
                break;
        }
        systick_delay_ms(200);
    }
}

void Oled_refresh(void)
{
    OLED_Print_Num1(0, 2, test_ad_max);
    OLED_Print_Num1(40,2, test_ad_now);
    OLED_Print_Num1(80,2, test_ad_min);
    
    OLED_Print_Num1(0, 3, change_flag);
    OLED_Print_Num1(0, 4, road_mode_switch);
    OLED_Print_Num1(40, 4, high_speed_time_cnt);
    
    OLED_Print_Num1(0, 5, ad_error);
    OLED_Print_Num1(40,5, ad_err_ring); 
    
    OLED_Print_Num1(0, 6,gy_ad_val[0]);
    OLED_Print_Num1(40,6,gy_ad_val[2]);
    OLED_Print_Num1(80,6,gy_ad_val[4]);
    OLED_Print_Num1(20,7,gy_ad_val[1]);
    OLED_Print_Num1(60,7,gy_ad_val[3]);    
}

void PrintData(void)
{
    ADdata[2]=(uint8)(gy_ad_val[0]>>8);
    ADdata[3]=(uint8)gy_ad_val[0];
    ADdata[4]=(uint8)(gy_ad_val[3]>>8);
    ADdata[5]=(uint8)gy_ad_val[3];
    ADdata[6]=(uint8)(gy_ad_val[1]>>8);
    ADdata[7]=(uint8)gy_ad_val[1];
    ADdata[8]=(uint8)(gy_ad_val[4]>>8);
    ADdata[9]=(uint8)gy_ad_val[4];
    ADdata[10]=(uint8)(gy_ad_val[2]>>8);
    ADdata[11]=(uint8)gy_ad_val[2];
    uart_putbuff (uart0, &ADdata[0], sizeof(ADdata));
}
