/**
 ****************************************************************************************************
 * @file        iap.c
 * @brief       IAP 引导程序: APP 有效性校验 + 跳转 + 串口升级 (USART1) + SD 卡升级
 * @note        1. 上电后若 SD 卡根目录存在 UPDATE.BIN 则自动升级(见 iap_sd.c);
 *              2. 校验 APP 区, 无效则常驻串口升级模式;
 *              3. APP 有效时等待 IAP_WAIT_UPGRADE_MS 毫秒等待升级命令, 超时直接跳转 APP;
 *              4. 串口升级协议见 iap.h 文件头注释.
 ****************************************************************************************************
 */

#include "iap.h"
#include "iap_sd.h"
#include "./BSP/LED/led.h"
#include <stdarg.h>

/* 内部参数 */
#define IAP_UART_BAUDRATE           115200U
#define IAP_RX_BYTE_TIMEOUT_MS      20U             /* 单字节接收超时 */
#define IAP_FLASH_PAGE_SIZE         0x800U          /* STM32F103ZE 大容量产品, 页大小 2KB */
#define IAP_PRINT_BUF_SIZE          128U

static uint8_t  s_payload[IAP_FRAME_MAX_LEN];       /* 帧负载缓存 */
static uint32_t s_img_size = 0U;                    /* START 命令声明的固件长度 */
static uint32_t s_rx_bytes = 0U;                    /* 已经写入 Flash 的字节数 */

/* 函数声明 */
static void     iap_uart_init(uint32_t baudrate);
static void     iap_uart_send(const uint8_t *buf, uint16_t len);
static int      iap_uart_recv_byte(uint8_t *ch, uint32_t timeout_ms);
static int      iap_recv_frame(uint8_t *payload, uint16_t *len, uint32_t timeout_ms);
static void     iap_send_reply(uint8_t cmd, uint8_t result);
static uint32_t iap_get_u32(const uint8_t *p);
static uint8_t  iap_process_frame(uint8_t *payload, uint16_t len);
static void     iap_upgrade_loop(void);

/*==================================================================================================
 *                                          串口部分
 *================================================================================================*/

/**
 * @brief       USART1 初始化 (寄存器方式, 不占用 HAL 串口句柄/中断)
 * @param       baudrate: 波特率
 */
