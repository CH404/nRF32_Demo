#ifndef __GLOBAL_H
#define __GLOBAL_H

#define RTC_FREERTOS 1
#define MY_QUEUE_ENABLE 1

#include "nrf_strerror.h"

#include "sdk_config.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_drv_clock.h"	
#include "nrf_pwr_mgmt.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

#include "ble_gap.h"
#include "ble_types.h"

#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "peer_manager.h"
#include "ble_conn_params.h"
#include "ble_dis.h"

#include "main.h"
//#include "rtc.h"
#include "advertising.h"
#include "service.h"
//#include "hrs.h"
#include "nus.h"
#include "rtc.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_error.h"
#include "security_manager.h"

#include "my_queue.h"
#include "flash_storage.h"

#include "FreeRTOS.h"
#include "task.h"
#include "nrf_sdh_freertos.h"

#include "app_freertos.h"
#include "ble_bas.h"

#define G_CHECK_ERROR_CODE_INFO(err_code)    if(err_code != NRF_SUCCESS)\
{																									\
	NRF_LOG_INFO("Function: %s error code: %s line: %d.",__func__,NRF_LOG_ERROR_STRING_GET(err_code),__LINE__);\
}
	//APP_ERROR_CHECK(error_code); 
#endif


