#include "headfile.h"

//4500 8 1200
//#define SPEED_INIT 4500  //3500 35 780  //4000 30 680   //4500 20 905
//#define TURNPID_KP_INIT 8 //9 0.1 1800
//#define TURNPID_KD_INIT 1200//1200//950

#define SPEED_INIT 3500  //3500 35 780  //4000 30 680   //4500 20 905
#define TURNPID_KP_INIT 35 //9 0.1 1800
#define TURNPID_KD_INIT 1400//1200//950

#define SPEED_STRAIGHT 6000 // 5000 33 1100
#define TURNPID_KP_STRAIGHT 5
#define TURNPID_KD_STRAIGHT 300

#define SPEED_HIGH 4500 // 5000 33 1100
#define TURNPID_KP_HIGH 8
#define TURNPID_KD_HIGH 1200

//3500 35 1500
#define SPEED_RING 3500
#define TURNPID_KP_RING 35
#define TURNPID_KD_RING 1400

#define RINGPID_KP 45
#define RINGPID_KD 100

#define CHANGE_SPEED 0
#define LIUCHE 0

Road_Mode roadmode_last, roadmode = ROAD_MODE_OUTOF_RING;//ROAD_MODE_OUTOF_RING; ROAD_MODE_RING_OUT
uint16 count_10ms_ring = 0;
uint16 offset_outofring_time = 350;//3S屏蔽时间
uint8 inthe_ring_flag = 0, turning_ring_flag = 0;
uint16 mid_high_cnt;
uint16 speed_low_cnt;
uint8 speed_low_flag;

int16 leftpwm_set, rightpwm_set;
int16 leftpwm_set_last, rightpwm_set_last;
int16 leftpwm_change, rightpwm_change;
uint16 raw_speed = SPEED_INIT; // 2500  35 300  3.0r+
                               // 3500  35 600/780  3.7r+
                               // 4000  35 1100 3.7r-
                               // 4200  35 1300
                               // 4000  30 580
                               // 4500  50 4000                        //5000 65 6500

int16 err, abs_err;
uint16 straight_cnt, turn_cnt;

extern uint8 high_speed_flag;
extern uint16 high_speed_time_cnt;



void path_detect(void)
{
    roadmode_last = roadmode;
    
    switch(roadmode_last)
    {
        case ROAD_MODE_OUTOF_RING:
             if(gy_ad_val[MID_ADC] >= 90)
             {      
                 if(mid_high_cnt++ == 10)
                 {
                      gpio_set(C6,0);
                      mid_high_cnt = 0;
                      roadmode = ROAD_MODE_RING_IN;
                      turning_ring_flag = 1;
                      raw_speed = SPEED_RING;
                 }
             }
             else
             {
                break;
             }
        case ROAD_MODE_RING_IN:
            if(gy_ad_val[MID_ADC] <= 60)// && (gy_ad_val[MID_LEFT_ADC] <= 2 || gy_ad_val[MID_RIGHT_ADC] <= 2))
            {  
                gpio_set(C6,1);
                gpio_set(E2,0);
                roadmode = ROAD_MODE_INTHE_RING;
                inthe_ring_flag = 1;
                turning_ring_flag = 0;
            }
            else
            {
                break;
            }
        case ROAD_MODE_INTHE_RING:
            if(count_10ms_ring++ >= offset_outofring_time)
            {
                count_10ms_ring = 0;
                roadmode = ROAD_MODE_RING_OUT;
            }
            else
            {
                break;
            }
        case ROAD_MODE_RING_OUT:
            if(gy_ad_val[MID_ADC] <= 60)
            {
                gpio_set(E2,1);
                roadmode = ROAD_MODE_OUTOF_RING;
                inthe_ring_flag = 0;
                raw_speed = SPEED_INIT;
                high_speed_flag = 1;
                if(high_speed_time_cnt != 0)
                    enable_irq(PIT_CH1_IRQn);
            }
            else
            {
                break;
            }
    }
}

void set_motor_pwm(int16 leftpwm, int16 rightpwm)
{
    //左右轮均正转
    ftm_pwm_duty(ftm2, ftm_ch0, leftpwm);
    ftm_pwm_duty(ftm2, ftm_ch1, 0);
    ftm_pwm_duty(ftm2, ftm_ch2, 0);   
    ftm_pwm_duty(ftm2, ftm_ch3, rightpwm); 
}

