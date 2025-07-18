#include "User.h"
#include <stdint.h>

/********** 全局变量 **********/
uint8_t key_num;                            //led的数量
static key_t keys[] =  //初始化led引脚实例，移植时可用修改这里的端口，引脚
{
    {GPIOE, GPIO_PIN_0, GPIO_HIGH},
    {GPIOE, GPIO_PIN_1, GPIO_HIGH},
};
/********** 全局变量 **********/



/********** 1-外设数量初始化函数 **********/
void keynum_init(void)
{
    key_num = sizeof(keys)/sizeof(key_t);
}
/********** 1-外设数量初始化函数 **********/


/********** 3-外设调用函数 **********/
static uint8_t key_read(void)
{
    uint8_t key_value = 0;
    for(uint8_t i = 0; i < key_num; i++)
    {
        key_value |= ((!(GPIO_ReadPin(keys[i].port, keys[i].pin))) << i);
    }
    return key_value;
}

void key_task(void)
{
    static uint8_t key_value,key_down,key_up,key_last;
    key_value = key_read();
    key_down = key_value & (key_value ^ key_last);
    key_up = ~key_value & (key_value ^ key_last);
    key_last = key_value;

    if(key_down & 0x01 && !(key_down & 0x02))
    {
        
    }
    else if(key_down & 0x02 && !(key_down & 0x01))
    {
        
    }
    else if(key_down & 0x03)
    {
        
    }

    if(key_up)
    {
        
    }

}
/********** 3-外设调用函数 **********/

