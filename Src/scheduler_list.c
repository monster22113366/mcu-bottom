#include "scheduler.h"
#include "User.h"
#include <string.h>


// 全局变量
TCB_t *current_thread = NULL;
TCB_t *from_thread = NULL;
list_node_t priority_lists[MAX_PRIORITY];
list_node_t sleep_lists[MAX_PRIORITY];
list_node_t timeout_list = {NULL, NULL};
list_node_t defunct_list = {NULL, NULL};
uint32_t ready_bitmap = 0;
uint32_t sleep_bitmap = 0;
volatile uint32_t interrupt_nest = 0;

// 初始化双向链表
static void list_init(list_node_t *list) {
    list->next = list;
    list->prev = list;
}

// 插入节点到链表（尾部）
static void list_insert_tail(list_node_t *list, list_node_t *node) {
    node->prev = list->prev;
    node->next = list;
    list->prev->next = node;
    list->prev = node;
}

// 从链表移除节点
static void list_remove(list_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = NULL;
    node->prev = NULL;
}

// 线程创建
void thread_create(TCB_t *tcb, void (*entry)(void), uint32_t prio, uint32_t time_slice, uint32_t priority_boost) 
{
    //异常处理
    if (prio >= MAX_PRIORITY || priority_boost > prio) 
    {
        return;
    }
    //给线程栈操作台的各个成员赋值
    tcb->func = entry;
    tcb->priority = prio;                                           //线程当前优先级
    tcb->original_priority = prio;                                  //线程原始优先级
    tcb->priority_boost = priority_boost;                           //线程优先级提升幅度
    tcb->sleep_ticks = 0;                                           //线程睡眠时间
    tcb->time_slice = time_slice ? time_slice : TIME_SLICE_TICKS;   //线程档期当前时间片
    tcb->time_slice_max = tcb->time_slice;                          //线程最大时间片
    tcb->status = 0;                                                //线程状态
    tcb->ready_node.next = NULL;                                    //就绪链表节点
    tcb->ready_node.prev = NULL;                                    //就绪链表节点  
    tcb->sleep_node.next = NULL;                                    //睡眠链表节点
    tcb->sleep_node.prev = NULL;                                    //睡眠链表节点
    //初始化就绪链表
    if (priority_lists[prio].next == NULL) 
    {
        list_init(&priority_lists[prio]);
    }
    //初始化睡眠链表
    if (sleep_lists[prio].next == NULL) {
        list_init(&sleep_lists[prio]);
    }
    //将线程加入就绪链表
    list_insert_tail(&priority_lists[prio], &tcb->ready_node);
    //更新位图
    if (prio >= MAX_PRIORITY) {
        // 错误处理
        while (1);
        return;
    }
    ready_bitmap |= (1 << prio);
}



void scheduler_init(void)
{
    //初始化链表头
    for (int i = 0; i < MAX_PRIORITY; i++) 
    {
        list_init(&priority_lists[i]);
        list_init(&sleep_lists[i]);
    }
    list_init(&timeout_list);
    list_init(&defunct_list);
}

// 调度启动
void scheduler_start(void) 
{
    //选择高优先级线程
    if (ready_bitmap == 0) 
    {
        while (1); // 无线程，错误处理
    }
    uint32_t highest_prio = __builtin_ctz(ready_bitmap);
    if (highest_prio >= MAX_PRIORITY || priority_lists[highest_prio].next == &priority_lists[highest_prio]) 
    {
        while (1); // 无效优先级或空链表
    }

    from_thread = NULL;
    current_thread = (TCB_t *)((uint8_t *)priority_lists[highest_prio].next - offsetof(TCB_t, ready_node));
    //systick初始化
    Systick_Config(); // 配置SysTick定时器
    //外设初始化

    //开始调度
    current_thread->func();
}

