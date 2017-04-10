
#include "fft.h"

CHSV color1 = CHSV(0, 255, 255);
CHSV color2 = CHSV(64, 255, 255);

//audio connections
#include <Audio.h>
AudioInputAnalog         adc1(A9);  //A9 is on ADC0
AudioAnalyzeFFT256       fft256_1;
AudioConnection          patchCord1(adc1, fft256_1);

void fftinit(void) {
	AudioMemory(4);
	fft256_1.windowFunction(AudioWindowHanning256);
	fft256_1.averageTogether(6);
}

boolean fftcheck() {
	return fft256_1.available();
}

void fftmath(void) {

	for (uint8_t i = 0; i < 16; i++) {
		int16_t n = 1000 * fft256_1.read((i * 2), (i * 2) + 2);

		//de-emphasize lower frequencies
		switch (i) {
		case 0:  n = max(n - 100, 0); break;
		case 1:  n = max(n - 50, 0);  break;
		case 2:  n = max(n - 15, 0);  break;
		case 3:  n = max(n - 10, 0);  break;
		default: n = max(n - 3, 0);   break;
		}

		//falloff controll
		FFTdisplayValueMax16[i] = max(max(FFTdisplayValueMax16[i] * .97, n), 4);
		FFTdisplayValue16[i] = constrain(map(n, 0, FFTdisplayValueMax16[i], 0, 255), 0, 255);

		if (n == FFTdisplayValueMax16[i])    FFTdisplayValueMax16time[i] = millis();

		// downsample 16 samples to 8
		if (i & 0x01) {
			FFTdisplayValue8[i >> 1] = (FFTdisplayValue16[i] + FFTdisplayValue16[i - 1]) >> 1;
		}

	}
	// downsample 16 samples to 12
	for (uint8_t i = 0; i < 8; i++)  FFTdisplayValue12[i] = FFTdisplayValue16[i];
	FFTdisplayValue12[8] = (FFTdisplayValue16[8] + FFTdisplayValue16[9]) >> 1;
	FFTdisplayValue12[9] = (FFTdisplayValue16[10] + FFTdisplayValue16[11]) >> 1;
	FFTdisplayValue12[10] = (FFTdisplayValue16[12] + FFTdisplayValue16[13]) >> 1;
	FFTdisplayValue12[11] = (FFTdisplayValue16[14] + FFTdisplayValue16[15]) >> 1;

	for (uint8_t i = 0; i < 10; i++) {
		band[i] = FFTdisplayValue12[i];
		band[i] /= 900;
	}
	 
}

