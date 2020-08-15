#define SERVICE_GLOBAL
#include "global.h"



NRF_BLE_QWR_DEF(m_qwr);
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/*****************************************************
��������void service_init(void)
��������
���ã������ʼ�� �ް�������
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

//�������ʷ���ʵ��
BLE_HRS_DEF(m_hrs);

/*****************************************************
��������void service_init(void)
��������
���ã������ʼ�� ����his����
******************************************************/
void service_his_init(void)
{
	ret_code_t err_code;
	nrf_ble_qwr_init_t qwr_init;
	memset(&qwr_init,0,sizeof(qwr_init));
	
	qwr_init.error_handler = nrf_qwr_error_handler;
	err_code = nrf_ble_qwr_init(&m_qwr,&qwr_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
	//��ʼ��his
	ble_hrs_init_t hrs_init;
	memset(&hrs_init,0,sizeof(hrs_init));
	
	
	
	uint8_t boby_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_WRIST;
	//��������λ������ֵʱ�İ�ȫ����
	hrs_init.bsl_rd_sec = SEC_OPEN;
	hrs_init.hrm_cccd_wr_sec = SEC_OPEN;	//дCCCD(�ͻ�������������)ʱ�İ�ȫ����
	//�����¼��ص�����
	hrs_init.evt_handler =	NULL;
	//�������Ӵ����֧�ֱ�־
	hrs_init.is_sensor_contact_supported = true;
	//������λ��
	hrs_init.p_body_sensor_location = &boby_sensor_location;
	
	err_code = ble_hrs_init(&m_hrs,&hrs_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
}
#endif
