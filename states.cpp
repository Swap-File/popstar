#include "states.h"
#include "oled.h"
#include "coprocessors.h"
#include "el.h"
#include "nunchuk.h"
#include "popstar.h"

uint8_t menu_state_last = MENU_OFF;
uint8_t menu_state = MENU_OFF;

void auto_head_tilt() {
	if (roll_compensated < 14500)	 background_mode = BACKGROUND_FFT_HORZ_BARS_LEFT;
	else if (roll_compensated > 21500) background_mode = BACKGROUND_FFT_HORZ_BARS_RIGHT;
	else if (pitch_compensated < 14500) background_mode = BACKGROUND_FFT_VERT_BARS_UP;
	else if (pitch_compensated > 21500)  background_mode = BACKGROUND_FFT_VERT_BARS_DOWN;
}

void increment_background(int8_t number) {

	if (background_mode >= BACKGROUND_NOISE_FIRST && background_mode <= BACKGROUND_NOISE_LAST) {
		background_mode += number;
		if (background_mode < BACKGROUND_NOISE_FIRST) background_mode = BACKGROUND_NOISE_LAST;
		if (background_mode > BACKGROUND_NOISE_LAST) background_mode = BACKGROUND_NOISE_FIRST;
	}

	if (background_mode >= BACKGROUND_ANI_FIRST && background_mode <= BACKGROUND_ANI_LAST) {
		background_mode += number;
		if (background_mode < BACKGROUND_ANI_FIRST) background_mode = BACKGROUND_ANI_LAST;
		if (background_mode > BACKGROUND_ANI_LAST) background_mode = BACKGROUND_ANI_FIRST;
	}

	if (background_mode >= BACKGROUND_FFT_FIRST && background_mode <= BACKGROUND_FFT_LAST) {
		background_mode = random8(BACKGROUND_NOISE_FIRST, BACKGROUND_NOISE_LAST + 1);  //exit to random sound mode
	}

	background_change_time = millis();
}


uint32_t accel_cooldown = 0;
boolean inhibit_next_release = false;

void state_update() {

	if (EL_animation == EL_ANI_STOP && menu_state != MENU_OFF) 	supress_leds = false;

	//automatically change head tilt modes
	if (background_mode >= BACKGROUND_FFT_FIRST && background_mode <= BACKGROUND_FFT_LAST) {
		auto_head_tilt();
		on_sound_mode = false;// hold Z up to be ani mode, next/prev will be snd mode to exit fft
	}

	if (nunchuk_dpad == DPAD_NONE || nunchuk_dpad ==  DPAD_DEADZONE) inhibit_next_release = false;

	if (nunchuk_fresh) {

		uint8_t button_to_handle = nunchuk_button;
		uint8_t activity = 0;

		if ((nunchuk_button != nunchuk_button_last) && (nunchuk_button == BUTTON_NONE) && (!inhibit_next_release)) {  //handle releasing buttons
			button_to_handle = nunchuk_button_last;
			activity = ACTIVITIY_BUTTON;
		}
		else if (millis() - accel_cooldown > 200 && nunchuk_total_acceleration > 200000) { //handle a swipe while holding
			activity = ACTIVITIY_MOTION;
			inhibit_next_release = true;  //block out releasing the button unless it returns to dpad center and back
			accel_cooldown = millis();
		}

		gui(button_to_handle, activity);

		nunchuk_fresh = false;
	}
}

