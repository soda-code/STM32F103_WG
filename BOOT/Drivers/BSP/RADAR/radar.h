#ifndef _RADAR_H
#define _RADAR_H
#include "./SYSTEM/sys/sys.h"

//***************************************************************************************
#define RADAR_GPIO_PORT                  GPIOD
#define RADAR_GPIO_PIN                   GPIO_PIN_9
#define RADAR_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)            

#define RADAR_IN_GPIO_PORT               GPIOD
#define RADAR_IN_GPIO_PIN                GPIO_PIN_10
#define RADAR_IN_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)
#define RADAR_IN       HAL_GPIO_ReadPin(RADAR_IN_GPIO_PORT, RADAR_IN_GPIO_PIN)     /* 读取WKUP引脚 */
//***************************************************************************************

void radar_init(void);                                                                     
void radar_on(void);                                                                    //
void radar_off(void);                                                                   //   

#endif
