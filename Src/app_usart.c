#include "User.h"
#include <stdint.h>

/********** 全局变量 **********/
usart_t usarts[] = 
{
    {&huart1, {0}, 0, 0},
};

static uint8_t usart_num;
/********** 全局变量 **********/


/********** 1-外设数量初始化函数 **********/
void usartnum_init(void)
{
    usart_num = sizeof(usarts) / sizeof(usarts[0]);
}
/********** 1-外设数量初始化函数 **********/


/********** 2-外设硬件初始化函数 **********/
void usart_it_init(void)
{
    for (uint8_t i = 0; i < usart_num; i++)
    {
        UART_Receive_IT(usarts[i].huart, usarts[i].rxbuffer, 1);
    }
    
}
/********** 2-外设硬件初始化函数 **********/


/********** 3-外设调用函数 **********/
void usart_printf(UART_HandleTypeDef *huart, const char *fmt, ...)
{
    va_list ap;
    char string[128];
    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    va_end(ap);
    UART_Transmit(huart, (uint8_t *)string, strlen(string), 0xffff);
}



void UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        usarts[0].rxtick = uwTick;
        if (++usarts[0].rxindex >= sizeof(usarts[0].rxbuffer))
        {
            usarts[0].rxindex = 0;
        }
        UART_Receive_IT(&huart1, &usarts[0].rxbuffer[usarts[0].rxindex], 1);
    }
}

void usart_task(void)
{
    if(usarts[0].rxindex == 0) return;
    if(uwTick - usarts[0].rxtick > 100)
    {
        usarts[0].rxbuffer[usarts[0].rxindex] = '\0'; // null-terminate the string
        usart_printf(usarts[0].huart,"recive : %s\n", usarts[0].rxbuffer);
        memset(usarts[0].rxbuffer, 0, sizeof(usarts[0].rxbuffer)); // clear the buffer
        usarts[0].rxindex = 0;
        huart1.pRxBuffPtr = usarts[0].rxbuffer; // reset the pointer to the start of the buffer
    }
}
/********** 3-外设调用函数 **********/








