
#define MAIN_GLOBAL
#include "global.h"
#include "nrf_delay.h"
//#include "timers.h"
APP_TIMER_DEF(led_timer);	
NRF_BLE_GATT_DEF(m_gatt);//����gattģ��ʵ��



static uint16_t   m_conn_handle          = BLE_CONN_HANDLE_INVALID;                 /**< Handle of the current connection. */
static uint16_t   m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;            /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
#define STATIC_PASSKEY "890731"
static ble_opt_t static_option;
ble_gap_sec_params_t sec_params_auth;
ble_gap_sec_params_t sec_params;
ble_gap_sec_keyset_t sec_keyset;
ble_gap_enc_key_t m_own_enc_key;
ble_gap_enc_key_t m_peer_enc_key;





#if (NRF_LOG_ENABLED&&(!NRF_LOG_DEFERRED))
static TaskHandle_t m_logger_thread;
#endif
static void led_timer_handler(void *p_context)
{
	
		UNUSED_PARAMETER(p_context);	//��ʹ��p_context,��Ҫ����
		nrf_gpio_pin_toggle(LED_RED);
		//NRF_LOG_INFO("LED_toggle.");
}
/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}

static void power_management_init(void)
{
	ret_code_t err_code = nrf_pwr_mgmt_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
}

static void ble_evt_handler(ble_evt_t const *p_ble_evt,void *context)
{
	ret_code_t err_code = NRF_SUCCESS;
	switch(p_ble_evt->header.evt_id)
	{
		//�Ͽ������¼�
		case BLE_GAP_EVT_DISCONNECTED:
			NRF_LOG_INFO("Disconnected");
			advertising_start();
			break;
		case BLE_GAP_EVT_CONNECTED:
			
			NRF_LOG_INFO("Connected");
			m_conn_handle=p_ble_evt->evt.common_evt.conn_handle;
		//rr_code = sd_ble_gap_authenticate(m_conn_handle,&sec_params_auth);
			G_CHECK_ERROR_CODE_INFO(err_code);
			// hrs_timer_start();
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
		case BLE_GAP_EVT_CONN_PARAM_UPDATE:		//�յ�BLE_GAP_EVT_SEC_PARAMS_REQUEST�����ð�ȫ��������ʾ
			NRF_LOG_INFO("BLE_GAP_EVT_CONN_PARAM_UPDATE");		
			break;
		case BLE_GAP_EVT_SEC_PARAMS_REQUEST:	//�յ���Ե�����
			NRF_LOG_INFO("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
		//	err_code = sd_ble_gap_sec_params_reply(m_conn_handle,BLE_GAP_SEC_STATUS_SUCCESS,&sec_params,NULL);
			G_CHECK_ERROR_CODE_INFO(err_code);
			break;

		case BLE_GAP_EVT_SEC_INFO_REQUEST:
			NRF_LOG_INFO("BLE_GAP_EVT_SEC_INFO_REQUEST");

			break;
		case BLE_GAP_EVT_PASSKEY_DISPLAY: //��ʾ����
			NRF_LOG_INFO("passkey: %s",p_ble_evt->evt.gap_evt.params.passkey_display.passkey);
			break;
		case PM_EVT_CONN_SEC_CONFIG_REQ://��������
			// Reject pairing request from an already bonded peer.
			NRF_LOG_INFO("PM_EVT_CONN_SEC_CONFIG_REQ");
			pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
			//�����ٴ����
			pm_conn_sec_config_reply(p_ble_evt->evt.common_evt.conn_handle, &conn_sec_config);
	        break;
		 case BLE_GAP_EVT_AUTH_STATUS:	//BLE_GAP_EVT_CONN_SEC_UPDATE�� �����֤��ɣ�����Ƿ���Գɹ�
		 	if(p_ble_evt->evt.gap_evt.params.auth_status.auth_status == BLE_GAP_SEC_STATUS_SUCCESS)
            {
                NRF_LOG_INFO("Pair success!");
            }
            else
            {
               NRF_LOG_INFO("Pair failed!");
               sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            } 
			  NRF_LOG_INFO("BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
                          p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                          p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                          p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
            break;

		case BLE_GAP_EVT_CONN_SEC_UPDATE:	//������������,�������¼�
			NRF_LOG_INFO("connection security update");
			break;
		case BLE_GATTC_EVT_HVX:
                  uint8_t i;
				uint8_t data_buff[7] = {0};
				ble_gattc_evt_hvx_t const *p_write_evt = &p_ble_evt->evt.gattc_evt.params.hvx;

			NRF_LOG_INFO("BLE_GATTC_EVT_HVX");
                        if(p_write_evt->len <= 0)
                        {
                          break;
                        }
				memcpy(data_buff,&p_write_evt->data,p_write_evt->len);
	for(i=0;i<p_write_evt->len;i++)
		{
			NRF_LOG_INFO("%d",p_write_evt->data[i]);
		}
			break;

		case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
			NRF_LOG_INFO("BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP");
			break;
		default:

			NRF_LOG_INFO("%d: %s: %d:",p_ble_evt->header.evt_id ,__func__,__LINE__);
			break;
	}
}

static void ble_stack_init(void)
{
	//����ʹ��softdevice,�����sdk_config�������õ�Ƶʱ��
	ret_code_t err_code = nrf_sdh_enable_request();
	//G_CHECK_ERROR_CODE_INFO(err_code);
	
	//����sdk_config.h�еĲ�����Ƥ��BLEЭ��ջ����ȡRAM��ʼ��ַ
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG,&ram_start);
	//G_CHECK_ERROR_CODE_INFO(err_code);
	
	//ʹ��BLEЭ��ջ
	err_code = nrf_sdh_ble_enable(&ram_start);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
	//ע��BLE�¼��ص�����
	NRF_SDH_BLE_OBSERVER(m_ble_observer,APP_BLE_OBSERVER_PRIO,ble_evt_handler,NULL);
	
	
}

static void gap_params_init(void)
{
	ret_code_t err_code;
	uint8_t passkey[] = STATIC_PASSKEY;
	
	ble_gap_conn_params_t gap_conn_params;	//���Ӳ���
	ble_gap_conn_sec_mode_t sec_mode;	//���Ӱ�ȫģʽ
	
	//����gap��ȫģʽ
	BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&sec_mode);
	
	err_code = sd_ble_gap_device_name_set(&sec_mode,
				             (const uint8_t*)DEVICE_NAME,
				      strlen(DEVICE_NAME));
   G_CHECK_ERROR_CODE_INFO(err_code);
													
	err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_MEDIA_PLAYER);
	G_CHECK_ERROR_CODE_INFO(err_code);
							
	memset(&gap_conn_params,0,sizeof(gap_conn_params));												
	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency     = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);		
	G_CHECK_ERROR_CODE_INFO(err_code);
													
