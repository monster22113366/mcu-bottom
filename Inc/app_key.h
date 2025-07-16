#ifndef APP_KEY_H
#define APP_KEY_H
#include "stm32f4xx_hal.h"

/*************一些宏定义，修改宏即可移植************/
//移植时HAL_GPIO_WritePin修改为你的gpio写函数就好
#define GPIO_WritePin HAL_GPIO_WritePin
//移植时HAL_GPIO_ReadPin修改为你的gpio读函数就好 
#define GPIO_ReadPin HAL_GPIO_ReadPin 
//移植时将GPIO_PIN_SET修改为你的高电平定义
#define GPIO_HIGH GPIO_PIN_SET
//移植时将GPIO_PIN_RESET修改成你的低电平定义
#define GPIO_LOW GPIO_PIN_RESET
/************************************************/


/********定义了一个按键的结构体，移植时修改你的端口，引脚，状态的数据类型即可******/
typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t pin;
    GPIO_PinState state;
}key_t;
/**************************************************************************/

/*****************************声明函数***************************************/
void keynum_init(void);
void key_task(void);
/****************************************************************************/



#endif //APP_KEY_H