void print_background(void) {
	if (menu_state == MENU_OFF) {
		oled_load("OFF");
	}
	else {
		switch (background_mode) {
		case BACKGROUND_FFT_HORZ_BARS_LEFT:
			oled_load("FFT L");
			break;
		case  BACKGROUND_FFT_HORZ_BARS_RIGHT:
			oled_load("FFT L");
			break;
		case  BACKGROUND_FFT_HORZ_BARS_STATIC:
			oled_load("FFT HS");
			break;
		case  BACKGROUND_FFT_VERT_BARS_UP:
			oled_load("FFT U");
			break;
		case  BACKGROUND_FFT_VERT_BARS_DOWN:
			oled_load("FFT D");
			break;
		case  BACKGROUND_FFT_VERT_BARS_STATIC:
			oled_load("FFT VS");
			break;
		case  BACKGROUND_NOISE_1:
			oled_load("NOISE 1");
			break;
		case  BACKGROUND_NOISE_2:
			oled_load("NOISE 2");
			break;
		case  BACKGROUND_NOISE_3:
			oled_load("NOISE 3");
			break;
		case  BACKGROUND_NOISE_4:
			oled_load("NOISE 4");
			break;
		case  BACKGROUND_NOISE_5:
			oled_load("NOISE 5");
			break;
		case  BACKGROUND_NOISE_6:
			oled_load("NOISE 6");
			break;
		case  BACKGROUND_NOISE_7:
			oled_load("NOISE 7");
			break;
		case  BACKGROUND_NOISE_8:
			oled_load("NOISE 8");
			break;
		case  BACKGROUND_NOISE_9:
			oled_load("NOISE 9");
			break;
		case  BACKGROUND_NOISE_10:
			oled_load("NOISE 10");
			break;
		case  BACKGROUND_NOISE_11:
			oled_load("NOISE 11");
			break;
		case  BACKGROUND_ANI_GLITTER:
			oled_load("GLITTER");
			break;
		case  BACKGROUND_ANI_JUGGLE:
			oled_load("JUGGLE");
			break;
		case  BACKGROUND_ANI_DRIFT:
			oled_load("DRIFT 1");
			break;
		case  BACKGROUND_ANI_DRIFT2:
			oled_load("DRIFT 2");
			break;
		case  BACKGROUND_ANI_MUNCH:
			oled_load("MUNCH");
			break;
		case  BACKGROUND_ANI_SNAKE:
			oled_load("SNAKE");
			break;
		case  BACKGROUND_ANI_WAVE:
			oled_load("WAVE");
			break;
		case  BACKGROUND_ANI_LIFE:
			oled_load("LIFE");
			break;
		default:
			oled_load("UNKNOWN");
			break;
		}
	}
}

