#ifndef __GLOBAL_H
#define __GLOBAL_H





#include "sdk_config.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_drv_clock.h"	
#include "nrf_pwr_mgmt.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

#include "ble_gap.h"

#include "nrf_ble_gatt.h"
#include "ble_advertising.h"
#include "nrf_ble_qwr.h"
#include "peer_manager.h"
#include "ble_conn_params.h"

#include "main.h"
//#include "rtc.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"





#define G_CHECK_ERROR_CODE_INFO(error_code)		\
APP_ERROR_CHECK(error_code);							\
NRF_LOG_INFO("Function: %s error code: %s.",__func__,NRF_LOG_ERROR_STRING_GET(error_code))

#endif


