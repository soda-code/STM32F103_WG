//******************************************************************************
//  File Name          : debug_app.h
//  Description        : This file provides code for the debug_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __DEBUG_APP_H
#define __DEBUG_APP_H

#include "./SYSTEM/sys/sys.h"

typedef enum
{
  DEBUG_OFF = 0,
  DEBUG_ON,
  DEBUG_MAX
} DEBUG_StateTypeDef;

void debug_get_status(uint8_t *state);
void debug_run(void);

#endif

