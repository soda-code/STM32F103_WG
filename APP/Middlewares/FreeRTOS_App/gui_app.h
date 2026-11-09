//******************************************************************************
//  File Name          : gui_app.h
//  Description        : This file provides code for the gui_app.c file
//  Author             : MCD Application Team
//  Date               : 01-July-2016
//******************************************************************************

#ifndef __GUI_APP_H
#define __GUI_APP_H

#include "./SYSTEM/sys/sys.h"

void gui_run(void);
void LCD_Curve_Init(void);
void LCD_Curve_AddPoint(int16_t value);
void LCD_Curve_AddPoint_Fast(int16_t value);
#endif
