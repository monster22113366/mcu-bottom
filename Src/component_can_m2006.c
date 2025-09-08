/********** m2006使用流程 **********/
// 1.调用can_filter_init，启动can，激活中断并设置滤波
// 2. 调用CAN_cmd_motor，发送can，让电机转动
// 3. HAL_CAN_RxFifo0MsgPendingCallback中自动获取电机数据
// 4. 在定时器中将获取的速度位置等赋值给pid，调用pid函数进行输出更新
/****************************************/
#include "User.h"

/********** 全局变量 **********/
static motor_measure_t motor_chassis[7];
/********** 全局变量 **********/

/********** 2-外设硬件初始化函数 **********/
void can_filter_init(void) {

  CAN_FilterTypeDef can_filter_st;
  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
  can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
  can_filter_st.FilterIdHigh = 0x0000;
  can_filter_st.FilterIdLow = 0x0000;
  can_filter_st.FilterMaskIdHigh = 0x0000;
  can_filter_st.FilterMaskIdLow = 0x0000;

  can_filter_st.FilterBank = 0;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
  HAL_CAN_ConfigFilter(&my_can_1, &can_filter_st);
  HAL_CAN_Start(&my_can_1);
  HAL_CAN_ActivateNotification(&my_can_1, CAN_IT_RX_FIFO0_MSG_PENDING);
}
/********** 2-外设硬件初始化函数 **********/

/********** 3-外设调用函数 **********/
// motor data read
#define get_motor_measure(ptr,data)                                         {                                                                            \
    (ptr)->last_ecd = (ptr)->ecd;                                              \
    (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);                       \
    (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);                 \
    (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);             \
    (ptr)->temperate = (data)[6];                                              \
}



/**
 * @brief          hal CAN fifo call back, receive motor data
 * @param[in]      hcan, the point to CAN handle
 * @retval         none
 */
/**
 * @brief          hal库CAN回调函数,接收电机数据
 * @param[in]      hcan:CAN句柄指针
 * @retval         none
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];

  HAL_CAN_GetRxMessage(my_can_1, CAN_RX_FIFO0, &rx_header, rx_data);

  switch (rx_header.StdId) {
  case CAN_3508_M1_ID:
  case CAN_3508_M2_ID:
  case CAN_3508_M3_ID:
  case CAN_3508_M4_ID:
  case CAN_YAW_MOTOR_ID:
  case CAN_PIT_MOTOR_ID:
  case CAN_TRIGGER_MOTOR_ID: {
    static uint8_t i = 0;
    // get motor id
    i = rx_header.StdId - CAN_3508_M1_ID;
    get_motor_measure(&motor_chassis[i], rx_data);
    break;
  }

  default: {
    break;
  }
  }
}





/**
 * @brief          发送电机控制电流(0x201,0x202,0x203,0x204)
 * @param[in]      motor1: (0x201) 3508电机控制电流, 范围 [-16384,16384]
 * @param[in]      motor2: (0x202) 3508电机控制电流, 范围 [-16384,16384]
 * @param[in]      motor3: (0x203) 3508电机控制电流, 范围 [-16384,16384]
 * @param[in]      motor4: (0x204) 3508电机控制电流, 范围 [-16384,16384]
 * @retval         none·
 */
void CAN_cmd_motor(int16_t motor1, int16_t motor2, int16_t motor3,
                     int16_t motor4, uint8_t _case) {
  uint32_t send_mail_box;
  CAN_TxHeaderTypeDef motor_tx_message;
  uint8_t motor_can_send_data[8];
  uint32_t STDID;
  CAN_HandleTypeDef can_handler;
  if (_case) {
    can_handler = GIMBAL_CAN;
    STDID = CAN_GIMBAL_ALL_ID;
  } else {
    can_handler = CHASSIS_CAN;
    STDID = CAN_CHASSIS_ALL_ID;
  }
  motor_tx_message.StdId = STDID;
  motor_tx_message.IDE = CAN_ID_STD;
  motor_tx_message.RTR = CAN_RTR_DATA;
  motor_tx_message.DLC = 0x08;
  motor_can_send_data[0] = motor1 >> 8;
  motor_can_send_data[1] = motor1;
  motor_can_send_data[2] = motor2 >> 8;
  motor_can_send_data[3] = motor2;
  motor_can_send_data[4] = motor3 >> 8;
  motor_can_send_data[5] = motor3;
  motor_can_send_data[6] = motor4 >> 8;
  motor_can_send_data[7] = motor4;

  HAL_CAN_AddTxMessage(&can_handler, &motor_tx_message, motor_can_send_data,
                       &send_mail_box);
}



/**
 * @brief          返回底盘电机 3508电机数据指针
 * @param[in]      i: 电机编号,范围[0,7]
 * @retval         电机数据指针
 */
const motor_measure_t *get_chassis_motor_measure_point(uint8_t i) {
  return &motor_chassis[(i & 0x07)];
}



/**
 * @brief          发送ID为0x700的CAN包,它会设置3508电机进入快速设置ID
 * @param[in]      none
 * @retval         none
 */
void CAN_cmd_chassis_reset_ID(void) {
  uint32_t send_mail_box;
  CAN_TxHeaderTypeDef chassis_tx_message;
  uint8_t chassis_can_send_data[8];
  chassis_tx_message.StdId = 0x700;
  chassis_tx_message.IDE = CAN_ID_STD;
  chassis_tx_message.RTR = CAN_RTR_DATA;
  chassis_tx_message.DLC = 0x08;
  chassis_can_send_data[0] = 0;
  chassis_can_send_data[1] = 0;
  chassis_can_send_data[2] = 0;
  chassis_can_send_data[3] = 0;
  chassis_can_send_data[4] = 0;
  chassis_can_send_data[5] = 0;
  chassis_can_send_data[6] = 0;
  chassis_can_send_data[7] = 0;

  HAL_CAN_AddTxMessage(&CHASSIS_CAN, &chassis_tx_message, chassis_can_send_data,
                       &send_mail_box);
}
/********** 3-外设调用函数 **********/
