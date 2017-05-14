#include "states.h"
#include "oled.h"
#include "zx.h"
#include "coprocessors.h"
#include "el.h"

#define UNLOCK_GESTURE_TIMEOUT 400
uint32_t unlock_timer = 0;

char menu_master[4][5][10] = {
	{ "LOCKED","LOCKED" ,"LOCKED" ,"LOCKED" ,"LOCKED"},
	{ "EFFECTS","COLORS","?" ,"?" ,"?" },
	{ "Noise 1","Noise 2","Noise 3","Noise 4","Noise 5" },
	{ "Demon","Hydra","Spider" ,"?" ,"?" }
};


void auto_head_tilt(){
		if (roll_compensated  < 13500)	 background_mode = BACKGROUND_FFT_HORZ_BARS_LEFT;
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

	if (background_mode >= BACKGROUND_ANI_FIRST && background_mode<= BACKGROUND_ANI_LAST) {
		background_mode += number;
		if (background_mode < BACKGROUND_ANI_FIRST) background_mode = BACKGROUND_ANI_LAST;
		if (background_mode > BACKGROUND_ANI_LAST) background_mode = BACKGROUND_ANI_FIRST;
	}
	else {
		auto_head_tilt();//override mode if tilting and not in an animation
	}	
}

void state_update() {
	//automatically change head tilt modes
	if (background_mode >= BACKGROUND_FFT_FIRST && background_mode <= BACKGROUND_FFT_LAST) {
		auto_head_tilt();
	}

	if (menu_x == 0) {
		if (sensor1.gesture_fresh) {
			if (sensor1.gesture == GESTURE_LEFT) {
				increment_background(-1);
				EL_animation = EL_ANI_LEFT;
			}
			else if (sensor1.gesture == GESTURE_RIGHT) {
				increment_background(1);
				EL_animation = EL_ANI_RIGHT;
			}


			if (sensor2.gesture_fresh) {
				if (sensor2.gesture == GESTURE_LEFT) {
					requested_palette--;
					EL_animation = EL_ANI_LEFT;
				}
				if (sensor2.gesture == GESTURE_RIGHT) {
					requested_palette++;
					EL_animation = EL_ANI_RIGHT;
				}

				ChangeTargetPalette(1);
			}
		}
	}
	//unlock
	if (sensor_first->gesture_fresh) {
		if (sensor_first->gesture == GESTURE_NONE) unlock_timer = millis();
	}
	if (sensor_first->gesture_fresh && millis() - unlock_timer < UNLOCK_GESTURE_TIMEOUT) {
		if (sensor_first->gesture == GESTURE_HOVER_UP) {
			sensor_first->gesture_fresh = false;
			if (menu_x == 0) 			menu_x = 1;
			else						menu_x = 0;
			
		}
	}

	//back a menu

	menu_y = constrain((sensor_second->z) / 8, 0, 4);
	if (menu_x == 1) {

		if (sensor_first->gesture_fresh) {
			if (menu_y == 0) menu_x = 2;
			if (menu_y == 1) menu_x = 3;
		}
	}
	else if (menu_x == 2) {
		if (sensor_first->gesture_fresh) {
			if (menu_y == 0) background_mode = BACKGROUND_NOISE_1;
			if (menu_y == 1) background_mode = BACKGROUND_NOISE_2;
			if (menu_y == 2) background_mode = BACKGROUND_NOISE_3;
			if (menu_y == 3) background_mode = BACKGROUND_NOISE_4;
			if (menu_y == 4) background_mode = BACKGROUND_NOISE_5;
		}
	}

	
}

