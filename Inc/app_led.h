#ifndef APP_LED_H
#define APP_LED_H
#include "stm32f4xx_hal.h"

/*************** 宏定义 *****************/
typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t pin;
    GPIO_PinState state;
}led_t;

//移植时HAL_GPIO_WritePin修改为你的gpio写函数就好
#define GPIO_WritePin HAL_GPIO_WritePin 
//移植时将GPIO_PIN_SET修改为你的高电平定义
#define GPIO_HIGH GPIO_PIN_SET
//移植时将GPIO_PIN_RESET修改成你的低电平定义
#define GPIO_LOW GPIO_PIN_RESET
/*************** 宏定义 *****************/


/*************** 函数声明 *****************/
void lednum_init(void);
void led_task(void);
/*************** 函数声明 *****************/


/**************** 全局变量声明 ***************/

/**************** 全局变量声明 ***************/

#endif //APP_LED_H

