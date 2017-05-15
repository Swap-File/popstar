#include "el.h"
#include "popstar.h"
#include "states.h"
#include <Audio.h>
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include "FastLED.h"
#include "coprocessors.h"
#include <Metro.h>
#include "noise.h"
#include "fft.h"
#include "zx.h"
#include "oled.h"
#include <ADC.h>

//FFT stuff
uint16_t FFTdisplayValueMax16[16]; //max vals for normalization over time
uint32_t FFTdisplayValueMax16time[16]; //when maxval is hit
uint8_t FFTdisplayValue16[16]; //max vals for normalization over time
uint8_t FFTdisplayValue8[8]; //max vals for normalization over time
uint8_t FFTdisplayValue12[12];
float band[16];//holdover from noise effects, adjust noise to directly use fft bins for speedup

//Color stuff
CHSV color1 = CHSV(0, 255, 255);
CHSV color2 = CHSV(64, 255, 255);
CHSV color1Target = CHSV(0, 255, 255);
CHSV color2Target = CHSV(64, 255, 255);
CRGBPalette16 PaletteNoiseCurrent(PartyColors_p);
CRGBPalette16 PaletteAniCurrent(PartyColors_p);
CRGBPalette16 PaletteNoiseTarget(PartyColors_p);
CRGBPalette16 PaletteAniTarget(PartyColors_p);
int8_t requested_palette = 0;
#define BLEND_AMT 3

CRGB Output_Array[128 * 8]; //actual output for Fastled with OctoWS2811
CRGB Snapshot_Array[24][16];  //snapshot for fades
CRGB Background_Array[24][16]; //actual background Array

//auto update true or false
boolean palette_auto = false;
boolean background_auto = false;


float voltage = 24.0;

uint32_t ani_timer_delay = 5;
uint32_t ani_timer = 0;

uint8_t menu_x = 0;
uint8_t menu_y = 0;
#define MATRIX_WIDTH 24
#define MATRIX_HEIGHT 16
const int MATRIX_CENTER_X = MATRIX_WIDTH / 2;
const int MATRIX_CENTER_Y = MATRIX_HEIGHT / 2;


#include "PatternsEffects.h"
#include "PatternsSnake.h"
#include "Patterns.h"
#include "PatternsMunch.h"

Metro timer_1hz = Metro(1000, 1);
Metro timer_100hz = Metro(10, 1);
Metro PaletteBlender = Metro(10, 1);

int32_t palette_change_time = 0;
int32_t background_change_time = 0;
int8_t background_mode = BACKGROUND_ANI_LAST;
int8_t background_mode_last = BACKGROUND_ANI_LAST;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// adc object for battery and temp meter, will use ADC1
ADC *adc = new ADC();

void setup() {
	oled_init();
	FastLED.addLeds<OCTOWS2811>(Output_Array, 128);
	FastLED.setBrightness(128);
	Serial.begin(115200); //debug
	Serial1.begin(57600);  //el wire output and IMU input
	ChangeTargetPalette(1);  //set initial palette
	zx_init();
	fft_init();

	//ADC1 setup
	pinMode(A3, INPUT);
	adc->setReference(ADC_REFERENCE::REF_3V3, ADC_1);
	adc->setAveraging(32, ADC_1);
	adc->setResolution(16, ADC_1);
	adc->startContinuous(A3, ADC_1);
}

