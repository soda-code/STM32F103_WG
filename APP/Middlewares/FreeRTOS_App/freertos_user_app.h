//******************************************************************************
//  File Name          : freertos_user_app.h
//  Description        : This file provides code for the freertos_user_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __FREERTOS_USER_APP_H
#define __FREERTOS_USER_APP_H

#include "./SYSTEM/sys/sys.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

typedef enum
{
    FREERTOS_USER_APP_OFF = 0,
    FREERTOS_USER_APP_ON,
    FREERTOS_USER_APP_MAX
} FREERTOS_USER_APP_StateTypeDef;

typedef struct
{
    FREERTOS_USER_APP_StateTypeDef state;

} FREERTOS_USER_APP_HandleTypeDef;

#endif

