
#include "oled.h"
#include "el.h"
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include "FastLED.h"
#include "coprocessors.h"
#include "popstar.h"
#include "states.h"
#include "nunchuk.h"

char oled_text[10];
uint32_t oled_action_time = 0;

Adafruit_SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);

boolean invert_text = false;

void oled_init(void) {
	oled.begin(SSD1306_SWITCHCAPVCC);
	//oled.setRotation(2);
	oled.display();
	oled.setTextSize(1);
	oled.setTextColor(WHITE);
	oled.setTextWrap(false);
}

void oled_reint(void) {
	oled.reinit();
}

#define BACKGROUND_OFFSET_X 19
#define BACKGROUND_OFFSET_Y 0
#define EL_OFFSET_X 24
#define EL_OFFSET_Y 18
#define SENSOR1_X 0
#define SENSOR1_Y 0
#define SENSOR2_X 48
#define SENSOR2_Y 0
#define offset_vrtf 2
#define offset_vrt 25
#define offset_horz 50

void oled_update(void) {

	oled.clearDisplay();

	//draw LED indicators
	for (uint8_t x = 0; x < 24; x++) {
		for (uint8_t y = 0; y < 16; y++) {
			if (Background_Array[x][y].getLuma() >= 30) {  //This sets where decayed pixels disappear from HUD
				oled.drawPixel(BACKGROUND_OFFSET_X + x + 1, BACKGROUND_OFFSET_Y + y + 1, WHITE);
			}
		}
	}
	
	//draw EL wire indicators
	for (uint8_t i = 0; i < 6; i++) {
		if (bitRead(EL_data, i)) {
			oled.drawLine(EL_OFFSET_X + 1, 2 + EL_OFFSET_Y + (i << 1), EL_OFFSET_X + 12, 2 + EL_OFFSET_Y + (i << 1), WHITE);
			//oled.drawLine(EL_OFFSET_X + 1, 18+2 + EL_OFFSET_Y + (i << 1), EL_OFFSET_X + 12, 18 + 2 + EL_OFFSET_Y + (i << 1), WHITE);
		}
	}
	if (bitRead(EL_data, 6)) oled.drawRect(EL_OFFSET_X, EL_OFFSET_Y, 14, 19, WHITE);
	if (bitRead(EL_data, 7)) oled.drawRect(EL_OFFSET_X + 2, EL_OFFSET_Y + 14, 10, 3, WHITE);
	
	//Draw hand indicators
	oled.setCursor(0, 40);

	if (millis() - oled_action_time < 500) invert_text = true;
	else invert_text = false;

	//blackout the background

	if (invert_text) oled.setTextColor(BLACK, WHITE);
	else		    oled.setTextColor(WHITE,BLACK);
	oled.print(oled_text);
	oled.setTextColor(WHITE, BLACK);
	
	int pitch = map(pitch_compensated, 13500, 22500, 0, 8);

	pitch = 7 - constrain(pitch, 0, 7);
	oled.drawLine(offset_vrtf + 0, offset_vrt + pitch, offset_vrtf + 8, offset_vrt + pitch, WHITE);

	int roll = map(roll_compensated, 13500, 22500, 0, 8);
	roll = constrain(roll, 0, 7);
	oled.drawLine(offset_vrtf + 0, offset_vrt + 7 - roll, offset_vrtf + 8, offset_vrt + roll, WHITE);

	int yaw = map(yaw_compensated, 0, 36000, 0, 255);

	int x = int(sin8(yaw));
	int y = int(cos8(yaw));
	x = map(x, 0, 255, 0, 9);
	y = map(y, 0, 255, 0, 9);
	x = constrain(x, 0, 8);
	y = constrain(y, 0, 8);
	oled.drawLine(offset_horz + x, offset_vrt + y, offset_horz + 8 - x, offset_vrt + 8 - y, WHITE);

	oled.display();
}

void oled_load(const char * input) {

	//add code to center text
	//dont update if inverted
	if (!invert_text)	strcpy(oled_text, input);
}