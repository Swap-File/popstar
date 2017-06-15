#ifndef _STATES_h
#define _STATES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MENU_OFF 0
#define MENU_ON 1

#define MENU_LIST_FIRST 10
#define MENU_TOGGLE_AUTO_BACKGROUND  12
#define MENU_TOGGLE_AUTO_COLOR 11
#define MENU_ENTER_ANI_MODE 10
#define MENU_ENTER_NOISE_MODE 14
#define MENU_TURN_OFF 13
#define MENU_TOGGLE_SPOTLIGHT 15
#define MENU_TOGGLE_IR_BONUS 16

#define ACTIVITIY_NONE 0
#define ACTIVITIY_BUTTON 1
#define ACTIVITIY_MOTION 2

extern uint8_t menu_state_last;
extern uint8_t menu_state;
void gui(uint8_t button_to_handle, uint8_t activitiy);
void state_update(void);
void increment_background(int8_t number);

#endif

