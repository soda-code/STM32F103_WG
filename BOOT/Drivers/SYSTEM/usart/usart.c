#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <stdarg.h> /* 包含处理可变参数的头文件 */

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */

    USART_UX->DR = (uint8_t)ch;             /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/

#if USART_EN_RX /*如果使能了接收*/

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t g_usart_rx_sta = 0;

uint8_t g_rx_buffer[RXBUFFERSIZE];  /* HAL库使用的串口接收缓冲 */

UART_HandleTypeDef g_uart1_handle;  /* UART句柄 */

//**********************************************************************
// * @brief       串口X初始化函数
// * @param       baudrate: 波特率, 根据自己需要设置波特率值
// * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
// *              这里的USART的时钟源在sys_stm32_clock_init()函数中已经设置过了.
// * @retval      无
//**********************************************************************
void usart_init(uint32_t baudrate)
{
    g_uart1_handle.Instance = USART_UX;
    g_uart1_handle.Init.BaudRate = baudrate;
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&g_uart1_handle);

    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE); 
}

/**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;

    if (huart->Instance == USART_UX)                            /* 如果是串口1，进行串口1 MSP初始化 */
    {
        USART_TX_GPIO_CLK_ENABLE();                             /* 使能串口TX脚时钟 */
        USART_RX_GPIO_CLK_ENABLE();                             /* 使能串口RX脚时钟 */
        USART_UX_CLK_ENABLE();                                  /* 使能串口时钟 */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* 串口发送引脚号 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO速度设置为高速 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);
                
        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* 串口RX脚 模式设置 */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* 串口RX脚 必须设置成输入模式 */
       
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* 使能USART1中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* 组2，最低优先级:抢占优先级3，子优先级3 */

    }
}
//*****************************************************************
//* brife   :串口累加校验函数
//* param   :null
//* retval  :无
//***************************************************************** 
uint8_t usart_check_sum(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t sum = 0;

    for (i = 0; i < len; i++)
    {
        sum += buf[i];
    }

    return sum;
}

//*****************************************************************
//* brife :串口接收中断回调函数
//* param  :huart:串口句柄
//* retval :无
//***************************************************************** 
uint8_t data_get[USART_REC_LEN-2] = {0};
uint8_t *return_usart_receive_callback(void)
{
    if(data_get[0] != 0)
    {
        return data_get;
    }
    else
    {
        return NULL;
    }
}

//*****************************************************************
//* brife   :串口接受完成函数
//* param   :null
//* retval  :无
//***************************************************************** 
void usart_receive_complete(uint8_t *buf)
{
    uint8_t len = 0;
    uint8_t check_sum = 0;

    uint8_t data_sum = 0;
    
    g_usart_rx_buf[g_usart_rx_sta] = buf[0]; /* 将接收到的数据存入到接收缓冲数组中, 注意要防止越界 */
    if(g_usart_rx_sta < USART_REC_LEN)
    {
        g_usart_rx_sta ++;
    }
    else
    {
        g_usart_rx_sta = 0; /* 接收数据错误, 重新开始接收 */
    }
    if(g_usart_rx_sta >4)
    {
        if((g_usart_rx_buf[0] == 0xAA)&&(g_usart_rx_buf[1] == 0x55))
        {
            len      = g_usart_rx_buf[2];
            data_sum = g_usart_rx_buf[len+3];
            check_sum = usart_check_sum(&g_usart_rx_buf[0], len+3);
            if(check_sum == data_sum)
            {
                memcpy(data_get, &g_usart_rx_buf[2], len+1); /* 将接收到的数据存入到全局变量data_get中, 以便在其他地方使用 */
                memset(g_usart_rx_buf, 0, USART_REC_LEN); /* 清空接收缓冲数组，为下一次接收做准备 */
                /* 接收数据成功, 可以在这里添加接收完成后的处理代码 */
							g_usart_rx_sta=0;
            }
        }
    }
}

//*****************************************************************
//* brife :串口接收中断回调函数
//* param  :huart:串口句柄
//* retval :无
//***************************************************************** 

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART_UX)                    /* 如果是串口1 */
    {   
        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
				usart_receive_complete(g_rx_buffer);
    }
}

/**
 * @brief       串口X中断服务函数
                注意,读取USARTx->SR能避免莫名其妙的错误
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{
    HAL_UART_IRQHandler(&g_uart1_handle);                               /* 调用HAL库中断处理公用函数 */
}
#endif

//*****************************************************************
//* brife :重定义printf函数, 通过DMA串口发送格式化字符串
//* param  :format:格式化字符串, ...:可变参数列表   
//* retval :无
//*****************************************************************
volatile uint8_t uart_tx_busy = 0;
static char tx_buf[1024];

void my_printf(const char *format, ...)
{
    va_list args;

    while(uart_tx_busy);   // 等待发送完成

    uart_tx_busy = 1;

    va_start(args, format);
    vsnprintf(tx_buf, sizeof(tx_buf), format, args);
    va_end(args);

    HAL_UART_Transmit_DMA(&g_uart1_handle, (uint8_t *)tx_buf, strlen(tx_buf));
}

//*****************************************************************
//* brife :串口发送完成回调函数
//* param  :huart:串口句柄
//* retval :无
//*****************************************************************
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        uart_tx_busy = 0;
    }
}