static void iap_uart_init(uint32_t baudrate)
{
    GPIO_InitTypeDef gpio_init_struct;
    uint32_t pclk2;
    uint32_t brr;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    gpio_init_struct.Pin   = GPIO_PIN_9;                /* PA9 : TX */
    gpio_init_struct.Mode  = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull  = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    gpio_init_struct.Pin   = GPIO_PIN_10;               /* PA10 : RX */
    gpio_init_struct.Mode  = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    pclk2 = HAL_RCC_GetPCLK2Freq();
    if (pclk2 == 0U)
    {
        pclk2 = 72000000U;
    }

    brr = (pclk2 + (baudrate / 2U)) / baudrate;

    USART1->CR1 = 0U;
    USART1->BRR = brr;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

/**
 * @brief       发送一个字节
 */
static void iap_uart_putc(uint8_t ch)
{
    while ((USART1->SR & USART_SR_TXE) == 0U)
    {
    }
    USART1->DR = ch;
}

/**
 * @brief       发送一段数据
 */
static void iap_uart_send(const uint8_t *buf, uint16_t len)
{
    uint16_t i;

    for (i = 0U; i < len; i++)
    {
        iap_uart_putc(buf[i]);
    }
    while ((USART1->SR & USART_SR_TC) == 0U)
    {
    }
}

/**
 * @brief       接收一个字节 (带超时)
 * @retval      1: 收到; 0: 超时
 */
static int iap_uart_recv_byte(uint8_t *ch, uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();

    while ((USART1->SR & USART_SR_RXNE) == 0U)
    {
        if ((HAL_GetTick() - start) >= timeout_ms)
        {
            return 0;
        }
    }

    *ch = (uint8_t)(USART1->DR & 0xFFU);
    return 1;
}

/**
 * @brief       串口格式化打印 (阻塞发送)
 */
void iap_printf(const char *fmt, ...)
{
    char buf[IAP_PRINT_BUF_SIZE];
    va_list args;
    int n;

    va_start(args, fmt);
    n = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (n > 0)
    {
        if (n > (int)(sizeof(buf) - 1U))
        {
            n = (int)(sizeof(buf) - 1U);
        }
        iap_uart_send((const uint8_t *)buf, (uint16_t)n);
    }
}

/*==================================================================================================
 *                                          协议部分
 *================================================================================================*/

/**
 * @brief       接收一帧数据
 * @param       payload: 输出, 帧负载 (payload[0] 为命令字)
 * @param       len    : 输出, 负载长度
 * @param       timeout_ms: 每字节接收超时
 * @retval      1: 收到完整且校验正确的帧; 0: 超时/校验错
 */
static int iap_recv_frame(uint8_t *payload, uint16_t *len, uint32_t timeout_ms)
{
    uint8_t  ch;
    uint8_t  sum;
    uint8_t  crc;
    uint8_t  head_state = 0U;
    uint16_t n;
    uint16_t i;

    /* 找帧头 AA 55 */
    while (1)
    {
        if (!iap_uart_recv_byte(&ch, timeout_ms))
        {
            return 0;
        }

        if (head_state == 0U)
        {
            if (ch == IAP_FRAME_HEAD0)
            {
                head_state = 1U;
            }
        }
        else
        {
            if (ch == IAP_FRAME_HEAD1)
            {
                break;
            }
            head_state = (ch == IAP_FRAME_HEAD0) ? 1U : 0U;
        }
    }

    /* 长度 */
    if (!iap_uart_recv_byte(&ch, timeout_ms))
    {
        return 0;
    }
    n = ch;
    if ((n == 0U) || (n > IAP_FRAME_MAX_LEN))
    {
        return 0;
    }

    /* 负载 */
    sum = (uint8_t)(IAP_FRAME_HEAD0 + IAP_FRAME_HEAD1 + n);
    for (i = 0U; i < n; i++)
    {
        if (!iap_uart_recv_byte(&ch, timeout_ms))
        {
            return 0;
        }
        payload[i] = ch;
        sum = (uint8_t)(sum + ch);
    }

    /* 校验和 */
    if (!iap_uart_recv_byte(&crc, timeout_ms))
    {
        return 0;
    }
    if (crc != sum)
    {
        return 0;
    }

    *len = n;
    return 1;
}

/**
 * @brief       回应答帧
 */
static void iap_send_reply(uint8_t cmd, uint8_t result)
{
    uint8_t frame[6];

    frame[0] = IAP_FRAME_HEAD0;
    frame[1] = IAP_FRAME_HEAD1;
    frame[2] = 2U;
    frame[3] = result;
    frame[4] = cmd;
    frame[5] = (uint8_t)(frame[0] + frame[1] + frame[2] + frame[3] + frame[4]);

    iap_uart_send(frame, 6U);
}

/**
 * @brief       取 4 字节小端整数
 */
static uint32_t iap_get_u32(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

/*==================================================================================================
 *                                    Flash 操作 / APP 跳转
 *================================================================================================*/

/**
 * @brief       擦除 APP 区
 * @param       addr: 起始地址 (必须 >= IAP_APP_ADDR)
 * @param       size: 需要擦除的字节数
 * @retval      1: 成功; 0: 失败
 */
uint8_t iap_flash_erase(uint32_t addr, uint32_t size)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t page_error = 0U;
    uint32_t pages;
    uint8_t  ret = 0U;

    if ((size == 0U) || (addr < IAP_APP_ADDR)
        || ((addr + size) > (IAP_APP_ADDR + IAP_APP_MAX_SIZE)))
    {
        return 0U;
    }

    pages = (size + IAP_FLASH_PAGE_SIZE - 1U) / IAP_FLASH_PAGE_SIZE;

    erase.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase.Banks       = FLASH_BANK_1;
    erase.PageAddress = addr;
    erase.NbPages     = pages;

    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&erase, &page_error) == HAL_OK)
    {
        ret = 1U;
    }
    HAL_FLASH_Lock();

    return ret;
}

/**
 * @brief       写 APP 区 (半字编程, 地址与长度必须是偶数)
 * @retval      1: 成功; 0: 失败
 */
