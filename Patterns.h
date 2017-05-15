
void drawFrame_IncrementalDrift() {
  uint8_t dim = beatsin8(2, 230, 250);
  DimAll(dim);

  for (int i = 2; i <=MATRIX_WIDTH / 2; i++)  {
    
    CRGB color = ColorFromCurrentPalette((i - 2) * (240 / (MATRIX_WIDTH / 2)));

    uint8_t x = beatcos8((17 - i) * 2, MATRIX_CENTER_X - i, MATRIX_CENTER_X + i);
    uint8_t y = beatsin8((17 - i) * 2, MATRIX_CENTER_Y - i, MATRIX_CENTER_Y + i);

    if (x < MATRIX_WIDTH && y < MATRIX_HEIGHT)    Pixel(x, y, color);
    
  }
}

void drawFrame_IncrementalDrift2() {
  uint8_t dim = beatsin8(2, 170, 250);
  DimAll(dim);

  for (uint8_t i = 0; i < 16; i++)
  {
    CRGB color;

    uint8_t x = 0;
    uint8_t y = 0;

    if (i < 16) {
      x = beatcos8((i + 1) * 2, i, MATRIX_WIDTH - i);
      y = beatsin8((i + 1) * 2, i, MATRIX_HEIGHT - i);
      color = ColorFromCurrentPalette(i * 14);
    }
    else
    {
      x = beatsin8((32 - i) * 2, MATRIX_WIDTH - i, i + 1);
      y = beatcos8((32 - i) * 2, MATRIX_HEIGHT - i, i + 1);
      color = ColorFromCurrentPalette((31 - i) * 14);
    }

        if (x < MATRIX_WIDTH && y < MATRIX_HEIGHT)    Pixel(x, y, color);
    
  }

}


void drawFrame_glitter
(uint8_t points) {
  for (uint8_t i = 0; i < points; i++) {
    // fadeToBlackBy(&Background_Array[0][0], 384, 10);
    DimAll(240);
    uint8_t x = random8(24);
    uint8_t y = random8(16);
   Background_Array[x][y] += CHSV(random8(255), 200, 255);
//	Pixel(x, y, random8(255));
  }
}

uint8_t x(uint16_t i)  {
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



