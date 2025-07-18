/************************** 定时器启动及使用函数 *********************************************/
// // 启动 TIM6 中断
// HAL_TIM_Base_Start_IT(&htim6);

// //中断回调函数
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//   if (htim->Instance == TIM6) // 确认是 TIM6 中断
//   {
//     // 每 1ms 执行的代码
//     HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // 翻转 LED
//   }
// }
/**********************************************************************************/
#include "User.h"


/********** 全局变量 **********/
static uint8_t tim_it_num;
TIM_HandleTypeDef *tim_its[] = 
{
    my_tim_it_1,
}
/********** 全局变量 **********/


/********** 1-外设数量初始化函数 **********/
void tim_it_num_init(void)
{
    tim_it_num = sizeof(tim_its) / sizeof(tim_its[0]);
}
/********** 1-外设数量初始化函数 **********/


/********** 2-外设硬件初始化函数 **********/
void tim_it_init(void)
{
  for(uint8_t i = 0; i < tim_it_num; i++)
  {
      // 启动 TIM 中断
      HAL_TIM_Base_Start_IT(tim_its[i]);
  }
}
/********** 2-外设硬件初始化函数 **********/


/********** 3-外设调用函数 **********/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == my_tim_it_1->Instance) // 确认是 TIM 中断
  {

  }

}
/********** 3-外设调用函数 **********/


