// states.h

#ifndef _STATES_h
#define _STATES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void state_update(void);


#define MENU_LOCKED 128
#define MENU_LOCKED_TXT "LOCKED"
#define MENU_ROOT_FIRST 129
#define MENU_ROOT_EFFECTS 129
#define MENU_ROOT_EFFECTS_TXT "EFFECTS"
#define MENU_ROOT_COLORS 130
#define MENU_ROOT_COLORS_TXT "COLORS"
#define MENU_ROOT_LAST 130

#endif

