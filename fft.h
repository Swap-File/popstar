#ifndef _FFT_h
#define _FFT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "popstar.h"


boolean fft_check(void);
void fft_init(void);
void fft_math(void);
void fft_update_background(uint8_t background_mode);
#endif

