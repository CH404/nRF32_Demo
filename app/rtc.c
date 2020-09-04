#define RTC_GLOBAL
#include "rtc.h"
#include "global.h"

#if NRFX_RTC_ENABLED
#if NRFX_RTC0_ENABLED
const nrf_drv_rtc_t rtc0 = NRF_DRV_RTC_INSTANCE(0);	//nrf_drv �Ǿɿ�
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
}
void RTC1_Init(void)
{
	nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
	//Initialize RTC instance
	ret_code_t err_code = nrf_drv_rtc_init(&rtc1,&config,rtc_handler);
	
	APP_ERROR_CHECK(err_code);
	if(err_code == NRF_SUCCESS)
		NRF_LOG_INFO("RTC1 init success");
	
	//Power on RTC instance
    nrf_drv_rtc_enable(&rtcE);
}
#endif

#if NRFX_RTC1_ENABLED
const nrfx_rtc_t rtc1 = NRFX_RTC_INSTANCE(1);	//nrfx �¿⣬֧�־ɿ���
#endif
#if NRFX_RTC2_ENABLED
const nrfx_rtc_t rtc2 = NRFX_RTC_INSTANCE(2);
#endif


static uint32_t real_time = 0;


/************************************************
˵��: rtc tick event�ص�����
������:void tick_event_handler(void)
����:
����ֵ:
**************************************************/
static void tick_event_handler(void)
{
	static uint8_t tick_cnt = 0;
				//һ��tick 128ms * 8 = 1s
	if(++tick_cnt < 8) 
	{
		return;
	}
	else
	{
          real_time++;
          tick_cnt = 0;
	}
}

/************************************************
˵��:rtc2 handler
������:rtc2_handler(nrfx_rtc_int_type_t evt_type)
����:evt_type: [in] rtc��event����
����ֵ:
**************************************************/
uint32_t timeeeee = 0;
static void rtc2_handler(nrfx_rtc_int_type_t evt_type)
{
	static uint8_t tick_cnt = 0;
	switch(evt_type)
	{
	case NRFX_RTC_INT_TICK:
		tick_event_handler();
		NRF_LOG_INFO("NRFX_RTC_INT_TICK");
		break;
	case NRFX_RTC_INT_COMPARE0:
        nrfx_rtc_counter_clear(&rtc2);
		nrfx_rtc_cc_set(&rtc2,0, 8,true);
		NRF_LOG_INFO("NRFX_RTC_INT_COMPARE0");
		break;
	}
}

