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
RTC_EXT void update_time(void);
RTC_EXT void data_convert(real_time_t data,uint8_t * buff);
RTC_EXT ret_code_t service_rtc_init(void);
RTC_EXT void ble_service_rtcTime_init(void);
RTC_EXT void bel_service_rtcTime_send(void);


#endif

