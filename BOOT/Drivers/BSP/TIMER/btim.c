#include "./BSP/LED/led.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/RADAR/radar.h"
TIM_HandleTypeDef g_timx_handle;      /* 定时器x句柄 */

/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    BTIM_TIMX_INT_CLK_ENABLE();                                      /* 使能TIMx时钟 */
    
    g_timx_handle.Instance = BTIM_TIMX_INT;                          /* 通用定时器x */
    g_timx_handle.Init.Prescaler = psc;                              /* 分频 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;             /* 向上计数器 */
    g_timx_handle.Init.Period = arr;                                 /* 自动装载值 */
    g_timx_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;       /* 时钟分频因子 */
    HAL_TIM_Base_Init(&g_timx_handle);
    
    HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 1, 3);  /* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);          /* 开启ITMx中断 */
    
    HAL_TIM_Base_Start_IT(&g_timx_handle);           /* 使能定时器x和定时器x更新中断 */
    
    
}

/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);
}

/**
 * @brief       定时器更新中断回调函数
* @param        htim:定时器句柄指针
 * @retval      无
 */
extern uint8_t radar_state ; /* 雷达状态变量 */
extern uint16_t radar_timer_count ; /* 雷达定时器计数变量 */
uint8_t lock =0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (htim == (&g_timx_handle))
    {
        if(radar_state )
        {
            if((RADAR_IN == GPIO_PIN_SET)&&(lock==0)) /* 如果雷达输入引脚为高电平且锁定变量为0 */
            {
                lock = 1; /* 锁定变量置1，表示正在测距 */
            }
            else if((RADAR_IN == GPIO_PIN_RESET)&&(lock==1)) /* 如果雷达输入引脚为低电平且锁定变量为1 */
            {
                lock = 0; /* 锁定变量清零，表示测距完成 */
            }
            if(lock == 1) /* 如果锁定变量为1，表示正在测距 */
            {
                radar_timer_count++; /* 雷达定时器计数变量加1 */
            }
		}
        //LED1_TOGGLE();
    }
}










