#ifndef __IAP_SD_H
#define __IAP_SD_H

#include "./SYSTEM/sys/sys.h"

/*
 * SD 卡升级: 把固件文件拷贝到 SD 卡根目录, 命名为 UPDATE.BIN (8.3 大写, 不支持长文件名),
 * 然后上电(或在串口升级模式下发送 0x15 命令), 引导程序会自动读取该文件并写入 APP 区.
 * 升级成功后文件被重命名为 UPDATE.OK, 避免每次上电重复升级.
 */
#define IAP_SD_FILE_NAME        "0:/UPDATE.BIN"     /* 升级文件 */
#define IAP_SD_DONE_NAME        "0:/UPDATE.OK"      /* 升级完成后重命名 */

uint8_t iap_sd_update_present(void);                /* 卡内是否存在升级文件 */
uint8_t iap_sd_upgrade(void);                       /* 执行 SD 卡升级, 1: 成功 */

#endif