uint8_t iap_flash_write(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint16_t half_word;
    uint8_t  ret = 1U;

    if (((addr & 1U) != 0U) || ((len & 1U) != 0U) || (len == 0U))
    {
        return 0U;
    }
    if ((addr + len) > (IAP_APP_ADDR + IAP_APP_MAX_SIZE))
    {
        return 0U;
    }

    HAL_FLASH_Unlock();
    for (i = 0U; i < len; i += 2U)
    {
        half_word = (uint16_t)buf[i] | ((uint16_t)buf[i + 1U] << 8);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, half_word) != HAL_OK)
        {
            ret = 0U;
            break;
        }
    }
    HAL_FLASH_Lock();

    return ret;
}

/**
 * @brief       校验 APP 是否有效 (栈顶在 SRAM 内 + 复位向量在 APP 区内)
 * @retval      1: 有效; 0: 无效
 */
uint8_t iap_app_valid(void)
{
    uint32_t sp = *(volatile uint32_t *)IAP_APP_ADDR;               /* 栈顶地址 */
    uint32_t rv = *(volatile uint32_t *)(IAP_APP_ADDR + 4U);        /* 复位向量 */

    if ((sp & 0x2FFE0000U) != 0x20000000U)
    {
        return 0U;
    }
    if ((rv < IAP_APP_ADDR) || (rv >= (IAP_APP_ADDR + IAP_APP_MAX_SIZE)))
    {
        return 0U;
    }
    if ((rv & 1U) == 0U)                                            /* Thumb 状态 */
    {
        return 0U;
    }

    return 1U;
}

/**
 * @brief       关闭中断并跳转到 APP
 */
void iap_jump_to_app(void)
{
    uint32_t sp = *(volatile uint32_t *)IAP_APP_ADDR;
    uint32_t rv = *(volatile uint32_t *)(IAP_APP_ADDR + 4U);
    uint32_t i;

    sys_intx_disable();

    /* 关闭并清空 SysTick */
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;

    /* 关闭所有中断通道, 清除所有挂起中断 */
    for (i = 0U; i < 8U; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }

    /* 关闭 BOOT 使用过的外设, 让 APP 从干净状态启动 */
    USART1->CR1 = 0U;
    SDIO->POWER = 0U;

    /* 中断向量表重定位到 APP */
    SCB->VTOR = IAP_APP_ADDR;

    /* 设置 APP 栈顶并跳转到 APP 复位向量 */
    sys_msr_msp(sp);
    ((void (*)(void))rv)();
}

/*==================================================================================================
 *                                          命令处理
 *================================================================================================*/

/**
 * @brief       处理一帧命令
 * @param       payload: 帧负载 (payload[0] = 命令字)
 * @param       len    : 负载长度
 * @retval      1: 已进入/保持升级模式; 0: 未知命令, 继续等待
 */
static uint8_t iap_process_frame(uint8_t *payload, uint16_t len)
{
    uint8_t  cmd = payload[0];
    uint32_t size;
    uint32_t offset;
    uint16_t data_len;

    switch (cmd)
    {
        case IAP_CMD_PING:
            iap_send_reply(cmd, IAP_ACK);
            iap_printf("[boot] upgrade command received\r\n");
            return 1U;

        case IAP_CMD_START:
            if (len < 5U)
            {
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }

            size = iap_get_u32(&payload[1]);
            if ((size == 0U) || (size > IAP_APP_MAX_SIZE))
            {
                iap_printf("[boot] invalid image size: %u\r\n", size);
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }

            iap_printf("[boot] erasing %u bytes, please wait...\r\n", size);
            if (iap_flash_erase(IAP_APP_ADDR, size) == 0U)
            {
                iap_printf("[boot] erase failed\r\n");
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }

            s_img_size = size;
            s_rx_bytes = 0U;
            iap_send_reply(cmd, IAP_ACK);
            iap_printf("[boot] erase done, ready to receive\r\n");
            return 1U;

        case IAP_CMD_DATA:
            if (len < 7U)
            {
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }

            offset   = iap_get_u32(&payload[1]);
            data_len = (uint16_t)(len - 5U);

            /* 偏移必须顺序递增, 数据长度必须为偶数 */
            if ((s_img_size == 0U) || ((data_len & 1U) != 0U) || ((offset & 1U) != 0U)
                || (offset != s_rx_bytes) || ((offset + data_len) > s_img_size))
            {
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }

            if (iap_flash_write(IAP_APP_ADDR + offset, &payload[5], data_len) == 0U)
            {
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }

            s_rx_bytes += data_len;
            iap_send_reply(cmd, IAP_ACK);
            return 1U;

        case IAP_CMD_END:
            if ((s_img_size == 0U) || (s_rx_bytes != s_img_size) || (iap_app_valid() == 0U))
            {
                iap_printf("[boot] verify failed, %u/%u bytes\r\n", s_rx_bytes, s_img_size);
                iap_send_reply(cmd, IAP_NACK);
                s_img_size = 0U;
                s_rx_bytes = 0U;
                return 1U;
            }

            iap_send_reply(cmd, IAP_ACK);
            iap_printf("[boot] upgrade success, jump to app\r\n");
            HAL_Delay(100);
            iap_jump_to_app();
            return 1U;

        case IAP_CMD_JUMP:
            if (iap_app_valid() == 0U)
            {
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }
            iap_send_reply(cmd, IAP_ACK);
            HAL_Delay(50);
            iap_jump_to_app();
            return 1U;

        case IAP_CMD_SD_UPGRADE:
            if (iap_sd_upgrade() == 0U)
            {
                iap_send_reply(cmd, IAP_NACK);
                return 1U;
            }
            iap_send_reply(cmd, IAP_ACK);
            iap_printf("[boot] jump to app\r\n");
            HAL_Delay(100);
            iap_jump_to_app();
            return 1U;

        default:
            iap_send_reply(cmd, IAP_NACK);
            return 0U;
    }
}

