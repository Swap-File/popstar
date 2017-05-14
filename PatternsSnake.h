extern CRGB Background_Array[24][16];
extern CRGBPalette16 currentPalette;

static const int snakeCount = 6;
static const byte SNAKE_LENGTH = 16;
enum Direction {
  UP, DOWN, LEFT, RIGHT
};


struct Pixel_s {
  uint8_t x;
  uint8_t y;
};


struct Snake {
  Pixel_s pixels[SNAKE_LENGTH];

  Direction direction;

  void newDirection() {
    switch (direction) {
      case UP:
      case DOWN:
        direction = random(0, 2) == 1 ? RIGHT : LEFT;
        break;

      case LEFT:
      case RIGHT:
        direction = random(0, 2) == 1 ? DOWN : UP;

      default:
        break;
    }
  }

  void shuffleDown() {
    for (byte i = SNAKE_LENGTH - 1; i > 0; i--) {
      pixels[i] = pixels[i - 1];
    }
  }

  void reset() {
    direction = UP;
    for (int i = 0; i < SNAKE_LENGTH; i++) {
      pixels[i].x = 0;
      pixels[i].y = 0;
    }
  }

  void move() {
    switch (direction) {
      case UP:
        pixels[0].y = (pixels[0].y + 1) % MATRIX_HEIGHT;
        break;
      case LEFT:
        pixels[0].x = (pixels[0].x + 1) % MATRIX_WIDTH;
        break;
      case DOWN:
        pixels[0].y = pixels[0].y == 0 ? MATRIX_HEIGHT - 1 : pixels[0].y - 1;
        break;
      case RIGHT:
        pixels[0].x = pixels[0].x == 0 ? MATRIX_WIDTH - 1 : pixels[0].x - 1;
        break;
    }
  }

  void draw(CRGB colors[SNAKE_LENGTH]) {
    for (byte i = 0; i < SNAKE_LENGTH; i++) {
      Background_Array[pixels[i].x][pixels[i].y] = colors[i] %= (255 - i * (255 / SNAKE_LENGTH));
    }
  }
};

Snake snakes[snakeCount];
CRGB colors[SNAKE_LENGTH];
uint8_t initialHue;

void drawFrame_Snake_Setup() {
  for (int i = 0; i < snakeCount; i++) {
    Snake* snake = &snakes[i];
    snake->reset();
  }
}

void drawFrame_Snake() {
  DimAll(255);

  fill_palette(colors, SNAKE_LENGTH, initialHue++, 5, currentPalette, 255, LINEARBLEND);

  for (int i = 0; i < snakeCount; i++) {
    Snake* snake = &snakes[i];

    snake->shuffleDown();

    if (random(10) > 7) {
      snake->newDirection();
    }

    snake->move();
    snake->draw(colors);
  }

  return;
}


