//******************************************************************************
//  File Name          : led_app.h
//  Description        : This file provides code for the led_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __LED_APP_H
#define __LED_APP_H

#include "./SYSTEM/sys/sys.h"

typedef enum
{
  LED_OFF = 0,
  LED_ON,
  LED_TOGGLE,
  LED_MAX
} LED_StateTypeDef;

void led_get_status(uint8_t *state);
void led_run(void);
#endif
