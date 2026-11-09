#include "waring_sound_app.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/RADAR/radar.h"
#include "freertos_user_app.h"
#include "radar_app.h"

RADAR_StateTypeDef enRadarState = RADAR_UPDATE; /* 雷达状态变量，初始为关闭 */
extern WARING_SOUND_StateTypeDef waring_sound_state; /* 蜂鸣器状态变量，初始为关闭 */


uint8_t radar_state = 0; /* 雷达状态变量，0表示关闭，1表示打开 */
uint16_t radar_timer_count = 0; /* 雷达定时器计数变量 */
float radar_distance = 0.0; /* 雷达测距结果变量 */
extern uint8_t lock; /* 锁定变量，0表示未锁定，1表示锁定 */
static uint16_t radar_timer_count_OLD = 0; /* 上一次雷达定时器计数变量的值，用于计算距离变化 */
//******************************************************************************
//  FUNCTION        : radar_distance_waring
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************
void radar_distance_waring(void)
{
    if (radar_distance == 0.0f) /* 如果距离为0，表示测距异常 */
    {
        waring_sound_state = WARING_SOUND_OFF;
    }
    else if (radar_distance < 0.3f) /* 如果距离小于0.5米，表示非常近，发出高级警告声音 */
    {
        waring_sound_state = WARING_SOUND_HIGH;
    }
    else if (radar_distance < 0.6f) /* 如果距离小于1米，表示较近，发出中等警告声音 */
    {
        waring_sound_state = WARING_SOUND_MEDIUM;
    }
    else if (radar_distance < 0.9f) /* 如果距离小于2米，表示近，发出正常警告声音 */
    {
        waring_sound_state = WARING_SOUND_NORMAL;
    }
    else /* 如果距离大于等于2米，表示安全，关闭警告声音 */
    {
        waring_sound_state = WARING_SOUND_OFF;
    }
    
}
//******************************************************************************
//  FUNCTION        : radar_distance_update
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************


void radar_distance_update(void)
{
       if(radar_state == 0)
        {
             radar_on(); /* 雷达开 */
             vTaskDelay(10);
             radar_off(); /* 雷达关 */
             radar_state = 1; /* 雷达状态变量置1，表示正在测距 */
        }
        else
        {
            if(radar_state == 1)
            {
                if (radar_timer_count >= 2200) /* 如果雷达定时器计数变量超过一定值，表示测距异常，重置状态 */
                {
                    radar_timer_count=0; /* 将雷达定时器计数变量恢复到上一次的值，避免测距结果异常 */
                }
                
                radar_distance= radar_timer_count * 0.034f / 20.0f; /* 根据计数值计算距离，单位为米 */
                radar_state = 0;
                radar_timer_count = 0; /* 雷达定时器计数变量清零 */
                lock = 0; /* 锁定变量清零 */
            }
            
        }
        radar_distance_waring(); /* 根据测距结果更新警告声音状态 */
}

//******************************************************************************
//  FUNCTION        : radar_app_run
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************

void radar_app_run(void)
{
    switch (enRadarState)
    {
        case RADAR_OFF:

            break;
        case RADAR_ON:
       
            break;
        case RADAR_ERR:
         
            break;
        case RADAR_INIT:
      
            break;
        case RADAR_UPDATE:
            radar_distance_update(); /* 更新雷达测距结果 */
            break;
        case RADAR_LOCK:
            break;
        default:
            break;
    }
}
