#include "./SYSTEM/sys/sys.h"
#include "./BSP/LED/led.h"
#include "iap.h"

/**
 * @brief       BOOT 引导程序入口
 * @note        1. 校验 APP 区 (0x08008000) 是否有效;
 *              2. 有效则等待 1.5s 升级命令, 超时直接跳转 APP;
 *              3. 无效或收到升级命令则常驻串口升级模式 (USART1, 115200).
 */
int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    led_init();                         /* 初始化LED */

    iap_run();                          /* IAP 引导: 升级或跳转APP */

    while (1)
    {
    }
}