// 线程睡眠
void thread_sleep(uint32_t ticks) 
{
    if (current_thread->status == 1) 
    {
        return;
    }
    //更新睡眠时间
    current_thread->sleep_ticks = ticks;
    current_thread->status = 1;
    uint32_t old_prio = current_thread->priority;
    // 恢复原始优先级
    if (current_thread->priority != current_thread->original_priority) 
    {
        current_thread->priority = current_thread->original_priority;
    }
    //从就绪链表移除
    if (current_thread->ready_node.next != NULL) 
    {
        list_remove(&current_thread->ready_node);
        //处理原本的就绪位图
        if (priority_lists[old_prio].next == &priority_lists[old_prio]) 
        {
            if (old_prio >= MAX_PRIORITY) {
                // 错误处理
                while (1);
                return;
            }
            ready_bitmap &= ~(1 << old_prio);
        }
    }

    //加入睡眠链表并更新睡眠位图
    if (sleep_lists[current_thread->priority].next == NULL) 
    {
        list_init(&sleep_lists[current_thread->priority]);
    }
    list_insert_tail(&sleep_lists[current_thread->priority], &current_thread->sleep_node);
    sleep_bitmap |= (1 << current_thread->priority);
    //休眠当前线程后，继续调度
    current_thread->func();
}

// 线程退出
void thread_exit(void) 
{
    if(current_thread -> status == 2) 
    {
        return;
    }
    current_thread->status = 2;
    //移出就绪链表
    if (current_thread->ready_node.next != NULL) 
    {
        list_remove(&current_thread->ready_node);
        if (priority_lists[current_thread->priority].next == &priority_lists[current_thread->priority]) 
        {
            ready_bitmap &= ~(1 << current_thread->priority);
        }
    }
    //移出睡眠链表
    if (current_thread->sleep_node.next != NULL) {
        list_remove(&current_thread->sleep_node);
        if (sleep_lists[current_thread->original_priority].next == &sleep_lists[current_thread->original_priority]) 
        {
            sleep_bitmap &= ~(1 << current_thread->original_priority);
        }
    }
    //加入失效链表
    if (defunct_list.next == NULL) 
    {
        list_init(&defunct_list);
    }
    list_insert_tail(&defunct_list, &current_thread->sleep_node);
    //继续调度
    current_thread->func();
}

