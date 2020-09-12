#define RTC_GLOBAL
#include "rtc.h"
#include "global.h"

#if NRFX_RTC_ENABLED
#if NRFX_RTC0_ENABLED
const nrf_drv_rtc_t rtc0 = NRF_DRV_RTC_INSTANCE(0);	//nrf_drv 是旧库
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
const nrfx_rtc_t rtc1 = NRFX_RTC_INSTANCE(1);	//nrfx 新库，支持旧库名
#endif
#if NRFX_RTC2_ENABLED
const nrfx_rtc_t rtc2 = NRFX_RTC_INSTANCE(2);


BLE_RTCDATE_DEF(m_rtcdate);

void on_write(ble_rtcdate_t * p_rtcdate, ble_evt_t const * p_ble_evt)
{
	uint8_t data_buff[7];
	uint8_t i;
	ble_gatts_evt_write_t const *p_write_evt = &p_ble_evt->evt.gatts_evt.params.write;
	if(p_rtcdate->rtc_characteristic_date_handle.value_handle != p_write_evt->handle)
		{
			return;
		}

	//读出数据
	memcpy(data_buff,&p_write_evt->data,p_write_evt->len);
	for(i=0;i<p_write_evt->len;i++)
		{
			NRF_LOG_INFO("%d",p_write_evt->data[i]);
		}
}

void ble_RTCDATE_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context)
{
	ble_rtcdate_t* p_rtcdate = (ble_rtcdate_t *)p_context;
	switch(p_ble_evt->header.evt_id)
		{
			case BLE_GAP_EVT_CONNECTED:

				//存储链接客户端handle
				p_rtcdate->rtc_connect = p_ble_evt->evt.gap_evt.conn_handle;
				break;
			case BLE_GAP_EVT_DISCONNECTED:
				p_rtcdate->rtc_connect = BLE_CONN_HANDLE_INVALID;
				break;
			case BLE_GATTS_EVT_WRITE:
				on_write(p_rtcdate,p_ble_evt);
				break;
				
		}
}

static uint32_t real_time = 0;

