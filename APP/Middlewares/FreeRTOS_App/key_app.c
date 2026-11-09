#include "key_app.h"
#include "./BSP/KEY/key.h"

//******************************************************************************
//  File Name          : key_get_status
//  Description        : 获取当前按键状态
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
static KEY_StateTypeDef key_state = KEY_ON;

//******************************************************************************
//  File Name          : key_run
//  Description        : This function runs the key application logic
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

void key_run(void)
{
    switch (key_state)
    {
        case KEY_OFF:
            break;
        case KEY_ON:
            key_scan(0); /* 调用按键扫描函数，参数0表示不支持连续按 */
            break;
        default:
            break;
    }
}
