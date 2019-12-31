#ifndef _MYADC_H
#define _MYADC_H

enum
{
    LEFT_ADC            = 0,
    MID_LEFT_ADC        = 1,
    MID_ADC             = 2,
    MID_RIGHT_ADC       = 3,
    RIGHT_ADC           = 4   
};

#define LEFT_ADC_CH         ADC0_SE2
#define RIGHT_ADC_CH        ADC0_SE7
#define MID_ADC_CH          ADC0_SE4
#define MID_LEFT_ADC_CH     ADC0_SE3
#define MID_RIGHT_ADC_CH    ADC0_SE6

extern int16 gy_ad_val[5];
extern int16 ad_error;
extern int16 ad_err_ring;
extern uint16 position;

void AD_get(); //ªÒ»°AD÷µ
void Data_analyse();

#endif