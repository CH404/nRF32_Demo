#ifndef __RTC_H
#define __RTC_H

#ifdef RTC_GLOBAL
#define RTC_EXT 
#else
#define RTC_EXT extern
#endif

#include "nrf_drv_rtc.h"
#include "ble.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"


#define BLE_RTCDATE_BLE_OBSERVER_PRIO 2

typedef struct
{
	uint16_t rtc_connect;
	uint16_t rtc_service_handle;
	ble_gatts_char_handles_t rtc_characteristic_date_handle;
}ble_rtcdate_t;


#define BLE_RTCDATE_DEF(_name)\
static ble_rtcdate_t _name;\
NRF_SDH_BLE_OBSERVER(_name ## _obs,\
	BLE_RTCDATE_BLE_OBSERVER_PRIO,\
        ble_RTCDATE_on_ble_evt,\
        &_name)



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

RTC_EXT real_time_t current_time;
RTC_EXT SemaphoreHandle_t DateUpdateSem;


void ble_RTCDATE_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context);


RTC_EXT void RTC1_Init(void);
RTC_EXT void RTC2_init(nrfx_rtc_handler_t handler);
RTC_EXT void update_time(void);
RTC_EXT void data_convert(real_time_t data,uint8_t * buff);
ret_code_t service_rtc_init(ble_rtcdate_t *p_rtcdate);
RTC_EXT void ble_service_rtcTime_init(void);
RTC_EXT void ble_service_rtcTime_send(ble_rtcdate_t *p_rtcdate,uint8_t *data_buff,uint16_t data_length);



#endif

