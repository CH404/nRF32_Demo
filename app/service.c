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
  //  G_CHECK_ERROR_CODE_INFO(err_code);
//	NRF_LOG_INFO("%d",err_code);
	G_CHECK_ERROR_CODE_INFO(err_code);
	service_dis_init();
}

#define MANUFANCTURER_NAME "IIII"
void service_dis_init(void)
{
	ble_dis_init_t dis_init;
	memset(&dis_init,0,sizeof(dis_init));
	
	ble_srv_ascii_to_utf8(&dis_init.manufact_name_str,(char*)MANUFANCTURER_NAME);
	
	dis_init.dis_char_rd_sec = SEC_OPEN;
	ret_code_t err_code = ble_dis_init(&dis_init);
	
	G_CHECK_ERROR_CODE_INFO(err_code);
}