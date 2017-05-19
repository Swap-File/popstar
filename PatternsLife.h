
class Cell {
public:
	byte alive : 1;
	byte prev : 1;
	byte prev2 : 1;
	byte hue : 6;
	byte brightness;
};

Cell world[MATRIX_WIDTH][MATRIX_HEIGHT];
unsigned int density = 20;

void randomFillWorld() {
	for (int i = 0; i < MATRIX_WIDTH; i++) {
		for (int j = 0; j < MATRIX_HEIGHT; j++) {
			if (j == 3) j = 21; //skip middle
			if (random(100) < density) {
				world[i][j].alive = 1;
				world[i][j].brightness = 255;
				world[i][j].prev = 1;
				world[i][j].prev2 = 1;
				world[i][j].hue = 0;
			}
		}
	}
}

int neighbours(int x, int y) {
	return (world[(x + 1) % MATRIX_WIDTH][y].prev) +
		(world[x][(y + 1) % MATRIX_HEIGHT].prev) +
		(world[(x + MATRIX_WIDTH - 1) % MATRIX_WIDTH][y].prev) +
		(world[x][(y + MATRIX_HEIGHT - 1) % MATRIX_HEIGHT].prev) +
		(world[(x + 1) % MATRIX_WIDTH][(y + 1) % MATRIX_HEIGHT].prev) +
		(world[(x + MATRIX_WIDTH - 1) % MATRIX_WIDTH][(y + 1) % MATRIX_HEIGHT].prev) +
		(world[(x + MATRIX_WIDTH - 1) % MATRIX_WIDTH][(y + MATRIX_HEIGHT - 1) % MATRIX_HEIGHT].prev) +
		(world[(x + 1) % MATRIX_WIDTH][(y + MATRIX_HEIGHT - 1) % MATRIX_HEIGHT].prev);
}

void drawFrame_Life_Setup() {
	for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
		for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
			world[x][y].alive = 0;
			world[x][y].prev = 0;
			world[x][y].prev2 = 0;
			world[x][y].brightness = 0;
		}
	}

	randomFillWorld();
}

uint8_t generations = 0;
void drawFrame_Life() {
	DimAll(250);
	// Display current generation
	for (int i = 0; i < MATRIX_WIDTH; i++) {
		for (int j = 0; j < MATRIX_HEIGHT; j++) {
			Background_Array[i][j] = ColorFromCurrentPalette(world[i][j].hue * 4, world[i][j].brightness);
		}
	}

	// Birth and death cycle
	for (int x = 0; x < MATRIX_WIDTH; x++) {
		for (int y = 0; y < MATRIX_HEIGHT; y++) {
			// Default is for cell to stay the same
			if (world[x][y].brightness > 0 && world[x][y].prev == 0)
				world[x][y].brightness *= 0.9;
			int count = neighbours(x, y);
			if (count == 3 && world[x][y].prev == 0) {
				// A new cell is born
				world[x][y].alive = 1;
				world[x][y].hue += 2;
				world[x][y].brightness = 255;
			}
			else if ((count < 2 || count > 3) && world[x][y].prev == 1) {
				// Cell dies
				world[x][y].alive = 0;
			}
		}
	}

	uint16_t changes = 0;
	// Copy next generation into place
	for (int x = 0; x < MATRIX_WIDTH; x++) {
		for (int y = 0; y < MATRIX_HEIGHT; y++) {
			if ((world[x][y].prev != world[x][y].alive) && (world[x][y].prev2 != world[x][y].alive))    changes++;
			world[x][y].prev2 = world[x][y].prev;
			world[x][y].prev = world[x][y].alive;

		}
	}

	//auto-spawn more
	if (++generations == 240 || changes < 1) {
		generations = 0;
		randomFillWorld();
	}


	return;
}
