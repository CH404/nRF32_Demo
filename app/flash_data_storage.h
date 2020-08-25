#ifndef __FLASH_DATA_STORAGE_H
#define __FLASH_DATA_STORAGE_H

#include <stdint.h>

#include "nrf_nvmc.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_nvmc.h"
#include "nrf_fstorage_sd.h"
#include "nrf_atomic.h"
#include "crc16.h"
#include "nrf_atfifo.h"
#include "fds.h"
#ifdef FLASH_DATA_STORAGE_GLOBAL
#define FLASH_DATA_STORAGE_EXT
#else
#define FLASH_DATA_STORAGE_EXT extern 
#endif

#ifdef __cpulspuls
extern "C"{
#endif

typedef struct
{
    uint16_t record_key;
    uint16_t file_id;
    uint16_t data_length;
    void     const * p_data;
	
}m_fds_record_t;

typedef struct
{
	char hw_version[12];
	char sw_version[12];
}__attribute__((aligned(4)))device_version_t;
typedef struct
{
	uint32_t size;
}__attribute__((aligned(4)))fw_information_t;


static bool fds_init_compelet;



void fds_evt_handler(fds_evt_t const * p_fds_evt);






#ifdef __cpulspuls
}
#endif

#endif

