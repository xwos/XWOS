/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * Copyright (c) 2023 xwos.tech.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
#include <time.h>

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
  sDate.Month = RTC_MONTH_AUGUST;
  sDate.Date = 1;
  sDate.Year = 23;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
xwer_t MX_RTC_Set_DateTime(struct tm * tm, suseconds_t ms)
{
  RTC_DateTypeDef rtcdate = {0};
  RTC_TimeTypeDef rtctime = {0};
  HAL_StatusTypeDef halrc;
  xwer_t rc;

  XWOS_UNUSED(ms);
  if (LL_RTC_HOURFORMAT_24HOUR == LL_RTC_GetHourFormat(hrtc.Instance)) {
    rtctime.Hours = tm->tm_hour;
    rtctime.TimeFormat = 0;
  } else {
    if (tm->tm_hour > 12) {
      rtctime.Hours = tm->tm_hour - 12;
      rtctime.TimeFormat = RTC_HOURFORMAT12_PM;
    } else {
      rtctime.Hours = tm->tm_hour;
      rtctime.TimeFormat = RTC_HOURFORMAT12_AM;
    }
  }
  rtctime.Minutes = tm->tm_min;
  rtctime.Seconds = tm->tm_sec;
  halrc = HAL_RTC_SetTime(&hrtc, &rtctime, RTC_FORMAT_BIN);
  if (HAL_OK == halrc) {
    rtcdate.WeekDay = (tm->tm_wday == 0) ? RTC_WEEKDAY_SUNDAY : tm->tm_wday;
    rtcdate.Month = tm->tm_mon + 1;
    rtcdate.Date = tm->tm_mday;
    rtcdate.Year = tm->tm_year + 1900 - 2000;
    halrc = HAL_RTC_SetDate(&hrtc, &rtcdate, RTC_FORMAT_BIN);
    if (HAL_OK == halrc) {
      rc = XWOK;
    } else {
      rc = -ETIMEDOUT;
    }
  } else {
    rc = -ETIMEDOUT;
  }
  return rc;
}

xwer_t MX_RTC_Get_DateTime(struct tm * tm, suseconds_t * ms)
{
  RTC_DateTypeDef rtcdate;
  RTC_TimeTypeDef rtctime;

  HAL_RTC_GetTime(&hrtc, &rtctime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &rtcdate, RTC_FORMAT_BIN);

  tm->tm_sec = rtctime.Seconds;
  tm->tm_min = rtctime.Minutes;
  tm->tm_hour = rtctime.Hours + (rtctime.TimeFormat * 12);
  tm->tm_mday = rtcdate.Date;
  tm->tm_mon = rtcdate.Month - 1;
  tm->tm_year = rtcdate.Year + 2000 - 1900;
  tm->tm_wday = (rtcdate.WeekDay == RTC_WEEKDAY_SUNDAY) ? 0 : rtcdate.WeekDay;
  tm->tm_yday = 0;
  tm->tm_isdst = 0;
  *ms = (((rtctime.SecondFraction - rtctime.SubSeconds) * 1000000) / (rtctime.SecondFraction + 1));
  return XWOK;
}

/* USER CODE END 1 */
