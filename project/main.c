
#define MAIN_GLOBAL
#include "global.h"

APP_TIMER_DEF(led_timer);
NRF_BLE_GATT_DEF(m_gatt);//定义gatt模块实例
static void led_timer_handler(void *p_context)
{
	
		UNUSED_PARAMETER(p_context);	//不使用p_context,不要报警
		nrf_gpio_pin_toggle(LED_RED);
		//NRF_LOG_INFO("LED_toggle.");
}

static void power_management_init(void)
{
	ret_code_t err_code = nrf_pwr_mgmt_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
}

static void ble_evt_handler(ble_evt_t const *p_ble_evt,void *context)
{
	//ret_code_t err_code = NRF_SUCCESS;
	switch(p_ble_evt->header.evt_id)
	{
		//断开连接事件
		case BLE_GAP_EVT_DISCONNECTED:
			NRF_LOG_INFO("Disconnected");
			break;
		case BLE_GAP_EVT_CONNECTED:
			NRF_LOG_INFO("Connected");
			 hrs_timer_start();
			break;
		case BLE_GAP_EVT_TIMEOUT:
			NRF_LOG_INFO("Timeout");
			break;
		case BLE_GAP_EVT_ADV_SET_TERMINATED:
			NRF_LOG_INFO("advertising terminated");
			main_led_off();
			break;
		case BLE_GATTS_EVT_HVN_TX_COMPLETE:
				NRF_LOG_INFO("Notification transmission complete");
			break;
		default:
			
			NRF_LOG_INFO("%d: %s: %d:",p_ble_evt->header.evt_id ,__func__,__LINE__);
			break;
	}
}

static void ble_stack_init(void)
{
	//请求使能softdevice,会根据sdk_config参数配置低频时钟
	ret_code_t err_code = nrf_sdh_enable_request();
	//G_CHECK_ERROR_CODE_INFO(err_code);
	
	//根据sdk_config.h中的参数而皮质BLE协议栈，获取RAM起始地址
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG,&ram_start);
	//G_CHECK_ERROR_CODE_INFO(err_code);
	
	//使能BLE协议栈
	err_code = nrf_sdh_ble_enable(&ram_start);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
	//注册BLE事件回调函数
	NRF_SDH_BLE_OBSERVER(m_ble_observer,APP_BLE_OBSERVER_PRIO,ble_evt_handler,NULL);
	
	
}

static void gap_params_init(void)
{
	ret_code_t err_code;
	
	ble_gap_conn_params_t gap_conn_params;	//连接参数
	ble_gap_conn_sec_mode_t sec_mode;	//连接安全模式
	
	//配置gap安全模式
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
	
	err_code = sd_ble_gap_device_name_set(&sec_mode,
				             (const uint8_t*)DEVICE_NAME,
				      strlen(DEVICE_NAME));
   G_CHECK_ERROR_CODE_INFO(err_code);
													
	err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT);
	G_CHECK_ERROR_CODE_INFO(err_code);
							
	memset(&gap_conn_params,0,sizeof(gap_conn_params));												
	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency     = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);		
	G_CHECK_ERROR_CODE_INFO(err_code);
													
	static ble_gap_addr_t my_addr;
	err_code = sd_ble_gap_addr_get(&my_addr);
	 G_CHECK_ERROR_CODE_INFO(err_code);
/*	if(err_code == NRF_SUCCESS)
	{
		NRF_LOG_INFO("Address type: %02x",my_addr.addr_type);
		NRF_LOG_INFO("Address: %02x:%02x:%02x:%02x:%02x:%02x",my_addr.addr[0],my_addr.addr[1],my_addr.addr[2],
																				my_addr.addr[3],my_addr.addr[4],my_addr.addr[5]);
	}
	 NRF_LOG_INFO("name size %d",sizeof(my_addr));	*/											
}


static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    G_CHECK_ERROR_CODE_INFO(err_code);
}





uint32_t get_rtc_counter(void)
{
    return NRF_RTC1->COUNTER;
}


void main_log_init(void)
{
	//初始化log
	ret_code_t err_code = NRF_LOG_INIT(get_rtc_counter);
	//APP_ERROR_CHECK(err_code);
	
	//初始化RTT
	NRF_LOG_DEFAULT_BACKENDS_INIT();
//	NRF_LOG_INFO("log_init success.");
	G_CHECK_ERROR_CODE_INFO(err_code);
	
}
void main_timer_init(void)
{
	ret_code_t err_code = app_timer_init();
	G_CHECK_ERROR_CODE_INFO(err_code);

	err_code = app_timer_create(&led_timer,APP_TIMER_MODE_REPEATED,led_timer_handler);
	G_CHECK_ERROR_CODE_INFO(err_code);

	
	err_code = app_timer_start(led_timer,LED_TOGGLE_INTERVAL,NULL);//并不会启动RTC1

	G_CHECK_ERROR_CODE_INFO(err_code);

	hrs_timer_create();
	
}

void main_leds_init(void)
{
	
	//配置输出
	nrf_gpio_cfg_output(LED_RED);
	nrf_gpio_cfg_output(LED_GREEN);
	
	//置1,LED亮
	nrf_gpio_pin_set(LED_RED);
	//清0,LED灭
	nrf_gpio_pin_clear(LED_GREEN);
//	NRF_LOG_INFO("leds_init success");
}

void main_led_off(void)
{
	app_timer_stop(led_timer);
	nrf_gpio_pin_clear(LED_RED);
}




//配置低频时钟
void main_lfclk_config(void)
{
	//初始化时钟模块，设置低频时钟源
	ret_code_t err_code = nrf_drv_clock_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
	//请求低频时钟，输入NULL启动时钟后不产生event
	nrf_drv_clock_lfclk_request(NULL);
	
}
//连接参数更新事件
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;
	NRF_LOG_INFO("on_conn_params_evt");
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}
//连接参数更新失败
static void conn_params_error_handler(uint32_t nrf_error)
{
	NRF_LOG_INFO("conn_params_error_handler");
    APP_ERROR_HANDLER(nrf_error);
}

static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}
/*static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);
	 NRF_LOG_INFO("pm_evt_handler");
    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            advertising_start(false);
            break;

        default:
            break;
    }
}
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}*/

static void idle_state_handle(void)
{
	 //NRF_LOG_INFO("idle_state_handle");
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}
int main(void)
{
	main_log_init();
	main_leds_init();
//	main_lfclk_config();
	main_timer_init();
	power_management_init();
	ble_stack_init();
	gap_params_init();
	gatt_init();
	
//	advertising_init();
 //advertising1_init();
   advertising_all_params_init();
	
	service_init();
	service_his_init();
	conn_params_init();
//	peer_manager_init();
   advertising_start();
	
	for(;;)
	{
	//	NRF_LOG_INFO("time_cnt: %d\n",app_timer_cnt_get());
	//	app_timer_start(led_timer,APP_TIMER_TICKS(50),NULL);
		//app_timer_start(led_timer,APP_TIMER_TICKS(50),NULL);
		idle_state_handle();
	}
}

