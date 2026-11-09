#include "lsens_app.h"
#include "./BSP/LSENS/lsens.h"
#include "freertos_user_app.h"

extern QueueHandle_t Lsens_Data_Queue;  /* 队列句柄 */
static LSENS_PacketTypeDef lsens_packet;

//******************************************************************************
//  File Name          : lsens_设置状态
//  Description        : 设置当前LSENS状态
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

void lsens_set_state(LSENS_StateTypeDef state)
{
    switch (state)
    {
        case LSENS_OFF:
            lsens_packet.state = LSENS_OFF;
            break;
        case LSENS_ON:
            lsens_packet.state = LSENS_GET;
            break;
        case LSENS_BUSY:
            lsens_packet.state = LSENS_BUSY;
            break;
        case LSENS_ERROR:
            lsens_packet.state = LSENS_ERROR;
            break;
        case LSENS_GET:
            lsens_packet.state = LSENS_GET;
            break;
        default:
            break;
    }
}


//******************************************************************************
//  File Name          : lsens_run
//  Description        : This function runs the LSENS application logic
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

void lsens_run(void)
{
    BaseType_t xStatus;
    switch (lsens_packet.state)
    {
        case LSENS_OFF:
            break;
        case LSENS_ON:
            break;
        case LSENS_BUSY:
            xStatus = uxQueueSpacesAvailable(Lsens_Data_Queue);
            if(xStatus != 0 ) // 队列已满，无法发送新数据
            {
                lsens_packet.state = LSENS_GET; /* 设置状态为ON，继续获取数据 */
            }
            break;
        case LSENS_ERROR:
            /* 处理错误状态的逻辑 */
            break;
        case LSENS_GET:
            lsens_packet.data = Lsens_get_lux(); /* 获取传感器数据并存储在lsens_packet.data中 */
            xStatus = uxQueueSpacesAvailable(Lsens_Data_Queue); // 获取队列剩余空间
            if(xStatus != 0 ) // 队列已满，无法发送新数据
            {
                xQueueSend(Lsens_Data_Queue, &lsens_packet.data, 0); /* 将数据发送到队列 */
            }
            else
            {
                lsens_packet.state = LSENS_BUSY; /* 设置状态为BUSY，等待队列有空间 */
            }   
  
            break;
        default:
            break;
    }
}
