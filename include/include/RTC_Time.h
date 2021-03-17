/****************************************Copyright (c)****************************************************
**
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               RTC_Time.h
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/

#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "myType.h"
#include "time.h"
#include <string.h>
/* Private function prototypes -----------------------------------------------*/
void RTC_Init(void);
// struct tm Time_GetCalendarTime(void);
struct tm Time_GetCalendarTime(unsigned int *ktime);
void Time_Battery_Display(u16 sx, u16 sy, u8 size, u8 mode);
void Time_Regulate(void);
char* Time_Date_Feedback(u8 feedbackmode);
void copy_date_and_time(void);
void Time_Regulate_For_Menu(void);
void Set_RTC_Calibration(u16 fastsecper30days);
u16 Get_RTC_Calibration_RegValue(void);
time_t Convert_TimeToUnix(RTC_TimeTypeDef stime, RTC_DateTypeDef sdata);
/* Private variables ---------------------------------------------------------*/
extern FunctionalState TimeDisplay;
extern tmType readtime;
extern char timeanddate[24];
extern struct tm set_time;
extern u8 time_set_flag;
extern u16 FastSecPerOnedays;
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
