#ifndef APP_PWM_H
#define APP_PWM_H
#include "stm32f4xx_hal.h"
/*************** 宏定义 *****************/
typedef struct 
{
    TIM_HandleTypeDef * htim;
    uint8_t channel;
    uint16_t ccr;
}tim_pwm_t;

#define pwm_arr 59999 
#define tim_for_pwm_num 2
/*************** 宏定义 *****************/


/*************** 函数声明 *****************/
void tim_pwm_num_init(void);
void tim_pwm_init(void);
void pid_output_pwm(float ccrValue);
/*************** 函数声明 *****************/


/**************** 全局变量声明 ***************/
extern uint8_t pwm_start_flag;
/**************** 全局变量声明 ***************/

#endif //APP_PWM_H


