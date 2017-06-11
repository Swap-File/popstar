
#ifndef _OLErD_h
#define _OLErD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

//dpad masks
#define DPAD_LEFT B00001000
#define DPAD_RIGHT B0000010
#define DPAD_UP B00000001
#define DPAD_DOWN B00000100
#define DPAD_UP_LEFT B00001001
#define DPAD_DOWN_RIGHT B00000110
#define DPAD_UP_RIGHT B00000011
#define DPAD_DOWN_LEFT B00001100
#define DPAD_DEADZONE B00010000
#define DPAD_NONE B00000000

#define BUTTON_NONE 0
#define BUTTON_C 1
#define BUTTON_CZ 2
#define BUTTON_Z 3

extern uint8_t nunchuk_dpad;
extern uint8_t nunchuk_button;
extern uint8_t nunchuk_button_last;
extern boolean nunchuk_fresh;

extern int nunchuk_analogMagnitude;
extern int nunchuk_analogAngle;
extern int nunchuk_accelX;
extern int nunchuk_accelY;
extern int nunchuk_accelZ;

extern int nunchuk_total_acceleration;

extern int nunchuk_accelAngle;


extern void nunchuk_update();





#endif