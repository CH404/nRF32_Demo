#define SERVICE_GLOBAL
#include "global.h"


NRF_BLE_QWR_DEF(m_qwr);


static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/*****************************************************
函数名：void service_init(void)
参数：无
作用：服务初始化 无包含服务
******************************************************/
void service_init(void)
{
   ret_code_t err_code;
   nrf_ble_qwr_init_t qwr_init = {0};
	
   qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);

	G_CHECK_ERROR_CODE_INFO(err_code);
    ble_service_rtcTime_init();
	service_bas_init();
	

}
#if BLE_DIS_ENABLED
#define MANUFANCTURER_NAME "IIII"
void service_dis_init(void)
{
	ble_dis_init_t dis_init;
	memset(&dis_init,0,sizeof(dis_init));
	
	ble_srv_ascii_to_utf8(&dis_init.manufact_name_str,(char*)MANUFANCTURER_NAME);
	
	dis_init.dis_char_rd_sec = SEC_OPEN;
	ret_code_t err_code = ble_dis_init(&dis_init);
	
}

#endif

#if BLE_BAS_ENABLED
BLE_BAS_DEF(m_battery);
void service_bas_init(void)
{
  ret_code_t err_code;
	ble_bas_init_t battery_init;
	battery_init.bl_cccd_wr_sec = SEC_OPEN;
	battery_init.bl_rd_sec = SEC_OPEN;
	battery_init.bl_report_rd_sec = SEC_OPEN;
	battery_init.initial_batt_level = 50;
	battery_init.support_notification = true;
	battery_init.evt_handler = NULL;

	err_code = ble_bas_init(&m_battery,&battery_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
}

void bas_notification_send(uint8_t data,uint16_t conn_handle)
{
        ble_bas_battery_level_update(&m_battery,data,conn_handle);
}

#endif




