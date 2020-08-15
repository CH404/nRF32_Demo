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
}





#if BLE_HRS_ENABLED

//定义心率服务实例
BLE_HRS_DEF(m_hrs);

/*****************************************************
函数名：void service_init(void)
参数：无
作用：服务初始化 包含his服务
******************************************************/
void service_his_init(void)
{
	ret_code_t err_code;
	nrf_ble_qwr_init_t qwr_init;
	memset(&qwr_init,0,sizeof(qwr_init));
	
	qwr_init.error_handler = nrf_qwr_error_handler;
	err_code = nrf_ble_qwr_init(&m_qwr,&qwr_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
	//初始化his
	ble_hrs_init_t hrs_init;
	memset(&hrs_init,0,sizeof(hrs_init));
	
	
	
	uint8_t boby_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_WRIST;
	//读传感器位置特征值时的安全需求
	hrs_init.bsl_rd_sec = SEC_OPEN;
	hrs_init.hrm_cccd_wr_sec = SEC_OPEN;	//写CCCD(客户端特征描述符)时的安全需求
	//服务事件回调函数
	hrs_init.evt_handler =	NULL;
	//传感器接触检测支持标志
	hrs_init.is_sensor_contact_supported = true;
	//身体测点位置
	hrs_init.p_body_sensor_location = &boby_sensor_location;
	
	err_code = ble_hrs_init(&m_hrs,&hrs_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
}
#endif
