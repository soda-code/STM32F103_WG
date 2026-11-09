#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  // TIM3 时钟使能

#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          GPIO_PIN_5
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   // PB口时钟使能


#define GTIM_TIMX_PWM_CHY_GPIO_REMAP()      do{__HAL_RCC_AFIO_CLK_ENABLE();\
                                                __HAL_AFIO_REMAP_TIM3_PARTIAL();\
                                            }while(0)           // TIM3部分重映射, 将TIM3_CH2~CH4的输出重映射到PB4~PB6, 这里我们用到的是TIM3_CH2, 所以重映射到PB5

#define GTIM_TIMX_PWM                       TIM3 
#define GTIM_TIMX_PWM_CHY                   TIM_CHANNEL_2                           // 通道Y,  1<= Y <=4
#define GTIM_TIMX_PWM_CHY_CCRX              TIM3->CCR2                              // 通道Y的捕获/比较寄存器
#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)   // TIM3 时钟使能


#define GTIM_TIMX_CAP_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CAP_CHY_GPIO_PIN             GPIO_PIN_0
#define GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   // PA口时钟使能

#define GTIM_TIMX_CAP                          TIM5                       
#define GTIM_TIMX_CAP_IRQn                     TIM5_IRQn
#define GTIM_TIMX_CAP_IRQHandler               TIM5_IRQHandler
#define GTIM_TIMX_CAP_CHY                      TIM_CHANNEL_1                        // 通道Y,  1<= Y <=4
#define GTIM_TIMX_CAP_CHY_CCRX                 TIM5->CCR1               // 通道Y的捕获/比较寄存器
#define GTIM_TIMX_CAP_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM5_CLK_ENABLE(); }while(0)    // TIM5 时钟使能


#define GTIM_TIMX_CNT_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CNT_CHY_GPIO_PIN             GPIO_PIN_0
#define GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   // PA口时钟使能

#define GTIM_TIMX_CNT                          TIM2
#define GTIM_TIMX_CNT_IRQn                     TIM2_IRQn
#define GTIM_TIMX_CNT_IRQHandler               TIM2_IRQHandler
#define GTIM_TIMX_CNT_CHY                      1                            // 通道Y,  1<= Y <=2
#define GTIM_TIMX_CNT_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)    // TIM2 时钟使能

/******************************************************************************************/

void gtim_timx_int_init(uint16_t arr, uint16_t psc);        // 定时器中断初始化函数
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    // 通用定时器 PWM 初始化函数
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc);    // 通用定时器 输入捕获 初始化函数
void gtim_timx_cnt_chy_init(uint16_t psc);                  // 通用定时器 脉冲计数 初始化函数
uint32_t gtim_timx_cnt_chy_get_count(void);                 // 通用定时器 获取脉冲计数 函数
void gtim_timx_cnt_chy_restart(void);                       // 通用定时器 重启脉冲计数 函数

#endif

















