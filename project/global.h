#ifndef __GLOBAL_H
#define __GLOBAL_H



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

#include "main.h"
//#include "rtc.h"
#include "advertising.h"
#include "service.h"

#include "ble_hrs.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_error.h"




#define G_CHECK_ERROR_CODE_INFO(error_code)    if(error_code != NRF_SUCCESS)\
{																									\
	NRF_LOG_INFO("Function: %s error code: %s line: %d.",__func__,NRF_LOG_ERROR_STRING_GET(error_code),__LINE__);\
}
	//APP_ERROR_CHECK(error_code); 
#endif


