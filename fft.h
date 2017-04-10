// fft.h

#ifndef _FFT_h
#define _FFT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "globals.h"
boolean fftcheck(void);
void fftinit(void);
void fftmath(void);
void update_fft_background(uint8_t background_mode);
void calcfftcolor(CHSV * temp_color, uint8_t input);
CHSV map_hsv(uint8_t input, uint8_t in_min, uint8_t in_max, CHSV* out_starting, CHSV* out_ending);

uint16_t XY(uint8_t x, uint8_t y);

#endif

