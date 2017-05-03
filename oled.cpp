
#include "oled.h"
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include "FastLED.h"
#include "zx.h"
#include "coprocessors.h"
#include "popstar.h"
#include "states.h"

#define GESTURE_DISPLAY_TIME 400

Adafruit_SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);


void oled_init(void) {
	oled.begin(SSD1306_SWITCHCAPVCC);
	oled.setRotation(2);
	oled.display();
	oled.setTextSize(1);
	oled.setTextColor(WHITE);
	oled.setTextWrap(false);

}

void oled_reint(void) {
	oled.reinit();
}

 void graph_gesture(zx_sensor *sensor, uint8_t x, uint8_t y);
 void display_gesture_text(uint8_t gesture);
#define BACKGROUND_OFFSET_X 19
#define BACKGROUND_OFFSET_Y 0
#define EL_OFFSET_X 24
#define EL_OFFSET_Y 18
#define SENSOR1_X 0
#define SENSOR1_Y 0
#define SENSOR2_X 48
#define SENSOR2_Y 0
void oled_update(void) {

	oled.clearDisplay();
	
		//draw LED indicators
	for (uint8_t x = 0; x < 24; x++) {
		for (uint8_t y = 0; y < 16; y++) {
			if (Background_Array[x][y].getLuma() >= 30) {  //This sets where decayed pixels disappear from HUD
				oled.drawPixel(BACKGROUND_OFFSET_X+x+1,BACKGROUND_OFFSET_Y+y+1, WHITE);
			}
		}
	}


	//draw EL wire indicators
	for (uint8_t i = 0; i < 6; i++) {
		if (EL_Strips[i]) {
			oled.drawLine(EL_OFFSET_X + 1, 2 + EL_OFFSET_Y + (i << 1), EL_OFFSET_X + 12, 2 + EL_OFFSET_Y + (i << 1), WHITE);
			//oled.drawLine(EL_OFFSET_X + 1, 18+2 + EL_OFFSET_Y + (i << 1), EL_OFFSET_X + 12, 18 + 2 + EL_OFFSET_Y + (i << 1), WHITE);
		}
	}
	if (EL_Strips[6]) oled.drawRect(EL_OFFSET_X, EL_OFFSET_Y, 14, 19, WHITE);
	if (EL_Strips[7]) oled.drawRect(EL_OFFSET_X+2, EL_OFFSET_Y+14, 10, 3, WHITE);


	//Draw hand indicators

	oled.setCursor(16, 40);
	
	switch (menu_location) {
	case MENU_LOCKED:
		oled.print(MENU_LOCKED_TXT);
		break;
	case MENU_ROOT_EFFECTS:
		oled.print(MENU_ROOT_EFFECTS_TXT);
		break;
	case MENU_ROOT_COLORS:
		oled.print(MENU_ROOT_COLORS_TXT);
		break;	
	}

	int pitch = map(pitch_compensated, 13500, 22500, 0, 8);


#define offset_vrtf 2
#define offset_vrt 25
#define offset_horz 50

pitch = 7-constrain(pitch,0,7);
	oled.drawLine(offset_vrtf+0, offset_vrt +pitch, offset_vrtf+8, offset_vrt + pitch,WHITE);

	int roll = map(roll_compensated, 13500, 22500, 0, 8);
	roll = constrain(roll, 0, 7);
	oled.drawLine(offset_vrtf+0, offset_vrt + 7 - roll, offset_vrtf+8, offset_vrt +roll, WHITE);

	
	int yaw = map(yaw_compensated, 0, 36000, 0, 255);

	
	int x = int(sin8(yaw));
	int y = int(cos8(yaw));
	x = map(x, 0, 255, 0, 9);
	y = map(y, 0, 255, 0, 9);

	x = constrain(x, 0, 8);
	y = constrain(y, 0, 8);
	oled.drawLine(offset_horz+x, offset_vrt+ y, offset_horz+ 8-x, offset_vrt+ 8-y, WHITE);


	graph_gesture(&sensor1, SENSOR1_X, SENSOR1_Y);
	graph_gesture(&sensor2, SENSOR2_X, SENSOR2_Y);

	oled.display();
}

void graph_gesture(zx_sensor *sensor,uint8_t x, uint8_t y) {
	int tempx = map(sensor->x, 0, 240, 0, 16);
	int tempy = map(sensor->z, 0, 240, 0, 16);
	tempx = constrain(tempx, 0, 15);
	tempy = constrain(tempy, 0, 15);
	//oled.drawPixel(x + 1 + constrain(tempx, 0, 15), y + 1 + constrain(tempy, 0, 15), WHITE);
	//oled.drawRect(x, y, 17, 17, WHITE);
	oled.drawLine(x + tempx, y + 0, x + tempx, y + 15, WHITE);
	oled.drawLine(x+0, y + tempy, x + 15, y + tempy, WHITE);

	oled.setCursor(x, y + 17);

	if (sensor->order == 0)  oled.print(" ");
	else                    oled.print(sensor->order);

	if (millis() - sensor->gesture_time < GESTURE_DISPLAY_TIME) {
		display_gesture_text(sensor->gesture);
	}
	
}

void display_gesture_text(uint8_t gesture) {
	if (gesture == GESTURE_NONE)				oled.print(" X");
	else if (gesture == GESTURE_RIGHT)			oled.print(" >");
	else if (gesture == GESTURE_LEFT)			oled.print(" <");
	else if (gesture == GESTURE_UP)				oled.print(" ^");
	else if (gesture == GESTURE_HOVER)			oled.print(" _");
	else if (gesture == GESTURE_HOVER_LEFT)		oled.print("_<");
	else if (gesture == GESTURE_HOVER_RIGHT)	oled.print("_>");
	else if (gesture == GESTURE_HOVER_UP)		oled.print("_^");
	else										oled.print(" ?");
}

