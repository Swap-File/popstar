// oled.h

#ifndef _OLED_h
#define _OLED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_GFX.h> //use modified version with pixelread
#include <Adafruit_SSD1306.h> //use modified version for 64x48 OLED
#include "popstar.h"
#define OLED_DC     16
#define OLED_CS     22
#define OLED_RESET  15

void oled_reint(void);
void oled_init(void);
void oled_update(void);
void oled_load(const char * input);
extern uint32_t oled_action_time;

#endif

