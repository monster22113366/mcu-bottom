
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define MAX_PRIORITY 32          // 优先级级别 (0 最高)
#define STACK_GUARD 0xDEADBEEF  // 栈哨兵值
#define MPU_REGION_NUMBER 0      // 当前线程栈区域
#define TIME_SLICE_TICKS 10      // 时间片大小 (10ms/tick)

// 双向链表节点
typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

// 线程控制块
typedef struct TCB {
    void  (*func)(void );         // 线程入口函数
    list_node_t ready_node;      // 就绪链表节点 (同优先级)
    list_node_t sleep_node;      // 睡眠/超时/失效链表节点
    uint32_t priority;           // 当前优先级 (0 最高)
    uint32_t original_priority;  // 原始优先级 (创建时)
    uint32_t priority_boost;     // 优先级提升幅度 (超时时)
    uint32_t sleep_ticks;        // 剩余睡眠节拍 (0 表示就绪)
    uint32_t time_slice;         // 剩余时间片 (ticks)
    uint32_t time_slice_max;     // 最大时间片 (ticks)
    uint32_t status;             // 状态 (0: 就绪, 1: 睡眠, 2: 退出)
} TCB_t;

// 全局变量声明
extern TCB_t *current_thread;
extern TCB_t *from_thread;
extern list_node_t priority_lists[MAX_PRIORITY];
extern list_node_t sleep_lists[MAX_PRIORITY];
extern list_node_t timeout_list;
extern list_node_t defunct_list;
extern uint32_t ready_bitmap;
extern uint32_t sleep_bitmap;
extern volatile uint32_t sys_tick;
extern volatile uint32_t interrupt_nest;

// 函数声明

void thread_create(TCB_t *tcb, void (*entry)(void), uint32_t prio, uint32_t time_slice, uint32_t priority_boost);
void scheduler(void);
void scheduler_start(void);
void thread_sleep(uint32_t ticks);
void thread_exit(void);
void scheduler_init(void);
#endif