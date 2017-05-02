#include "states.h"
#include "oled.h"
#include "zx.h"

#define UNLOCK_GESTURE_TIMEOUT 400
uint32_t unlock_timer = 0;

zx_sensor* sensor_first = &sensor1;
zx_sensor* sensor_second = &sensor2;

void state_update() {

	if (sensor1.gesture_fresh) {
		if (sensor1.gesture == GESTURE_LEFT) background_mode--;
		if (sensor1.gesture == GESTURE_RIGHT) background_mode++;

		if (background_mode < BACKGROUND_FIRST) background_mode = BACKGROUND_LAST;
		if (background_mode > BACKGROUND_LAST) background_mode = BACKGROUND_FIRST;

	}

	if (sensor1.order == 1) {
		 sensor_first = &sensor1;
		 sensor_second = &sensor2;
	}
	else {
		 sensor_first = &sensor2;
		 sensor_second = &sensor1;
	}

	if (sensor_first->gesture_fresh) {
		if (sensor_first->gesture == GESTURE_NONE) unlock_timer = millis();
	}

	if (sensor_first->gesture_fresh && millis() - unlock_timer < UNLOCK_GESTURE_TIMEOUT) {
		if (sensor_first->gesture == GESTURE_HOVER_UP) {
			if (menu_location == MENU_LOCKED) menu_location = MENU_ROOT_FIRST;
			else  (menu_location = MENU_LOCKED);
		}
	}


	
}
