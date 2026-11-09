#include "date_read_app.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/RTC/rtc.h"

//******************************************************************************
#include "FreeRTOS.h"
#include "task.h"

//******************************************************************************
//  File Name          : date_read_get_status
//  Description        : 获取当前DATE_READ状态
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
static DATE_READ_StateTypeDef date_read_state = DATE_READ_ON;

_calendar_obj rtc_time; /* RTC时间结构体 */

//******************************************************************************
//  File Name          : date_read_get_status
//  Description        : 获取当前DATE_READ状态
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
void date_read_get_status(uint8_t *state)
{
    if(state != NULL)
    {
        date_read_state = (DATE_READ_StateTypeDef)(*state); /* 将输入参数state转换为DATE_READ_StateTypeDef类型并赋值给date_read_state */
    }
    else
    {
        date_read_state = DATE_READ_MAX; /* 处理输入参数state为NULL的情况，可以选择返回错误码或者执行其他操作 */
    }
}
//******************************************************************************
//  File Name          : date_read_run
//  Description        : This function runs the DATE_READ application logic
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
void date_read_run(void)
{
    date_read_state = DATE_READ_ON; /* 设置状态为ON，开始获取时间 */
    switch (date_read_state)
    {
        case DATE_READ_OFF:
            // DATE_READ functionality off
            break;
        case DATE_READ_ON:
        	rtc_time = rtc_get_time(); /* 获取当前时间 */
            my_printf("DATE_READ: Time = %02d:%02d:%02d\r\n", rtc_time.hour, rtc_time.min, rtc_time.sec); /* 输出当前tick数和时间 */
            break;
        default:
            break;
    }
}


