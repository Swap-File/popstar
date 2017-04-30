// zx.h

#ifndef _ZX_h
#define _ZX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "globals.h"

//i2c addresses
#define SENSOR1             0x10
#define SENSOR2             0x11

#define ZX_STATUS           0x00
#define ZX_XPOS             0x08
#define ZX_ZPOS             0x0A
#define ZX_GESTURE          0x04
#define ZX_GSPEED           0x05

//gesture constants
#define GESTURE_NONE 0
#define GESTURE_RIGHT 1
#define GESTURE_LEFT 2
#define GESTURE_UP 3
#define GESTURE_HOVER 5
#define GESTURE_HOVER_LEFT 6
#define GESTURE_HOVER_RIGHT 7
#define GESTURE_HOVER_UP 8


typedef struct {
	uint8_t    speed;
	uint32_t    gesture_time;
	uint8_t    gesture;
	uint8_t    time;
	uint8_t    x;
	uint8_t    z;
	uint8_t    x_filtered;
	uint8_t    z_filtered;
	bool       gesture_fresh;
	bool       zx_fresh;
	uint8_t    status;
	uint8_t    address;
} zx_sensor;

 
void zx_init(void);
void zx_update(void);


extern zx_sensor sensor1;
extern zx_sensor sensor2;

#endif

