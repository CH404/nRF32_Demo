#ifndef __RTC_H
#define __RTC_H

#ifdef RTC_GLOBAL
#define RTC_EXT 
#else
#define RTC_EXT extern
#endif

#include "nrf_drv_rtc.h"


typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hours;
	uint8_t minute;
	uint8_t second;
	uint8_t week;
}real_time_t;





RTC_EXT void RTC1_Init(void);
RTC_EXT void RTC2_init(nrfx_rtc_handler_t handler);

#endif

