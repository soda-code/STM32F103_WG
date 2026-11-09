#include "./BSP/LED/led.h"


//***************************************************************************************
// @brief       LED初始化
// @param       无
// @retval      无
//***************************************************************************************
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    LED0_GPIO_CLK_ENABLE();                                 
    LED1_GPIO_CLK_ENABLE();                                

    gpio_init_struct.Pin = LED0_GPIO_PIN;                   
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            
    gpio_init_struct.Pull = GPIO_PULLUP;                    
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;         
    HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct);       

    gpio_init_struct.Pin = LED1_GPIO_PIN;                   
    HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct); 

    HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_RESET); //led 先关闭
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);  
    
}

//***************************************************************************************
// @brief       LED 开
// @param       无
// @retval      无
//***************************************************************************************
void led_on(Led_Channel channel)
{
    switch (channel)
    {
        case LED0_INDEX:
            HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_SET);
            break;
        case LED1_INDEX:
            HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}


//***************************************************************************************
// @brief       LED 关
// @param       无
// @retval      无
//***************************************************************************************

void led_off(Led_Channel channel)
{
    switch (channel)
    {
        case LED0_INDEX:
            HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_RESET);
            break;
        case LED1_INDEX:
            HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

