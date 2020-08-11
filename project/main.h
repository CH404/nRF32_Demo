#ifndef __MAIN_H
#define __MAIN_H

/*********************************************
	P0.25											NC
	P0.26											P0.24
	P0.27											P0.23
	P0.28											P0.22
	P0.29											SCLK
	P0.30											SWD
	P0.31											P0.21
	P0.02											P0.20
	P0.03											P0.19
	GND											P0.18
	VCC											P0.17
	NC	  P P	P P P	P  P	P	P	P	P	P  P0.16
		  4 5 6 7 8 9 10 11 12 13 14 15

***********************************************/



#define LED_RED		15
#define LED_GREEN		18
#define LED_BLINKY_TIME    500ms


static void log_init(void);
static void timer_init(void);
static void leds_init(void);





#endif

