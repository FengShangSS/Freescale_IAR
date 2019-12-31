#include "isr.h"

uint8 head[2];
uint8 body[22];
uint8 getHeadFlag;
uint8 idx;
int testv;

uint8 pit_ch0_count, pit_ch0_ledcnt;
uint8 pit_ch1_count;
uint8 high_speed_flag;

uint8 show_flag;
uint8 change_flag, change_done_flag;
uint8 clear_flag;
extern uint16 high_speed_time_cnt;
extern uint8  road_mode_switch;

void PIT_CH0_IRQHandler(void)//5ms interrupt
{
    PIT_FlAG_CLR(pit0);
    
    pit_ch0_count++;
    if(pit_ch0_count == 1)      //状态处理
    { 
        AD_get();
        Data_analyse();
    }
    else if(pit_ch0_count > 1)  //方向处理  
    { 
        if(show_flag == 0)
            chassis_task();
        pit_ch0_count = 0;
    }
    
    if(pit_ch0_ledcnt++ == 100)
    {
        pit_ch0_ledcnt = 0;
        //RED_LED_TOGGLE;
    }
    
}


void PIT_CH1_IRQHandler(void)
{
    PIT_FlAG_CLR(pit1);
    if(pit_ch1_count++ == 100)
    {
        pit_ch1_count = 0;
        high_speed_flag = 0;
        disable_irq(PIT_CH1_IRQn);
    }
}

void IRQ_IRQHandler(void)
{
    CLEAR_IRQ_FLAG;
}

// C3 B4 B5 E5
uint32 kbi_flag;

void KBI0_IRQHandler(void)
{
    if(show_flag == 1)
    {
        kbi_flag = READ_KBI0_FLAG;
        if(kbi_flag == 0x2000)  //key_left
        {
            if(++change_flag == 3)
            {
                change_flag = 0;
            }
        }
        else if(kbi_flag == 0x1000) //key_down
        {
            switch(change_flag)
            {
                case 1:
                    road_mode_switch = 1;
                    change_done_flag = 1;
                    break;
                case 2:
                    if(high_speed_time_cnt >= 10)
                    {
                        high_speed_time_cnt -= 10;
                        change_done_flag = 1;
                    }
                    break;
                default:
                    break;
            }
        }
        else if(kbi_flag == 0x80000) //key_up
        {
            switch(change_flag)
            {
                case 1:
                    road_mode_switch = 2;
                    change_done_flag = 1;
                    break;
                case 2:
                    if(high_speed_time_cnt <= 300)
                    {
                        high_speed_time_cnt += 10;
                        change_done_flag = 1;
                    }
                    break;
                default:
                    break;
            }
        }
        CLEAN_KBI0_FLAG;  
    }
}

void KBI1_IRQHandler(void)
{
    kbi_flag = READ_KBI1_FLAG;
    if(kbi_flag == 0x20)  //key_right
    {
        if(show_flag == 0)
        {
            show_flag = 1;
            gpio_set(C6,0);
        }
        else if(show_flag == 1)
        {
            show_flag = 0;
            gpio_set(C6,1);
            clear_flag = 1;
        }
    }
    CLEAN_KBI1_FLAG;  
}

void UART1_IRQHandler(void)
{
    uint8 ch;
    ch = uart_getchar(uart1);
    head[1] = ch;
    if((head[1] == 0xfd) && (head[0] == 0xfe))
    {
        getHeadFlag = 1;
        idx = 0;
    }
    if(getHeadFlag)
    {
        body[idx] = head[1];
        if((idx == 2) && (body[idx] == 0xfc))
        {
            //得到发车指令
            if(body[1] == 1)
            {
               testv = (int)body[1];
               enable_irq(PIT_CH0_IRQn);
               uart_rx_irq_dis(uart1);
            }
        }
        idx++;
    }
    head[0] = head[1];
}



/*
中断函数名称，用于设置对应功能的中断函数
Sample usage:当前启用了周期定时器 通道0得中断
void PIT_CH0_IRQHandler(void)
{
    ;
}
记得进入中断后清除标志位

FTMRE_IRQHandler      
PMC_IRQHandler        
IRQ_IRQHandler        
I2C0_IRQHandler       
I2C1_IRQHandler       
SPI0_IRQHandler       
SPI1_IRQHandler       
UART0_IRQHandler 
UART1_IRQHandler 
UART2_IRQHandler 
ADC0_IRQHandler       
ACMP0_IRQHandler      
FTM0_IRQHandler       
FTM1_IRQHandler       
FTM2_IRQHandler       
RTC_IRQHandler        
ACMP1_IRQHandler      
PIT_CH0_IRQHandler    
PIT_CH1_IRQHandler    
KBI0_IRQHandler       
KBI1_IRQHandler       
Reserved26_IRQHandler 
ICS_IRQHandler        
WDG_IRQHandler        
PWT_IRQHandler        
MSCAN_Rx_IRQHandler   
MSCAN_Tx_IRQHandler   
*/



