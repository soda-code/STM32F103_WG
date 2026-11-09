#ifndef __IAP_H
#define __IAP_H

#include "./SYSTEM/sys/sys.h"

/*==============================================================================================
 * Flash 分区 (STM32F103ZE, 512KB Flash, 页大小 2KB)
 *   0x08000000 ~ 0x08007FFF : BOOT  (32KB)   <- 本工程
 *   0x08008000 ~ 0x0807FFFF : APP   (480KB)  <- 由本引导程序升级
 * 注意: APP 工程的 IROM1 起始地址必须改为 0x08008000,
 *       并且 APP 中要把中断向量表重定位到 0x08008000
 *       (VECT_TAB_OFFSET = 0x8000, 或者启动后 SCB->VTOR = 0x08008000)
 *============================================================================================*/
#define IAP_APP_ADDR                0x08008000U     /* APP 运行地址 */
#define IAP_APP_MAX_SIZE            0x00078000U     /* APP 区最大容量 480KB */

/*==============================================================================================
 * 串口协议 (USART1: PA9-TX / PA10-RX, 115200 8N1)
 *
 * 请求帧:  AA 55 LEN PAYLOAD[LEN] SUM
 *   LEN     : PAYLOAD 字节数, 1 ~ 133, PAYLOAD[0] 为命令字
 *   SUM     : AA + 55 + LEN + PAYLOAD[0..LEN-1] 的 8 位累加和 (取低 8 位)
 *
 * 应答帧:  AA 55 02 RESULT CMD SUM
 *   RESULT  : 0x79 成功(ACK) / 0x1F 失败(NACK)
 *   CMD     : 对应的命令字
 *
 * 升级流程: PING -> START(固件长度) -> DATA(偏移+数据, 顺序发送) ... -> END(固件长度)
 *============================================================================================*/
#define IAP_FRAME_HEAD0             0xAAU
#define IAP_FRAME_HEAD1             0x55U
#define IAP_FRAME_MAX_LEN           133U            /* 133 = 命令字1 + 偏移4 + 数据128 */

#define IAP_CMD_PING                0x10U           /* 无参, 应答 ACK; 用于进入升级模式 */
#define IAP_CMD_START               0x11U           /* + 固件长度(4B, 小端), 擦除对应扇区后应答 */
#define IAP_CMD_DATA                0x12U           /* + 偏移(4B, 小端) + 数据(偶数长度, 顺序发送) */
#define IAP_CMD_END                 0x13U           /* + 固件长度(4B, 小端), 校验通过后跳转 APP */
#define IAP_CMD_JUMP                0x14U           /* 无参, 直接跳转 APP */
#define IAP_CMD_SD_UPGRADE          0x15U           /* 无参, 用 SD 卡中的 UPDATE.BIN 升级 */

#define IAP_ACK                     0x79U
#define IAP_NACK                    0x1FU

/* 上电后等待升级命令的时间(ms), 超时且 APP 有效则直接跳转 APP */
#define IAP_WAIT_UPGRADE_MS         1500U

/* 上电时自动检测 SD 卡中的升级文件 (1: 开启, 0: 只响应串口 0x15 命令) */
#define IAP_SD_AUTO_UPGRADE         1

void iap_run(void);

/*==============================================================================================
 * 内部接口 (供 iap_sd.c 复用)
 *============================================================================================*/
void    iap_printf(const char *fmt, ...);
uint8_t iap_app_valid(void);
uint8_t iap_flash_erase(uint32_t addr, uint32_t size);
uint8_t iap_flash_write(uint32_t addr, const uint8_t *buf, uint16_t len);
void    iap_jump_to_app(void);

#endif