void loop() {

	zx_update(); //initiate i2c background DMA Transfers
	SerialUpdate();
	state_update();

	//check if FFT is available from audio
	if (fft_check()) {
		//do the fft math to load arrays
		fft_math();
		//render fft arrays to the background
		if (background_mode <= BACKGROUND_FFT_LAST && background_mode >= BACKGROUND_FFT_FIRST) fft_update_background(background_mode);
		if (background_mode <= BACKGROUND_NOISE_LAST && background_mode >= BACKGROUND_NOISE_FIRST) Noise(background_mode - BACKGROUND_NOISE_FIRST + 1);
	}

	update_el();

	if (PaletteBlender.check()) {
		nblendPaletteTowardPalette(PaletteNoiseCurrent, PaletteNoiseTarget, BLEND_AMT);
		nblendPaletteTowardPalette(PaletteAniCurrent, PaletteAniTarget, BLEND_AMT);
		nblend(color1, color1Target, BLEND_AMT);
		nblend(color2, color2Target, BLEND_AMT);
	}
	 
	if (background_mode <= BACKGROUND_ANI_LAST && background_mode >= BACKGROUND_ANI_FIRST) {

		if (background_mode != background_mode_last || millis() - ani_timer > ani_timer_delay) {
			switch (background_mode) {
			case BACKGROUND_ANI_MUNCH:
				drawFrame_Munch();
				ani_timer_delay = 10;
				break;
			case BACKGROUND_ANI_DRIFT:
				drawFrame_IncrementalDrift();
				ani_timer_delay = 10;
				break;
			case BACKGROUND_ANI_GLITTER:
				if (background_mode_last != background_mode) drawFrame_glitter(3);
				drawFrame_glitter(1);
				ani_timer_delay = 10;
				break;
			case BACKGROUND_ANI_JUGGLE:
				drawFrame_juggle();
				ani_timer_delay = 10;
				break;
			case BACKGROUND_ANI_SNAKE:
				if (background_mode_last != background_mode) drawFrame_Snake_Setup();
				drawFrame_Snake();
				ani_timer_delay = 20;
				break;
			case BACKGROUND_ANI_DRIFT2:
				drawFrame_IncrementalDrift2();
				ani_timer_delay = 10;
				break;
			}
			ani_timer = millis();
		}
	}

	//update the array
	for (uint8_t y = 0; y < 16; y++) {
		for (uint8_t x = 0; x < 24; x++) {
			CRGB final_color = CRGB(0, 0, 0);
			final_color = Background_Array[x][y];
			Output_Array[XY(x, y)] = final_color;
		}
	}

	//update internal strip
	for (uint8_t i = 0; i < 8; i++) {
		Output_Array[i + 1*128] = Background_Array[i][0];
	}
	//100hz framerate
	if (timer_100hz.check()) {
		oled_update();
		FastLED.show();
		elwire_output();
	}

	if (timer_1hz.check()) {
		//keep re-initing the screen for hot plug support.
		oled_reint();
		voltage = voltage * .95 + .05 * ((uint16_t)adc->analogReadContinuous(ADC_1)) / 3516.083016;
		Serial.println(voltage);
	}

	if (millis() - palette_change_time > 10000 && palette_auto) {//30 second palette changer
		requested_palette++;
		ChangeTargetPalette(0);
		palette_change_time = millis();
	}

	if (millis() - background_change_time > 10000 && background_auto) {//30 second background changer
		increment_background(1);
		background_change_time = millis();
	}


	//snapshot the last image if we changed modes
	if (background_mode != background_mode_last) memcpy(Snapshot_Array, Background_Array, 24 * 16 * sizeof(CRGB));
	background_mode_last = background_mode;
}


