#include "headfile.h"

void Bsp_Init(void)
{
    get_clk();              //��ȡʱ��Ƶ�� ����ִ��
    DisableInterrupts;      //��ֹ���ж�
    
    //------------OLED��ʼ�� --------------
    OLED_Init();
    
    //------------���ڳ�ʼ�� --------------
    //uart_init(uart0, 57600);
   
    uart_init(uart1, 115200);
    set_irq_priority(UART1_IRQn, 2);
    uart_rx_irq_en(uart1);
    
    //------------��ʱ����ʼ�� --------------
    pit_init_ms(pit0, 5);                // ��ʱ��0�ж� 3ms
    set_irq_priority(PIT_CH0_IRQn, 0);	 //����pit0���ȼ�
    disable_irq(PIT_CH0_IRQn);           //pit0�ж�

    pit_init_ms(pit1, 100);                // ��ʱ��1�ж� 100ms
    set_irq_priority(PIT_CH1_IRQn, 1);	 //����pit1���ȼ�
    disable_irq(PIT_CH1_IRQn);           //pit1�ж�

    //------------������ʼ�� --------------
    kbi_init(KBI0_P12, IRQ_FALLING);
    kbi_init(KBI0_P13, IRQ_FALLING);
    kbi_init(KBI0_P19, IRQ_FALLING);
    set_irq_priority(KBI0_IRQn,2);	//�������ȼ�,�����Լ����������� �����÷�ΧΪ 0 - 3
    enable_irq(KBI0_IRQn);
    
    kbi_init(KBI1_P5, IRQ_FALLING);		            
    set_irq_priority(KBI1_IRQn,2);	//�������ȼ�,�����Լ����������� �����÷�ΧΪ 0 - 3
    enable_irq(KBI1_IRQn);
    
    //------------ADC��ʼ�� --------------
    AD_Init();
    
    //------------���PWM��ʼ�� --------------
    gpio_init(H7,GPO,1);//MOTO_EN right
    gpio_init(G1,GPO,1);//RSW2?  left
    PWM_Init();
    
    //------------LED�����ʼ�� --------------
    gpio_init(C6,GPO,1);//RED  --led1
    gpio_init(E2,GPO,1);//BLUE --led3
    
    //------------�������ٶȶ�ȡ --------------
//    gpio_init(E0, GPI, 1);   //����ת�������°��Ϊ A0
//    gpio_init(E7, GPI, 1);   //����ת������

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
    ftm_count_init(ftm0);  //��E0���������������м���
    ftm_count_init(ftm1);  //��E7���������������м���
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