/************************************************
说明: rtc tick event回调函数
函数名:void tick_event_handler(void)
参数:
返回值:
**************************************************/
static void tick_event_handler(void)
{
	static uint8_t tick_cnt = 0;
				//一个tick 128ms * 8 = 1s
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
说明:rtc2 handler
函数名:rtc2_handler(nrfx_rtc_int_type_t evt_type)
参数:evt_type: [in] rtc的event类型
返回值:
**************************************************/
static void rtc2_handler(nrfx_rtc_int_type_t evt_type)
{
	BaseType_t pxHigherPriorityTaskWoken;
	static uint8_t tick_cnt = 0;
	switch(evt_type)
	{
	case NRFX_RTC_INT_TICK:
		tick_event_handler();
		NRF_LOG_INFO("NRFX_RTC_INT_TICK");
		break;
	case NRFX_RTC_INT_COMPARE0:
		real_time++;
		xSemaphoreGiveFromISR(DateUpdateSem, &pxHigherPriorityTaskWoken);
        nrfx_rtc_counter_clear(&rtc2);
		nrfx_rtc_cc_set(&rtc2,0, 8,true);
	//	NRF_LOG_INFO("NRFX_RTC_INT_COMPARE0");
		break;
	}
}

/************************************************
说明:初始化rtc2
函数名:void RTC2_init(nrfx_rtc_handler_t handler)
参数:handler: [in] rtc的event处理函数
返回值:
**************************************************/
void RTC2_init(nrfx_rtc_handler_t handler)
{
	//初始化为sdk.config.h 里的参数
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;	
	config.prescaler = 4095;
	if(handler == NULL)
		handler = &rtc2_handler;
	ret_code_t err_code = nrfx_rtc_init(&rtc2,&config,handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
  
	//power on RTC2
	    nrfx_rtc_enable(&rtc2);
      //  nrfx_rtc_tick_enable(&rtc2,true);  //使能rtc2 和 tick event
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
	
//存储结果	
real_time_t current_time;

/************************************************
说明:将real_time换算成日期时间
函数名:void update_time(void)
参数:
返回值:
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
说明:初始化rtc 服务
函数名:void update_time(void)
参数:
返回值:
**************************************************/
void data_convert(real_time_t data,uint8_t * buff)
{
        uint8_t yearH,yearL;
        yearH = (uint8_t)(data.year / 100);
        yearL = (uint8_t)(data.year % 100);
  
	buff[0] = yearH;
	buff[1] = yearL;
	buff[2] = data.month;
	buff[3] = data.day;
	buff[4] = data.hours;
	buff[5] = data.minute;
	buff[6] = data.second;
	buff[7] = data.week;	
}


/************************************************
说明:初始化rtc 服务
函数名:void update_time(void)
参数:
返回值:
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
//ble_gatts_char_handles_t rtc_characteristic_date_handle;
//ble_gatts_char_handles_t rtc_characteristic_time_handle;

ret_code_t service_rtc_init(ble_rtcdate_t *p_rtcdate)
{
	ret_code_t err_code = NRF_SUCCESS;
	ble_uuid_t ble_uuid;
	uint8_t uuid_type;
        uint8_t data_buff[7];
	
	ble_uuid128_t rtc_base_uuid = RTC_BASE_UUID;

	ble_add_char_params_t add_char_params;

	data_convert(time_base,data_buff);

	err_code = sd_ble_uuid_vs_add(&rtc_base_uuid,&uuid_type);
	G_CHECK_ERROR_CODE_INFO(err_code);

	ble_uuid.uuid = RTC_SERVICE_UUID;
	ble_uuid.type = uuid_type;
	sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&ble_uuid,&p_rtcdate->rtc_service_handle);
	G_CHECK_ERROR_CODE_INFO(err_code);

	memset(&add_char_params,0,sizeof(add_char_params));
	add_char_params.is_var_len = true;	//特征值的字节长度是否可变
	add_char_params.max_len = BEL_RTC_MAX_DATA_LEN;		//特征可传输的最大长度
	add_char_params.cccd_write_access =SEC_OPEN; 	    //写CCCD的安全要求，应该是server端使能或禁用时的安全
	//add_char_params.char_ext_props					//特征扩展属性，允许写入特征用户描述描述符，允许使用排队写入操作写入值
	//特征属性
//	add_char_params.char_props.broadcast = 1;//广播此特征
	add_char_params.char_props.notify  = 1; //使能通知
	add_char_params.char_props.write = 1; //使能可写
	add_char_params.write_access = SEC_MITM;
	add_char_params.init_len = sizeof(data_buff);							//初始化时的特征值长度
//	add_char_params.is_defered_read = true;						//indicate 是否延时读操作
//	add_char_params.is_defered_write = true;					//indicate 是否延时写操作
	add_char_params.p_init_value = data_buff;						//初始化时特征的值
//	add_char_params.p_presentation_format				//指向特征的格式，如果需要使用的话
//	add_char_params.p_user_descr						//指向用户描述
//	add_char_params.read_access = SEC_OPEN;							//读特征值的安全需求
	add_char_params.uuid = 	RTC_DATE_CHAR_UUID;
	add_char_params.uuid_type = uuid_type;
//	add_char_params.write_access = SEC_OPEN;						//写特征值的安全需求

	err_code = characteristic_add(rtc_service_handle,&add_char_params,&p_rtcdate->rtc_characteristic_date_handle);
	G_CHECK_ERROR_CODE_INFO(err_code);
	NRF_LOG_INFO("%d",sizeof(data_buff));
}

/************************************************
说明:初始rtc时间服务
函数名:void ble_service_rtcTime_init(void);
参数:
返回值:
**************************************************/
void ble_service_rtcTime_init(void)
{
	RTC2_init(NULL);
	service_rtc_init(&m_rtcdate);
}

/************************************************
说明:notify 函数
函数名:void ble_service_rtcTime_send(ble_rtcdate_t *p_rtcdate,uint8_t *data_buff,uint16_t data_length)
参数:p_rtcdate handle
返回值:
**************************************************/

void ble_service_rtcTime_send(ble_rtcdate_t *p_rtcdate,uint8_t *data_buff,uint16_t data_length)
{
	ble_gatts_hvx_params_t hvx_params;
	memset(&hvx_params,0,sizeof(hvx_params));
	

	hvx_params.handle =p_rtcdate->rtc_characteristic_date_handle.value_handle;
	hvx_params.p_data = data_buff;
	hvx_params.p_len = &data_length;
	hvx_params.type = BLE_GATT_HVX_NOTIFICATION;	//使用notification 通知
	sd_ble_gatts_hvx(p_rtcdate->rtc_connect,&hvx_params);	//notification 函数	
}

#if RTC_FREERTOS

#define DateUpdateSemMaxCount 1
#define DateUpdateSemInitCount 0
//SemaphoreHandle_t DateUpdateSem;

#define DATE_UPDATE_TASK_SIZE 50
#define DATE_UPDATE_TASK_PRIO 2
TaskHandle_t DateUpdateTaskHandle;
void DateUpdateTaskHandler(void *pvParamenters);

void DateUpdateTaskHandler(void *pvParamenters)
{
	static uint8_t battery_level = 0;
	BaseType_t xReturn = pdPASS;
	uint8_t buff[7] = {0};
	while(1)
	{
		xReturn = xSemaphoreTake(DateUpdateSem, portMAX_DELAY);
		if(pdTRUE == xReturn)
			{
			update_time();
		data_convert(current_time,buff);
		ble_service_rtcTime_send(&m_rtcdate,buff,7);
		battery_level++;
		if(battery_level >= 100)
			battery_level = 0;
		bas_notification_send(battery_level,m_rtcdate.rtc_connect);
			
			}
	}
}	


/************************************************
说明:创建freertos任务
函数名:void rtcdate_task_init(void)
参数:p_rtcdate handle
返回值:
**************************************************/
void rtcdate_task_init(void)
{
	//创建计数信号量
	DateUpdateSem = xSemaphoreCreateCounting(DateUpdateSemMaxCount,DateUpdateSemInitCount);
	if(DateUpdateSem == NULL)
	{
		NRF_LOG_ERROR("Semaphore create error");
	}
	xTaskCreate((TaskFunction_t)DateUpdateTaskHandler,"DateUpdate",DATE_UPDATE_TASK_SIZE,NULL,DATE_UPDATE_TASK_PRIO,DateUpdateTaskHandle);
}

#endif


#endif


#endif

