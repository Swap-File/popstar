
extern CRGB Background_Array[24][16];

byte count = 0;
byte dir = 1;
byte flip = 0;
byte generation = 0;

void drawFrame_Munch() {
  for (byte x = 0; x < MATRIX_WIDTH; x++) {
    for (byte y = 0; y < MATRIX_HEIGHT; y++) {
      Background_Array[x][y] = (x ^ y ^ flip) < count ? ColorFromCurrentPalette(((x ^ y) << 3) + generation) : CRGB::Black;
    }
  }

  count += dir;

  if (count <= 0 || count >= MATRIX_WIDTH) {
    dir = -dir;
  }

  if (count <= 0) {
    if (flip == 0)
      flip = 31;
    else
      flip = 0;
  }

  generation++;


}


