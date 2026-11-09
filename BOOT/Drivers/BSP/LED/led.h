#ifndef _LED_H
#define _LED_H
#include "./SYSTEM/sys/sys.h"

//***************************************************************************************
#define LED0_GPIO_PORT                  GPIOB
#define LED0_GPIO_PIN                   GPIO_PIN_5
#define LED0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)            

#define LED1_GPIO_PORT                  GPIOE
#define LED1_GPIO_PIN                   GPIO_PIN_5
#define LED1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)           


//***************************************************************************************/
typedef enum
{
    LED0_INDEX = 0,
    LED1_INDEX = 1
} Led_Channel;


/******************************************************************************************/
void led_init(void);                                                                     
void led_on(Led_Channel channel);                                                                    //
void led_off(Led_Channel channel);                                                                   //   

#endif
