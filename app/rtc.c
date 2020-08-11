#define RTC_GLOBAL
#include "global.h"


const nrf_drv_rtc_t rtcE = NRF_DRV_RTC_INSTANCE(1);

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
}

void RTC1_Init(void)
{
	nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
	//Initialize RTC instance
	ret_code_t err_code = nrf_drv_rtc_init(&rtcE,&config,rtc_handler);
	
	APP_ERROR_CHECK(err_code);
	if(err_code == NRF_SUCCESS)
		NRF_LOG_INFO("RTC1 init success");
	
	//Power on RTC instance
    nrf_drv_rtc_enable(&rtcE);
}

