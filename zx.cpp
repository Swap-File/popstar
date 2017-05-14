#include "zx.h"

#include <i2c_t3.h>
#include <Metro.h>

static uint8_t state = 0;  //state machine to space out DMA requests
static uint8_t fps = 0;  //simple FPS counter 
static uint16_t hangs = 0; //keep track of how many times the i2c bus hung
static uint32_t next_start_time = 0;  //when the next cycle needs to start in ms
static uint32_t bus_timeout = 0; //how long the i2c bus has been hung
static uint32_t order_set_time = 0; 
static uint8_t i2c_temp_data;  //hold a byte to update all vars on same cycle

zx_sensor sensor1;
zx_sensor sensor2;

zx_sensor* sensor_first;
zx_sensor* sensor_second;

//keep track of what sensor we are reading this cycle
zx_sensor* current_sensor = &sensor1;

void zx_init() {
	Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000, I2C_OP_MODE_DMA);
	Wire.setDefaultTimeout(100);
	Wire.resetBus();
	zx_reset_flags();
}

void zx_update() {

	//clear all fresh indicators if not cleared via state machine
	sensor1.zx_fresh = 0;
	sensor2.zx_fresh = 0;
	sensor1.gesture_fresh = 0;
	sensor2.gesture_fresh = 0;

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
		i2c_temp_data = Wire.read();  //save position to update at the same time
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
		//read and filter data
		current_sensor->x = (current_sensor->x >> 1) + (Wire.read() >> 1);
		current_sensor->z = (current_sensor->z >> 1) + (i2c_temp_data >> 1);
		current_sensor->zx_fresh = true;
		current_sensor->zx_time = millis();
		zx_update_order();
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
		 i2c_temp_data  = Wire.read();
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
		current_sensor->gesture_speed = Wire.read();
		current_sensor->gesture = i2c_temp_data;
		current_sensor->gesture_fresh = true;
		current_sensor->gesture_time = millis();
		state = 24; //goto done
	}
	if (state == 24 ) {   //done
		fps++;
		state = 0;
		zx_update_order();
		zx_next_sensor();
	}

	if (millis() - bus_timeout > I2C_BUS_TIMEOUT) {
		Serial.println("I2C Hung?");
		hangs++;
		Wire.resetBus();
		zx_reset_flags();
		zx_next_sensor();
	}
}

void zx_reset_flags(void) {
	bus_timeout = millis();
	next_start_time = millis();
	order_set_time = 0;
	sensor1.address = SENSOR1;
	sensor2.address = SENSOR2;
	sensor1.gesture_fresh = false;
	sensor2.gesture_fresh = false;
	sensor1.zx_fresh = false;
	sensor2.zx_fresh = false;
	sensor1.order = 0;
	sensor2.order = 0;
	order_set_time = 0;
	state = 0;
	sensor_first = &sensor1;
	sensor_second = &sensor2;
}

void zx_update_order(void) {
	if ((sensor1.zx_fresh || sensor1.gesture_fresh) && sensor2.order == 0) 	sensor1.order = 1;
	if ((sensor2.zx_fresh || sensor2.gesture_fresh) && sensor1.order == 0) 	sensor2.order = 1;
	if ((sensor1.zx_fresh || sensor1.gesture_fresh) && sensor2.order == 1) 	sensor1.order = 2;
	if ((sensor2.zx_fresh || sensor2.gesture_fresh) && sensor1.order == 1) 	sensor2.order = 2;

	if (sensor1.zx_fresh || sensor2.zx_fresh || sensor1.gesture_fresh || sensor2.gesture_fresh) {
		order_set_time = millis();
	}

	if (millis() - order_set_time > ORDER_RESET_TIMEOUT) {
		sensor1.order = 0;
		sensor2.order = 0;
	}

	if (sensor2.order == 1) {
		sensor_first = &sensor2;
		sensor_second = &sensor1;
	}
	else {
		sensor_first = &sensor1;
		sensor_second = &sensor2;
	}
}

void zx_next_sensor(void){
	if (current_sensor == &sensor1) current_sensor = &sensor2;
	else							current_sensor = &sensor1;
}