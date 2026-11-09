//******************************************************************************
//  File Name          : radar_app.h
//  Description        : This file provides code for the radar_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __RADAR_APP_H
#define __RADAR_APP_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/bsp_app.h"

typedef enum
{
    RADAR_OFF = 0,
    RADAR_ON,
    RADAR_ERR,
    RADAR_INIT,
    RADAR_BUSY,
    RADAR_UPDATE,
    RADAR_LOCK,
    RADAR_MAX,
} RADAR_StateTypeDef;

typedef struct
{
    RADAR_StateTypeDef state;
    uint8_t *data; /* 传感器数据 */
} RADAR_PacketTypeDef;

void radar_app_run(void);

#endif
