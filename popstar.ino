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

//init extern globals
uint16_t FFTdisplayValueMax16[16]; //max vals for normalization over time
uint32_t FFTdisplayValueMax16time[16]; //when maxval is hit
uint8_t FFTdisplayValue16[16]; //max vals for normalization over time
uint8_t FFTdisplayValue8[8]; //max vals for normalization over time
uint8_t FFTdisplayValue12[12];
float band[16];//holdover from noise effects, adjust noise to directly use fft bins for speedup
CRGBPalette16 currentPalette(LavaColors_p);
CRGB Background_Array[24][16];
boolean EL_Strips[8];
uint8_t menu_location = MENU_LOCKED;

#define HOLD_PALETTES_X_TIMES_AS_LONG 3
#define CHNGSPEED 2

CRGBPalette16 targetPalette(PartyColors_p);

int8_t requested_palette = 0;

Metro timer_1hz = Metro(1000, 1);
Metro timer_100hz = Metro(10, 1);
Metro GlitterSpeed = Metro(10, 1);
Metro PaletteBlender = Metro(10, 1);

int8_t background_mode = BACKGROUND_FFT_FIRST;
int8_t background_mode_last = 255;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

				  //snapshot for fades
CRGB Snapshot_Array[24][16];

//actual output for Fastled with OctoWS2811
CRGB Output_Array[128 * 8];

void setup() {
	oled_init();
	FastLED.addLeds<OCTOWS2811>(Output_Array, 128);
	FastLED.setBrightness(128);
	Serial.begin(115200); //debug
	Serial1.begin(57600);  //el wire output and IMU input
	ChangeTargetPalette(requested_palette);  //set initial palette
	zx_init();
	fft_init();
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

	if (PaletteBlender.check()) {
		nblendPaletteTowardPalette(currentPalette, targetPalette, CHNGSPEED);
	}

	if (background_mode == BACKGROUND_GLITTER) {
		if (background_mode_last != BACKGROUND_GLITTER) add_glitter(3); //prime the glitter
		if (GlitterSpeed.check()) add_glitter(1);
	}

	//update the array
	for (uint8_t y = 0; y < 16; y++) {
		for (uint8_t x = 0; x < 24; x++) {
			CRGB final_color = CRGB(0, 0, 0);
			final_color = Background_Array[x][y];
			Output_Array[XY(x, y)] = final_color;
		}
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
	}

	//snapshot the last image if we changed modes
	if (background_mode != background_mode_last) memcpy(Snapshot_Array, Background_Array, 24 * 16 * sizeof(CRGB));

	background_mode_last = background_mode;
}

void add_glitter(uint8_t points) {
	for (uint8_t i = 0; i < points; i++) {
		fadeToBlackBy(&Background_Array[0][0], 128 * 3, 10);
		uint8_t x = random16(24);
		uint8_t y = random16(16);
		Background_Array[x][y] += CHSV(gHue + random8(64), 200, 255);
	}
}

void ChangeTargetPalette(uint8_t requested_palette)
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
		if (requested_palette == 0) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, w, w, b, b, b, b, b, bl); }
		if (requested_palette == 1) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, w, a, g, g, g, r, bl, bl); }
		if (requested_palette == 2) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, a, p, pi, pi, pi, r, bl, bl); }
		if (requested_palette == 3) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, g, a, b, b, b, b, bl, bl); }
		if (requested_palette == 4) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, p, p, y, y, bl, bl, bl, bl); }
		if (requested_palette == 5) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, a, a, y, y, y, y, bl, bl); }
		if (requested_palette == 6) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, b, r, bl, bl, bl, bl, bl, bl); }
		if (requested_palette == 7) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, o, o, p, bl, bl, bl, bl, bl); }
		if (requested_palette == 8) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, r, o, y, g, bl, bl, bl, bl); }
		if (requested_palette == 9) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, b, b, a, ra, ra, bl, bl, bl); }
		if (requested_palette == 10) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, r, r, random8(), random8(), bl, bl, bl, bl); }
		if (requested_palette == 11) { targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, bl, b, p, pi, r, bl, bl, bl, bl); }
}

uint16_t XY(uint16_t x, uint16_t y) {

	uint16_t tempindex = 0;
	//determine if row is even or odd abd place pixel
	if ((x & 0x01) == 1)  tempindex = ((7 - (x % 8)) << 4) + y; //<< 4 is multiply by 16 pixels per row
	else                  tempindex = ((7 - (x % 8)) << 4) + 15 - y; //<< 4 is multiply by 16 pixels per row

	if (x >= 16)      tempindex += 6 * 128;
	else if (x >= 8)  tempindex += 1 * 128;
	else              tempindex += 0 * 128;

	return tempindex;
}

