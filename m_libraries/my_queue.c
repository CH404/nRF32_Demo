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
��������uint8_t queue_push(queue_manager *p_queue_mgr,uint8_t data)
���룺p_queue_mgr: ���й���ṹ�� 
�����data: pull data
return: fail or success
���ã�push a byte
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
	//�������ֵ���ͷ��ʼ,rear��һֱ�ۼӣ������ʱ����ֱ��ָ��0
	//p_queue_mgr->queue_buff[rear%length] = data;
	//p_queue_mgr->rear++;

	//��buff����󣬴�ͷ��ʼ
	if((rear+1)%length == 1)
		rear = 0;
	p_queue_mgr->queue_buff[rear++] = data;
	p_queue_mgr->rear = rear;
	return QUEUE_SUCCESS;

}
/**********************************************************
��������uint8_t queue_pull(queue_manager *p_queue_mgr)
���룺p_queue_mgr: ���й���ṹ�� 
�����
return: pull������
���ã�pull a byte
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
��������uint32_t queue_pull_all(queue_manager *p_queue_mgr,uint8_t *data_buff,uint32_t pull_length)
���룺p_queue_mgr: ���й���ṹ�壬pull_length: pull ����Ŀ��0 == all pull
�����data_buff: ��������buff
return: pull �ܸ���
���ã�pull queue��pull_lengthΪ0 ���ȫ�����ݣ�����pull pull_length������
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
		return 0;	//������,�Ż�0������
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
