#include "gui_app.h"
#include "./BSP/bsp_app.h"
#include "freertos_user_app.h"
#include "radar_app.h"
#include "waring_sound_app.h"

extern QueueHandle_t Lsens_Data_Queue;  /* 队列句柄 */

extern WARING_SOUND_StateTypeDef waring_sound_state; /* 蜂鸣器状态变量，初始为关闭 */
extern float radar_distance; /* 雷达测距结果变量，单位为米 */
//******************************************************************************
//  File Name          : gui_run
//  Description        : This function runs the GUI application logic
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
void gui_run(void)
{
    uint8_t tick_disp[20] = {0};
    uint32_t tick_count = 0;
    float lux_value = 0.0;
    uint8_t lcd_buf[50]; /* LCD显示缓冲区 */
    while (1)
    {

        if (xQueueReceive(Lsens_Data_Queue, &lux_value, 0) == pdPASS)
        {
            lux_value = Lsens_get_lux(); /* 获取光强值并更新曲线显示 */
            sprintf(tick_disp, "Lux: %.2f", lux_value); /* 将光强值转换为字符串 */
            lcd_show_string(10, 40, lcddev.width - 20, lcddev.height - 20, 24, tick_disp, g_point_color); /* 显示光强值 */
        }
        sprintf((char*)lcd_buf, "Distance: %.3f m", radar_distance); /* 将距离格式化为字符串 */
        my_printf("Distance: %.3f m\r\n", radar_distance); /* 通过串口输出距离 */
        lcd_show_string(10, 10, 200, 50, 24, lcd_buf, RED); /* 在LCD上显示距离 */
        if (waring_sound_state == WARING_SOUND_HIGH)
        {
           lcd_fill(60, 100, 200, 300, BRRED); /* 在LCD上显示高级警告 */
        }
        else if (waring_sound_state == WARING_SOUND_MEDIUM)
        {
            lcd_fill(60, 100, 200, 300, YELLOW); /* 在LCD上显示中级警告 */
        }
        else if (waring_sound_state == WARING_SOUND_NORMAL)
        {
            lcd_fill(60, 100, 200, 300, GREEN); /* 在LCD上显示正常警告 */
        }
        else
        {
            lcd_fill(60, 100, 200, 300, BLUE); /* 在LCD上显示安全状态 */
        }

    }
}



