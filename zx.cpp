#include "zx.h"
#include <i2c_t3.h>
#include <Metro.h>

Metro Changemodef = Metro(1000, 1);

static uint8_t state = 0;
static uint8_t fps = 0;
static uint16_t hangs = 0;
static uint32_t next_start_time = 0;
static uint32_t bus_timeout = 0;

zx_sensor sensor1;
zx_sensor sensor2;
zx_sensor* current_sensor = &sensor1;
void next_sensor(void);

void zx_init() {
	Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000, I2C_OP_MODE_DMA);
	Wire.setDefaultTimeout(100);
	Wire.resetBus();
	bus_timeout = millis();
	next_start_time = millis();
	sensor1.address = SENSOR1;
	sensor2.address = SENSOR2;
}

void zx_update() {

	if (Wire.done() && (state % 2)) { //check if dma transaction is done, all odd states are wait to finish
		state++;  //go to next even state
		bus_timeout = millis(); //reset bus hung timer
	}

	if (state == 0) {  //get status of sensor1
		if (millis() > next_start_time) {  //throttle polling
			next_start_time += 10; //throttle to 50hz per sensor, 10ms delay since it alternates 2 sensors
			if (next_start_time < millis()) next_start_time = millis() + 10; //catchup if a cycle is missed
			Wire.beginTransmission(current_sensor->address);
			Wire.write(ZX_STATUS);
			Wire.sendTransmission();
			state++;
		}
	}
	if (state == 2) {  //check if we get Z or X status of sensor1
		Wire.requestFrom(current_sensor->address, 1, I2C_STOP);
		state++;
	}
	if (state == 4 && Wire.available()) {
		current_sensor->status = Wire.read();
		if bitRead(current_sensor->status, 0) {
			Wire.beginTransmission(current_sensor->address);
			Wire.write(ZX_ZPOS);
			Wire.sendTransmission();
			state++;
		}
		else {
			state = 14;
		}
	}
	if (state == 6) {
		Wire.requestFrom(current_sensor->address, 1, I2C_STOP);
		state++;
	}
	if (state == 8 && Wire.available()) {
		current_sensor->z = Wire.read();
		Wire.beginTransmission(current_sensor->address);
		Wire.write(ZX_XPOS);
		Wire.sendTransmission();
		state++;
	}
	if (state == 10) {
		Wire.requestFrom(current_sensor->address, 1, I2C_STOP);
		state++;
	}
	if (state == 12 && Wire.available()) {
		current_sensor->x = Wire.read();
		current_sensor->zx_fresh = true;
		state = 14; //dont wait, go immediately to reply
	}
	if (state == 14) { //read gesture
		if (bitRead(current_sensor->status, 2) || bitRead(current_sensor->status, 3) || bitRead(current_sensor->status, 4)) {
			Wire.beginTransmission(current_sensor->address);
			Wire.write(ZX_GESTURE);
			Wire.sendTransmission();
			state++;
		}
		else {
			state = 24; //goto done
		}
	}
	if (state == 16) {
		Wire.requestFrom(current_sensor->address, 1, I2C_STOP);
		state++;
	}
	if (state == 18 && Wire.available()) {
		current_sensor->gesture = Wire.read();
		Wire.beginTransmission(current_sensor->address);
		Wire.write(ZX_GSPEED);
		Wire.sendTransmission();
		state++;
	}
	if (state == 20) {
		Wire.requestFrom(current_sensor->address, 1, I2C_STOP);
		state++;
	}
	if (state == 22 && Wire.available()) {
		current_sensor->speed = Wire.read();
		current_sensor->gesture_fresh = true;
		current_sensor->gesture_time = millis();
		state = 24; //goto done
	}
	if (state == 24 ) {   //done

		fps++;
		state = 0;

		current_sensor->x_filtered = (current_sensor->x_filtered >> 1) + (current_sensor->x >> 1);
		current_sensor->z_filtered = (current_sensor->z_filtered >> 1) + (current_sensor->z >> 1);

		next_sensor();
	}

	if (Changemodef.check()) {
		Serial.print("<");
		Serial.print(fps);
		Serial.print(" ");
		Serial.print(hangs);
		Serial.println(">");
		fps = 0;
	}

	if (millis() - bus_timeout > 30) {
		Serial.println("Hung?");
		hangs++;
		Wire.resetBus();
		state = 0;
		next_sensor();
		bus_timeout = millis();
		next_start_time = millis();
	}

}

void next_sensor(void){
	if (current_sensor == &sensor1) current_sensor = &sensor2;
	else							current_sensor = &sensor1;
}