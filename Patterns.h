
void drawFrame_IncrementalDrift() {
	uint8_t dim = beatsin8(2, 230, 250);
	DimAll(dim);

	for (int i = 2; i <= MATRIX_WIDTH / 2; i++) {

		CRGB color = ColorFromCurrentPalette((i - 2) * (240 / (MATRIX_WIDTH / 2)));

		uint8_t x = beatcos8((17 - i) * 2, MATRIX_CENTER_X - i, MATRIX_CENTER_X + i);
		uint8_t y = beatsin8((17 - i) * 2, MATRIX_CENTER_Y - i, MATRIX_CENTER_Y + i);

		if (x < MATRIX_WIDTH && y < MATRIX_HEIGHT)   Background_Array[x][y] = color;

	}
}

void drawFrame_IncrementalDrift2() {
	uint8_t dim = beatsin8(2, 170, 250);
	DimAll(dim);

	for (uint8_t i = 0; i < 32; i++)
	{
		CRGB color;

		int x = 0;
		int y = 0;

		if (i < 16) {
			x = beatcos8((i + 1) * 2, i + 3, MATRIX_WIDTH - i + 3);
			y = beatsin8((i + 1) * 2, i, MATRIX_HEIGHT - i);
			color = ColorFromCurrentPalette(i * 14);
		}
		else
		{
			x = beatsin8((24 - i) * 2, MATRIX_WIDTH - i + 3, i + 1 + 3);
			y = beatcos8((24 - i) * 2, MATRIX_HEIGHT - i, i + 1);
			color = ColorFromCurrentPalette((31 - i) * 14);
		}

		if (x < 24 && y < 16) Background_Array[x][y] = color;
	}

	//blank the center pixel 
	Background_Array[11][8] = CRGB(0, 0, 0);
}

byte thetaUpdate = 0;
byte thetaUpdateFrequency = 0;
byte theta = 0;

byte hueUpdate = 0;
byte hueUpdateFrequency = 0;
byte hue = 0;

void drawFrame_wave() {

	for (int y = 0; y < MATRIX_HEIGHT; y++) {
		int n = quadwave8(y * 2 + theta) / 11;  //11 is the number to constrain output to the range of 0-23
		Background_Array[n][y] = ColorFromPalette(PaletteAniCurrent, y + hue);
	}

	DimAll(240);

	if (thetaUpdate >= thetaUpdateFrequency) {
		thetaUpdate = 0;
		theta++;
	}
	else {
		thetaUpdate++;
	}

	if (hueUpdate >= hueUpdateFrequency) {
		hueUpdate = 0;
		hue++;
	}
	else {
		hueUpdate++;
	}

	return;
}

void drawFrame_glitter
(uint8_t points) {
	for (uint8_t i = 0; i < points; i++) {
		// fadeToBlackBy(&Background_Array[0][0], 384, 10);
		DimAll(240);
		uint8_t x = random8(24);
		uint8_t y = random8(16);

		//Background_Array[x][y] += CHSV(random8(255), 200, 255);
		Pixel(x, y, random8(255));
	}
}

uint8_t x(uint16_t i) {
	return i >> 4; //division by 16
}

uint8_t y(uint16_t i) {
	return i % 16;
}

void drawFrame_juggle() {
	//fadeToBlackBy(&Background_Array[0][0], 384, 20);
	DimAll(240);
	byte dothue = 0;
	for (uint8_t i = 0; i < 8; i++) {
		uint16_t temp_loc = beatsin16(i + 7, 0, 384 - 1);
		Background_Array[x(temp_loc)][y(temp_loc)] |= CHSV(dothue, 200, 255);
		dothue += 32;
	}
}



