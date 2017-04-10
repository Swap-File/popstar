

#include "globals.h"
#include <Metro.h>
#define HOLD_PALETTES_X_TIMES_AS_LONG 3
#define CHNGSPEED 2

#include "noise.h"
#include "fft.h"

//extern globals
uint16_t FFTdisplayValueMax16[16]; //max vals for normalization over time
uint32_t FFTdisplayValueMax16time[16]; //when maxval is hit
uint8_t FFTdisplayValue16[16]; //max vals for normalization over time
uint8_t FFTdisplayValue8[8]; //max vals for normalization over time
uint8_t FFTdisplayValue12[12];
float band[10];
CRGBPalette16 currentPalette(LavaColors_p);
CRGB Background_Array[24][16];


CRGBPalette16 targetPalette(PartyColors_p);

uint8_t colorLoop = 0;

Metro LEDoutput = Metro(10, 1);

Metro Changemode = Metro(5000, 1);

Metro GlitterSpeed = Metro(10, 1);
Metro PAlletshifter = Metro(10, 1);

uint8_t background_mode = BACKGROUND_FFT_VERT_BARS_STATIC;
uint8_t background_mode_last = 255;

//snapshot for fades
CRGB Snapshot_Array[24][16];

CRGB Output_Array[128 * 8];


uint8_t gHue = 0; // rotating "base color" used by many of the patterns

				  // the setup routine runs once when you press reset:
void setup() {

	FastLED.addLeds<OCTOWS2811>(Output_Array, 128);
	FastLED.setBrightness(128);

	Serial.begin(15200);
	// initialize the digital pin as an output.

	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	pinMode(3, OUTPUT);

	fftinit();

}


void loop() {
	
	//check if FFT is available from audio
	if (fftcheck()) {
		//do the fft math to load arrays
		fftmath();
		//render fft arrays to the background
		if (background_mode <= BACKGROUND_FFT_LAST && background_mode >= BACKGROUND_FFT_FIRST) update_fft_background(background_mode);
		if (background_mode <= BACKGROUND_NOISE_LAST && background_mode >= BACKGROUND_NOISE_FIRST) Noise(background_mode - BACKGROUND_NOISE_FIRST);
	}

	if (PAlletshifter.check()) {
		ChangePalettePeriodically();
		nblendPaletteTowardPalette(currentPalette, targetPalette, CHNGSPEED);
	}
	
	if (background_mode == BACKGROUND_GLITTER) {
		if(background_mode_last != BACKGROUND_GLITTER) add_glitter(3); //prime the glitter
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
	

	if (LEDoutput.check()) {
		FastLED.show();

		//el wire
		if (millis() - FFTdisplayValueMax16time[0] < 150)  digitalWrite(0, 1);
		else digitalWrite(0, 0);
		if (millis() - FFTdisplayValueMax16time[1] < 150)  digitalWrite(1, 1);
		else digitalWrite(1, 0);
		if (millis() - FFTdisplayValueMax16time[2] < 150)  digitalWrite(3, 1);
		else digitalWrite(3, 0);
	}



	background_mode_last = background_mode;
	if (Changemode.check()) {
		
		   background_mode++;
		  if ( background_mode >  BACKGROUND_GLITTER) background_mode = BACKGROUND_FFT_FIRST;
		  Serial.println(background_mode);
	}
	
	//snapshot the last image if we changed modes
	if (background_mode != background_mode_last) memcpy(Snapshot_Array, Background_Array , 24 * 16 * sizeof(CRGB));
}

void add_glitter(uint8_t points) {
	for (uint8_t i = 0; i < points; i++) {
		fadeToBlackBy(&Background_Array[0][0], 128 * 3, 10);
		uint8_t x = random16(24);
		uint8_t y = random16(16);
		Background_Array[x][y] += CHSV(gHue + random8(64), 200, 255);
	}
}

void ChangePalettePeriodically()
{
	uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
	static uint8_t lastSecond = 99;

	if (lastSecond != secondHand) {
		lastSecond = secondHand;
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
		if (secondHand == 0) {
			// targetPalette = CRGBPalette16( bl, bl, bl, bl, bl, bl, bl, bl, w, w, b, b, b, b, b, bl);
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, w, w, w, b, b, b, b, b, bl);
		}
		if (secondHand == 5) {
			//targetPalette = CRGBPalette16( bl, bl, bl, bl, bl, bl, bl, bl, w, a, g, g, g, r, bl, bl);
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, w, w, a, g, g, g, r, bl, bl);
		}
		if (secondHand == 10) {
			// targetPalette = CRGBPalette16( bl, bl, bl, bl, bl, bl, bl, bl, a, p, pi, pi, pi, r, bl, bl);
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, a, a, p, pi, pi, pi, r, bl, bl);
		}
		if (secondHand == 15) {
			//  targetPalette = CRGBPalette16( bl, bl, bl, bl, bl, bl, bl, bl, g, a, b, b, b, b, bl, bl);
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, g, g, a, b, b, b, b, bl, bl);
		}
		if (secondHand == 20) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, p, p, p, y, y, bl, bl, bl, bl);
		}
		if (secondHand == 25) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, a, a, a, y, y, y, y, bl, bl);
		}
		if (secondHand == 30) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, b, b, r, bl, bl, bl, bl, bl, bl);
		}
		if (secondHand == 35) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, o, o, o, p, bl, bl, bl, bl, bl);
		}
		if (secondHand == 40) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, r, r, o, y, g, bl, bl, bl, bl);
		}
		if (secondHand == 45) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, b, b, b, a, ra, ra, bl, bl, bl);
		}
		if (secondHand == 50) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, r, r, r, random8(), random8(), bl, bl, bl, bl);
		}
		if (secondHand == 55) {
			targetPalette = CRGBPalette16(bl, bl, bl, bl, bl, bl, bl, b, b, p, pi, r, bl, bl, bl, bl);
		}
	}
}
