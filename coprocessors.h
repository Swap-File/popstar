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


//serial commands

#define SERIAL_CVG_UP_BUTTON 0
#define SERIAL_CVG_R1_BUTTON 1
#define SERIAL_CVG_R2_BUTTON 2
#define SERIAL_CVG_R3_BUTTON 3
#define SERIAL_CVG_R4_BUTTON 4
#define SERIAL_CVG_R5_BUTTON 5

#define SERIAL_CVG_DOWN_BUTTON 6
#define SERIAL_CVG_G1_BUTTON 7
#define SERIAL_CVG_G2_BUTTON 8
#define SERIAL_CVG_G3_BUTTON 9
#define SERIAL_CVG_G4_BUTTON 10
#define SERIAL_CVG_G5_BUTTON 11

#define SERIAL_CVG_OFF_BUTTON 12
#define SERIAL_CVG_B1_BUTTON 13
#define SERIAL_CVG_B2_BUTTON 14
#define SERIAL_CVG_B3_BUTTON 15
#define SERIAL_CVG_B4_BUTTON 16
#define SERIAL_CVG_B5_BUTTON 17

#define SERIAL_CVG_ON_BUTTON 18
#define SERIAL_CVG_W_BUTTON 19
#define SERIAL_CVG_FLASH_BUTTON 20
#define SERIAL_CVG_STROBE_BUTTON 21
#define SERIAL_CVG_FADE_BUTTON 22
#define SERIAL_CVG_SMOOTH_BUTTON 23


#define SERIAL_SPOT_UP_BUTTON 24
#define SERIAL_SPOT_R1_BUTTON 25
#define SERIAL_SPOT_R2_BUTTON 26
#define SERIAL_SPOT_R3_BUTTON 27
#define SERIAL_SPOT_R4_BUTTON 28
#define SERIAL_SPOT_R5_BUTTON 29

#define SERIAL_SPOT_DOWN_BUTTON 30
#define SERIAL_SPOT_G1_BUTTON 31
#define SERIAL_SPOT_G2_BUTTON 32
#define SERIAL_SPOT_G3_BUTTON 33
#define SERIAL_SPOT_G4_BUTTON 34
#define SERIAL_SPOT_G5_BUTTON 35

#define SERIAL_SPOT_OFF_BUTTON 36
#define SERIAL_SPOT_B1_BUTTON 37
#define SERIAL_SPOT_B2_BUTTON 38
#define SERIAL_SPOT_B3_BUTTON 39
#define SERIAL_SPOT_B4_BUTTON 40
#define SERIAL_SPOT_B5_BUTTON 41

#define SERIAL_SPOT_ON_BUTTON 42
#define SERIAL_SPOT_W_BUTTON 43
#define SERIAL_SPOT_FLASH_BUTTON 44
#define SERIAL_SPOT_STROBE_BUTTON 45
#define SERIAL_SPOT_FADE_BUTTON 46
#define SERIAL_SPOT_SMOOTH_BUTTON 47

#define SERIAL_CVG_REPEAT_COMMAND 48
#define SERIAL_SPOT_REPEAT_COMMAND 49

#define SERIAL_NONE 255

extern uint8_t ir_cvg_data;
extern uint8_t ir_spot_data;
extern uint8_t ir_timer;

extern int32_t yaw_compensated;  //yaw pitch and roll in degrees * 100
extern int32_t pitch_compensated;
extern int32_t roll_compensated;

extern boolean ir_spot_on;
extern boolean ir_cvg_on;
#endif

