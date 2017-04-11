// globals.h
#ifndef _GLOBALS_h
#define _GLOBALS_h

#include <Audio.h>
#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include <FastLED.h>

#define kMatrixWidth 24
#define kMatrixHeight 16

//background modes
#define BACKGROUND_FFT_FIRST 0
#define BACKGROUND_FFT_HORZ_BARS_LEFT 0
#define BACKGROUND_FFT_HORZ_BARS_RIGHT 1
#define BACKGROUND_FFT_HORZ_BARS_STATIC 2
#define BACKGROUND_FFT_VERT_BARS_UP 3
#define BACKGROUND_FFT_VERT_BARS_DOWN 4
#define BACKGROUND_FFT_VERT_BARS_STATIC 5
#define BACKGROUND_FFT_LAST 5
#define BACKGROUND_NOISE_FIRST 6
#define BACKGROUND_NOISE_1 6
#define BACKGROUND_NOISE_2 7
#define BACKGROUND_NOISE_3 8
#define BACKGROUND_NOISE_4 9
#define BACKGROUND_NOISE_5 10
#define BACKGROUND_NOISE_6 11
#define BACKGROUND_NOISE_7 12
#define BACKGROUND_NOISE_8 13
#define BACKGROUND_NOISE_9 14
#define BACKGROUND_NOISE_10 15
#define BACKGROUND_NOISE_11 16
#define BACKGROUND_NOISE_LAST 16
#define BACKGROUND_GLITTER 17



//FFT data
extern uint16_t FFTdisplayValueMax16[16]; //max vals for normalization over time
extern uint32_t FFTdisplayValueMax16time[16]; //when maxval is hit
extern uint8_t FFTdisplayValue16[16]; //max vals for normalization over time
extern uint8_t FFTdisplayValue8[8]; //max vals for normalization over time
extern uint8_t FFTdisplayValue12[12];
extern float band[16];
extern CRGBPalette16 currentPalette;
extern CRGB Background_Array[24][16];

#endif

