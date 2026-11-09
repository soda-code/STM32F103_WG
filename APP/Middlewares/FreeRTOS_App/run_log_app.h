//******************************************************************************
//  File Name          : run_log_app.h
//  Description        : This file provides code for the run_log_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __RUN_LOG_APP_H
#define __RUN_LOG_APP_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/bsp_app.h"

typedef enum
{
    RUN_LOG_OFF = 0,
    RUN_LOG_ON,
    RUN_LOG_INIT,
    RUN_LOG_IDLE,
    RUN_LOG_ERROR,
    RUN_LOG_WRITE_ERROR,
    RUN_LOG_WRITE_CONFIG,
    RUN_LOG_WRITE_DATA,
    RUN_LOG_READ_ERROR,
    RUN_LOG_READ_CONFIG,
    RUN_LOG_READ_DATA,
    RUN_LOG_MAX,
} RUN_LOG_StateTypeDef;

typedef struct
{
    RUN_LOG_StateTypeDef state;
    uint8_t *data; /* 传感器数据 */
} RUN_LOG_PacketTypeDef;

void run_log_run(void);

#endif
