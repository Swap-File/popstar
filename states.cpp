#include "states.h"
#include "oled.h"
#include "coprocessors.h"
#include "el.h"
#include "nunchuk.h"
#include "popstar.h"


uint8_t menu_state_last = MENU_OFF;
uint8_t menu_state = MENU_OFF;


boolean latch_reset = true;
uint8_t debounce = 0;
uint8_t debounce1 = 0;

void auto_head_tilt() {
	if (roll_compensated < 13500)	 background_mode = BACKGROUND_FFT_HORZ_BARS_LEFT;
	else if (roll_compensated > 22500) background_mode = BACKGROUND_FFT_HORZ_BARS_RIGHT;
	else if (pitch_compensated < 13500) background_mode = BACKGROUND_FFT_VERT_BARS_UP;
	else if (pitch_compensated > 22500)  background_mode = BACKGROUND_FFT_VERT_BARS_DOWN;
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
	else {
		auto_head_tilt();//override mode if tilting and not in an animation
	}

	background_change_time = millis();
}


uint32_t accel_cooldown = 0;

void state_update() {

	if (EL_animation == EL_ANI_STOP && menu_state != MENU_OFF) 	supress_leds = false;

	//automatically change head tilt modes
	if (background_mode >= BACKGROUND_FFT_FIRST && background_mode <= BACKGROUND_FFT_LAST) {
		auto_head_tilt();
	}

	if (nunchuk_fresh) {

		uint8_t button_to_handle = BUTTON_NONE;
		boolean motion = false;

		if ((nunchuk_button != nunchuk_button_last) && (nunchuk_button == BUTTON_NONE)) {  //handle releasing buttons
			button_to_handle = nunchuk_button_last;
		}
		else if (millis() - accel_cooldown > 200 && nunchuk_total_acceleration > 200000) { //handle a swipe while holding
			button_to_handle = nunchuk_button;
			motion = true;
			accel_cooldown = millis();
		}

		if (button_to_handle == BUTTON_CZ) {
			switch (nunchuk_dpad) {
			case DPAD_NONE:
				if (motion) {
					if (menu_state == MENU_OFF) menu_state = MENU_ON;
					else {
						menu_state = MENU_OFF;
						supress_leds = true;
					}
				}
				break;
			case DPAD_UP_RIGHT:
				background_auto = !background_auto;
				break;
			case  DPAD_UP_LEFT:
				palette_auto = !palette_auto;
				break;
			case DPAD_RIGHT:
				ir_spot_on = !ir_spot_on;
				break;
			case  DPAD_LEFT:
				ir_cvg_on = !ir_cvg_on;
				break;
			case DPAD_UP:
				if (menu_state == MENU_OFF) menu_state = MENU_ON;
				break;
			case DPAD_DOWN:
				menu_state = MENU_OFF;
				supress_leds = true;
				break;
			}
		}
		else if (button_to_handle == BUTTON_Z) {
			switch (nunchuk_dpad) {
			case DPAD_NONE:
				if (motion) {
					increment_background(1);
					EL_animation = EL_ANI_RIGHT;
				}
				break;
			case DPAD_UP_RIGHT:
				break;
			case DPAD_UP_LEFT:
				increment_background(-1);
				EL_animation = EL_ANI_LEFT;
				break;
			case DPAD_RIGHT:
				increment_background(1);
				EL_animation = EL_ANI_RIGHT;
				break;
			case  DPAD_LEFT:
				
				break;
			case DPAD_UP:
				on_sound_mode = !on_sound_mode;
				if (on_sound_mode)		background_mode = BACKGROUND_ANI_WAVE;
				else background_mode = BACKGROUND_NOISE_6;
				break;
			case DPAD_DOWN:
			
				break;
			}
		}
		else if (button_to_handle == BUTTON_C) {
			switch (nunchuk_dpad) {
			case DPAD_NONE:
				if (motion) {
					requested_palette++;
					EL_animation = EL_ANI_RIGHT;
				}
				break;
			case DPAD_UP_RIGHT:
				break;
			case DPAD_UP_LEFT:
				requested_palette--;
				EL_animation = EL_ANI_LEFT;
				break;
			case DPAD_RIGHT:
				requested_palette++;
				EL_animation = EL_ANI_RIGHT;
				break;
			case  DPAD_LEFT:

				break;
			case DPAD_UP:
		
				break;
			case DPAD_DOWN:

				break;
			}
		}

		nunchuk_fresh = false;
	}
}


