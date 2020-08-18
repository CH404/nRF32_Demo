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

#endif

