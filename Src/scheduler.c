#include "User.h"
#include "app_key.h"
#include "app_led.h"
#include <stdint.h>


uint8_t task_num;
uint8_t ticktest;

typedef struct MyStruct
{
    void (*task) (void);
    uint16_t task_period;
    uint32_t task_last_runtime;
}task_t;

void task1(void)
{
    ticktest++;
}

task_t tasks[] = 
{
    {task1,1000,0},
    {led_task,1,0},
    {key_task,10,0},
    {usart_task,200,0},
};

void tasknum_init(void)
{
    task_num = sizeof(tasks) / sizeof(task_t);
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