/************************************************
˵��:��ʼ��rtc2
������:void RTC2_init(nrfx_rtc_handler_t handler)
����:handler: [in] rtc��event������
����ֵ:
**************************************************/
void RTC2_init(nrfx_rtc_handler_t handler)
{
	//��ʼ��Ϊsdk.config.h ��Ĳ���
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;	
	config.prescaler = 4095;
	if(handler == NULL)
		handler = &rtc2_handler;
	ret_code_t err_code = nrfx_rtc_init(&rtc2,&config,handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
  
	//power on RTC2
	    nrfx_rtc_enable(&rtc2);
      //  nrfx_rtc_tick_enable(&rtc2,true);  //ʹ��rtc2 �� tick event
   err_code =  nrfx_rtc_cc_set(&rtc2,0,8,true);
	 G_CHECK_ERROR_CODE_INFO(err_code);
 

}


uint8_t month_table[12]={31,29,31,30,31,30,31,31,30,31,30,31};
real_time_t time_base = {
	.year = 2000,
    .month = 1,
    .day = 1,
    .hours = 0,
    .minute = 0,
    .second = 0,
    .week =   0,
};
	
//�洢���	
real_time_t current_time;

/************************************************
˵��:��real_time���������ʱ��
������:void update_time(void)
����:
����ֵ:
**************************************************/
void update_time(void)
{
	 uint8_t month_ad = 0;
     uint8_t year_ad = 0;
     uint8_t i = 0;
     uint32_t hours_tmp  = 0U;
     uint32_t days_elapsed = 0U;
     uint16_t a_yes_days = 366U;
     uint16_t year_tmp = time_base.year;
     current_time.minute = (uint8_t)((real_time/60)%60);
     current_time.second = (uint8_t)(real_time%60U);
     hours_tmp = (real_time / 3600U);

     if(hours_tmp >= 24)
     {
         days_elapsed = (hours_tmp/24);
         current_time.hours = (hours_tmp%24);
     }
     else
         current_time.hours = hours_tmp;

     while (days_elapsed >= a_yes_days)
         {
             days_elapsed -= a_yes_days;
             year_tmp++;
             if((year_tmp%4 == 0)&&((year_tmp%100 !=0)||(year_tmp%400 == 0)))
             {
                 a_yes_days =366;
                 month_table[1] = 29;
                 time_base.year = year_tmp;
                 real_time -= ((real_time / 3600U)/24 - days_elapsed)*24U*3600U;
             }
             else
             {
                 a_yes_days = 365;
                 month_table[1] = 28;
             }
         }

     while(days_elapsed >= month_table[i])
         {
             days_elapsed-= month_table[i];
             i++;
         }
     current_time.day = time_base.day + days_elapsed;
     current_time.month = time_base.month+i;
     current_time.year = year_tmp;
    month_table[1] = 29;
    if(current_time.month < 3)
    {
        month_ad = 12;
        year_ad = 1;
    }
     current_time.week = (current_time.day+2*(current_time.month+month_ad)+
                          3*(current_time.month+1+month_ad)/5+
                          (current_time.year-year_ad)+
                          (current_time.year-year_ad)/4-
                          (current_time.year-year_ad)/100+
                          (current_time.year-year_ad)/400)%7;
}


/************************************************
˵��:��ʼ��rtc ����
������:void update_time(void)
����:
����ֵ:
**************************************************/
void data_convert(real_time_t data,uint8_t * buff)
{
	buff[0] = (uint8_t)(data.year >> 8);
	buff[1] = (uint8_t)(data.year & 0x00FF);
	buff[2] = data.month;
	buff[3] = data.day;
	buff[4] = data.hours;
	buff[5] = data.minute;
	buff[6] = data.second;
	buff[7] = data.week;	
}


/************************************************
˵��:��ʼ��rtc ����
������:void update_time(void)
����:
����ֵ:
**************************************************/
#define OPCODE_LENGTH 1
#define HANDLE_LENGTH 2
#define BEL_RTC_MAX_DATA_LEN NRF_SDH_BLE_GATT_MAX_MTU_SIZE-HANDLE_LENGTH-OPCODE_LENGTH

#define RTC_BASE_UUID {{0x9d,0x14,0xa9,0x7e,0x53,0x73,0x4a,0xb5,0x8f,0x58,0xc0,0xf5,0x00,0x00,0xd2,0x92}}
#define RTC_SERVICE_UUID	0xee42
#define RTC_DATE_CHAR_UUID  0xee41
#define RTC_TIME_CHAR_UUID  0xee43
//#define RTC_DATE_LENGTH 	 

uint16_t rtc_service_handle;
ble_gatts_char_handles_t rtc_characteristic_date_handle;
uint16_t rtc_characteristic_time_handle;

ret_code_t service_rtc_init(void)
{
	ret_code_t err_code = NRF_SUCCESS;
	ble_uuid_t ble_uuid;
	uint8_t uuid_type;
	uint8_t data_buff[7] = {0};
	
	ble_uuid128_t rtc_base_uuid = RTC_BASE_UUID;

	ble_add_char_params_t add_char_params;

	data_convert(time_base,data_buff);

	err_code = sd_ble_uuid_vs_add(&rtc_base_uuid,&uuid_type);
	G_CHECK_ERROR_CODE_INFO(err_code);

	ble_uuid.uuid = RTC_SERVICE_UUID;
	ble_uuid.type = uuid_type;
	sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&ble_uuid,&rtc_service_handle);
	G_CHECK_ERROR_CODE_INFO(err_code);

	memset(&add_char_params,0,sizeof(add_char_params));
	add_char_params.is_var_len = true;	//����ֵ���ֽڳ����Ƿ�ɱ�
	add_char_params.max_len = BEL_RTC_MAX_DATA_LEN;		//�����ɴ������󳤶�
	add_char_params.cccd_write_access = SEC_OPEN; 	    //дCCCD�İ�ȫҪ��Ӧ����server��ʹ�ܻ����ʱ�İ�ȫ
	//add_char_params.char_ext_props					//������չ���ԣ�����д�������û�����������������ʹ���Ŷ�д�����д��ֵ
	//��������
//	add_char_params.char_props.broadcast = 1;//�㲥������
	add_char_params.char_props.notify  = 1; //ʹ��֪ͨ
//	add_char_params.char_props.write = 1; //ʹ�ܿ�д
	
	add_char_params.init_len = sizeof(data_buff);							//��ʼ��ʱ������ֵ����
//	add_char_params.is_defered_read = true;						//indicate �Ƿ���ʱ������
//	add_char_params.is_defered_write = true;					//indicate �Ƿ���ʱд����
	add_char_params.p_init_value = data_buff;						//��ʼ��ʱ������ֵ
//	add_char_params.p_presentation_format				//ָ�������ĸ�ʽ�������Ҫʹ�õĻ�
//	add_char_params.p_user_descr						//ָ���û�����
//	add_char_params.read_access = SEC_OPEN;							//������ֵ�İ�ȫ����
	add_char_params.uuid = 	RTC_DATE_CHAR_UUID;
	add_char_params.uuid_type = uuid_type;
//	add_char_params.write_access = SEC_OPEN;						//д����ֵ�İ�ȫ����

	err_code = characteristic_add(rtc_service_handle,&add_char_params,&rtc_characteristic_date_handle);
	G_CHECK_ERROR_CODE_INFO(err_code);
	NRF_LOG_INFO("%d",sizeof(data_buff));
}

/************************************************
˵��:��ʼrtcʱ�����
������:void ble_service_rtcTime_init(void);
����:
����ֵ:
**************************************************/
void ble_service_rtcTime_init(void)
{
	RTC2_init(&rtc2_handler);
	service_rtc_init();
}
uint16_t rtc_connect;

void bel_service_rtcTime_send(void)
{
	uint8_t data_buff[7];
	uint16_t data_length;
	ble_gatts_hvx_params_t hvx_params;
	memset(&hvx_params,0,sizeof(hvx_params));
	
	data_convert(time_base,data_buff);
	NRF_LOG_INFO("%d%d%d%d",data_buff[0],data_buff[1],data_buff[2],data_buff[3]);
	hvx_params.handle =rtc_characteristic_date_handle.value_handle;
	hvx_params.p_data = data_buff;
	hvx_params.p_len = &data_length;
	hvx_params.type = BLE_GATT_HVX_NOTIFICATION;	//ʹ��notification ֪ͨ
	sd_ble_gatts_hvx(rtc_connect,&hvx_params);	//notification ����	
}




#endif