//	static ble_gap_addr_t my_addr;
//	err_code = sd_ble_gap_addr_get(&my_addr);
	// G_CHECK_ERROR_CODE_INFO(err_code);
/*	if(err_code == NRF_SUCCESS)
	{
		NRF_LOG_INFO("Address type: %02x",my_addr.addr_type);
		NRF_LOG_INFO("Address: %02x:%02x:%02x:%02x:%02x:%02x",my_addr.addr[0],my_addr.addr[1],my_addr.addr[2],
																				my_addr.addr[3],my_addr.addr[4],my_addr.addr[5]);
	}
	 NRF_LOG_INFO("name size %d",sizeof(my_addr));	*/	
#if SET_PASSKEY 
	sd_ble_opt_set(BLE_GAP_OPT_PASSKEY,&static_option);
	static_option.gap_opt.passkey.p_passkey = passkey;
	err_code = sd_ble_opt_set(BLE_GAP_OPT_PASSKEY,&static_option);
	G_CHECK_ERROR_CODE_INFO(err_code);
#endif
	//main_sec_params_bond(m_conn_handle);

}


static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    G_CHECK_ERROR_CODE_INFO(err_code);
	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	 G_CHECK_ERROR_CODE_INFO(err_code);
}





uint32_t get_rtc_counter(void)
{
    return NRF_RTC1->COUNTER;
}

