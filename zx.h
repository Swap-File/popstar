// zx.h

#ifndef _ZX_h
#define _ZX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "popstar.h"

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

#define ORDER_RESET_TIMEOUT 2000
#define I2C_BUS_TIMEOUT 30

typedef struct {
	uint8_t    address; //the i2c address of the sensor
	uint8_t    status; //internal status

	uint8_t    gesture_speed; //the speed of the gesture
	uint32_t   gesture_time; //when a gesture happened
	uint8_t    gesture; //the gesture itself
	bool       gesture_fresh; //if a gesture has been processed

	uint8_t    zx_time;
	uint8_t    x;  //the x coord of the sensor
	uint8_t    z; //the z cord of the sensor
	bool       zx_fresh;// if the coords have been processed

	uint8_t    order;  //0 is not in use, sequential from there in order of activation

} zx_sensor;

extern zx_sensor* sensor_first;
extern zx_sensor* sensor_second;
extern zx_sensor sensor1;
extern zx_sensor sensor2;
extern zx_sensor* current_sensor;

void zx_init(void);
void zx_update(void);
void zx_update_order(void);
void zx_reset_flags(void);
void zx_next_sensor(void);

#endif