/*
if (menu_state == 1) {
	if (sensor1.gesture_fresh) {
		if (sensor1.gesture == GESTURE_LEFT) {
			increment_background(-1);
			EL_animation = EL_ANI_LEFT;
		}
		else if (sensor1.gesture == GESTURE_RIGHT) {
			increment_background(1);
			EL_animation = EL_ANI_RIGHT;
		}
	}

	if (sensor2.gesture_fresh) {
		if (sensor2.gesture == GESTURE_LEFT) {
			requested_palette--;
			EL_animation = EL_ANI_LEFT;
		}
		else if (sensor2.gesture == GESTURE_RIGHT) {
			requested_palette++;
			EL_animation = EL_ANI_RIGHT;
		}
		ChangeTargetPalette(1);

	}
}

//poweron
if (menu_state == MENU_OFF) {
	if (sensor2.gesture == GESTURE_HOVER_UP && sensor1.gesture == GESTURE_HOVER_UP) { //kick from off to on
		if (abs((int32_t)(sensor1.gesture_time - sensor2.gesture_time)) < 300)  menu_state = MENU_ON;
	}
}

if (menu_state == MENU_ON) { //kick from on to menu
	if ((sensor1.z < 10 || sensor2.z < 10) && latch_reset) 	debounce1++;
	else 	debounce1 = 0;

	if (sensor1.z > 10 && sensor2.z > 10) latch_reset = true;

	if (debounce1 == 255) {
		debounce1 = 0;
		latch_reset = false;
		if (background_mode <= BACKGROUND_ANI_LAST && background_mode >= BACKGROUND_ANI_FIRST) menu_state = MENU_ENTER_NOISE_MODE;
		else  menu_state = MENU_ENTER_ANI_MODE;
	}
}

if (menu_state >= MENU_LIST_FIRST) {
	//navigate menu left
	if ((sensor1.gesture_fresh && sensor1.gesture == GESTURE_LEFT) || (sensor2.gesture_fresh && sensor2.gesture == GESTURE_LEFT)) {
		if (menu_state == MENU_TOGGLE_AUTO_BACKGROUND)		menu_state = MENU_TOGGLE_IR_BONUS;
		else if (menu_state == MENU_TOGGLE_AUTO_COLOR)		menu_state = MENU_TOGGLE_AUTO_BACKGROUND;
		else if (menu_state == MENU_ENTER_ANI_MODE)			menu_state = MENU_TOGGLE_AUTO_COLOR;
		else if (menu_state == MENU_ENTER_NOISE_MODE)		menu_state = MENU_ENTER_ANI_MODE;
		else if (menu_state == MENU_TURN_OFF)				menu_state = MENU_ENTER_NOISE_MODE;
		else if (menu_state == MENU_TOGGLE_SPOTLIGHT)		menu_state = MENU_TURN_OFF;
		else if (menu_state == MENU_TOGGLE_IR_BONUS)		menu_state = MENU_TOGGLE_SPOTLIGHT;

	}
	//navigate menu right
	if ((sensor1.gesture_fresh && sensor1.gesture == GESTURE_RIGHT) || (sensor2.gesture_fresh && sensor2.gesture == GESTURE_RIGHT)) {
		if (menu_state == MENU_TOGGLE_AUTO_BACKGROUND)		menu_state = MENU_TOGGLE_AUTO_COLOR;
		else if (menu_state == MENU_TOGGLE_AUTO_COLOR)		menu_state = MENU_ENTER_ANI_MODE;
		else if (menu_state == MENU_ENTER_ANI_MODE)			menu_state = MENU_ENTER_NOISE_MODE;
		else if (menu_state == MENU_ENTER_NOISE_MODE)		menu_state = MENU_TURN_OFF;
		else if (menu_state == MENU_TURN_OFF)				menu_state = MENU_TOGGLE_SPOTLIGHT;
		else if (menu_state == MENU_TOGGLE_SPOTLIGHT)		menu_state = MENU_TOGGLE_IR_BONUS;
		else if (menu_state == MENU_TOGGLE_IR_BONUS)		menu_state = MENU_TOGGLE_AUTO_BACKGROUND;
	}
	//do actions
	if ((sensor1.gesture_fresh && sensor1.gesture == GESTURE_UP) || (sensor2.gesture_fresh && sensor2.gesture == GESTURE_UP) || (sensor1.gesture_fresh && sensor1.gesture == GESTURE_HOVER_UP) || (sensor2.gesture_fresh && sensor2.gesture == GESTURE_HOVER_UP)) {
		switch (menu_state) {
		case MENU_TOGGLE_AUTO_BACKGROUND:
			background_auto = !background_auto;
			menu_state = MENU_ON;
			break;
		case MENU_TOGGLE_AUTO_COLOR:
			palette_auto = !palette_auto;
			menu_state = MENU_ON;
			break;
		case MENU_ENTER_ANI_MODE:
			background_mode = BACKGROUND_ANI_WAVE;
			menu_state = MENU_ON;
			break;
		case MENU_ENTER_NOISE_MODE:
			background_mode = BACKGROUND_NOISE_6;
			menu_state = MENU_ON;
			break;
		case MENU_TURN_OFF:
			menu_state = MENU_OFF;
			supress_leds = true;
			break;
		case MENU_TOGGLE_SPOTLIGHT:
			ir_spot_on = !ir_spot_on;
			ir_timer = 0;  // do it now
			menu_state = MENU_ON;
			break;
		case MENU_TOGGLE_IR_BONUS:
			ir_cvg_on = !ir_cvg_on;
			ir_timer = 0; // do it now
			menu_state = MENU_ON;
			break;
		}
	}

	//exit menu
	if (sensor2.z > 60 && sensor1.z > 60) debounce++; //make this better?
	else debounce = 0;
	if (debounce == 255) menu_state = MENU_ON;

}
*/