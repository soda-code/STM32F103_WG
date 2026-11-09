/**
 ****************************************************************************************************
 * @file        iap_sd.c
 * @brief       SD 卡升级: 读取根目录 UPDATE.BIN 并写入 APP 区
 * @note        依赖 FatFS + SDIO (HAL_SD) ; 内部复用 iap.c 的 Flash 操作与 APP 校验接口
 ****************************************************************************************************
 */

#include "iap.h"
#include "iap_sd.h"
#include "./BSP/LED/led.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "ff.h"

#define IAP_SD_DRIVE            "0:"
#define IAP_SD_BUF_SIZE         (4U * 1024U)        /* 每次读取 4KB */
#define IAP_SD_REPORT_STEP      (64U * 1024U)       /* 每写 64KB 报告一次进度 */

static FATFS s_fs;                                  /* FatFs 文件系统对象 */
static FIL   s_file;                                /* 文件对象 */
static BYTE  s_buf[IAP_SD_BUF_SIZE + 1U];           /* 读缓冲 (+1 用于奇数长度处理) */
static uint8_t s_sd_ready = 0U;                     /* SD 卡是否已经初始化成功 */

/**
 * @brief       SD 卡初始化 (只做一次)
 * @retval      1: 可用; 0: 卡不存在/初始化失败
 */
static uint8_t iap_sd_ready_check(void)
{
    if (s_sd_ready != 0U)
    {
        return 1U;
    }

    if (Sdio_Init() != 0U)
    {
        return 0U;
    }

    s_sd_ready = 1U;
    return 1U;
}

/**
 * @brief       判断 SD 卡内是否存在升级文件
 * @retval      1: 存在; 0: 不存在或没有卡
 */
uint8_t iap_sd_update_present(void)
{
    FILINFO info;
    uint8_t ret = 0U;

    if (iap_sd_ready_check() == 0U)
    {
        return 0U;
    }

    if (f_mount(&s_fs, IAP_SD_DRIVE, 1) != FR_OK)
    {
        return 0U;
    }

    if (f_stat(IAP_SD_FILE_NAME, &info) == FR_OK)
    {
        ret = 1U;
    }

    f_mount(NULL, IAP_SD_DRIVE, 0);
    return ret;
}

/**
 * @brief       SD 卡升级: UPDATE.BIN -> APP 区
 * @retval      1: 升级成功且 APP 校验通过; 0: 失败
 */
uint8_t iap_sd_upgrade(void)
{
    FRESULT  res;
    UINT     br;
    uint16_t len;
    uint32_t size;
    uint32_t rd = 0U;                               /* 已读字节数 */
    uint32_t wr = 0U;                               /* 已写字节数 */
    uint32_t report = 0U;
    uint8_t  carry = 0U;                            /* 上一包剩余的奇数字节 */
    uint8_t  carry_valid = 0U;
    uint8_t  ret = 0U;

    if (iap_sd_ready_check() == 0U)
    {
        iap_printf("[boot] sd card not found\r\n");
        return 0U;
    }

    if (f_mount(&s_fs, IAP_SD_DRIVE, 1) != FR_OK)
    {
        iap_printf("[boot] sd mount failed\r\n");
        return 0U;
    }

    if (f_open(&s_file, IAP_SD_FILE_NAME, FA_READ) != FR_OK)
    {
        iap_printf("[boot] %s not found\r\n", IAP_SD_FILE_NAME);
        f_mount(NULL, IAP_SD_DRIVE, 0);
        return 0U;
    }

    size = (uint32_t)f_size(&s_file);
    iap_printf("[boot] sd image size: %u bytes\r\n", size);

    if ((size == 0U) || (size > IAP_APP_MAX_SIZE))
    {
        iap_printf("[boot] invalid image, max %u bytes\r\n", (uint32_t)IAP_APP_MAX_SIZE);
        goto exit_close;
    }

    iap_printf("[boot] erasing %u bytes, please wait...\r\n", size);
    if (iap_flash_erase(IAP_APP_ADDR, size) == 0U)
    {
        iap_printf("[boot] erase failed\r\n");
        goto exit_close;
    }

    while (rd < size)
    {
        UINT want = IAP_SD_BUF_SIZE - ((carry_valid != 0U) ? 1U : 0U);

        if (want > (size - rd))
        {
            want = size - rd;
        }

        res = f_read(&s_file, &s_buf[(carry_valid != 0U) ? 1U : 0U], want, &br);
        if ((res != FR_OK) || (br == 0U))
        {
            iap_printf("[boot] sd read failed (%d)\r\n", (int)res);
            goto exit_close;
        }

        if (carry_valid != 0U)
        {
            s_buf[0] = carry;
        }

        rd += br;
        len = (uint16_t)(br + ((carry_valid != 0U) ? 1U : 0U));

        /* 奇数长度时把最后一个字节留到下一包, 保证 Flash 半字写入对齐 */
        if ((len & 1U) != 0U)
        {
            carry = s_buf[len - 1U];
            carry_valid = 1U;
        }
        else
        {
            carry_valid = 0U;
        }
        len &= 0xFFFEU;

        if ((len != 0U) && (iap_flash_write(IAP_APP_ADDR + wr, s_buf, len) == 0U))
        {
            iap_printf("[boot] flash write failed at %u\r\n", wr);
            goto exit_close;
        }
        wr += len;

        if ((rd - report) >= IAP_SD_REPORT_STEP)
        {
            report = rd;
            iap_printf("[boot] writing %u/%u\r\n", rd, size);
            HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
        }
    }

    if (carry_valid != 0U)                          /* 补齐最后一个字节 */
    {
        s_buf[0] = carry;
        s_buf[1] = 0xFF;
        if (iap_flash_write(IAP_APP_ADDR + wr, s_buf, 2U) == 0U)
        {
            iap_printf("[boot] flash write failed at %u\r\n", wr);
            goto exit_close;
        }
        wr += 2U;
    }

    ret = iap_app_valid();
    iap_printf("[boot] sd upgrade %s, written %u bytes\r\n", (ret != 0U) ? "ok" : "failed", wr);

exit_close:
    f_close(&s_file);

    if (ret != 0U)
    {
        f_unlink(IAP_SD_DONE_NAME);                 /* 防止重名导致重命名失败 */
        f_rename(IAP_SD_FILE_NAME, IAP_SD_DONE_NAME);
    }

    f_mount(NULL, IAP_SD_DRIVE, 0);
    return ret;
}
