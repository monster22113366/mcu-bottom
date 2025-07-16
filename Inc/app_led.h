#ifndef APP_LED_H
#define APP_LED_H
#include "stm32f4xx_hal.h"

/*************一些宏定义，修改宏即可移植************/
//移植时HAL_GPIO_WritePin修改为你的gpio写函数就好
#define GPIO_WritePin HAL_GPIO_WritePin 
//移植时将GPIO_PIN_SET修改为你的高电平定义
#define GPIO_HIGH GPIO_PIN_SET
//移植时将GPIO_PIN_RESET修改成你的低电平定义
#define GPIO_LOW GPIO_PIN_RESET
/************************************************/


/********定义了一个led的结构体，移植时修改你的端口，引脚，状态的数据类型即可******/
typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t pin;
    GPIO_PinState state;
}led_t;
/**************************************************************************/





/*****************************led函数声明**************************************/
void lednum_init(void);
void led_task(void);
/****************************************************************************/




#endif //APP_LED_H

