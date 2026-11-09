//******************************************************************************
//  File Name          : pc_cmd_app.h
//  Description        : This file provides code for the pc_cmd_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
// ******************************************************************************

#ifndef __PC_CMD_APP_H
#define __PC_CMD_APP_H

#include "led_app.h"
#include "./SYSTEM/sys/sys.h"

typedef enum
{
  PC_CMD_NONE = 0,
  PC_CMD_LED,
  PC_CMD_BEEP,
  PC_CMD_LIGHT,
  PC_CMD_LCD_BK,
  PC_CMD_MAX
} PC_CMD_TypeDef;

typedef struct
{
  PC_CMD_TypeDef cmd;
} PC_CMD_PacketTypeDef;

void pc_cmd_run(void);

#endif
