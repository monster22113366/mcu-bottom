#include "User.h"


/********** 全局变量 **********/
pid_t pids[] = 
{
    {.kp = kp1,.ki = ki1,.target = target1}, //速度环pid
};
/********** 全局变量 **********/


/********** 3-外设调用函数 **********/
float pid_value_update(pid_t *pid, float current_value) 
{
    float p_term,i_term,d_term; //比例，积分，微分的结果
    // 计算误差
    float error = pid->target - current_value;
    
    
    /********** 比例项 **********/
    p_term = pid->kp * error;
    /********** 比例项 **********/


    /********** 积分项 **********/
    // 积分项（使用梯形积分）
    pid->error_sum += error; //* encode_period_time;
    // 抗积分饱和
    if (pid->error_sum > output_max) 
    {
      pid->error_sum = output_max;
    } 
    else if (pid->error_sum < output_min) 
    {
      pid->error_sum = output_min;
    }
    i_term = pid->ki * pid->error_sum;
    /********** 积分项 **********/


    /********** 微分项 **********/
    float derivative = (error - pid->error_last) / encode_period_time;
    d_term = pid->kd * derivative;
    /********** 微分项 **********/


    // 计算总输出
    float output = p_term + i_term + d_term;


    // 限制输出范围
    if (output > output_max) {
        output = output_max;
    } else if (output < output_min) {
        output = output_min;
    }
    
    // 更新上一次误差
    pid->error_last = error;

    
    return output;
}

/********** 3-外设调用函数 **********/