static void logger_thread(void *arg)
{
	UNUSED_PARAMETER(arg);
	while(1)
	{
		NRF_LOG_FLUSH();
		vTaskSuspend(NULL);
	}
}
void main_log_init(void)
{
#if NRF_LOG_ENABLED
	
	//��ʼ��log
	ret_code_t err_code = NRF_LOG_INIT(get_rtc_counter);
	//APP_ERROR_CHECK(err_code);
	
	//��ʼ��RTT
	NRF_LOG_DEFAULT_BACKENDS_INIT();
//	NRF_LOG_INFO("log_init success.");
	G_CHECK_ERROR_CODE_INFO(err_code);
#elif (!NRF_LOG_DEFERRED)
	if(pdPASS != xTaskCreate(logger_thread,"LOGGER",256,NULL,1&m_logger_thread))
	{
		APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
	}        
#endif  
	
}
void main_timer_init(void)
{
	ret_code_t err_code = app_timer_init();
	G_CHECK_ERROR_CODE_INFO(err_code);

	err_code = app_timer_create(&led_timer,APP_TIMER_MODE_REPEATED,led_timer_handler);
	G_CHECK_ERROR_CODE_INFO(err_code);

	
	err_code = app_timer_start(led_timer,LED_TOGGLE_INTERVAL,NULL);//����������RTC1

	G_CHECK_ERROR_CODE_INFO(err_code);

	//hrs_timer_create();
	
}

void main_leds_init(void)
{
	
	//�������
	nrf_gpio_cfg_output(LED_RED);
	nrf_gpio_cfg_output(LED_GREEN);
	
	//��1,LED��
	nrf_gpio_pin_set(LED_RED);
	//��0,LED��
	nrf_gpio_pin_clear(LED_GREEN);
//	NRF_LOG_INFO("leds_init success");
}

void main_led_off(void)
{
	app_timer_stop(led_timer);
	nrf_gpio_pin_clear(LED_RED);
}




//���õ�Ƶʱ��
void main_lfclk_config(void)
{
	//��ʼ��ʱ��ģ�飬���õ�Ƶʱ��Դ
	ret_code_t err_code = nrf_drv_clock_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
	//�����Ƶʱ�ӣ�����NULL����ʱ�Ӻ󲻲���event
	nrf_drv_clock_lfclk_request(NULL);
	
}
//���Ӳ��������¼�
//static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
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
//���Ӳ�������ʧ��
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

static const char * m_roles_str[] =
{
    "Invalid Role",
    "Peripheral",
    "Central",
};
	static const char * m_sec_procedure_str[] =
	{
		"Encryption",
		"Bonding",
		"Pairing",
	};
		static const char * m_data_id_str[] =
		{
			"Outdated (0)",
			"Service changed pending flag",
			"Outdated (2)",
			"Outdated (3)",
			"Application data",
			"Remote database",
			"Peer rank",
			"Bonding data",
			"Local database",
			"Central address resolution",
		};
			static const char * m_data_action_str[] =
			{
				"Update",
				"Delete"
			};

