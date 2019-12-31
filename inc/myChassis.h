#ifndef __MYCHASSIS_H__
#define __MYCHASSIS_H__

void path_detect(void);
void set_motor_pwm(int16 leftpwm, int16 rightpwm);
void chassis_param_init(void);
void chassis_task(void);

#endif