void update_fft_background(uint8_t background_mode) {

	if (background_mode == BACKGROUND_FFT_HORZ_BARS_RIGHT) {
		//move  data left 1
		for (uint8_t x = 1; x < 24; x++) {
			for (uint8_t y = 0; y < 16; y++) {
				Background_Array[x - 1][y] = Background_Array[x][y];
			}
		}
	}
	else if (background_mode == BACKGROUND_FFT_HORZ_BARS_LEFT) {
		//move  data right 1
		for (uint8_t x = 23; x > 0; x--) {
			for (uint8_t y = 0; y < 16; y++) {
				Background_Array[x][y] = Background_Array[x - 1][y];
			}
		}
	}
	else if (background_mode == BACKGROUND_FFT_VERT_BARS_UP) {
		//move  data up  1
		for (uint8_t y = 15; y > 0; y--) {
			for (uint8_t x = 0; x < 24; x++) {
				Background_Array[x][y] = Background_Array[x][y - 1];
			}
		}
	}
	else if (background_mode == BACKGROUND_FFT_VERT_BARS_DOWN) {
		//move data up  1
		for (uint8_t y = 1; y < 16; y++) {
			for (uint8_t x = 0; x < 24; x++) {
				Background_Array[x][y - 1] = Background_Array[x][y];
			}
		}
	}

	if (background_mode == BACKGROUND_FFT_HORZ_BARS_RIGHT || background_mode == BACKGROUND_FFT_HORZ_BARS_LEFT) {
		for (uint8_t i = 0; i < 16; i++) {
			//make the tip of the color be color 2
			CHSV temp_color;
			calcfftcolor(&temp_color, FFTdisplayValue16[i]);
			if (background_mode == BACKGROUND_FFT_HORZ_BARS_RIGHT)  Background_Array[23][i] = temp_color;
			else if (background_mode == BACKGROUND_FFT_HORZ_BARS_LEFT) Background_Array[0][i] = temp_color;
		}
	}
	else if (background_mode == BACKGROUND_FFT_HORZ_BARS_STATIC) {
		for (uint8_t i = 0; i < 16; i++) {
			//make the tip of the color be color 2
			CHSV temp_color;
			calcfftcolor(&temp_color, FFTdisplayValue16[i]);
			for (uint8_t index = 0; index < 24; index++) {
				Background_Array[index][i] = temp_color;
			}
		}
	}
	else if (background_mode == BACKGROUND_FFT_VERT_BARS_UP || background_mode == BACKGROUND_FFT_VERT_BARS_DOWN) {
		for (uint8_t i = 0; i < 16; i++) {
			//make the tip of the color be color 2
			CHSV temp_color;
			calcfftcolor(&temp_color, FFTdisplayValue16[i]);
			if (background_mode == BACKGROUND_FFT_VERT_BARS_UP) {
				Background_Array[i][0] = temp_color;
				Background_Array[23 - i][0] = temp_color;
			}
			else if (background_mode == BACKGROUND_FFT_VERT_BARS_DOWN) {
				Background_Array[i][15] = temp_color;
				Background_Array[23 - i][15] = temp_color;
			}
		}
	}
	else if (background_mode == BACKGROUND_FFT_VERT_BARS_STATIC) {
		for (uint8_t i = 0; i < 12; i++) {
			//make the tip of the color be color 2
			CHSV temp_color;
			calcfftcolor(&temp_color, FFTdisplayValue12[i]);
			for (uint8_t index = 0; index < 16; index++) {
				Background_Array[i][index] = temp_color;
				Background_Array[23 - i][index] = temp_color;
			}
		}
	}
}


void calcfftcolor(CHSV * temp_color, uint8_t input) {

	//make the tip of the color be color 2
	*temp_color = (input > 240) ? map_hsv(input, 240, 255, &color1, &color2) : color1;

	//ignore brightness, max it.
	temp_color->v = input;

	return;
}


CHSV map_hsv(uint8_t input, uint8_t in_min, uint8_t in_max, CHSV* out_starting, CHSV* out_ending) {


	if (input <= in_min) return CHSV(*out_starting);
	if (input >= in_max) return CHSV(*out_ending);

	//calculate shortest path between colors
	int16_t shortest_path = out_ending->h; //no rollover
	if ((((int16_t)out_ending->h) + 256) - ((int16_t)out_starting->h) <= 127) {
		shortest_path += 256;  //rollover
	}
	else if ((int16_t)(out_starting->h) - (((int16_t)out_ending->h) - 255) <= 127) {
		shortest_path -= 256; //rollunder
	}


	return CHSV(
		((input - in_min) * (shortest_path - out_starting->h + 1) / (in_max - in_min + 1) + out_starting->h), \
		(input - in_min) * (out_ending->s - out_starting->s + 1) / (in_max - in_min + 1) + out_starting->s, \
		(input - in_min) * (out_ending->v - out_starting->v + 1) / (in_max - in_min + 1) + out_starting->v);
}


uint16_t XY(uint8_t x, uint8_t y)
{

	uint16_t tempindex = 0;
	//determine if row is even or odd abd place pixel
	if ((x & 0x01) == 1)  tempindex = ((7 - (x % 8)) << 4) + y; //<< 4 is multiply by 16 pixels per row
	else                  tempindex = ((7 - (x % 8)) << 4) + 15 - y; //<< 4 is multiply by 16 pixels per row

	if (x >= 16)      tempindex += 6 * 128;
	else if (x >= 8)  tempindex += 1 * 128;
	else              tempindex += 0 * 128;

	return tempindex;
}

