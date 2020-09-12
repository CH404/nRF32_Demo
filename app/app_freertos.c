#define APP_FREE_GLOBAL
#include "app_freertos.h"
#include "global.h"




#define DATE_TIMER_TICKS 1000
#define DATE_TIMER_WAIT 2	//消息队列已满下，等待多少个tick
//实例化timer handle 
static TimerHandle_t m_date_timer;



static void date_timerout_handler(TimerHandle_t xTimer)
{
	UNUSED_PARAMETER(xTimer);
	//bel_service_rtcTime_send();
     //    NRF_LOG_INFO("**");
}

void os_timer_init(void)
{									
	m_date_timer = xTimerCreate("date_timer",//定时器名字,用于调试
					DATE_TIMER_TICKS,		//定时时间
					pdTRUE,					//是否循环
					NULL,					//定时器ID,当多个timer使用一个处理函数时handler，用于区分timer
					date_timerout_handler);	//回调函数handler
	if(m_date_timer == NULL)
	{
		NRF_LOG_ERROR("timer create error");	
	}
}
void os_timer_start(void)
{
	if(pdFAIL ==  xTimerStart(m_date_timer,DATE_TIMER_WAIT))//成功启动定时器前的最大等待时间设置，单位系统时钟节拍，定时器组的大部分API函数不是直接运行的（见19.2小节说明），而是通过消息队列给定时器任务发消息来实现的，此参数设置的等待时间就是当消息队列已经满的情况下，等待消息队列有空间时的最大等待时间。
		{
			NRF_LOG_ERROR("timer start error");	
		}

}




