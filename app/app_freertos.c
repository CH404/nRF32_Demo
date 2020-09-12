#define APP_FREE_GLOBAL
#include "app_freertos.h"
#include "global.h"




#define DATE_TIMER_TICKS 1000
#define DATE_TIMER_WAIT 2	//��Ϣ���������£��ȴ����ٸ�tick
//ʵ����timer handle 
static TimerHandle_t m_date_timer;



static void date_timerout_handler(TimerHandle_t xTimer)
{
	UNUSED_PARAMETER(xTimer);
	//bel_service_rtcTime_send();
     //    NRF_LOG_INFO("**");
}

void os_timer_init(void)
{									
	m_date_timer = xTimerCreate("date_timer",//��ʱ������,���ڵ���
					DATE_TIMER_TICKS,		//��ʱʱ��
					pdTRUE,					//�Ƿ�ѭ��
					NULL,					//��ʱ��ID,�����timerʹ��һ��������ʱhandler����������timer
					date_timerout_handler);	//�ص�����handler
	if(m_date_timer == NULL)
	{
		NRF_LOG_ERROR("timer create error");	
	}
}
void os_timer_start(void)
{
	if(pdFAIL ==  xTimerStart(m_date_timer,DATE_TIMER_WAIT))//�ɹ�������ʱ��ǰ�����ȴ�ʱ�����ã���λϵͳʱ�ӽ��ģ���ʱ����Ĵ󲿷�API��������ֱ�����еģ���19.2С��˵����������ͨ����Ϣ���и���ʱ��������Ϣ��ʵ�ֵģ��˲������õĵȴ�ʱ����ǵ���Ϣ�����Ѿ���������£��ȴ���Ϣ�����пռ�ʱ�����ȴ�ʱ�䡣
		{
			NRF_LOG_ERROR("timer start error");	
		}

}




