/********** 张大头电机控制相关函数 **********/
/**********************************************************
位置模式：位置范围（0-2^32）
**********************************************************/	
  // Emm_V5_Pos_Control(1, 0, 1000, 0, 3200, 0, 0);
  // Emm_V5_Pos_Control(2, 0, 1000, 0, 3200, 0, 0);
/**********************************************************
速度模式：速度范围（0-5000），单位rpm
**********************************************************/	
  //Emm_V5_Vel_Control(2, 1, 0, 0, 0);
  //Emm_V5_Vel_Control(1, 0, 0, 0, 0);
/**********************************************************
编码器清零：
**********************************************************/	
 //Emm_V5_Reset_CurPos_To_Zero(1);
 //Emm_V5_Reset_CurPos_To_Zero(2);
/**********************************************************
启动多机同步：
**********************************************************/	
 //Emm_V5_Synchronous_motion(1);
 //Emm_V5_Synchronous_motion(2);
/********** 张大头电机控制相关函数 **********/

#include "User.h"
#include <stdint.h>

/********** 全局变量 **********/
zdt_motor_t zdt_motors[] = 
{
  {.addr = 1,.snF = 1,.vel_set = 1100,.clk_set = 2000},
  {.addr = 2,.snF = 1,.vel_set = 1100,.clk_set = 1000},
};

uint8_t zdt_motor_num;
/********** 全局变量 **********/


/********** 1-外设数量初始化函数 **********/
void zdt_motornum_init(void)
{
  zdt_motor_num = sizeof(zdt_motors)/sizeof(zdt_motors[0]);
}
/********** 1-外设数量初始化函数 **********/



/********** 2-外设调用函数 **********/
void zdt_speed_get(void)
{

    Emm_V5_Read_Sys_Params(zdt_motors[0].addr, S_VEL);
  
}

void zdt_location_get(void)
{
    uint32_t pos;
    Emm_V5_Read_Sys_Params(1, S_CPOS);
    if(can.rxData[0] == 0x36 && can.CAN_RxMsg.DLC == 7)
    {
      // 拼接成uint32_t类型
      pos = (uint32_t)(
                        ((uint32_t)can.rxData[2] << 24)		|
                        ((uint32_t)can.rxData[3] << 16)  	|
                        ((uint32_t)can.rxData[4] << 8)   	|
                        ((uint32_t)can.rxData[5] << 0)
                      );

      // 转换成角度
      zdt_motors[0].vel_get = (float)pos * 360.0f / 65536.0f;

      // 符号
      if(can.rxData[1]) {zdt_motors[0].vel_get  = -zdt_motors[0].vel_get;}
  }
}

void zdt_speed_set(void)
{
  for(uint8_t i = 0; i < zdt_motor_num; i++)
  {
    Emm_V5_Vel_Control(zdt_motors[i].addr,zdt_motors[i].dir,zdt_motors[i].vel_set,zdt_motors[i].acc,zdt_motors[i].snF);
    HAL_Delay(10);																 // 每条命令后面延时10毫秒，防止粘包
  }
  Emm_V5_Synchronous_motion(0); 								 // 广播地址0触发
	HAL_Delay(10);																 // 每条命令后面延时10毫秒，防止粘包
}

void zdt_location_set(void)
{
  for(uint8_t i = 0; i < zdt_motor_num; i++)
  {
    Emm_V5_Pos_Control(zdt_motors[i].addr,zdt_motors[i].dir,zdt_motors[i].vel_set,zdt_motors[i].acc,zdt_motors[i].clk_set,zdt_motors[i].raF,zdt_motors[i].snF);
    HAL_Delay(10);																 // 每条命令后面延时10毫秒，防止粘包
  }
  Emm_V5_Synchronous_motion(0); 								 // 广播地址0触发
	HAL_Delay(10);																 // 每条命令后面延时10毫秒，防止粘包

}

void zdt_motor_task(void)
{
  static uint16_t vals[2];
  static uint32_t poses[2];

  if(((vals[0] != zdt_motors[0].vel_set) || (vals[1] != zdt_motors[1].vel_set)) || ((poses[0] != zdt_motors[0].clk_set) || (poses[1] != zdt_motors[1].clk_set)))
  {
    // zdt_speed_set();
    zdt_location_set();
    for(uint8_t i = 0; i < zdt_motor_num; i++)
    {
      vals[i] = zdt_motors[i].vel_set;
      poses[i] = zdt_motors[i].clk_set;
    }
  }

}
/********** 3-外设调用函数 **********/

