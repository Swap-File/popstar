#include "zx.h"
#include <i2c_t3.h>

#include <Metro.h>

Metro Changemodef = Metro(1000, 1);

#define SENSOR1             0x10
#define SENSOR2             0x11

#define ZX_XPOS             0x08
#define ZX_ZPOS             0x0A
uint32_t last_done = 0;
uint8_t state = 0;
uint32_t timestart = 0;
uint32_t timespend = 0;
uint16_t fps = 0;
uint16_t hangs = 0;
uint32_t  next_start = 0;

uint8_t sensor1_xpos = 0;
uint8_t sensor1_zpos = 0;
uint8_t sensor2_xpos = 0;
uint8_t sensor2_zpos = 0;

void zx_init() {
	Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000, I2C_OP_MODE_DMA);
	Wire.setDefaultTimeout(100);
	Wire.resetBus();
	last_done = millis();
	next_start = millis();
}


void zx_update() {

	if (state == 0) {
		if (millis() > next_start) {
			next_start += 10;
			if (next_start < millis()) next_start = millis() + 10; //catchup

			Wire.beginTransmission(SENSOR1);
			Wire.write(ZX_XPOS);
			Wire.sendTransmission();
			state++;
		}
	}
	else if (state == 2) {
		Wire.requestFrom(SENSOR1, 1, I2C_STOP);
		state++;
	}
	else  if (state == 4 && Wire.available()) {
		sensor1_xpos = Wire.read();
		state++;
	}
	else if (state == 6) {
		Wire.beginTransmission(SENSOR1);
		Wire.write(ZX_ZPOS);
		Wire.sendTransmission();
		state++;
	}
	else if (state == 8) {
		Wire.requestFrom(SENSOR1, 1, I2C_STOP);
		state++;
	}
	else if (state == 10 && Wire.available()) {
		sensor1_zpos = Wire.read();
		state++;
	}
	else if (state == 12) {
		Wire.beginTransmission(SENSOR2);
		Wire.write(ZX_XPOS);
		Wire.sendTransmission();
		state++;
	}
	else if (state == 14) {
		Wire.requestFrom(SENSOR2, 1, I2C_STOP);
		state++;
	}
	else  if (state == 16 && Wire.available()) {
		sensor2_xpos = Wire.read();
		state++;
	}
	else if (state == 18) {
		Wire.beginTransmission(SENSOR2);
		Wire.write(ZX_ZPOS);
		Wire.sendTransmission();
		state++;
	}
	else if (state == 20) {
		Wire.requestFrom(SENSOR2, 1, I2C_STOP);
		state++;
	}
	else if (state == 22 && Wire.available()) {
		sensor2_zpos = Wire.read();
		state = 0;
		fps++;
	}
	else if (Wire.done() && (state % 2)) {
		state++;
		last_done = millis();
	}
	

	if (Changemodef.check()) {
		Serial.print("<");
		Serial.print(fps);
		Serial.print(" ");
		Serial.print(hangs);
		Serial.println(">");
		fps = 0;
	}

	if (millis() - last_done > 20) {
		Serial.println("Hung?");
		hangs++;
		Wire.resetBus();
		state = 0;
		last_done = millis();
		next_start = millis();
	}

}