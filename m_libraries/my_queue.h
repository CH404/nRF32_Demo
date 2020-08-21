#ifndef __MY_QUEUE_H
#define __MY_QUEUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if MY_QUEUE_ENABLE
#define QUEUE_SUCCESS 0
#define QUEUE_PARAMS_ERROR 1
#define QUEUE_EMPTY  2



typedef struct
{
	uint8_t *queue_buff;
	uint32_t queue_length;
	uint32_t rear;
	uint32_t front;
}queue_manager;


uint8_t queue_init(queue_manager * p_queue_mgr,uint8_t *buff,uint32_t length);
uint8_t queue_push(queue_manager * p_queue_mgr, uint8_t data);
uint8_t queue_pull(queue_manager * p_queue_mgr);

#endif


#ifdef __cplusplus
}
#endif





#endif

