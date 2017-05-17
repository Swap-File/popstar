
#ifndef _POPSTAR_h
#define _POPSTAR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


#include <Audio.h>
#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include "FastLED.h"



#define kMatrixWidth 24
#define kMatrixHeight 16

//background modes
#define BACKGROUND_FIRST 0
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
#define BACKGROUND_ANI_FIRST 17
#define BACKGROUND_ANI_GLITTER 17
#define BACKGROUND_ANI_JUGGLE 18
#define BACKGROUND_ANI_DRIFT 19
#define BACKGROUND_ANI_DRIFT2 20
#define BACKGROUND_ANI_MUNCH 21
#define BACKGROUND_ANI_SNAKE 22
#define BACKGROUND_ANI_WAVE 23
#define BACKGROUND_ANI_LIFE 24
#define BACKGROUND_ANI_LAST 24
#define BACKGROUND_LAST 24

#define PALETTE_FIRST 0
#define PALETTE_LAST 11

//FFT data
extern uint16_t FFTdisplayValueMax16[16]; //max vals for normalization over time
extern uint32_t FFTdisplayValueMax16time[16]; //when maxval is hit
extern uint8_t FFTdisplayValue16[16]; //max vals for normalization over time
extern uint8_t FFTdisplayValue8[8]; //max vals for normalization over time
extern uint8_t FFTdisplayValue12[12];
extern float band[16];

extern CRGB Background_Array[24][16];
extern int8_t background_mode;
extern int8_t requested_palette;

extern uint8_t menu_x;
extern uint8_t menu_y;

extern uint8_t EL_data;

void ChangeTargetPalette(uint8_t immediate);

extern CRGBPalette16 PaletteNoiseCurrent;
extern CRGBPalette16 PaletteAniCurrent;
extern CRGBPalette16 PaletteNoiseTarget;
extern CRGBPalette16 PaletteAniTarget;
extern CHSV color1;
extern CHSV color2;
#endif