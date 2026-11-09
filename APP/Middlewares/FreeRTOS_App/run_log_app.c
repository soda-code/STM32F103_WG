#include "run_log_app.h"
#include "fatfs_app.h"
//******************************************************************************
//  File Name          : run_log_on
//  Description        : 获取当前RUN_LOG状态
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
static RUN_LOG_PacketTypeDef run_log_packet;

void run_log_set_state(RUN_LOG_StateTypeDef state)
{
    switch (state)
    {
        case RUN_LOG_OFF:
            run_log_packet.state = RUN_LOG_OFF;
            break;
        case RUN_LOG_ON:
            run_log_packet.state = RUN_LOG_ON;
            break;
        case RUN_LOG_ERROR:
            run_log_packet.state = RUN_LOG_ERROR;   
            break;
        case RUN_LOG_WRITE_ERROR:
            run_log_packet.state = RUN_LOG_WRITE_ERROR;
            break;
        case RUN_LOG_WRITE_CONFIG:
            run_log_packet.state = RUN_LOG_WRITE_CONFIG;
            break;
        case RUN_LOG_WRITE_DATA:
            run_log_packet.state = RUN_LOG_WRITE_DATA;
            break;
        case RUN_LOG_READ_ERROR:
            run_log_packet.state = RUN_LOG_READ_ERROR;
            break;
        case RUN_LOG_READ_CONFIG:
            run_log_packet.state = RUN_LOG_READ_CONFIG;
            break;
        case RUN_LOG_READ_DATA:
            run_log_packet.state = RUN_LOG_READ_DATA;
            break;
        default:
            break;
    }

}

//**************************************************************************
//  File Name          : scan_sd_card
//  Description        : This function scans the SD card for log files
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************
void scan_sd_card(void) 
{
    FRESULT res = FR_OK;
    res = f_mount(fs[0], "0:", 1);
    if (res == FR_OK) 
    {
        my_printf("FatFS 挂载成功！\n");
        run_log_packet.state = RUN_LOG_IDLE; // 切换到IDLE状态，准备进行读写测试
    } 
    else 
    {
        my_printf("挂载失败，错误代码: %d\n", res);
        run_log_packet.state = RUN_LOG_ERROR; // 切换到ERROR状态
    }
}

//**************************************************************************
//  File Name          : FatFS_SD_Test
//  Description        : This function tests SD card read/write using FatFS
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

void FatFS_SD_Test(void)
{
    FATFS fs;           // 文件系统对象
    FIL fil;            // 文件对象
    FRESULT res;        // FatFS 返回值
    UINT bw, br;        // 实际写入/读取的字节数
    char write_buf[] = "Hello SD Card! This is a test message."; // 待写入的数据
    char read_buf[64];  // 缓存读取的数据

    res = f_open(&fil, "0:test.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if (res == FR_OK) 
    {
        // 3. 写入数据
        res = f_write(&fil, write_buf, sizeof(write_buf), &bw);
        if (res == FR_OK && bw > 0) 
        {
            my_printf("写入成功! 写入内容: %s\r\n", write_buf);
        } 
        else 
        {
            my_printf("写入失败 (%d)\r\n", res);
        }

        // 4. 将文件指针移回文件开头，准备读取
        f_lseek(&fil, 0);

        // 5. 读取数据
        memset(read_buf, 0, sizeof(read_buf)); // 清空缓冲区
        res = f_read(&fil, read_buf, sizeof(read_buf), &br);
        if (res == FR_OK && br > 0) 
        {
            my_printf("读取成功! 读取内容: %s\r\n", read_buf);
        } 
        else 
        {
            my_printf("读取失败 (%d)\r\n", res);
        }

        // 6. 数据对比校验
        if (strcmp(write_buf, read_buf) == 0) 
        {
            my_printf(">>> 读写测试通过！数据一致。 <<<\r\n");
        } 
        else 
        {
            my_printf(">>> 警告：读写数据不匹配！ <<<\r\n");
        }

        // 7. 关闭文件
        f_close(&fil);
    } 
    else 
    {
        my_printf("文件打开失败 (%d)\r\n", res);
    }
    run_log_packet.state = RUN_LOG_OFF; // 测试完成，保持在IDLE状态等待下一次操作

}

//**************************************************************************
//  File Name          : run_log_run
//  Description        : This function runs the RUN_LOG application logic
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************


void run_log_run(void)
{
    switch (run_log_packet.state)
    {
        case RUN_LOG_OFF:
            break;
        case RUN_LOG_ON:
            break;
        case RUN_LOG_INIT:
						scan_sd_card();
            break;
        case RUN_LOG_IDLE:
            FatFS_SD_Test();
            break;
        case RUN_LOG_ERROR:
            break;
        case RUN_LOG_WRITE_ERROR:
            break;
        case RUN_LOG_WRITE_CONFIG:
            break;
        case RUN_LOG_WRITE_DATA:
            break;
        case RUN_LOG_READ_ERROR:
            break;
        case RUN_LOG_READ_CONFIG:
            break;
        case RUN_LOG_READ_DATA:
            break;
        default:
            break;
    }
}
