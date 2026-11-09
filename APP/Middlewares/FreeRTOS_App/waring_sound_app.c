#include "waring_sound_app.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/RADAR/radar.h"
#include "freertos_user_app.h"

WARING_SOUND_StateTypeDef waring_sound_state = WARING_SOUND_OFF; /* 蜂鸣器状态变量，初始为关闭 */
//******************************************************************************
//  FUNCTION        : waring_sound_off
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************
void waring_sound_off(void)
{
    beep_off(); /* 关闭蜂鸣器 */

}

//******************************************************************************
//  FUNCTION        : waring_sound_on
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************
void waring_sound_on(void)
{
    beep_on(); /* 打开蜂鸣器 */
}

//******************************************************************************
//  FUNCTION        : waring_sound_normal
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************
void waring_sound_normal(void)
{
    beep_on(); /* 正常警告声音，打开蜂鸣器 */
    vTaskDelay(100);
    beep_off();
    vTaskDelay(100);
}

//******************************************************************************
//  FUNCTION        : waring_sound_medium
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************
void waring_sound_medium(void)
{
    beep_on();      // 中等警告声音，打开蜂鸣器
    vTaskDelay(50); // 延时50ms
    beep_off();     // 关闭蜂鸣器
    vTaskDelay(50); // 延时50ms
}

//******************************************************************************
//  FUNCTION        : waring_sound_high
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016  
//******************************************************************************
void waring_sound_high(void)
{
    beep_on();          // 高级警告声音，打开蜂鸣器
    vTaskDelay(10);     // 延时10ms
}

//******************************************************************************
//  FUNCTION        : waring_sound_run
//  DESCRIPTION     : This function runs the key application logic
//  INPUT           : None
//  OUTPUT          : None
//  RETURN          : None
//  AUTHOR          : MCD Application Team
//  DATE            : 01-July-2016
//******************************************************************************

void waring_sound_run(void)
{
    switch (waring_sound_state)
    {
        case WARING_SOUND_OFF:
            waring_sound_off(); /* 关闭蜂鸣器 */
            break;
        case WARING_SOUND_ON:
            waring_sound_on(); /* 打开蜂鸣器 */
            break;
        case WARING_SOUND_NORMAL:
            waring_sound_normal(); /* 正常警告声音，打开蜂鸣器 */
            break;
        case WARING_SOUND_MEDIUM:
            waring_sound_medium(); /* 中等警告声音，打开蜂鸣器 */
            break;
        case WARING_SOUND_HIGH:
            waring_sound_high(); /* 高级警告声音，打开蜂鸣器 */
            break;
        default:
             break;
    }
}
