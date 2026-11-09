#include "freertos_app.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/RADAR/radar.h"
/*FreeRTOS*********************************************************************************************/
#include "freertos_user_app.h"
#include "gui_app.h"
#include "led_app.h"
#include "pc_cmd_app.h"
#include "lsens_app.h"
#include "debug_app.h"
#include "run_log_app.h"
#include "key_app.h"
#include "date_read_app.h"
#include "waring_sound_app.h"
#include "radar_app.h"
/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */


//* GUI_TASK 任务 配置
#define GUI_PRIO         2                      /* 任务优先级 */
#define GUI_STK_SIZE     512                   /* 任务堆栈大小 */
TaskHandle_t            Gui_Task_Handler;       /* 任务句柄 */
void gui_task(void *pvParameters);              /* 任务函数 */


//* DEBUG_TASK 任务 配置
#define DEBUG_PRIO      2                       /* 任务优先级 */
#define DEBUG_STK_SIZE  128                     /* 任务堆栈大小 */
TaskHandle_t            Debug_Task_Handler;     /* 任务句柄 */
void debug_task(void *pvParameters);           

/* LED_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define LED_TASK_PRIO      3                   /* 任务优先级 */
#define LED_TASK_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            LED_Task_Handler;  /* 任务句柄 */
void led_task(void *pvParameters);             /* 任务函数 */

//* LSENS_CMD_TASK 任务 配置
#define LSENS_CMD_PRIO         4                    /* 任务优先级 */
#define LSENS_CMD_STK_SIZE     128                  /* 任务堆栈大小 */
TaskHandle_t            LSENS_Cmd_Task_Handler;     /* 任务句柄 */
void lsens_cmd_task(void *pvParameters);            /* 任务函数 */
//* PC_CMD_TASK 任务 配置
#define PC_CMD_PRIO         4                       /* 任务优先级 */
#define PC_CMD_STK_SIZE     128                     /* 任务堆栈大小 */
TaskHandle_t            Pc_Cmd_Task_Handler;        /* 任务句柄 */
void pc_cmd_task(void *pvParameters);               /* 任务函数 */

//* KEY_TASK 任务 配置
#define KEY_PRIO         5                      /* 任务优先级 */
#define KEY_STK_SIZE     128                    /* 任务堆栈大小 */
TaskHandle_t            Key_Task_Handler;       /* 任务句柄 */
void key_task(void *pvParameters);              /* 任务函数 */

//* WARING_SOUND_TASK 任务 配置
#define WARING_SOUND_PRIO         5                     /* 任务优先级 */
#define WARING_SOUND_STK_SIZE     256                  /* 任务堆栈大小 */
TaskHandle_t            Waring_Sound_Task_Handler;      /* 任务句柄 */
void waring_sound_task(void *pvParameters);             /* 任务函数 */

//* LSENS_CMD_TASK 任务 配置
#define RUN_LOG_PRIO         6                      /* 任务优先级 */
#define RUN_LOG_STK_SIZE     1024                   /* 任务堆栈大小 */
TaskHandle_t            Run_Log_Task_Handler;       /* 任务句柄 */
void run_log_task(void *pvParameters);              /* 任务函数 */

//* RADAR_APP 任务 配置
#define RADAR_APP_PRIO         7                     /* 任务优先级 */
#define RADAR_APP_STK_SIZE     512                   /* 任务堆栈大小 */
TaskHandle_t            Radar_App_Task_Handler;      /* 任务句柄 */
void radar_app_task(void *pvParameters);             /* 任务函数 */

  /* 任务函数 */


