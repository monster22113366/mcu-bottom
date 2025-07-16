#ifndef APP_USART_H
#define APP_USART_H
#include "stm32f4xx_hal.h"
/**************************宏定义********************************/
#define UART_Transmit HAL_UART_Transmit //串口接收函数
#define UART_Receive_IT HAL_UART_Receive_IT //串口接收中断函数
#define UART_RxCpltCallback HAL_UART_RxCpltCallback
/***************************************************************/

/*********************定义串口结构体****************************/
typedef struct
{
    UART_HandleTypeDef *huart;
    uint8_t rxbuffer[128];
    uint32_t rxtick;
    uint8_t rxindex;
}usart_t;
/***************************************************************/

/**************************函数声明***********************************/
void usartnum_init(void);
void usart_it_init(void);
void usart_printf(UART_HandleTypeDef *huart, const char *fmt, ...);
void usart_task(void);
/*********************************************************************/


#endif //APP_USART_H

