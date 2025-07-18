/********** 定时器pwm相关函数 **********/
// HAL_TIM_PWM_Start(tims_pwms[i].htim,tims_pwms[i].channel); //定时器pwm初始化
// __HAL_TIM_SET_COMPARE(tims_pwms[i].htim,tims_pwms[i].channel,tims_pwms[i].ccr); //定时器pwm设置占空比
/*************************************/

#include "User.h"

/********** 全局变量 **********/
static uint8_t tim_pwm_num;
uint8_t pwm_start_flag;

tim_pwm_t tims_pwms[] = 
{
    {&htim1,TIM_CHANNEL_3,0},
    {&htim1,TIM_CHANNEL_4,0},
};
/********** 全局变量 **********/

/********** 1-外设数量初始化函数 **********/
void tim_pwm_num_init(void)
{
    tim_pwm_num = sizeof(tims_pwms) / sizeof(tim_pwm_t);
}
/********** 1-外设数量初始化函数 **********/


/********** 2-外设硬件初始化函数 **********/
void tim_pwm_init(void) 
{
    for(uint8_t i = 0; i < tim_num ; i++ )
    {
        HAL_TIM_PWM_Start(tims_pwms[i].htim,tims_pwms[i].channel);
    }
}
/********** 2-外设硬件初始化函数 **********/


/********** 3-外设调用函数 **********/
void pid_output_pwm(float ccrValue) 
{
    /********** 限制CCR值范围 **********/
    if (ccrValue > pwm_arr) 
    {
        ccrValue = pwm_arr;
    }
    else if (ccrValue < (-pwm_arr))
    {
        ccrValue = (-pwm_arr);
    }
    /********** 限制CCR值范围 **********/

    /********** 判断方向 **********/
    if(pwm_start_flag)
    {
      if (ccrValue > 0) // 前进
      {
        // 取绝对值
        tims_pwms[1].ccr = fabsf(ccrValue);
        tims_pwms[0].ccr = 0;
      } 
      else if (ccrValue < 0) // 后退
      {
        // 取绝对值
        // 取绝对值
        tims_pwms[0].ccr = fabsf(ccrValue);
        tims_pwms[1].ccr = 0;
      } 
      else if (ccrValue == 0) 
      {

        tims_pwms[1].ccr = 0;
        tims_pwms[0].ccr = 0;
      }
    }
    else
    {
        tims_pwms[1].ccr = 0;
        tims_pwms[0].ccr = 0;
    }
    /********** 判断方向 **********/

    /********** 设置占空比 **********/
    for(uint8_t i = 0; i < tim_pwm_num ; i++)
    {
         __HAL_TIM_SET_COMPARE(tims_pwms[i].htim,tims_pwms[i].channel,tims_pwms[i].ccr);
    }
    /********** 设置占空比 **********/

}
/********** 3-外设调用函数 **********/