TimerHandle_t RtcTimer;
void ReadDateCallback(TimerHandle_t xTimer);
/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_app(void)
{
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )led_task,                 /* 任务函数 */
                (const char*    )"led_task",               /* 任务名称 */
                (uint16_t       )LED_TASK_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )LED_TASK_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&LED_Task_Handler);   /* 任务句柄 */
    xTaskCreate((TaskFunction_t )key_task,                 /* 任务函数 */
                (const char*    )"key_task",               /* 任务名称 */
                (uint16_t       )KEY_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )KEY_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Key_Task_Handler);   /* 任务句柄 */
   xTaskCreate((TaskFunction_t )run_log_task,                 /* 任务函数 */
                (const char*    )"run_log_task",               /* 任务名称 */
                (uint16_t       )RUN_LOG_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )RUN_LOG_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Run_Log_Task_Handler);   /* 任务句柄 */
                    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )lsens_cmd_task,                 /* 任务函数 */
                (const char*    )"lsens_cmd_task",               /* 任务名称 */
                (uint16_t       )LSENS_CMD_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )LSENS_CMD_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&LSENS_Cmd_Task_Handler);   /* 任务句柄 */
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )pc_cmd_task,                 /* 任务函数 */
                (const char*    )"pc_cmd_task",               /* 任务名称 */
                (uint16_t       )PC_CMD_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )PC_CMD_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Pc_Cmd_Task_Handler);   /* 任务句柄 */
                    /* 创建任务4 */
    xTaskCreate((TaskFunction_t )waring_sound_task,                 /* 任务函数 */
                (const char*    )"waring_sound_task",               /* 任务名称 */
                (uint16_t       )WARING_SOUND_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )WARING_SOUND_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Waring_Sound_Task_Handler);   /* 任务句柄 */
    /* 创建任务4 */
    xTaskCreate((TaskFunction_t )gui_task,                 /* 任务函数 */
                (const char*    )"gui_task",               /* 任务名称 */
                (uint16_t       )GUI_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )GUI_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Gui_Task_Handler);   /* 任务句柄 */

    xTaskCreate((TaskFunction_t )radar_app_task,                 /* 任务函数 */
                (const char*    )"radar_app_task",               /* 任务名称 */
                (uint16_t       )RADAR_APP_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )RADAR_APP_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Radar_App_Task_Handler);   /* 任务句柄 */
    /* 创建任务3 */
    xTaskCreate((TaskFunction_t )debug_task,                 /* 任务函数 */
                (const char*    )"debug_task",               /* 任务名称 */
                (uint16_t       )DEBUG_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )DEBUG_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Debug_Task_Handler);   /* 任务句柄 */
                
    RtcTimer = xTimerCreate("RTC",pdMS_TO_TICKS(1000),pdTRUE,NULL,ReadDateCallback);
		if( RtcTimer != NULL ) 
    {
        // 启动定时器，等待最大 100ms 以确保命令发送成功
        if( xTimerStart( RtcTimer, pdMS_TO_TICKS( 1000 ) ) != pdPASS ) 
        {
        }
    }
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

//***************************************************************************************
// @function:       ReadDateCallback
// @description:    定时器回调函数，读取日期
// @parameters:     xTimer : 定时器句柄
// @return:         无
//***************************************************************************************

void ReadDateCallback(TimerHandle_t xTimer)
{
    date_read_run(); /* 调用获取时间函数，更新时间 */
}
/**
 * @brief       LED_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void led_task(void *pvParameters)
{
    while (1)
    {
        led_run(); /* 运行LED任务，处理LED相关逻辑 */
        vTaskDelay(1);
    }
}

/**
 * @brief       key_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void key_task(void *pvParameters)
{
    while (1)
    {
        key_run(); /* 运行按键任务，处理按键相关逻辑 */
        vTaskDelay(500);
    }
}

/**
 * @brief       run_log_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void run_log_task(void *pvParameters)
{
    while (1)
    {
        run_log_run(); /* 运行运行日志任务，处理相关逻辑 */
        vTaskDelay(500);
    }
}

/**
 * @brief       GUI_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */

void gui_task(void *pvParameters)
{
    while (1)
    {
        gui_run(); /* 运行GUI任务，处理相关逻辑 */    
        vTaskDelay(100);
    }
}
/**
 * @brief       waring_sound_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void waring_sound_task(void *pvParameters)
{
	while(1)
	{
      waring_sound_run(); /* 运行警告声音任务，处理相关逻辑 */
	    vTaskDelay(1);
	}
}

/**
 * @brief       radar_app_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void radar_app_task(void *pvParameters)
{
	while(1)
	{
    radar_app_run(); /* 运行雷达应用任务，处理相关逻辑 */
    vTaskDelay(20);
	}
}
/**
 * @brief       LSENS_CMD_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */

//********************************创建队列
#define LSENS_data_len 100  
#define LSENS_data_size sizeof(float)
QueueHandle_t Lsens_Data_Queue;  /* 队列句柄 */


void lsens_cmd_task(void *pvParameters)
{
    /* 创建队列 */
    lsens_set_state(LSENS_ON); /* 设置状态为ON，开始获取数据 */
    Lsens_Data_Queue = xQueueCreate(LSENS_data_len, LSENS_data_size);
    if (Lsens_Data_Queue == NULL)
    {
        my_printf("Failed to create Lsens_Data_Queue\r\n");
    }
    while (1)
    {
        lsens_run(); /* 运行LSENS命令任务，处理来自USART接口的LSENS相关命令 */
        vTaskDelay(100);
    }
}
/**
 * @brief       PC_CMD_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void pc_cmd_task(void *pvParameters)
{
    while (1)
    {
        pc_cmd_run(); /* 运行PC命令任务，处理来自USART接口的命令 */
        vTaskDelay(500);
    }
}

/**
 * @brief       DEBUG_TASK
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void debug_task(void *pvParameters)
{
    while (1)
    {
        debug_run(); /* 运行DEBUG任务，处理DEBUG相关逻辑 */
        vTaskDelay(1000);
    }
}

