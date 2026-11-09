#include "debug_app.h"
#include "./SYSTEM/usart/usart.h"
#include "./MALLOC/malloc.h"
//******************************************************************************
#include "FreeRTOS.h"
#include "task.h"

//******************************************************************************
//  File Name          : debug_get_status
//  Description        : 获取当前DEBUG状态
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
static DEBUG_StateTypeDef debug_state = DEBUG_ON;

void debug_get_status(uint8_t *state)
{
    if(state != NULL)
    {
        debug_state = (DEBUG_StateTypeDef)(*state); /* 将输入参数state转换为DEBUG_StateTypeDef类型并赋值给debug_state */
    }
    else
    {
        debug_state = DEBUG_MAX; /* 处理输入参数state为NULL的情况，可以选择返回错误码或者执行其他操作 */
    }
}
//******************************************************************************
//  File Name          : debug_run
//  Description        : This function runs the DEBUG application logic
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
   uint16_t mem_usage; /* 获取内存使用率 */
void debug_run(void)
{
    uint8_t send_buffer[50];
    static float tick_test = 0.0f;
    static TickType_t vTasktick = 0;
		tick_test+=1;
    vTasktick = xTaskGetTickCount(); /* 获取当前系统运行的tick数 */
 

    switch (debug_state)
    {
        case DEBUG_OFF:
            // Debug functionality off
            break;
        case DEBUG_ON:
        	mem_usage = my_mem_perused(SRAMIN); /* 获取内存使用率 */
           // my_printf("DEBUG: Tick Count = %lu, Memory Usage = %.2f%%\r\n", vTasktick, mem_usage / 10.0f); /* 输出当前tick数和内存使用率 */
            break;
        default:
            break;
    }
}


