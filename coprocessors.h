// coprocessors.h

#ifndef _COPROCESSORS_h
#define _COPROCESSORS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "popstar.h"

void SerialUpdate(void);
void elwire_output(void);
void receivePacket(const uint8_t* buffer, size_t size);
void center_IMU(void);

extern int32_t yaw_compensated;  //yaw pitch and roll in degrees * 100
extern int32_t pitch_compensated;
extern int32_t roll_compensated;
#endif

