//******************************************************************************
//  File Name          : key_app.h
//  Description        : This file provides code for the key_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __KEY_APP_H
#define __KEY_APP_H

#include "./SYSTEM/sys/sys.h"

typedef enum
{
  KEY_OFF = 0,
  KEY_ON,
  KEY_MAX
} KEY_StateTypeDef;

void key_run(void);

#endif