/*==================================================================================================
 *                                          主流程
 *================================================================================================*/

/**
 * @brief       升级模式: 持续等待并处理命令, 直到升级完成跳转 APP
 */
static void iap_upgrade_loop(void)
{
    uint16_t len;
    uint32_t blink_tick;

    iap_printf("[boot] enter upgrade mode, waiting firmware...\r\n");
    led_on(LED0_INDEX);
    blink_tick = HAL_GetTick();

    while (1)
    {
        if (iap_recv_frame(s_payload, &len, IAP_RX_BYTE_TIMEOUT_MS))
        {
            iap_process_frame(s_payload, len);
            blink_tick = HAL_GetTick();
        }

        /* 未收到数据时 LED0 慢闪, 指示处于升级模式 */
        if ((HAL_GetTick() - blink_tick) >= 500U)
        {
            blink_tick = HAL_GetTick();
            HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
        }
    }
}

/**
 * @brief       IAP 引导入口: 校验 APP -> 等待升级命令 -> 跳转 APP
 * @note        正常情况不会返回 (跳转 APP 或常驻升级模式)
 */
void iap_run(void)
{
    uint16_t len;
    uint32_t start;

    iap_uart_init(IAP_UART_BAUDRATE);

    iap_printf("\r\n[boot] IAP bootloader, app addr = 0x%08X, size = %uKB\r\n",
               (uint32_t)IAP_APP_ADDR, (uint32_t)(IAP_APP_MAX_SIZE / 1024U));

#if (IAP_SD_AUTO_UPGRADE != 0)
    /* SD 卡升级: 卡内存在 UPDATE.BIN 时优先执行 */
    if (iap_sd_update_present() != 0U)
    {
        iap_printf("[boot] sd update file found\r\n");
        led_on(LED0_INDEX);

        if (iap_sd_upgrade() != 0U)
        {
            iap_printf("[boot] sd upgrade success, jump to app\r\n");
            HAL_Delay(100);
            iap_jump_to_app();
        }

        iap_printf("[boot] sd upgrade failed\r\n");
        led_off(LED0_INDEX);
    }
#endif

    if (iap_app_valid() == 0U)
    {
        iap_printf("[boot] no valid app found\r\n");
        iap_upgrade_loop();
    }

    iap_printf("[boot] app is valid, wait %ums for upgrade command...\r\n",
               (uint32_t)IAP_WAIT_UPGRADE_MS);

    start = HAL_GetTick();
    while ((HAL_GetTick() - start) < IAP_WAIT_UPGRADE_MS)
    {
        if (iap_recv_frame(s_payload, &len, IAP_RX_BYTE_TIMEOUT_MS))
        {
            if (iap_process_frame(s_payload, len) != 0U)
            {
                iap_upgrade_loop();                 /* 进入升级模式 */
            }
        }
    }

    iap_printf("[boot] no upgrade request, jump to app\r\n");
    HAL_Delay(50);
    iap_jump_to_app();
}