void chassis_param_init(void)
{
    PID_struct_init(&pid_chassis_motor_turnpwm, POSITION_PID, 1500, 0,
                  TURNPID_KP_INIT, 0, TURNPID_KD_INIT); 
    PID_struct_init(&pid_chassis_motor_ringpwm, POSITION_PID, 1500, 0,
                  RINGPID_KP, 0, RINGPID_KD); 
}

void chassis_task(void)
{
    if(gy_ad_val[0] <= 30 && gy_ad_val[1] <= 30 && gy_ad_val[2] <= 30 && gy_ad_val[3] <= 30 && gy_ad_val[4] <= 30)
    {
        set_motor_pwm(0, 0);
    }
    else
    {
        path_detect();
        if(turning_ring_flag == 0)
        {
            if(inthe_ring_flag == 0)
            {
                if(high_speed_flag == 0)
                {
                    pid_chassis_motor_turnpwm.p = TURNPID_KP_INIT;
                    pid_chassis_motor_turnpwm.d = TURNPID_KD_INIT;
                    raw_speed = SPEED_INIT; 
                }
                else
                {
#if(CHANGE_SPEED == 1)
                    err = abs(ad_error);
                    if(err <= 5) //gy_ad_val[MID_LEFT_ADC] < 2 && gy_ad_val[MID_RIGHT_ADC] < 2 && 
                    {
                        turn_cnt = 0;
                        if(straight_cnt++ == 30) //0.3s判断是否是直道
                        {
                            straight_cnt = 0;
                            pid_chassis_motor_turnpwm.p = TURNPID_KP_STRAIGHT;
                            pid_chassis_motor_turnpwm.d = TURNPID_KD_STRAIGHT;
                            raw_speed = SPEED_STRAIGHT;   
                            gpio_set(E2,0);
                        }
                    }
                    else if(err > 5)
                    {
                        straight_cnt = 0;
                        if(turn_cnt++ == 3)
                        { 
                            turn_cnt = 0;
                            pid_chassis_motor_turnpwm.p = TURNPID_KP_HIGH;
                            pid_chassis_motor_turnpwm.d = TURNPID_KD_HIGH;
                            raw_speed = SPEED_HIGH;
                            gpio_set(E2,1);
                        }
                    }
#else                  
                    pid_chassis_motor_turnpwm.p = TURNPID_KP_HIGH;
                    pid_chassis_motor_turnpwm.d = TURNPID_KD_HIGH;
                    raw_speed = SPEED_HIGH;
#endif                     
                }   
            }     
            else if(inthe_ring_flag == 1)
            {
                pid_chassis_motor_turnpwm.p = TURNPID_KP_RING;
                pid_chassis_motor_turnpwm.d = TURNPID_KD_RING;
            }
            
            pid_calc(&pid_chassis_motor_turnpwm,  ad_error,  0);
            leftpwm_set  = raw_speed - (int)pid_chassis_motor_turnpwm.out;
            rightpwm_set = raw_speed + (int)pid_chassis_motor_turnpwm.out;            

#if(LIUCHE == 1)
            leftpwm_change = leftpwm_set_last - leftpwm_set;
            rightpwm_change = rightpwm_set_last - rightpwm_set;
            if(leftpwm_change < 100 && leftpwm_change > 0)
            {
                RIGHT_PWM_EN;
                LEFT_PWM_NO;
                set_motor_pwm(leftpwm_set, rightpwm_set);
                gpio_set(C6,0);
            }
            else if(rightpwm_change < 100 && rightpwm_change > 0)
            {
                RIGHT_PWM_NO;
                LEFT_PWM_EN;
                set_motor_pwm(leftpwm_set, rightpwm_set);
                gpio_set(C6,0);
            }
            else
            {
                RIGHT_PWM_EN;
                LEFT_PWM_EN;
                set_motor_pwm(leftpwm_set, rightpwm_set);
                gpio_set(C6,1);
            }
            leftpwm_set_last = leftpwm_set;
            rightpwm_set_last = rightpwm_set;
#else
            set_motor_pwm(leftpwm_set, rightpwm_set);
#endif
        }
        else if(turning_ring_flag == 1)
        { 
            pid_calc(&pid_chassis_motor_ringpwm,  ad_err_ring,  0);
            leftpwm_set  = raw_speed - (int)pid_chassis_motor_ringpwm.out;
            rightpwm_set = raw_speed + (int)pid_chassis_motor_ringpwm.out;    
            set_motor_pwm(leftpwm_set, rightpwm_set);
        }        
    }
}