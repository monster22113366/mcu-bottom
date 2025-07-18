#ifndef APP_TIM_ENCODE
#define APP_TIM_ENCODE
#include "stm32f4xx_hal.h"

/*************** 宏定义 *****************/
typedef struct {
    TIM_HandleTypeDef *htim; // 编码器定时器句柄
    int encode_per_period;//当前最大144，每20ms
    volatile long int encode_total;
} encoder_t;

#define encode_max 65536//4294967296
#define encode_max_half 32768//2147483648
#define encode_period_time 0.02 //s
#define encode_per_revolution 1320
/****************************************/





/*************** 函数声明 *****************/
void tim_encode_num_init(void);
void tim_encode_init(void);
void encode_update(void);
float encode_get_speed(void);
float encode_get_location(void);
/****************************************/

/**************** 全局变量声明 ***************/
extern encoder_t encoders[];
/********************************************/

#endif //APP_TIM_ENCODE

