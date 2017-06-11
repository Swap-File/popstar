
#include "oled.h"
#include "el.h"
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include "FastLED.h"
#include "coprocessors.h"
#include "popstar.h"
#include "states.h"
#include "nunchuk.h"

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

	if (nunchuk_button == BUTTON_NONE) {
		switch (nunchuk_dpad) {
		case DPAD_NONE:
			print_background();
			break;
		case DPAD_UP_RIGHT:
			if (background_auto) oled.print("AUTO FX");
			else				  oled.print("MAN FX");
			break;
		case  DPAD_UP_LEFT:
			if (palette_auto) oled.print("AUTO COLOR");
			else			 oled.print("MAN COLOR");
			break;
		case DPAD_RIGHT:
			if (ir_spot_on) oled.print("SPOT ON");
			else		 oled.print("SPOT OFF");
			break;
		case  DPAD_LEFT:
			if (ir_cvg_on) oled.print("CVG ON");
			else		    oled.print("CVG OFF");
			break;
		case DPAD_UP:
			if (on_sound_mode) oled.print("SND START");
			else		       oled.print("ANI START");
			break;
		default:
			oled.print("=?=");
		}
	}
	else if (nunchuk_button == BUTTON_CZ) {
		switch (nunchuk_dpad) {
		case DPAD_NONE:
			if (menu_state == MENU_OFF) {
				if (on_sound_mode) oled.print("SND START?");
				else		       oled.print("ANI START?");
			}
			else						oled.print("OFF?");
			break;
		case DPAD_UP_RIGHT:
			if (background_auto)  oled.print("MAN FX?");
			else				  oled.print("AUTO FX?");
			break;
		case  DPAD_UP_LEFT:
			if (palette_auto) oled.print("MAN COLOR?");
			else			  oled.print("AUTO COLOR?");
			break;
		case DPAD_RIGHT:
			if (ir_spot_on)  oled.print("SPOT OFF?");
			else			 oled.print("SPOT ON?");
			break;
		case  DPAD_LEFT:
			if (ir_cvg_on) oled.print("CVG OFF?");
			else		   oled.print("CVG ON?");
			break;
		case DPAD_UP:
			if (on_sound_mode) oled.print("ON? ANI");
			else		       oled.print("ON? SND");
			break;
		case DPAD_DOWN:
			oled.print("OFF?");
			break;
		default:
			oled.print("=?=");
		}
	}
	else if (nunchuk_button == BUTTON_Z) {
		switch (nunchuk_dpad) {
		case DPAD_NONE:
			oled.print("EFFECT+?");
			break;
		case DPAD_LEFT:
			oled.print("EFFECT-?");
			break;
		case DPAD_RIGHT:
			oled.print("EFFECT+?");
			break;
		case DPAD_UP:
			if (on_sound_mode) oled.print("ANI?");
			else		       oled.print("SND?");
			break;
		default:
			oled.print("=?=");
		}

	}
	else if (nunchuk_button == BUTTON_C) {
		if (nunchuk_dpad == DPAD_NONE) oled.print("COLOR+?");
		else if (nunchuk_dpad == DPAD_LEFT) oled.print("COLOR-?");
		else if (nunchuk_dpad == DPAD_RIGHT) oled.print("COLOR+?");
	}




	//	Serial.println(nunchuk_accelZ);

	int pitch = map(pitch_compensated, 13500, 22500, 0, 8);


#define offset_vrtf 2
#define offset_vrt 25
#define offset_horz 50

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

void print_background(void) {
	if (menu_state == MENU_OFF) {
		oled.print("  OFF");
	}
	else {
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
			oled.print("B?");
			break;
		}
	}
}