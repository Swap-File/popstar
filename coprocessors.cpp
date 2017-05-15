#include "coprocessors.h"
#include <FastCRC.h>
#include <cobs.h> 

FastCRC8 CRC8;

static uint8_t el_packet_num = 0;

//serial com data
#define INCOMING_BUFFER_SIZE 128
static uint8_t incoming_raw_buffer[INCOMING_BUFFER_SIZE];
static uint8_t incoming_index = 0;
static uint8_t incoming_decoded_buffer[INCOMING_BUFFER_SIZE];

uint8_t crc_error = 0;
uint8_t framing_error = 0;

static int16_t yaw_raw = 18000;  //yaw pitch and roll in degrees * 100
static int16_t pitch_raw = 18000;
static int16_t roll_raw = 18000;
static int32_t yaw_offset = 0;  //yaw pitch and roll in degrees * 100
static int32_t pitch_offset = 0;
static int32_t roll_offset = 0;
int32_t yaw_compensated = 18000;  //yaw pitch and roll in degrees * 100
int32_t pitch_compensated = 18000;
int32_t roll_compensated = 18000;

void elwire_output(void){		//el wire
	uint8_t raw_buffer[15];
	
	raw_buffer[0] = EL_data;
	raw_buffer[1] = el_packet_num++;
	raw_buffer[2] = CRC8.maxim(raw_buffer, 2);

	uint8_t encoded_buffer[16];
	uint8_t encoded_size = COBSencode(raw_buffer, 3, encoded_buffer);

	Serial1.write(encoded_buffer, encoded_size);
	Serial1.write(0x00);
}

void SerialUpdate(void) {
	while (Serial1.available()) {

		//read in a byte
		incoming_raw_buffer[incoming_index] = Serial1.read();

		//check for end of packet
		if (incoming_raw_buffer[incoming_index] == 0x00) {

			//try to decode
			uint8_t decoded_length = COBSdecode(incoming_raw_buffer, incoming_index, incoming_decoded_buffer);

			//check length of decoded data (cleans up a series of 0x00 bytes)
			if (decoded_length > 0) {
				receivePacket(incoming_decoded_buffer, decoded_length);
			}

			//reset index
			incoming_index = 0;
		}
		else {
			//read data in until we hit overflow then start over
			incoming_index++;
			if (incoming_index == INCOMING_BUFFER_SIZE) incoming_index = 0;
		}
	}
}

void receivePacket(const uint8_t* buffer, size_t size)
{
	//check for framing errors
	if (size != 7) {
		framing_error++;
	}
	else {
		//check for crc errors
		byte crc = CRC8.maxim(buffer, size - 1);
		if (crc != buffer[size - 1]) {
			crc_error++;
		}
		else {

			yaw_raw = buffer[0] << 8 | buffer[1];
			pitch_raw = buffer[2] << 8 | buffer[3];
			roll_raw = buffer[4] << 8 | buffer[5];

			yaw_compensated = yaw_offset - yaw_raw;
			pitch_compensated = pitch_offset - pitch_raw;
			roll_compensated = roll_offset - roll_raw;

			//center data on 18000, meaning 180.00 degrees
			yaw_compensated = (yaw_compensated + 2 * 36000 + 18000) % 36000;
			pitch_compensated = (pitch_compensated + 2 * 36000 + 18000) % 36000;
			roll_compensated = (roll_compensated + 2 * 36000 + 18000) % 36000;
			
		}
	}
}

void center_IMU(void) {
	yaw_offset = yaw_compensated;
}