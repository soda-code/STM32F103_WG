/**
 ****************************************************************************************************
 * @file        diskio.c
 * @brief       FatFs 底层磁盘接口 (BOOT 专用, 仅支持 SD 卡, 卷标 0:)
 ****************************************************************************************************
 */

#include "ff.h"
#include "diskio.h"
#include "./BSP/SDIO/sdio_sdcard.h"

#define SD_CARD     0       /* SD 卡, 卷标为 0 */

static uint8_t s_sd_inited = 0U;    /* 避免每次 mount 都重复初始化 SD 卡 */

/**
 * @brief       获得磁盘状态
 */
DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv != SD_CARD)
    {
        return STA_NOINIT;
    }
    return 0;
}

/**
 * @brief       初始化磁盘
 */
DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv != SD_CARD)
    {
        return STA_NOINIT;
    }

    if (s_sd_inited != 0U)
    {
        return 0;
    }

    if (Sdio_Init() != 0U)
    {
        return STA_NOINIT;
    }

    s_sd_inited = 1U;
    return 0;
}

/**
 * @brief       读扇区
 */
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if ((pdrv != SD_CARD) || (count == 0U))
    {
        return RES_PARERR;
    }

    return (sd_read_disk(buff, (uint32_t)sector, count) == 0U) ? RES_OK : RES_ERROR;
}

/**
 * @brief       写扇区
 */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if ((pdrv != SD_CARD) || (count == 0U))
    {
        return RES_PARERR;
    }

    return (sd_write_disk((uint8_t *)buff, (uint32_t)sector, count) == 0U) ? RES_OK : RES_ERROR;
}

/**
 * @brief       获取其他控制参数
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv != SD_CARD)
    {
        return RES_PARERR;
    }

    switch (cmd)
    {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(WORD *)buff = (WORD)g_sdcard_handler.SdCard.BlockSize;
            return RES_OK;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = (DWORD)(((uint64_t)g_sdcard_handler.SdCard.BlockNbr
                                      * g_sdcard_handler.SdCard.BlockSize) / 512U);
            return RES_OK;

        default:
            return RES_PARERR;
    }
}

/**
 * @brief       给 FatFs 提供时间 (BOOT 不需要 RTC, 固定返回 0)
 */
DWORD get_fattime(void)
{
    return 0;
}
