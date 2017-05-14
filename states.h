#ifndef _STATES_h
#define _STATES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void state_update(void);
extern char menu_master[4][5][10];
void increment_background(int8_t number);

#endif

