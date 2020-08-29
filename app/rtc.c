#define RTC_GLOBAL
#include "rtc.h"
#include "global.h"

#if NRFX_RTC_ENABLED
#if NRFX_RTC0_ENABLED
const nrf_drv_rtc_t rtc0 = NRF_DRV_RTC_INSTANCE(0);	//nrf_drv �Ǿɿ�

real_time_t time = {
	.year = 1980,
	.month = 1,
	.day = 6,
	.hours = 8,
	.minute = 0,
	.second = 0,
	.week =   0;	
	};

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
}
void RTC1_Init(void)
{
	nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
	//Initialize RTC instance
	ret_code_t err_code = nrf_drv_rtc_init(&rtc1,&config,rtc_handler);
	
	APP_ERROR_CHECK(err_code);
	if(err_code == NRF_SUCCESS)
		NRF_LOG_INFO("RTC1 init success");
	
	//Power on RTC instance
    nrf_drv_rtc_enable(&rtcE);
}
#endif

#if NRFX_RTC1_ENABLED
const nrfx_rtc_t rtc1 = NRFX_RTC_INSTANCE(1);	//nrfx �¿⣬֧�־ɿ���
#endif
#if NRFX_RTC2_ENABLED
const nrfx_rtc_t rtc2 = NRFX_RTC_INSTANCE(2);
#endif


static uint32_t real_time = 0;


/************************************************
˵��: rtc tick event�ص�����
������:void tick_event_handler(void)
����:
����ֵ:
**************************************************/
static void tick_event_handler(void)
{
	static uint8_t tick_cnt = 0;
				//һ��tick 128ms * 8 = 1s
	if(++tick_cnt < 8) 
	{
		return;
	}
	else
	{
          real_time++;
          tick_cnt = 0;
	}
}

/************************************************
˵��:rtc2 handler
������:rtc2_handler(nrfx_rtc_int_type_t evt_type)
����:evt_type: [in] rtc��event����
����ֵ:
**************************************************/
static void rtc2_handler(nrfx_rtc_int_type_t evt_type)
{
	static uint8_t tick_cnt = 0;
	switch(evt_type)
	{
	case NRFX_RTC_INT_TICK:
		tick_event_handler();
		
		break;
	case NRFX_RTC_INT_COMPARE0:
		nrfx_rtc_counter_clear(&rtc2);
		NRF_LOG_INFO("NRFX_RTC_INT_COMPARE0");
		break;
	}
}
/************************************************
˵��:��ʼ��rtc2
������:void RTC2_init(nrfx_rtc_handler_t handler)
����:handler: [in] rtc��event������
����ֵ:
**************************************************/
void RTC2_init(nrfx_rtc_handler_t handler)
{
	//��ʼ��Ϊsdk.config.h ��Ĳ���
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;	
	config.prescaler = 4095;
	if(handler == NULL)
		handler = &rtc2_handler;
	ret_code_t err_code = nrfx_rtc_init(&rtc2,&config,handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
  
	//power on RTC2
	//nrfx_rtc_enable(&rtc2);
      //  nrfx_rtc_tick_enable(&rtc2,true);  //ʹ��rtc2 �� tick event
     nrfx_rtc_cc_set(&rtc2,0,8*125,true);
}


/************************************************
˵��:rtc2 handler
������:rtc2_handler(nrfx_rtc_int_type_t evt_type)
����:evt_type: [in] rtc��event����
����ֵ:
**************************************************/
void update_time(void)
{
	u32_t days;
	u32_t hours = 0UL;
	u32_t days_elapsed = 0U;

	
	hours = (real_time/3600UL);	
	time.minute = (u8_t)((real_time%3600UL)/60U);
	time.second = (u8_t)((real_time%3600UL)%60U);

	if(hours >= 24)
	{
		days_elapsed = (hours/24U);
		time.hours = (hours % 24U);
	}
	else
		time.hours = hours;

	hours

	
	
	
}



#endif

