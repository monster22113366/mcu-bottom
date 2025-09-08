#ifndef CAN_M2006_H
#define CAN_M2006_H
#include "stm32f1xx_hal.h"

/*************** 宏定义 *****************/
#define CHASSIS_CAN (hcan)
#define GIMBAL_CAN (hcan)

#define my_can_1 (hcan)

typedef enum {
  CAN_CHASSIS_ALL_ID = 0x200,
  CAN_3508_M1_ID = 0x201,
  CAN_3508_M2_ID = 0x202,
  CAN_3508_M3_ID = 0x203,
  CAN_3508_M4_ID = 0x204,

  CAN_YAW_MOTOR_ID = 0x205,
  CAN_PIT_MOTOR_ID = 0x206,
  CAN_TRIGGER_MOTOR_ID = 0x207,
  CAN_GIMBAL_ALL_ID = 0x1FF,

} can_msg_id_e;

typedef struct {
  uint16_t ecd;
  int16_t speed_rpm;
  int16_t given_current;
  uint8_t temperate;
  int16_t last_ecd;
} motor_measure_t;
/*************** 宏定义 *****************/

/*************** 函数声明 *****************/
void can_filter_init(void); //can启动&滤波
void CAN_cmd_motor(int16_t motor1, int16_t motor2, int16_t motor3,
                   int16_t motor4, uint8_t _case); //电机控制函数
void CAN_cmd_chassis_reset_ID(void); //电机id重置
const motor_measure_t *get_chassis_motor_measure_point(uint8_t i); //获取电机数据
/*************** 函数声明 *****************/

#endif // CAN_M2006_H

