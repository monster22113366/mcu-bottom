#ifndef APP_PID_H
#define APP_PID_H

/*************** 宏定义 *****************/
typedef struct {
    float kp; float ki; float kd;           // 比例 积分 微分
    float error_last; float error_sum;
    float now_value;float target;float output;
}pid_t;

#define kp1 40000
#define ki1 8000
#define kd1 0
#define target1 3.2
#define output_min -59999  // 输出最小值
#define output_max 59999  // 输出最大值
/*************** 宏定义 *****************/


/*************** 函数声明 *****************/
float pid_value_update(pid_t *pid, float current_value);
/*************** 函数声明 *****************/


/**************** 全局变量声明 ***************/
extern pid_t pids[];
/**************** 全局变量声明 ***************/


#endif //APP_PID_H