static void pm_evt_handler(pm_evt_t const * p_evt)
{
 //   pm_handler_on_pm_evt(p_evt);
    ret_code_t err_code;
    pm_handler_flash_clean(p_evt);
    switch (p_evt->evt_id)
	{
		case PM_EVT_BONDED_PEER_CONNECTED:		//�Ѱ��豸��������
			NRF_LOG_INFO("PM_EVT_BONDED_PEER_CONNECTED");
			//  conn_secure(p_evt->conn_handle, false);
			break;
	
		case PM_EVT_CONN_SEC_START:	//��������ȫ����(���)��������յ��������ӳɹ���ʧ��event
			NRF_LOG_INFO("PM_EVT_CONN_SEC_START");
			break;
	
		case PM_EVT_CONN_SEC_SUCCEEDED:	//��ȫ���ӳɹ�
			NRF_LOG_INFO("PM_EVT_CONN_SEC_SUCCEEDED");
			break;
	
		case PM_EVT_CONN_SEC_FAILED://��ȫ����ʧ��
			NRF_LOG_INFO("PM_EVT_CONN_SEC_FAILED");
			if(p_evt->params.conn_sec_failed.procedure == PM_CONN_SEC_PROCEDURE_ENCRYPTION&&
				p_evt->params.conn_sec_failed.error == PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING)
				{
					 NRF_LOG_INFO("Waiting for host to fix bonding\r\n");
				}
				else
				sd_ble_gap_disconnect(p_evt->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			break;
	
		case PM_EVT_CONN_SEC_CONFIG_REQ://�����豸�������������(�Ѱ�)��ʹ��pm_conn_sec_config_reply��Ӧ����Ȼʹ��peer_initʱ���õ�Ĭ�ϲ���
			NRF_LOG_INFO("PM_EVT_CONN_SEC_CONFIG_REQ");
		    pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
			break;
	
		case PM_EVT_CONN_SEC_PARAMS_REQ://�����ȡ��ȫ���Ӳ���������pm_conn_sec_config_reply������������Ȼʹ��peer_initʱ���õ�Ĭ�ϲ���
			NRF_LOG_INFO("PM_EVT_CONN_SEC_PARAMS_REQ");
		NRF_LOG_INFO("kdist_own.enc :%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.enc);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.id);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.sign);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.link);
		
		NRF_LOG_INFO("kdist_peer.enc: %d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.enc);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.id);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.sign);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.link);
			err_code =  pm_conn_sec_params_reply(p_evt->conn_handle, &sec_params, p_evt->params.conn_sec_params_req.p_context);
			G_CHECK_ERROR_CODE_INFO(err_code);
			break;
	
		case PM_EVT_STORAGE_FULL:
			NRF_LOG_INFO("Flash storage is full");
			break;
	
		case PM_EVT_ERROR_UNEXPECTED:
			NRF_LOG_INFO("PM_EVT_ERROR_UNEXPECTED");
			break;
	
		case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
			NRF_LOG_INFO("PM_EVT_PEER_DATA_UPDATE_SUCCEEDED");
			break;
	
		case PM_EVT_PEER_DATA_UPDATE_FAILED:
			// This can happen if the SoftDevice is too busy with BLE operations.
			NRF_LOG_INFO("PM_EVT_PEER_DATA_UPDATE_FAILED");
			break;
	
		case PM_EVT_PEER_DELETE_SUCCEEDED:
			NRF_LOG_INFO("PM_EVT_PEER_DELETE_SUCCEEDED");
			break;
	
		case PM_EVT_PEER_DELETE_FAILED:
			NRF_LOG_INFO("PM_EVT_PEER_DELETE_FAILED");
			break;
	
		case PM_EVT_PEERS_DELETE_SUCCEEDED:
			NRF_LOG_INFO("All peers deleted.");
			break;
	
		case PM_EVT_PEERS_DELETE_FAILED:
			NRF_LOG_INFO("PM_EVT_PEERS_DELETE_FAILED");
			break;
	
		case PM_EVT_LOCAL_DB_CACHE_APPLIED:
			NRF_LOG_INFO("PM_EVT_LOCAL_DB_CACHE_APPLIED");
			break;
	
		case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
			// This can happen when the local DB has changed.
			NRF_LOG_INFO("PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED");
			break;
	
		case PM_EVT_SERVICE_CHANGED_IND_SENT:
			NRF_LOG_INFO("Sending Service Changed indication.");
			break;
	
		case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
			NRF_LOG_INFO("Service Changed indication confirmed.");
			break;
	
		case PM_EVT_SLAVE_SECURITY_REQ:
			NRF_LOG_INFO("Security Request received from peer.");
			break;
	
		case PM_EVT_FLASH_GARBAGE_COLLECTED:
			NRF_LOG_INFO("Flash garbage collection complete.");
			break;
	
		case PM_EVT_FLASH_GARBAGE_COLLECTION_FAILED:
			NRF_LOG_INFO("PM_EVT_FLASH_GARBAGE_COLLECTION_FAILED");
			break;
	
		default:
		NRF_LOG_INFO("default");
			break;
	}
}
static void peer_manager_init(void)
{
//    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;
    bool erase_bonds = true;
    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

	if(erase_bonds)
	{
		err_code = pm_peers_delete();
		G_CHECK_ERROR_CODE_INFO(err_code);
	}
    memset(&sec_params, 0, sizeof(ble_gap_sec_params_t));
	
//Bonding just works 
sec_params.bond = SEC_PARAM_BOND;                //��
sec_params.mitm = SEC_PARAM_MITM;                         //�м��˹������� MITM
sec_params.lesc = SEC_PARAM_LESC;                            //ʹ��LE secure connetion ���
sec_params.keypress = SEC_PARAM_KEYPRESS;     
sec_params.io_caps = SEC_PARAM_IO_CAPABILITIES;
sec_params.oob = SEC_PARAM_OOB;
sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;

//�ַ����ص�key(���豸) 4��key
sec_params.kdist_own.enc = 1;	
sec_params.kdist_own.id = 0;	
//�ַ�Զ�˵�key(��Ե�����)
sec_params.kdist_peer.enc = 1;
sec_params.kdist_peer.id = 0;

/*c_keyset.keys_own.p_enc_key = &m_own_enc_key;
sec_keyset.keys_own.p_id_key = NULL;
sec_keyset.keys_own.p_pk = NULL;
sec_keyset.keys_own.p_sign_key = NULL;

sec_keyset.keys_peer.p_enc_key = &m_peer_enc_key;
sec_keyset.keys_peer.p_id_key = NULL;
sec_keyset.keys_peer.p_pk = NULL;
sec_keyset.keys_peer.p_sign_key = NULL;
*/










    err_code = pm_sec_params_set(&sec_params);
   // APP_ERROR_CHECK(err_code);
	G_CHECK_ERROR_CODE_INFO(err_code);
    err_code = pm_register(pm_evt_handler);
   // APP_ERROR_CHECK(err_code);
   G_CHECK_ERROR_CODE_INFO(err_code);

}

static void idle_state_handle(void)
{
	 //NRF_LOG_INFO("idle_state_handle");
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

/*****************************************************
������static void main_sec_params_bond(void)
������
���ã���ԣ�ʹ�ð�(����Ϊ�˼�����Ե�ʱ�䣬�����һ�ּ��ܣ�
��·����,��������·���ݰ�)
*******************************************************/
static void main_sec_params_bond(uint16_t conn_handle)
{
	ret_code_t err_code;
	sec_params.oob = 0;
	sec_params.bond = 0;
	sec_params.mitm = 1;	//MITM ���� �м��˹���:Man In The Middle attack
	sec_params.io_caps = BLE_GAP_IO_CAPS_DISPLAY_ONLY;
	sec_params.min_key_size = 7;
	sec_params.max_key_size = 16;
	//err_code = sd_ble_gap_sec_params_reply(conn_handle,BLE_GAP_SEC_STATUS_SUCCESS,&sec_params,NULL);
	//G_CHECK_ERROR_CODE_INFO(err_code);
}

int main(void)
{
	ret_code_t err_code;

	main_log_init();
//	main_leds_init();
//	main_lfclk_config();
//	main_timer_init();
   
	power_management_init();
	ble_stack_init();
	gap_params_init();
	gatt_init();
	service_init();
//    advertising_init();
  //  advertising1_init();
      advertising_all_params_init();
	
//	service_his_init();
	conn_params_init();
	peer_manager_init();
//   advertising_start();
   rtcdate_task_init();
   nrf_sdh_freertos_init(&advertising_free_start,NULL);

	
//  RTC2_init(NULL);
  vTaskStartScheduler();
 //
	while(1)
	{
		//NRF_LOG_INFO("time_cnt: %d\n",app_timer_cnt_get());
	//	app_timer_start(led_timer,APP_TIMER_TICKS(50),NULL);
		//app_timer_start(led_timer,APP_TIMER_TICKS(50),NULL);
       //   nrfx_rtc_counter_clear(&rtc2);
 
		idle_state_handle();
	}
}

