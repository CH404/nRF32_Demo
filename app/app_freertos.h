#ifndef __APP_FREE_H
#define __APP_FREE_H
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#ifdef APP_FREE_GLOBAL
#define APP_FREE_EXT
#else
#define APP_FREE_EXT extern 
#endif





APP_FREE_EXT void os_timer_init(void);
APP_FREE_EXT void os_timer_start(void);






#endif

