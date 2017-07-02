#include "nunchuk.h"
#include <i2c_t3.h>

#define ADDRESS 0x52

boolean nunchuk_fresh;
void nunchuk_update();
uint8_t nunchuk_button_last;
uint8_t nunchuk_button;
uint8_t nunchuk_dpad;

int nunchuk_analogX;
int nunchuk_analogY;
int nunchuk_analogMagnitude;
int nunchuk_analogAngle;
int nunchuk_accelX;
int nunchuk_accelY;
int nunchuk_accelZ;
int nunchuk_total_acceleration;
int nunchuk_accelAngle;


static int nunchuk_last_accelX;
static int nunchuk_last_accelY;
static int nunchuk_last_accelZ;

static uint32_t nunchuk_last_time = 0;

static boolean nunchuk_zButton;
static boolean nunchuk_cButton;
static boolean nunchuk_zButton_previous;
static boolean nunchuk_cButton_previous;
static uint32_t nunchuk_zButton_time;
static uint32_t nunchuk_cButton_time;

static uint32_t bus_timeout = 0; //how long the i2c bus has been hung
static uint8_t nunchuk_state = 0;


void nunchuk_update() {


	if (Wire.done()) {
		if ((nunchuk_state % 2) && (millis() - nunchuk_last_time > 10)) { //check if dma transaction is done, all odd states are wait to finish
			nunchuk_state++;  //go to next even state
			bus_timeout = millis(); //reset bus hung timer
		}

		if (nunchuk_state == 0) {
			Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, 100000, I2C_OP_MODE_DMA);
			Wire.setDefaultTimeout(20000);
			nunchuk_state++;
			nunchuk_last_time = millis();
		}
		else if (nunchuk_state == 2) {
			Wire.beginTransmission(ADDRESS);
			Wire.write(0xF0);
			Wire.write(0x55);
			Wire.sendTransmission();
			nunchuk_state++;
			nunchuk_last_time = millis();
		}
		else if (nunchuk_state == 4) {
			Wire.beginTransmission(ADDRESS);
			Wire.write(0xFB);
			Wire.write(0x00);
			Wire.sendTransmission();
			nunchuk_state++;
			nunchuk_last_time = millis();
		}
		else if (nunchuk_state == 6) {
			Wire.beginTransmission(ADDRESS);
			Wire.write(0x00);
			Wire.write(0x00);
			Wire.sendTransmission();
			nunchuk_state++;
			nunchuk_last_time = millis();
		}
		else if (nunchuk_state == 8) {
			Wire.sendRequest(ADDRESS, 6, I2C_STOP);
			nunchuk_state = 10; //jump immediately to reply wait
			nunchuk_last_time = millis();
		}
		else if (nunchuk_state == 10 && (Wire.available() == 6)) {

			uint8_t count = 0;
			uint8_t values[6];

			while (Wire.available()) {
				values[count] = Wire.readByte();
				count++;
			}

			nunchuk_analogX = 128 - values[0];
			nunchuk_analogY = 128 - values[1];
			nunchuk_accelX = (values[2] << 2) | ((values[5] >> 2) & 3);
			nunchuk_accelY = (values[3] << 2) | ((values[5] >> 4) & 3);
			nunchuk_accelZ = (values[4] << 2) | ((values[5] >> 6) & 3);
			nunchuk_zButton = !((values[5] >> 0) & 1);
			nunchuk_cButton = !((values[5] >> 1) & 1);

			if (nunchuk_zButton_previous != nunchuk_zButton && nunchuk_zButton == 0) nunchuk_zButton_time = millis();
			if (nunchuk_cButton_previous != nunchuk_cButton && nunchuk_cButton == 0) nunchuk_cButton_time = millis();

			nunchuk_button_last = nunchuk_button;
			if (nunchuk_button == BUTTON_CZ && (millis() - nunchuk_zButton_time < 50 || millis() - nunchuk_cButton_time < 50)) { //50ms max delay between events
				 //dont update
			}
			else {
				if (nunchuk_cButton   && nunchuk_zButton) nunchuk_button = BUTTON_CZ;
				else if (!nunchuk_cButton   && nunchuk_zButton) nunchuk_button = BUTTON_Z;
				else if (nunchuk_cButton && !nunchuk_zButton) nunchuk_button = BUTTON_C;
				else  nunchuk_button = BUTTON_NONE;
			}

			nunchuk_analogMagnitude = sqrt(pow(nunchuk_analogX, 2) + pow(nunchuk_analogY, 2));
			nunchuk_analogAngle = (atan2(nunchuk_analogY, nunchuk_analogX)*57.2957);

			nunchuk_total_acceleration = pow(nunchuk_last_accelX - nunchuk_accelX, 2) + pow(nunchuk_last_accelY - nunchuk_accelY, 2) + pow(nunchuk_last_accelZ - nunchuk_accelZ, 2);

			if (nunchuk_analogMagnitude > 40) {
				if (nunchuk_analogAngle < 10 && nunchuk_analogAngle > -10) {
					nunchuk_dpad = DPAD_LEFT;
				}
				else if (nunchuk_analogAngle < 55 && nunchuk_analogAngle > 35) {
					nunchuk_dpad = DPAD_DOWN_LEFT;
				}
				else if (nunchuk_analogAngle < -35 && nunchuk_analogAngle > -55) {
					nunchuk_dpad = DPAD_UP_LEFT;
				}
				else if (nunchuk_analogAngle < -80 && nunchuk_analogAngle > -100) {
					nunchuk_dpad = DPAD_UP;
				}
				else if (nunchuk_analogAngle < 100 && nunchuk_analogAngle > 80) {
					nunchuk_dpad = DPAD_DOWN;
				}
				else if (nunchuk_analogAngle < 145 && nunchuk_analogAngle > 125) {
					nunchuk_dpad = DPAD_DOWN_RIGHT;
				}
				else if (nunchuk_analogAngle < -125 && nunchuk_analogAngle > -145) {
					nunchuk_dpad = DPAD_UP_RIGHT;
				}
				else if (nunchuk_analogAngle < -170 || nunchuk_analogAngle > 170) {
					nunchuk_dpad = DPAD_RIGHT;
				}
				else {
					nunchuk_dpad = DPAD_DEADZONE;
				}
			}
			else {
				nunchuk_dpad = DPAD_NONE;
			}

			nunchuk_last_accelX = nunchuk_accelX;
			nunchuk_last_accelY = nunchuk_accelY;
			nunchuk_last_accelZ = nunchuk_accelZ;
			nunchuk_zButton_previous = nunchuk_zButton;
			nunchuk_cButton_previous = nunchuk_cButton;

			nunchuk_fresh = true;
			nunchuk_state = 5; //goto again
		}
	}

	if (millis() - bus_timeout > 1000) {
		Serial.println("I2C Hung?");
		Wire.resetBus();
		bus_timeout = millis();
		nunchuk_last_time = millis();
		nunchuk_state =0; //goto re init
	}

	//Serial.println(nunchuk_state);

	return;
}

