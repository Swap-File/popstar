#include "popstar.h"
#include "noise.h"

static void Caleidoscope1(void);
static void Caleidoscope2(void);
static void FillNoise16(void);
static void FillNoise8(void);
static void mapNoiseToLEDsUsingPalette(void);
static void mapNoiseToLEDsUsingPalette2(void);
//static uint16_t speed = 0; // speed is set dynamically once we've started up
static uint16_t scale = 2000; // scale is set dynamically once we've started up
static uint8_t noise[24][24];
static uint16_t x_noise;
static uint16_t y_noise;
static uint16_t z_noise;

void Noise(uint8_t noise_version) {
	switch (noise_version) {
	case 1:
		scale = 55;
		x_noise = x_noise + band[1] * 5;//0 to 5
		y_noise = y_noise + band[1] * 10;
		if (band[6] > 0.02) z_noise = z_noise + 2;
		if (band[9] > 0.02) z_noise = z_noise + 2;
		//else{ z=z-2;}
		FillNoise16();
		break;
	case 2:
		scale = 40;
		x_noise = x_noise + band[1] * 10;
		//y = y ;
		//if (band[4||5||6] > 0.06) z=z+2;
		//if (band[7||8||9] > 0.06) z=z+1;
		//else{ z=z-2;}
		FillNoise8();
		break;
	case 3:
		scale = 50;
		x_noise = x_noise + band[1] * 10;
		//y = y ;
		//if (band[4||5||6] > 0.06) z=z+2;
		//if (band[7||8||9] > 0.06) z=z+1;
		//else{ z=z-2;}
		FillNoise8();
		break;
	case 4:
		scale = 40;
		x_noise = x_noise + band[1] * 12;
		//y = y + band[1]*40;
		if (band[4 || 5 || 6] > 0.04) z_noise = z_noise + 2;
		if (band[7 || 8 || 9] > 0.04) z_noise = z_noise + 1;
		//else{ z=z-2;}
		FillNoise8();
		break;
	case 5:
		scale = 60;
		x_noise = x_noise + band[0] * 10;
		y_noise = y_noise + band[0] * 5;
		//if (band[4||5||6] > 0.06) z=z+1;
		//if (band[7||8||9] > 0.06) z=z+1;
		z_noise = z_noise - 2;
		FillNoise16();
		break;
	case 6:
		scale = 140 - (band[1] * 100);
		x_noise = x_noise + band[1] * 20;
		y_noise = y_noise + band[1] * 5;
		//if (band[4||5||6] > 0.06) z=z+3;
		//if (band[7||8||9] > 0.06) z=z+1;
		//else{ z=z-2;}
		FillNoise8();
		break;
	case 7:
		scale = 40;
		x_noise = x_noise - 1;
		y_noise = y_noise + band[0] * 20;
		if (band[4 || 5 || 6] > 0.06) z_noise = z_noise + 3;
		if (band[7 || 8 || 9] > 0.06) z_noise = z_noise + 3;
		z_noise = z_noise - 1;
		FillNoise8();
		break;
	case 8:
		scale = 100 - (band[1] * 100);
		x_noise = x_noise + band[1] * 5;
		y_noise = y_noise + band[1] * 10;
		if (band[4 || 5 || 6] > 0.06) z_noise = z_noise + 3;
		if (band[7 || 8 || 9] > 0.06) z_noise = z_noise + 1;
		else {
			z_noise = z_noise - 2;
		}
		FillNoise16();
		break;
	case 9:
		x_noise = x_noise + band[1] * 20;
		y_noise = y_noise + band[1] * 10;
		scale = 60;
		//if (band[4||5||6] > 0.06) z=z-1;
		//if (band[7||8||9] > 0.06) z=z-1;
		z_noise = z_noise - 2;
		FillNoise8();
		break;
	case 10:
		x_noise = x_noise + band[1] * 20;
		y_noise = y_noise + band[1] * 20;
		scale = 200;
		if (band[4 || 5 || 6] > 0.06) z_noise = z_noise - 1;
		if (band[7 || 8 || 9] > 0.06) z_noise = z_noise - 1;
		else {
			z_noise = z_noise - 2;
		}
		FillNoise16();
		break;
	case 11:
		x_noise = x_noise + band[1] * 80;
		y_noise = y_noise + band[1] * 80;
		scale = 60;
		if (band[4 || 5 || 6] > 0.06) z_noise = z_noise - 1;
		if (band[7 || 8 || 9] > 0.06) z_noise = z_noise - 1;
		else {
			z_noise = z_noise - 2;
		}
		FillNoise8();
		break;
	}

	
	mapNoiseToLEDsUsingPalette2();

	switch (noise_version) {
	case 1:
	case 2:
	case 5:
	case 6:
	case 7:
	case 9:
	case 11:
		Caleidoscope2();
		break;
	case 3:
	case 4:
	case 8:
	case 10:
		Caleidoscope1();
		break;
	}
}