void gui(uint8_t button_to_handle, uint8_t activitiy) {

	boolean tilt_ready = false;
	if( (roll_compensated < 14500) || (roll_compensated > 21500) || (pitch_compensated < 14500) || (pitch_compensated > 21500)) {
		tilt_ready = true;
	}
	if (activitiy > ACTIVITIY_NONE) oled_action_time = millis();

	if (button_to_handle == BUTTON_NONE) { //no actions, display only
		switch (nunchuk_dpad) {
		case DPAD_NONE:
			print_background();
			break;
		case DPAD_UP_RIGHT:
			if (background_auto) oled_load("AUTO FX");
			else				  oled_load("MAN FX");
			break;
		case  DPAD_UP_LEFT:
			if (palette_auto) oled_load("AUTO COLOR");
			else			 oled_load("MAN COLOR");
			break;
		case DPAD_RIGHT:
			if (ir_spot_on) oled_load("SPOT ON");
			else		 oled_load("SPOT OFF");
			break;
		case  DPAD_LEFT:
			if (ir_cvg_on) oled_load("CVG ON");
			else		    oled_load("CVG OFF");
			break;
		case DPAD_UP:
			if (on_sound_mode) oled_load("SND START");
			else		       oled_load("ANI START");
			break;
		case DPAD_DOWN_RIGHT:
			if (internal_preview) oled_load("LEDS ON");
			else		       oled_load("LEDS OFF");
			break;
		case DPAD_DOWN:
		{
			char temp[10];
			dtostrf(voltage, 5, 2, temp);
			temp[5] = 'v';
			temp[6] = '\0';
			oled_load(temp);
		}
		break;
		default:
			oled_load("UNUSED");
		}
	}
	else if (button_to_handle == BUTTON_CZ) {
		switch (nunchuk_dpad) {
		case DPAD_NONE:
			if (activitiy == ACTIVITIY_MOTION) {//action
				if (menu_state == MENU_OFF) menu_state = MENU_ON;
				else {
					menu_state = MENU_OFF;
					supress_leds = true;
				}
			}
			else {//display
				if (menu_state == MENU_OFF) {
					if (on_sound_mode) oled_load("SND START?");
					else		       oled_load("ANI START?");
				}
				else                   oled_load("OFF?");
			}
			break;
		case DPAD_UP_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_auto = !background_auto;
			}
			else {//display
				if (background_auto)  oled_load("MAN FX?");
				else				  oled_load("AUTO FX?");
			}
			break;
		case  DPAD_UP_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				palette_auto = !palette_auto;
			}
			else {//display
				if (palette_auto) oled_load("MAN COLOR?");
				else			  oled_load("AUTO COLOR?");
			}
			break;
		case DPAD_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				ir_spot_on = !ir_spot_on;
			}
			else {//display
				if (ir_spot_on)  oled_load("SPOT OFF?");
				else			 oled_load("SPOT ON?");
			}
			break;
		case DPAD_DOWN_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				el_audio = !el_audio;
			}
			else {//display
				if (ir_spot_on)  oled_load("EL SND OFF?");
				else			 oled_load("EL SND ON?");
			}
			break;
		case  DPAD_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				ir_cvg_on = !ir_cvg_on;
			}
			else {//display
				if (ir_cvg_on) oled_load("CVG OFF?");
				else		   oled_load("CVG ON?");
			}
			break;

		case DPAD_UP:
			if (activitiy > ACTIVITIY_NONE) {//action
				if (menu_state == MENU_OFF) {
					menu_state = MENU_ON;
				}
				else {
					increment_background(1);
					EL_animation = EL_ANI_RIGHT;
				}
			}
			else {
				if (menu_state == MENU_OFF) {
					if (on_sound_mode) oled_load("SND ON?");
					else		       oled_load("ANI ON?");
				}
				else {
					oled_load("EFFECT+?");
				}
			}
			break;
		case DPAD_DOWN_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				internal_preview = !internal_preview;
			}
			else {//display
				if (internal_preview)  oled_load("MON OFF?");
				else		           oled_load("MON ON?");
			}
			break;
			case DPAD_DOWN:
			if (activitiy > ACTIVITIY_NONE) {//action
				menu_state = MENU_OFF;
				supress_leds = true;
			}
			else {//display
				oled_load("OFF?");
			}
			break;
		default:
			oled_load("=?=");
		}
	}
	else if (button_to_handle == BUTTON_Z) {
		switch (nunchuk_dpad) {

		case DPAD_UP:
			if (activitiy > ACTIVITIY_NONE) {//action
				on_sound_mode = !on_sound_mode;
				if (on_sound_mode) { //switch mode type and pick random
					background_mode = random8(BACKGROUND_NOISE_FIRST, BACKGROUND_NOISE_LAST + 1);
				}
				else {
					background_mode = random8(BACKGROUND_ANI_FIRST, BACKGROUND_ANI_LAST + 1);
				}
			}
			else {//display
				if (on_sound_mode) oled_load("ANI?");
				else		       oled_load("SND?");
			}
			break;
		case DPAD_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				increment_background(-1);
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("EFFECT-?");
				break;
		case DPAD_NONE:
			if (activitiy == ACTIVITIY_MOTION) {//action
				increment_background(1);
				EL_animation = EL_ANI_RIGHT;
			}
			else if (activitiy == ACTIVITIY_BUTTON) {//action
				oled_action_time = 0; // supress
			}
			
			else {//display
				oled_load("EFFECT+?");
			}
			break;
		case DPAD_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				increment_background(1);
				EL_animation = EL_ANI_RIGHT;
			}
			else {//display
				oled_load("EFFECT+?");
			}
			break;
		case DPAD_DOWN:
			if (activitiy > ACTIVITIY_NONE) {//action
				auto_head_tilt();
			}
			else {//display
				if (tilt_ready)		oled_load("TILT GOOD!");
				else			oled_load("TILT MORE!");
			}
			break;

			//SHORTCUTS!
		case DPAD_DOWN_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_ANI_GLITTER;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("GLITTER?");
			}
			break;
		case DPAD_DOWN_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_ANI_DRIFT;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("DRIFT?");
			}
			break;
		case DPAD_UP_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_ANI_SNAKE;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("SNAKE?");
							}
			break;
		case DPAD_UP_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_ANI_WAVE;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("WAVE?");
			}
			break;
		
		default:
			oled_load("UNUSED");
			}
		}
	}
	else if (button_to_handle == BUTTON_C) {
		switch (nunchuk_dpad) {
		case DPAD_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				requested_palette++;
				ChangeTargetPalette(1);
				EL_animation = EL_ANI_RIGHT;
			}
			else {//display
				oled_load("COLOR+?");
			}
			break;
		case DPAD_NONE:
			if (activitiy == ACTIVITIY_MOTION) {//action
				requested_palette++;
				ChangeTargetPalette(1);
				EL_animation = EL_ANI_RIGHT;
			}
			else if (activitiy == ACTIVITIY_BUTTON) {//action
				oled_action_time = 0; // supress
			}
			else {//display
				oled_load("COLOR+?");
			}
			break;
		case DPAD_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				requested_palette++;
				ChangeTargetPalette(1);
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("COLOR-?");
			}
			break;


			//SHORTCUTS!
		case DPAD_DOWN_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_NOISE_1;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("NOISE <>?");
			}
			break;
		case DPAD_DOWN_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_NOISE_10;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("NOISE ::?");
			}
			break;
		case DPAD_UP_RIGHT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_NOISE_4;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("NOISE ZZ?");
			}
			break;
		case DPAD_UP_LEFT:
			if (activitiy > ACTIVITIY_NONE) {//action
				background_mode = BACKGROUND_NOISE_6;
				EL_animation = EL_ANI_LEFT;
			}
			else {//display
				oled_load("NOISE 88?");
			}
			break;
		default:
			oled_load("UNUSED");
		}
	}
	return;
}