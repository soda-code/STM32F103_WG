//******************************************************************************
//  File Name          : date_read_app.h
//  Description        : This file provides code for the date_read_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __DATE_READ_APP_H
#define __DATE_READ_APP_H

#include "./SYSTEM/sys/sys.h"

typedef enum
{
  DATE_READ_OFF = 0,
  DATE_READ_ON,
  DATE_READ_MAX
} DATE_READ_StateTypeDef;

void date_read_get_status(uint8_t *state);
void date_read_run(void);

#endif

