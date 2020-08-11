#ifndef __RTC_H
#define __RTC_H

#ifdef RTC_GLOBAL
#define RTC_EXT 
#else
#define RTC_EXT extern
#endif

#include "nrf_drv_rtc.h"

RTC_EXT void RTC1_Init(void);

#endif