static void Caleidoscope1(void) {
	for (int x = 0; x < kMatrixWidth / 2; x++) {
		for (int y = 0; y < kMatrixHeight / 2; y++) {
			Background_Array[kMatrixWidth - 1 - x][y] = Background_Array[y][x];
			Background_Array[kMatrixWidth - 1 - x][kMatrixHeight - 1 - y] = Background_Array[x][y];
			Background_Array[x][kMatrixHeight - 1 - y] = Background_Array[y][x];
		}
	}
}

static void Caleidoscope2(void) {
	for (int x = 0; x < kMatrixWidth / 2; x++) {
		for (int y = 0; y < kMatrixHeight / 2; y++) {
			Background_Array[kMatrixWidth - 1 - x][y] = Background_Array[x][y];
			Background_Array[x][kMatrixHeight - 1 - y] = Background_Array[x][y];
			Background_Array[kMatrixWidth - 1 - x][kMatrixHeight - 1 - y] = Background_Array[x][y];
		}
	}
}

static void FillNoise16(void) {
	for (int i = 0; i < kMatrixWidth; i++) {
		int ioffset = scale * i;
		for (int j = 0; j < kMatrixHeight; j++) {
			int joffset = scale * j;
			noise[i][j] = inoise16(x_noise + ioffset, y_noise + joffset);
		}
	}
}

static void FillNoise8(void) {
	for (int i = 0; i < kMatrixWidth; i++) {
		int ioffset = scale * i;
		for (int j = 0; j < kMatrixHeight; j++) {
			int joffset = scale * j;
			noise[i][j] = inoise8(x_noise + ioffset, y_noise + joffset);
		}
	}
}

void mapNoiseToLEDsUsingPalette(void)
{
	static uint8_t ihue = 0;

	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			// We use the value at the (i,j) coordinate in the noise
			// array for our brightness, and the flipped value from (j,i)
			// for our pixel's index into the color palette.

			uint8_t index = noise[j][i];
			uint8_t bri = noise[i][j];

			// if this palette is a 'loop', add a slowly-changing base value
			//if (colorLoop) {
			//	index += ihue;
			//}

			// brighten up, as the color palette itself often contains the
			// light/dark dynamic range desired
			if (bri > 127) {
				bri = 255;
			}
			else {
				bri = dim8_raw(bri * 2);
			}

			CRGB color = ColorFromPalette(PaletteNoiseCurrent, index, bri);
			Background_Array[i][j] = color;
		}
	}

	ihue += 1;
}


void mapNoiseToLEDsUsingPalette2(void)
{
	static uint8_t ihue = 0;

	for (int i = 0; i < kMatrixWidth; i++) {
		for (int j = 0; j < kMatrixHeight; j++) {
			// We use the value at the (i,j) coordinate in the noise
			// array for our brightness, and the flipped value from (j,i)
			// for our pixel's index into the color palette.

			uint8_t index = (noise[j][i] / 2) + (band[1] / 2 + (band[6] / 4)) * 160;
			uint8_t bri = (noise[i][j] / 2) + (band[1] / 2 + (band[6] / 4)) * 160;

			// if this palette is a 'loop', add a slowly-changing base value
		//	if (colorLoop) {
		//		index += ihue;
		//	}

			// brighten up, as the color palette itself often contains the
			// light/dark dynamic range desired
			if (bri > 200) {
				bri = 255;
			}
			else {
				bri = dim8_raw(bri * 3);
			}

			CRGB color = ColorFromPalette(PaletteNoiseCurrent, index, bri);
			Background_Array[i][j] = color;
		}
	}

	ihue += 1;
}