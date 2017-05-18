// el.h

#ifndef _EL_h
#define _EL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define EL_ANI_STOP 0
#define EL_ANI_LEFT 30
#define EL_ANI_RIGHT 20 
#define EL_ANI_BOOT  10
#define EL_ANI_OFF  40
#define EL_OFF 0
#define EL_ANI 1
#define EL_FFT 2

extern uint8_t EL_Mode;
extern uint8_t EL_Mode_last;
extern uint8_t EL_animation;


void update_el_state(void);

#endif

