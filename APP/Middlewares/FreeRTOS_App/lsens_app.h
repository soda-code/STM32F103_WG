//******************************************************************************
//  File Name          : lsens_app.h
//  Description        : This file provides code for the lsens_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __LSENS_APP_H
#define __LSENS_APP_H

#include "./SYSTEM/sys/sys.h"

typedef enum
{
    LSENS_OFF = 0,
    LSENS_ON,
    LSENS_BUSY,
    LSENS_ERROR,
    LSENS_GET,
    LSENS_MAX,
} LSENS_StateTypeDef;

typedef struct
{
    LSENS_StateTypeDef state;
    double data; /* 传感器数据 */
} LSENS_PacketTypeDef;

void lsens_run(void);

#endif
