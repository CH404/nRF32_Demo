#define FLASH_DATA_STORGAE_GLOBAL
#include "flash_data_storage.h"

#include "nrf_fstorage.h"
#include "nrf_fstorage_nvmc.h"
#include "nrf_fstorage_sd.h"
#include "nrf_atomic.h"
#include "crc16.h"
#include "nrf_atfifo.h"
#include "fds.h"
#include "global.h"

/* 所需文件
   name						 path								 brief
nrf_numc.c  			 modules\nrfx\hal  						 NVMC驱动
nrf_fstorage.c   	 components\libraries\fstorage				FS驱动文件
nrf_fstorage_nvmc.c  components\libraries\fstorage 	   FS后端nrf_fstorage_nvmc文件(裸机)
nrf_fstorage_sd.c    components\libraries\fstorage	FS后端nrf_fstorage_sd文件(Softdevice)
nrf_atomic.c			 components\libraries\atomic	   C11标准stdatomic.h简化API
crc16.c				 components\libraries\crc16					CRC16校验库文件
nrf_atfifo.c 		 components\libraries\atomic_fifo 	    	atomic FIFO
fds.c				 components\libraries\fds				    FDS 库文件
*/

//需要先注册FDS事件句柄，注册FDS事件需要先写事件处理函数
static void fds_evt_handler(fds_evt_t const * p_fds_evt)
{
	switch(p_fds_evt->id)
	{
		case FDS_EVT_INIT:
			NRF_LOG_INFO("FDS_EVT_INIT");
			fds_init_compelet = true;
			break;
		case FDS_EVT_WRITE:
			NRF_LOG_INFO("FDS_EVT_WRITE");
			break;
		case FDS_EVT_UPDATE:
			NRF_LOG_INFO("FDS_EVT_UPDATE");
			break;
		case FDS_EVT_DEL_RECORD:
			NRF_LOG_INFO("FDS_EVT_DEL_RECORD");
			break;
		case FDS_EVT_GC:
			NRF_LOG_INFO("FDS_EVT_GC");
			break;
		default:
			NRF_LOG_INFO("default");
			break;
			
	}
}




static void wait_for_fds_init(void)
{
	while (!fds_init_compelet);
}
void flash_data_storage_init(void)
{
	fds_register(fds_evt_handler);

	//初始化是异步的，需要等待
	fds_init_compelet = false;
 	ret_code_t err_code = fds_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
	wait_for_fds_init();
}
#define DEVICE_VERSION_KEY 0x22
#define DEVICE_FILE 0x33
void test_fds_record(void)
{
	static device_version_t device_version=
	{
		.hw_version = "HW-V1.11",
		.sw_version = "SW-V2.22"
	};
	static fw_information_t fw_information = 
	{
		.size = 11111,
	};
	static m_fds_record_t sd_version_record = 
	{
		.record_key = DEVICE_VERSION_KEY,
		.file_id = DEVICE_FILE,
		.p_data = &device_version,
		.data_length = (sizeof(device_version)+3)/sizeof(uint32_t),
	};

	static m_fds_record_t  fw_record = {
		.record_key = FW_INFORMATION_KEY,
		.file_id = DEVICE_FILE,
		.p_data = &fw_information,
		.data_length = (sizeof(fw_information)+3)/sizeof(uint32_t);
	};

	fds_record_desc_t desc = {0};
	fds_find_token_t tok = {0};
	memset(&desc,0,sizeof(fds_record_desc_t));
	memset(&tok,0,sizeof(fds_find_token_t));
	ret_code_t err_code = fds_record_write(desc,fw_record);
	G_CHECK_ERROR_CODE_INFO(err_code);
}

