
#include "oled.h"
#include "el.h"
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include "FastLED.h"
#include "zx.h"
#include "coprocessors.h"
#include "popstar.h"
#include "states.h"

#define GESTURE_DISPLAY_TIME 400

Adafruit_SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);

void print_background(void);
void print_menu(void);

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
		if (bitRead(EL_data, i)) {
			oled.drawLine(EL_OFFSET_X + 1, 2 + EL_OFFSET_Y + (i << 1), EL_OFFSET_X + 12, 2 + EL_OFFSET_Y + (i << 1), WHITE);
			//oled.drawLine(EL_OFFSET_X + 1, 18+2 + EL_OFFSET_Y + (i << 1), EL_OFFSET_X + 12, 18 + 2 + EL_OFFSET_Y + (i << 1), WHITE);
		}
	}
	if (bitRead(EL_data,6)) oled.drawRect(EL_OFFSET_X, EL_OFFSET_Y, 14, 19, WHITE);
	if (bitRead(EL_data,7)) oled.drawRect(EL_OFFSET_X+2, EL_OFFSET_Y+14, 10, 3, WHITE);


	//Draw hand indicators

	oled.setCursor(16, 40);
	
	if (menu_state == MENU_OFF) {
		oled.print("OFF");
	}
	else if (menu_state == MENU_ON) {
		print_background();
	}
	else if (menu_state > MENU_LIST_FIRST) {
		print_menu();
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

	//if (sensor->order == 0)  oled.print(" ");
	//else                    oled.print(sensor->order);

	if (millis() - sensor->gesture_time < GESTURE_DISPLAY_TIME) {
		display_gesture_text(sensor->gesture);
	}
	
}

void display_gesture_text(uint8_t gesture) {
	if (gesture == GESTURE_NONE)				oled.print(" X ");
	else if (gesture == GESTURE_RIGHT)			oled.print(" > ");
	else if (gesture == GESTURE_LEFT)			oled.print(" < ");
	else if (gesture == GESTURE_UP)				oled.print(" ^ ");
	else if (gesture == GESTURE_HOVER)			oled.print(" _ ");
	else if (gesture == GESTURE_HOVER_LEFT)		oled.print("_<_");
	else if (gesture == GESTURE_HOVER_RIGHT)	oled.print("_>_");
	else if (gesture == GESTURE_HOVER_UP)		oled.print("_^_");
	else										oled.print(" ?");
}

void print_background(void) {
	switch (background_mode) {
	case BACKGROUND_FFT_HORZ_BARS_LEFT:
		oled.print("FFT L");
		break;
	case  BACKGROUND_FFT_HORZ_BARS_RIGHT:
		oled.print("FFT L");
		break;
	case  BACKGROUND_FFT_HORZ_BARS_STATIC:
		oled.print("FFT HS");
		break;
	case  BACKGROUND_FFT_VERT_BARS_UP:
		oled.print("FFT U");
		break;
	case  BACKGROUND_FFT_VERT_BARS_DOWN:
		oled.print("FFT D");
		break;
	case  BACKGROUND_FFT_VERT_BARS_STATIC:
		oled.print("FFT VS");
		break;
	case  BACKGROUND_NOISE_1:
		oled.print("NOISE 1");
		break;
	case  BACKGROUND_NOISE_2:
		oled.print("NOISE 2");
		break;
	case  BACKGROUND_NOISE_3:
		oled.print("NOISE 3");
		break;
	case  BACKGROUND_NOISE_4:
		oled.print("NOISE 4");
		break;
	case  BACKGROUND_NOISE_5:
		oled.print("NOISE 5");
		break;
	case  BACKGROUND_NOISE_6:
		oled.print("NOISE 6");
		break;
	case  BACKGROUND_NOISE_7:
		oled.print("NOISE 7");
		break;
	case  BACKGROUND_NOISE_8:
		oled.print("NOISE 8");
		break;
	case  BACKGROUND_NOISE_9:
		oled.print("NOISE 9");
		break;
	case  BACKGROUND_NOISE_10:
		oled.print("NOISE 10");
		break;
	case  BACKGROUND_NOISE_11:
		oled.print("NOISE 11");
		break;
	case  BACKGROUND_ANI_GLITTER:
		oled.print("GLITTER");
		break;
	case  BACKGROUND_ANI_JUGGLE:
		oled.print("JUGGLE");
		break;
	case  BACKGROUND_ANI_DRIFT:
		oled.print("DRIFT 1");
		break;
	case  BACKGROUND_ANI_DRIFT2:
		oled.print("DRIFT 2");
		break;
	case  BACKGROUND_ANI_MUNCH:
		oled.print("MUNCH");
		break;
	case  BACKGROUND_ANI_SNAKE:
		oled.print("SNAKE");
		break;
	case  BACKGROUND_ANI_WAVE:
		oled.print("WAVE");
		break;
	case  BACKGROUND_ANI_LIFE:
		oled.print("LIFE");
		break;
	default:
		oled.print("?");
		break;
	}
}

void print_menu(void) {
	switch (menu_state) {
	case MENU_TOGGLE_AUTO_BACKGROUND:
		if(background_auto) oled.print("AUTO FX");
		else				oled.print("MAN FX");
		break;
	case  MENU_TOGGLE_AUTO_COLOR:
		if (palette_auto) oled.print("AUTO COLOR");
		else				 oled.print("MAN COLOR");
		break;
	case  MENU_ENTER_ANI_MODE:
		oled.print("ENTER ANI?");
		break;
	case  MENU_ENTER_NOISE_MODE:
		oled.print("ENTER SND?");
		break;
	case  MENU_TURN_OFF:
		oled.print("SHUTDOWN?");
		break;
	case  MENU_TOGGLE_SPOTLIGHT:
		if (spotlight_on)	oled.print("SPOT ON");
		else				oled.print("SPOT OFF");
		break;
	case  MENU_TOGGLE_IR_BONUS:
		if (ir_on)			oled.print("IR ON");
		else				oled.print("IR OFF");
		break;
	default:
		oled.print("?");
		break;
	}
}

