#ifndef SCHEDULER_H
#define SCHEDULER_H

/*************** 宏定义 *****************/
typedef struct MyStruct
{
    void (*task) (void);
    uint16_t task_period;
    uint32_t task_last_runtime;
}task_t;
/*************** 宏定义 *****************/


/*************** 函数声明 *****************/
void tasknum_init(void);
void task_run(void);
/*************** 函数声明 *****************/

#endif //SCHEDULER_H


