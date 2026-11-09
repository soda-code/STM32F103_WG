#include "pc_cmd_app.h"
#include "led_app.h"

#include "./SYSTEM/usart/usart.h"


//******************************************************************************
// @brief   : 返回给PC的命令处理函数
// @param   : 当前的执行状态
// @retval  : 无
//******************************************************************************
void pc_cmd_return(bool status)
{
    if (status == 0)
    {
        /* 命令执行成功，可以发送成功状态回PC */
    }
    else
    {
        /* 命令执行失败，可以发送错误状态回PC */
    }
    /* 这里可以添加代码来处理命令执行后的返回值，例如发送状态信息回PC */
}

//******************************************************************************
//  @brief    : pc_cmd_run
//  @param    : This function runs the PC command task, which processes commands received from the USART interface.
//  @author   : MCD Application Team
//  @date     : 01-July-2016
// ******************************************************************************


void pc_cmd_run(void)
{
    bool cmd_received = false; /* 假设有一个标志位来指示是否接收到命令 */
    uint8_t *pc_data = return_usart_receive_callback();
    if (pc_data != NULL)
    {
        PC_CMD_PacketTypeDef pc_cmd_packet;
        pc_cmd_packet.cmd = (PC_CMD_TypeDef)pc_data[1]; /* 假设命令数据在接收数据的第一个字节 */
    
        switch (pc_cmd_packet.cmd)
        {
            case PC_CMD_LED:
                led_get_status(&pc_data[2]); /* 获取LED状态，假设状态数据在接收数据的第二个字节 */
                break;
            case PC_CMD_BEEP:
                /* 处理蜂鸣器命令 */
                break;
            case PC_CMD_LIGHT:
                /* 处理灯光命令 */
                break;
            case PC_CMD_LCD_BK:
                /* 处理LCD背光命令 */
                break;
        
            default:
                /* 处理未知命令 */
                break;
        }
        pc_cmd_return(cmd_received);
    }

}
