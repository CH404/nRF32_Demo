#ifndef __SERVICE_H
#define __SERVICE_H

#ifndef SERVICE_GLOBAL
#define SERVICE_EXT
#else
#define SERVICE_EXT	extern
#endif


SERVICE_EXT void service_init(void);
SERVICE_EXT void service_his_init(void);

SERVICE_EXT void service_dis_init(void);
SERVICE_EXT void bas_notification_send(uint8_t data,uint16_t conn_handle);
SERVICE_EXT	void service_bas_init(void);

#endif

