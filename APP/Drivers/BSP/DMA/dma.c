#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"

DMA_HandleTypeDef  g_dma_handle;            /* DMA句柄 */
DMA_HandleTypeDef  g_dma_rx_handle;         /* DMA句柄 */
extern UART_HandleTypeDef g_uart1_handle;   /* UART句柄 */


/**
 * @brief       串口TX DMA初始化函数
 *   @note      这里的传输形式是固定的, 这点要根据不同的情况来修改
 *              从存储器 -> 外设模式/8位数据宽度/存储器增量模式
 *
 * @param       dmax_chy    : DMA的通道, DMA1_Channel1 ~ DMA1_Channel7, DMA2_Channel1 ~ DMA2_Channel5
 *                            某个外设对应哪个DMA, 哪个通道, 请参考<<STM32中文参考手册 V10>> 10.3.7节
 *                            必须设置正确的DMA及通道, 才能正常使用! 
 * @retval      无
 */
void dma_init(DMA_Channel_TypeDef* DMAx_CHx)
{
    if ((uint32_t)DMAx_CHx > (uint32_t)DMA1_Channel4)     /* 大于DMA1_Channel7, 则为DMA2的通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                      /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                      /* DMA1时钟使能 */
    }
    

    /* Tx DMA配置 */
    g_dma_handle.Instance = DMAx_CHx;                               /* USART1_TX使用的DMA通道为: DMA1_Channel4 */
    g_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;             /* DIR = 1 , 存储器到外设模式 */
    g_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;                 /* 外设非增量模式 */
    g_dma_handle.Init.MemInc = DMA_MINC_ENABLE;                     /* 存储器增量模式 */
    g_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;    /* 外设数据长度:8位 */
    g_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;       /* 存储器数据长度:8位 */
    g_dma_handle.Init.Mode = DMA_NORMAL;                            /* DMA模式:正常模式 */
    g_dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM;               /* 中等优先级 */
    HAL_DMA_Init(&g_dma_handle);
		
	__HAL_LINKDMA(&g_uart1_handle, hdmatx, g_dma_handle);           /* 将DMA与USART1联系起来(发送DMA) */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    
}

//********************************************************************
// * @brief       串口RX DMA初始化函数
// *   @note      这里的传输形式是固定的, 这点要根据不同的情况来修改
// *              从存储器 -> 外设模式/8位数据宽度/存储器增量模式
// *
// * @param       dmax_chy    : DMA的通道, DMA1_Channel1 ~ DMA1_Channel7, DMA2_Channel1 ~ DMA2_Channel5
// *                            某个外设对应哪个DMA, 哪个通道, 请参考<<STM32中文参考手册 V10>> 10.3.7节
// *                            必须设置正确的DMA及通道, 才能正常使用! 
// * @retval      无
//********************************************************************
void dma_rx_init(DMA_Channel_TypeDef* DMAx_CHx)
{
    if ((uint32_t)DMAx_CHx > (uint32_t)DMA1_Channel4)     /* 大于DMA1_Channel7, 则为DMA2的通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                      /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                      /* DMA1时钟使能 */
    }

    /* Rx DMA配置 */    
    g_dma_rx_handle.Instance = DMAx_CHx;                               /* USART1_RX使用的DMA通道为: DMA1_Channel5 */
    g_dma_rx_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;             /* DIR = 0 , 外设到存储器模式 */
    g_dma_rx_handle.Init.PeriphInc = DMA_PINC_DISABLE;                 /* 外设非增量模式 */
    g_dma_rx_handle.Init.MemInc = DMA_MINC_ENABLE;                     /* 存储器增量模式 */
    g_dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;    /* 外设数据长度:8位 */
    g_dma_rx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;       /* 存储器数据长度:8位 */
    g_dma_rx_handle.Init.Mode = DMA_NORMAL;                            /* DMA模式:正常模式 */
    g_dma_rx_handle.Init.Priority = DMA_PRIORITY_MEDIUM;               /* 中等优先级 */
    HAL_DMA_Init(&g_dma_rx_handle);
		
	__HAL_LINKDMA(&g_uart1_handle, hdmarx, g_dma_rx_handle);           /* 将DMA与USART1联系起来(接收DMA) */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}
//*****************************************************************
//* @brief       DMA1_Channel4中断服务函数
// * @param       无
// * @retval      无
//***************************************************************** 
void DMA1_Channel4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_handle);
}

//*****************************************************************
//* @brief       DMA1_Channel5中断服务函数
// * @param       无
// * @retval      无
//***************************************************************** 
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_rx_handle);
}



















