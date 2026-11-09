#ifndef __LSENS_H
#define __LSENS_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* ADC及引脚 定义 */

#define LSENS_CHY_GPIO_PORT                  GPIOA
#define LSENS_CHY_GPIO_PIN                   GPIO_PIN_1 
#define LSENS_CHY_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)  /* PA口时钟使能 */

#define LSENS_ADCX                           ADC3 
#define LSENS_CHY                            ADC_CHANNEL_1                                /* 通道Y,  0 <= Y <= 17 */ 
#define LSENS_CHY_CLK_ENABLE()               do{ __HAL_RCC_ADC3_CLK_ENABLE(); }while(0)   /* ADC1 时钟使能 */


/******************************************************************************************/
/******************************************************************************************/
/* 光敏传感器对应ADC3的输入引脚和通道 定义 */

#define LSENS_ADC3_CHX_GPIO_PORT            GPIOF
#define LSENS_ADC3_CHX_GPIO_PIN             GPIO_PIN_8
#define LSENS_ADC3_CHX_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF口时钟使能 */


#define LSENS_CHX                           ADC_CHANNEL_6       /* 通道Y,  0 <= Y <= 17 */ 

/******************************************************************************************/
 

void    Lsens_init(void);          /* 初始化光敏传感器 */
uint16_t Lsens_get_val(void);      /* 读取光敏传感器的值 */
float Lsens_get_lux(void);        /* 计算光强 Lux */
#endif 





















