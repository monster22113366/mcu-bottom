#include "User.h"

/********** 定时器编码器相关函数 **********/
// HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL); //1-启动编码器模式
// __HAL_TIM_GET_COUNTER(&htim5); //2-读取脉冲
// __HAL_TIM_SET_COUNTER(&htim5, 0);  //3-清零累计脉冲数
/****************************************/


/********** 全局变量 **********/
encoder_t encoders[] =
{
    {&htim5,0,0},
};

static uint8_t encode_num = 0;
/*****************************/


/********** 1-外设数量初始化函数 **********/
void tim_encode_num_init(void)
{
    encode_num = sizeof(encoders) / sizeof(encoder_t);
}
/*****************************************/


/********** 2-外设硬件初始化函数 **********/
void tim_encode_init(void)
{
  for (uint8_t i = 0; i < encode_num; i++) 
  {
    HAL_TIM_Encoder_Start(encoders[i].htim, TIM_CHANNEL_ALL); // 启动编码器模式
    __HAL_TIM_SET_COUNTER(encoders[i].htim, 0); // 清零累计脉冲数
  }
}
/*************************************/


/********** 2-外设硬件初始化函数 **********/
void encode_update(void)
{
    int encode_count_processed = 0;
    encode_count_processed = (-(__HAL_TIM_GET_COUNTER(encoders[0].htim))); //读取脉冲

    // 处理计数器溢出
    if (encode_count_processed <= (-encode_max_half)) 
    {
        encode_count_processed += encode_max;  // 向下溢出
    }

    encoders[0].encode_per_period = encode_count_processed;
    encoders[0].encode_total += encoders[0].encode_per_period;
    __HAL_TIM_SET_COUNTER(encoders[0].htim, 0);  //清零累计脉冲数
}


float encode_get_speed(void)
{
    return (encoders[0].encode_per_period / (encode_period_time * encode_per_revolution));
}

float encode_get_location(void)
{
    return encoders[0].encode_total;
}
/*************************************/


