#include "headfile.h"

void Bsp_Init(void)
{
    get_clk();              //获取时钟频率 必须执行
    DisableInterrupts;      //禁止总中断
    
    //------------OLED初始化 --------------
    OLED_Init();
    
    //------------串口初始化 --------------
    //uart_init(uart0, 57600);
   
    uart_init(uart1, 115200);
    set_irq_priority(UART1_IRQn, 2);
    uart_rx_irq_en(uart1);
    
    //------------定时器初始化 --------------
    pit_init_ms(pit0, 5);                // 定时器0中断 3ms
    set_irq_priority(PIT_CH0_IRQn, 0);	 //设置pit0优先级
    disable_irq(PIT_CH0_IRQn);           //pit0中断

    pit_init_ms(pit1, 100);                // 定时器1中断 100ms
    set_irq_priority(PIT_CH1_IRQn, 1);	 //设置pit1优先级
    disable_irq(PIT_CH1_IRQn);           //pit1中断

    //------------按键初始化 --------------
    kbi_init(KBI0_P12, IRQ_FALLING);
    kbi_init(KBI0_P13, IRQ_FALLING);
    kbi_init(KBI0_P19, IRQ_FALLING);
    set_irq_priority(KBI0_IRQn,2);	//设置优先级,根据自己的需求设置 可设置范围为 0 - 3
    enable_irq(KBI0_IRQn);
    
    kbi_init(KBI1_P5, IRQ_FALLING);		            
    set_irq_priority(KBI1_IRQn,2);	//设置优先级,根据自己的需求设置 可设置范围为 0 - 3
    enable_irq(KBI1_IRQn);
    
    //------------ADC初始化 --------------
    AD_Init();
    
    //------------电机PWM初始化 --------------
    gpio_init(H7,GPO,1);//MOTO_EN right
    gpio_init(G1,GPO,1);//RSW2?  left
    PWM_Init();
    
    //------------LED灯组初始化 --------------
    gpio_init(C6,GPO,1);//RED  --led1
    gpio_init(E2,GPO,1);//BLUE --led3
    
    //------------编码盘速度读取 --------------
//    gpio_init(E0, GPI, 1);   //车轮转动方向，新板改为 A0
//    gpio_init(E7, GPI, 1);   //车轮转动方向

    EnableInterrupts;
}

void PWM_Init()
{  
    ftm_pwm_init(ftm2, ftm_ch0, 8000, 0);  
    ftm_pwm_init(ftm2, ftm_ch1, 8000, 0);  
    ftm_pwm_init(ftm2, ftm_ch2, 8000, 0);  
    ftm_pwm_init(ftm2, ftm_ch3, 8000, 0);  
}

void FTM_Init()
{   
    ftm_count_init(ftm0);  //对E0引脚输入的脉冲进行计数
    ftm_count_init(ftm1);  //对E7引脚输入的脉冲进行计数
}

void AD_Init()
{  
    adc_init(ADC0_SE2);  
    adc_init(ADC0_SE3);  
    //adc_init(ADC0_SE4);  
    adc_init(ADC0_SE4); 
    adc_init(ADC0_SE6);  
    adc_init(ADC0_SE7);   
}