void scheduler(void) 
{
    // 恢复当前线程的原始优先级（在上下文切换前）
    if (current_thread && current_thread->priority != current_thread->original_priority) 
    {
        uint32_t old_prio = current_thread->priority;
        current_thread->priority = current_thread->original_priority;
        if (current_thread->ready_node.next != NULL) 
        {
            //移出原本的就绪链表
            list_remove(&current_thread->ready_node);
            //处理原本的就绪链表
            if (priority_lists[old_prio].next == &priority_lists[old_prio]) 
            {
                if (old_prio >= MAX_PRIORITY) 
                {
                    // 错误处理
                    while (1);
                    return;
                }
                ready_bitmap &= ~(1 << old_prio);
            }
            //加入原优先级链表
            if (priority_lists[current_thread->priority].next == NULL) 
            {
                list_init(&priority_lists[current_thread->priority]);
            }
            list_insert_tail(&priority_lists[current_thread->priority], &current_thread->ready_node);
            if (current_thread->priority >= MAX_PRIORITY) {
                // 错误处理
                while (1);
                return;
            }
            ready_bitmap |= (1 << current_thread->priority);
        }
    }
    // 处理超时线程
    if(timeout_list.next == NULL) 
    {
        list_init(&timeout_list);
    }
    list_node_t *node = timeout_list.next;
    while (node != &timeout_list) 
    {
        list_node_t *next = node->next;
        TCB_t *tcb = (TCB_t *)((uint8_t *)node - offsetof(TCB_t, sleep_node));
        //移除超时链表
        list_remove(&tcb->sleep_node);
        //重置时间片
        tcb->time_slice = tcb->time_slice_max;
        uint32_t old_prio = tcb->priority;
        //休眠结束后提高优先级
        tcb->priority = (tcb->priority_boost <= tcb->original_priority) ? (tcb->original_priority - tcb->priority_boost) : 0;
        //插入新优先级的就绪链表
        if (priority_lists[tcb->priority].next == NULL) 
        {
            list_init(&priority_lists[tcb->priority]);
        }
        list_insert_tail(&priority_lists[tcb->priority], &tcb->ready_node);
        if (tcb->priority >= MAX_PRIORITY) {
            // 错误处理
            while (1);
            return;
        }
        ready_bitmap |= (1 << tcb->priority);
        //处理旧优先级链表
        if (old_prio != tcb->priority && priority_lists[old_prio].next == &priority_lists[old_prio]) 
        {
            if (old_prio >= MAX_PRIORITY) {
                // 错误处理
                while (1);
                return;
            }
            ready_bitmap &= ~(1 << old_prio);
        }
        node = next;
    }
    list_init(&timeout_list);

    // 选择最高优先级线程
    if (ready_bitmap == 0) 
    {
        return;
    }
    uint32_t highest_prio = __builtin_ctz(ready_bitmap);
    if (highest_prio >= MAX_PRIORITY)
    {
        return;
    }
    list_node_t *best_node = priority_lists[highest_prio].next;
    if (best_node == &priority_lists[highest_prio]) 
    {
        return;
    }
    TCB_t *best = (TCB_t *)((uint8_t *)best_node - offsetof(TCB_t, ready_node));
    // 若当前线程时间片耗尽且有其他同优先级线程，切换到下一个线程
    if (best == current_thread && current_thread->time_slice == 0 && best_node->next != &priority_lists[highest_prio]) 
    {
        current_thread->time_slice = current_thread->time_slice_max;
        list_remove(&current_thread->ready_node);
        list_insert_tail(&priority_lists[highest_prio], &current_thread->ready_node);
        best_node = best_node->next; // 选择下一个线程
        best = (TCB_t *)((uint8_t *)best_node - offsetof(TCB_t, ready_node));
    } 
    //当前线程时间片耗尽且无其他同优先级线程，单纯重置时间片
    else if (best == current_thread && current_thread->time_slice == 0)
    {
        current_thread->time_slice = current_thread->time_slice_max; // 单线程重置时间片
    }

        // 切换线程
        from_thread = current_thread;
        current_thread = best;
        #ifdef use_mpu
        //线程栈保护
        mpu_configure_stack(current_thread->stack_bottom, current_thread->stack_size);
        #endif
        //将当前线程移到就绪链表尾部
        if (best_node->next != &priority_lists[highest_prio]) 
        {
            list_remove(&best->ready_node);
            list_insert_tail(&priority_lists[highest_prio], &best->ready_node);
        }
        //继续调度
        current_thread->func();
}

// SysTick 中断
void SysTick_Handler(void) 
{
    interrupt_nest++;
    uwTick++;
    //获取睡眠位图
    uint32_t bitmap = sleep_bitmap;
    while (bitmap != 0) 
    {
        //按优先级从高到低更新睡眠状态
        uint32_t prio = __builtin_ctz(bitmap);
        if (prio >= MAX_PRIORITY) 
        {
            break;
        }
        //在同一优先级中轮询更新睡眠状态
        list_node_t *node = sleep_lists[prio].next;
        while (node != &sleep_lists[prio]) 
        {
            //获取线程控制块
            TCB_t *tcb = (TCB_t *)((uint8_t *)node - offsetof(TCB_t, sleep_node));
            //获取控制块中的链表节点
            list_node_t *next = node->next;
            //更新睡眠时间
            if (tcb->sleep_ticks > 0) 
            {
                tcb->sleep_ticks--;
                //如果睡眠结束
                if (tcb->sleep_ticks == 0) 
                {
                    //移出睡眠链表
                    tcb->status = 0;
                    list_remove(&tcb->sleep_node);
                    if (sleep_lists[prio].next == &sleep_lists[prio]) {
                        sleep_bitmap &= ~(1 << prio);
                    }
                    //加入超时链表
                    if (timeout_list.next == NULL) {
                        list_init(&timeout_list);
                    }
                    list_insert_tail(&timeout_list, &tcb->sleep_node);
                }
            }
            node = next;
        }
        //更新当前优先级的暂时睡眠位图
        bitmap &= ~(1 << prio);
    }
    //更新当前线程时间片
    if (current_thread && current_thread->time_slice > 0) {
        current_thread->time_slice--;
    }
    //继续调度
    scheduler();
    interrupt_nest--;
}