void ChangeTargetPalette(uint8_t immediate)
{
	CRGB r = CHSV(HUE_RED, 255, 255);
	CRGB o = CHSV(HUE_ORANGE, 255, 255);
	CRGB y = CHSV(HUE_YELLOW, 255, 255);
	CRGB g = CHSV(HUE_GREEN, 255, 255);
	CRGB a = CHSV(HUE_AQUA, 255, 255);
	CRGB b = CHSV(HUE_BLUE, 255, 255);
	CRGB p = CHSV(HUE_PURPLE, 255, 255);
	CRGB pi = CHSV(HUE_PINK, 255, 255);
	//CRGB be = CRGB::Beige;
	CRGB bl = CRGB::Black;
	CRGB w = CRGB::White;
	CRGB ra = CHSV(75, 255, 255);

	requested_palette = (requested_palette + 12) % 12; //wrap palettes

	switch (requested_palette) {
	case 0:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, w, w, b, b, b, b, b, bl);
		PaletteAniTarget = CRGBPalette16(w, w, w, b, b, b, b, b, w, w, w, b, b, b, b, b);
		break;
	case 1:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, w, a, g, g, g, r, bl, bl);
		PaletteAniTarget = CRGBPalette16(w, a, g,  g, r, w, a, g, g, g, r, w, a, g,  g, r);
		break;
	case 2:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, a, p, pi, pi, pi, r, bl, bl);
		PaletteAniTarget = CRGBPalette16(a, p, pi, pi, r, a, p, pi, pi, pi, r, a, p,  pi, pi, r);
		break;
	case 3:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, g, a, b, b, b, b, bl, bl);
		PaletteAniTarget = CRGBPalette16(g, a, b, b, b,  g, a, b, b,  b, g, a, b, b, b,b );
		break;
	case 4:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, p, p, y, y, bl, bl, bl, bl);
		PaletteAniTarget = CRGBPalette16(p, p, y, y, p, p, y, y, p, p, y, y, p, p, y, y);
		break;
	case 5:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, a, a, y, y, y, y, bl, bl);
		PaletteAniTarget = CRGBPalette16(a, a, y, y,  y, a, a, y, y, y, y, a, a, y, y, y );
		break;
	case 6:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, b, r, bl, bl, bl, bl, bl, bl);
		PaletteAniTarget = CRGBPalette16(b, r, b, r, b, r, b, r, b, r, b, r, b, r, b, r);
		break;
	case 7:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, o, o, p, bl, bl, bl, bl, bl);
		PaletteAniTarget = CRGBPalette16(o, o, p, o, o, p, o, o, p, o, o, p, o, o, p, o);
		break;
	case 8:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, r, o, y, g, bl, bl, bl, bl);
		PaletteAniTarget = CRGBPalette16(r, o, y, g, r, o, y, g, r, o, y, g, r, o, y ,g);
		break;
	case 9:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, b, b, a, ra, ra, bl, bl, bl);
		PaletteAniTarget = CRGBPalette16(b, b, a, ra, ra, b, b, a, ra, ra, b, b, a, ra, ra ,b);
		//color1Target = 
	//	color2Target = 
		break;
	case 10:
	{
		uint8_t temp1 = random8();
		uint8_t temp2 = random8();
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, r, r, temp1, temp2, bl, bl, bl, bl);
		PaletteAniTarget = CRGBPalette16(r, r, temp1, temp2, r, r, temp1, temp2, r, r, temp1, temp2, r, r, temp1, temp2 );
	}
		break;
	case 11:
		PaletteNoiseTarget = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, b, p, pi, r, bl, bl, bl, bl);
		PaletteAniTarget = CRGBPalette16(b, p, pi, r, b, p, pi, r, b, p, pi, r, b, p, pi, r);
		break;
	}

	if (immediate) {
		color1 = color1Target;
		color2 = color2Target;
		PaletteNoiseCurrent = PaletteNoiseTarget;
		PaletteAniCurrent = PaletteAniTarget;
	}

	palette_change_time = millis();
}


uint16_t XY(uint16_t x, uint16_t y) {

	uint16_t tempindex = 0;
	//determine if row is even or odd abd place pixel
	if ((x & 0x01) == 1)  tempindex = ((7 - (x % 8)) << 4) + y; //<< 4 is multiply by 16 pixels per row
	else                  tempindex = ((7 - (x % 8)) << 4) + 15 - y; //<< 4 is multiply by 16 pixels per row

	if (x >= 16)      tempindex += 6 * 128;
	else if (x >= 8)  tempindex += 5 * 128;
	else              tempindex += 4 * 128;

	return tempindex;
}
