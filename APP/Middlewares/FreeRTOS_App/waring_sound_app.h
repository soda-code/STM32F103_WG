//******************************************************************************
//  File Name          : key_app.h
//  Description        : This file provides code for the key_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __WARING_SOUND_APP_H
#define __WARING_SOUND_APP_H
  
#include "./SYSTEM/sys/sys.h"

typedef enum
{
    WARING_SOUND_OFF = 0,
    WARING_SOUND_ON,
    WARING_SOUND_NORMAL,
    WARING_SOUND_MEDIUM,
    WARING_SOUND_HIGH,
    WARING_SOUND_MAX
} WARING_SOUND_StateTypeDef;

void waring_sound_run(void);

#endif
