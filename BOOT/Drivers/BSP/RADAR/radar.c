#include "./BSP/RADAR/radar.h"
    

//***************************************************************************************
// @brief       RADAR初始化
// @param       无
// @retval      无
//***************************************************************************************
void radar_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    RADAR_GPIO_CLK_ENABLE();     
    RADAR_IN_GPIO_CLK_ENABLE();
                                
    gpio_init_struct.Pin = RADAR_GPIO_PIN;                   
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            
    gpio_init_struct.Pull = GPIO_PULLUP;                    
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;         
    HAL_GPIO_Init(RADAR_GPIO_PORT, &gpio_init_struct);       

    //
    
    gpio_init_struct.Pin = RADAR_IN_GPIO_PIN;                       /* WKUP引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                      /* 下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(RADAR_IN_GPIO_PORT, &gpio_init_struct);           /* WKUP引脚模式设置,下拉输入 */
 
    
}

//***************************************************************************************
// @brief       LED 开
// @param       无
// @retval      无
//***************************************************************************************
void radar_on(void)
{
    HAL_GPIO_WritePin(RADAR_GPIO_PORT, RADAR_GPIO_PIN, GPIO_PIN_SET);
}


//***************************************************************************************
// @brief       LED 关
// @param       无
// @retval      无
//***************************************************************************************

void radar_off(void)
{
    HAL_GPIO_WritePin(RADAR_GPIO_PORT, RADAR_GPIO_PIN, GPIO_PIN_RESET);
}

