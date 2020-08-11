#include "main.h"
#include "global.h"

APP_TIMER_DEF(led_timer);
static void led_timer_handler(void *p_context)
{
		nrf_gpio_pin_toggle(LED_RED);
		NRF_LOG_INFO("LED_toggle.");
}

static void log_init(void)
{
	//初始化log
	ret_code_t err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);
	
	//初始化RTT
	NRF_LOG_DEFAULT_BACKENDS_INIT();
	NRF_LOG_INFO("log_init success.");
	
}
static void timer_init(void)
{
	uint8_t delay_ms = 500;
	ret_code_t err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);
	
	if(err_code == NRF_SUCCESS)
		NRF_LOG_INFO("timer_init success");
	

	err_code = app_timer_create(&led_timer,APP_TIMER_MODE_REPEATED,led_timer_handler);
	APP_ERROR_CHECK(err_code);
	if(err_code == NRF_SUCCESS)
		NRF_LOG_INFO("app timer creat success");
	
	
	
	err_code = app_timer_start(led_timer,50,NULL);
	APP_ERROR_CHECK(err_code);
   if(err_code == NRF_SUCCESS)
	NRF_LOG_INFO("APP timer start. %d",APP_TIMER_TICKS(50));
}

static void leds_init(void)
{
	//配置输出
	nrf_gpio_cfg_output(LED_RED);
	nrf_gpio_cfg_output(LED_GREEN);
	
	//置1,LED亮
	nrf_gpio_pin_set(LED_RED);
	//清0,LED灭
	nrf_gpio_pin_clear(LED_GREEN);
	NRF_LOG_INFO("leds_init success");
}

int main(void)
{
	log_init();
	leds_init();
	timer_init();
//	app_timer_start(led_timer,APP_TIMER_TICKS(50),NULL);
	for(;;)
	{
	//	NRF_LOG_INFO("time_cnt: %d\n",app_timer_cnt_get());
	//	app_timer_start(led_timer,APP_TIMER_TICKS(50),NULL);
		//app_timer_start(led_timer,APP_TIMER_TICKS(50),NULL);
	}
}

