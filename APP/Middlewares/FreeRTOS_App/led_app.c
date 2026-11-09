#include "led_app.h"
#include "./BSP/LED/led.h"
#include "freertos_user_app.h"
//******************************************************************************
//  File Name          : led_get_status
//  Description        : 获取当前LED状态
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
static LED_StateTypeDef led_state = LED_TOGGLE;

void led_get_status(uint8_t *state)
{
    if(state != NULL)
    {
        led_state = (LED_StateTypeDef)(*state); /* 将输入参数state转换为LED_StateTypeDef类型并赋值给led_state */
    }
    else
    {
        led_state = LED_MAX; /* 处理输入参数state为NULL的情况，可以选择返回错误码或者执行其他操作 */
    }
}

//***************************************************************************************
// @brief       LED 闪烁
// @param       间隔时间，单位毫秒
// @retval      无
    //***************************************************************************************
static void led_toggle(Led_Channel channel, uint32_t time)
{
    switch (channel)
    {
        case LED0_INDEX:
            HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
            break;
        case LED1_INDEX:
            HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN);
            break;
        default:
            break;
    }
    vTaskDelay(pdMS_TO_TICKS(time)); /* 延时 */
}
//******************************************************************************
//  File Name          : led_run
//  Description        : This function runs the LED application logic
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

void led_run(void)
{
    switch (led_state)
    {
        case LED_OFF:
            led_on(LED0_INDEX);
            break;
        case LED_ON:
            led_off(LED0_INDEX);
            break;
        case LED_TOGGLE:
            led_toggle(LED0_INDEX, 20);
            break;
        default:
            break;
    }
}
