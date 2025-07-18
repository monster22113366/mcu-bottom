#include "User.h"
#include "app_key.h"
#include "app_led.h"
#include <stdint.h>

/********** 全局变量 **********/
uint8_t task_num;
uint8_t ticktest;

task_t tasks[] = 
{
    {task1,1000,0},
    {led_task,1,0},
    {key_task,10,0},
    {usart_task,200,0},
};
/********** 全局变量 **********/


/********** 1-外设数量初始化函数 **********/
void tasknum_init(void)
{
    task_num = sizeof(tasks) / sizeof(task_t);
}
/********** 1-外设数量初始化函数 **********/


/********** 3-外设调用函数 **********/
void task1(void)
{
    ticktest++;
}


void task_run(void)
{
    for(uint8_t i = 0; i < task_num; i++)
    {
        uint32_t now_time = uwTick;
        if(now_time - tasks[i].task_last_runtime >= tasks[i].task_period)
        {
            tasks[i].task_last_runtime = now_time;
            tasks[i].task();
        }
    }
}
/********** 3-外设调用函数 **********/