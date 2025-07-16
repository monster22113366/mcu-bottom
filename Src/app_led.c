#include "app_led.h"
#include "User.h"


/*******************定义全局变量************************/
uint8_t led_status[] = {0,1,0}; //led的初始状态 0:下面 1：中间 ；2：上面
uint8_t led_num;                            //led的数量


static led_t leds[] =  //初始化led引脚实例，移植时可用修改这里的端口，引脚
{
    {GPIOD, GPIO_PIN_11, GPIO_LOW},
    {GPIOD, GPIO_PIN_10, GPIO_HIGH},
    {GPIOD, GPIO_PIN_9, GPIO_HIGH},
};
/*******************************************************/



/**************自动根据led_t初始化led的数量************************/
void lednum_init(void)
{
    led_num = sizeof(leds)/sizeof(led_t);
}
/*****************************************************************/

/***********************led状态更新函数，led_status数组代表led的状态,1亮0灭****************************/
void led_dis(void)
{
    static uint8_t led_last_status = 0xff;
    uint8_t led_current_status = 0;

    for(uint8_t i = 0; i < led_num; i++)
    {
        led_current_status |= (led_status[i] << i);
    }

    if(led_last_status != led_current_status)
    {
        led_last_status = led_current_status;
        for(uint8_t i = 0; i < led_num ; i++)
        {
            if(led_current_status & (1 << i))
            {
                leds[i].state = GPIO_LOW;
            }
            else
            {
                leds[i].state = GPIO_HIGH;
            }
            GPIO_WritePin(leds[i].port, leds[i].pin, leds[i].state);

        }
    }
}
/***************************************************************************************************/

/***********************最后添加led任务******************************/
void led_task(void)
{
    led_dis();
}
/*******************************************************************/

