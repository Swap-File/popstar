

CRGB ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255) {
  return ColorFromPalette(PaletteAniCurrent, index, brightness);
}

uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatcos = cos8(beat + phase_offset);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

void Pixel(uint8_t x, uint8_t y, uint8_t colorIndex) {
 Background_Array[x][y] = ColorFromCurrentPalette(colorIndex);
}

void DimAll(byte value)
{
  for (uint8_t y = 0; y < 16; y++) {
    for (uint8_t x = 0; x < 24; x++) {
      Background_Array[x][y].nscale8(value);
    }
  }
}




