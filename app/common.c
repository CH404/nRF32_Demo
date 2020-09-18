#define COMMON_GLOBAL
#include "common.h"
#include "nrfx_wdt.h"


uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;

#define WDTFEEDEVTWAITBIT  0x01
#define WDTTASKSIZE 50
#define WDTTASKPRIO 2
//void WdtFeedTask (void *paramenters);
TaskHandle_t wdtFeedTaskHanle;
EventGroupHandle_t wdtFeedEventHandle;


void WdtFeedTask (void *paramenters)
{
	EventBits_t wdtFeedEvt = 0;
	while(1)
		{
			//xEventGroupGetBits(wdtFeedEventHandle);
			wdtFeedEvt = xEventGroupWaitBits(wdtFeedEventHandle,
			WDTFEEDEVTWAITBIT,pdTRUE,pdTRUE,portMAX_DELAY);
			if(wdtFeedEvt == WDTFEEDEVTWAITBIT)
					nrfx_wdt_feed();		
		}
}



bool WdtFeedTaskInit(void)
{
	BaseType_t ret;
		ret = xTaskCreate((TaskFunction_t)WdtFeedTask,
											(const char *) "wdt", 
											(uint16_t)WDTTASKSIZE,
											NULL,
											(UBaseType_t)WDTTASKPRIO,
											(TaskHandle_t *)wdtFeedTaskHanle);
		if(ret == pdPASS)
			{
				wdtFeedEventHandle = xEventGroupCreate();
				if(wdtFeedEventHandle != NULL)
					{
						NRF_LOG_INFO("wdt task success");
					return true;
					}
				else
					return false;
			}
		return false;
	
}



void CommonWatchDogInit(void)
{
	ret_code_t err_code;
	nrfx_wdt_config_t config = NRFX_WDT_DEAFULT_CONFIG;
	config.reload_value = 49152;
	err_code = nrfx_wdt_init(&config,NULL);
	APP_ERROR_CHECK(err_code);
	nrfx_wdt_enable();
}




