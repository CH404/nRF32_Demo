#include "my_queue.h"
#include <stdio.h>

#if MY_QUEUE_ENABLE

uint8_t queue_init(queue_manager *p_queue_mgr,uint8_t *buff,uint32_t length)
{
	bool ret = true;
	if((p_queue_mgr->queue_buff == NULL)||(p_queue_mgr->queue_length == 0))
		return false;
	p_queue_mgr->front = 0;
	p_queue_mgr->rear = 0;
	p_queue_mgr->queue_buff = buff;
	p_queue_mgr->queue_length = length;
	return true;
}

/**********************************************************
函数名：uint8_t queue_push(queue_manager *p_queue_mgr,uint8_t data)
输入：p_queue_mgr: 队列管理结构体 
输出：data: pull data
return: fail or success
作用：push a byte
**********************************************************/

uint8_t queue_push(queue_manager *p_queue_mgr,uint8_t data)
{
	uint32_t length = p_queue_mgr->queue_length;
	uint32_t front = p_queue_mgr->front;
	uint32_t rear = p_queue_mgr->rear;

	if((rear+1) % length == front)
	{
		return QUEUE_FULL;
	}
	//超过最大值则从头开始,rear会一直累加，当溢出时，会直接指向0
	//p_queue_mgr->queue_buff[rear%length] = data;
	//p_queue_mgr->rear++;

	//到buff的最后，从头开始
	if((rear+1)%length == 1)
		rear = 0;
	p_queue_mgr->queue_buff[rear++] = data;
	p_queue_mgr->rear = rear;
	return QUEUE_SUCCESS;

}
/**********************************************************
函数名：uint8_t queue_pull(queue_manager *p_queue_mgr)
输入：p_queue_mgr: 队列管理结构体 
输出：
return: pull的数据
作用：pull a byte
**********************************************************/

uint8_t queue_pull(queue_manager *p_queue_mgr)
{
	uint32_t front = p_queue_mgr->front;
	uint32_t rear = p_queue_mgr->rear;
	uint32_t length = p_queue_mgr->queue_length;
	uint8_t data;
	if(rear == front)
	{
		return QUEUE_EMPTY;
	}
	
	data = p_queue_mgr->queue_buff[front];
	if((front+1)%length == 1)
	{
		p_queue_mgr->front = 0;
		return data;
	}
	p_queue_mgr->front++;
	return data;
}
/**********************************************************
函数名：uint32_t queue_pull_all(queue_manager *p_queue_mgr,uint8_t *data_buff,uint32_t pull_length)
输入：p_queue_mgr: 队列管理结构体，pull_length: pull 的数目，0 == all pull
输出：data_buff: 接收数据buff
return: pull 总个数
作用：pull queue，pull_length为0 输出全部数据，其他pull pull_length个数据
**********************************************************/

uint32_t queue_pull_all(queue_manager *p_queue_mgr,uint8_t *data_buff,uint32_t pull_length)
{
	uint32_t front = p_queue_mgr->front;
	uint32_t rear = p_queue_mgr->rear;
	uint32_t length = p_queue_mgr->queue_length;
	uint32_t cnt = 0;
	uint32_t present_length = (rear > front) ? (rear-front) : (length-(front-rear));;
	if(front == rear)
	{
		return 0;	//无数据,放回0个数据
	}
	
	if(length == 0)
	{
		pull_length = present_length;
	}
	else
	{
		pull_length = pull_length > present_length ? present_length : present_length;
	}
	
	  do{
	  	
	     data_buff[cnt++] = queue_pull(p_queue_mgr);
		 
	  }while(pull_length--)
	  
	  return cnt;
}	


#endif
