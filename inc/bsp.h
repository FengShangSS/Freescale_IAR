#ifndef _BSP_H
#define _BSP_H

#define RED_LED_TOGGLE  gpio_turn(C6)
#define BLUE_LED_TOGGLE  gpio_turn(E2)

#define RIGHT_PWM_EN    gpio_set(H7,1)
#define RIGHT_PWM_NO    gpio_set(H7,0)

#define LEFT_PWM_EN    gpio_set(G1,1)
#define LEFT_PWM_NO    gpio_set(G1,0)

void Bsp_Init(void);
void PWM_Init();
void FTM_Init();
void AD_Init();

#endif