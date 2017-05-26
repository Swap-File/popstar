#include <FastCRC.h>
#include <cobs.h> //cobs encoder and decoder 


#include "IRLremote.h"

//Remote Button Codes
#define CVG_PIN A4
#define CVG_REMOTE_ADDRESS 61184

#define CVG_UP_BUTTON 65280
#define CVG_R1_BUTTON 64260
#define CVG_R2_BUTTON 63240
#define CVG_R3_BUTTON 62220
#define CVG_R4_BUTTON 61200
#define CVG_R5_BUTTON 60180

#define CVG_DOWN_BUTTON 65025
#define CVG_G1_BUTTON 64005
#define CVG_G2_BUTTON 62985
#define CVG_G3_BUTTON 61965
#define CVG_G4_BUTTON 60945
#define CVG_G5_BUTTON 59925

#define CVG_OFF_BUTTON 64770
#define CVG_B1_BUTTON 63750
#define CVG_B2_BUTTON 62730
#define CVG_B3_BUTTON 61710
#define CVG_B4_BUTTON 60690
#define CVG_B5_BUTTON 59670

#define CVG_ON_BUTTON 64515
#define CVG_W_BUTTON 63495
#define CVG_FLASH_BUTTON 62475
#define CVG_STROBE_BUTTON 61455
#define CVG_FADE_BUTTON 60435
#define CVG_SMOOTH_BUTTON 59415

//Spotlight Button Codes
#define SPOT_PIN A3
#define SPOT_REMOTE_ADDRESS 65280

#define SPOT_UP_BUTTON 64005
#define SPOT_R1_BUTTON 62985
#define SPOT_R2_BUTTON 61965
#define SPOT_R3_BUTTON 59925
#define SPOT_R4_BUTTON 58905
#define SPOT_R5_BUTTON 60945

#define SPOT_DOWN_BUTTON 64260
#define SPOT_G1_BUTTON 63240
#define SPOT_G2_BUTTON 62220
#define SPOT_G3_BUTTON 60180
#define SPOT_G4_BUTTON 59160
#define SPOT_G5_BUTTON 61200

#define SPOT_OFF_BUTTON 63750
#define SPOT_B1_BUTTON 62730
#define SPOT_B2_BUTTON 61710
#define SPOT_B3_BUTTON 59670
#define SPOT_B4_BUTTON 58650
#define SPOT_B5_BUTTON 60690

#define SPOT_ON_BUTTON 63495
#define SPOT_W_BUTTON 62475
#define SPOT_FLASH_BUTTON 61455
#define SPOT_STROBE_BUTTON 59415
#define SPOT_FADE_BUTTON 58395
#define SPOT_SMOOTH_BUTTON 60435

//repeat
#define REPEAT_ADDRESS 0
#define REPEAT_COMMAND 65535

//serial commands

#define SERIAL_CVG_UP_BUTTON 0
#define SERIAL_CVG_R1_BUTTON 1
#define SERIAL_CVG_R2_BUTTON 2
#define SERIAL_CVG_R3_BUTTON 3
#define SERIAL_CVG_R4_BUTTON 4
#define SERIAL_CVG_R5_BUTTON 5

#define SERIAL_CVG_DOWN_BUTTON 6
#define SERIAL_CVG_G1_BUTTON 7
#define SERIAL_CVG_G2_BUTTON 8
#define SERIAL_CVG_G3_BUTTON 9
#define SERIAL_CVG_G4_BUTTON 10
#define SERIAL_CVG_G5_BUTTON 11

#define SERIAL_CVG_OFF_BUTTON 12
#define SERIAL_CVG_B1_BUTTON 13
#define SERIAL_CVG_B2_BUTTON 14
#define SERIAL_CVG_B3_BUTTON 15
#define SERIAL_CVG_B4_BUTTON 16
#define SERIAL_CVG_B5_BUTTON 17

#define SERIAL_CVG_ON_BUTTON 18
#define SERIAL_CVG_W_BUTTON 19
#define SERIAL_CVG_FLASH_BUTTON 20
#define SERIAL_CVG_STROBE_BUTTON 21
#define SERIAL_CVG_FADE_BUTTON 22
#define SERIAL_CVG_SMOOTH_BUTTON 23


#define SERIAL_SPOT_UP_BUTTON 24
#define SERIAL_SPOT_R1_BUTTON 25
#define SERIAL_SPOT_R2_BUTTON 26
#define SERIAL_SPOT_R3_BUTTON 27
#define SERIAL_SPOT_R4_BUTTON 28
#define SERIAL_SPOT_R5_BUTTON 29

#define SERIAL_SPOT_DOWN_BUTTON 30
#define SERIAL_SPOT_G1_BUTTON 31
#define SERIAL_SPOT_G2_BUTTON 32
#define SERIAL_SPOT_G3_BUTTON 33
#define SERIAL_SPOT_G4_BUTTON 34
#define SERIAL_SPOT_G5_BUTTON 35

#define SERIAL_SPOT_OFF_BUTTON 36
#define SERIAL_SPOT_B1_BUTTON 37
#define SERIAL_SPOT_B2_BUTTON 38
#define SERIAL_SPOT_B3_BUTTON 39
#define SERIAL_SPOT_B4_BUTTON 40
#define SERIAL_SPOT_B5_BUTTON 41

#define SERIAL_SPOT_ON_BUTTON 42
#define SERIAL_SPOT_W_BUTTON 43
#define SERIAL_SPOT_FLASH_BUTTON 44
#define SERIAL_SPOT_STROBE_BUTTON 45
#define SERIAL_SPOT_FADE_BUTTON 46
#define SERIAL_SPOT_SMOOTH_BUTTON 47

#define SERIAL_CVG_REPEAT_COMMAND 48
#define SERIAL_SPOT_REPEAT_COMMAND 49

#define SERIAL_NONE 255

//serial com data
#define INCOMING_BUFFER_SIZE 128
uint8_t incoming_raw_buffer[INCOMING_BUFFER_SIZE];
uint8_t incoming_index = 0;
uint8_t incoming_decoded_buffer[INCOMING_BUFFER_SIZE];
uint8_t last_packet_sequence_number = 0;
uint32_t last_good_serial_data_time = 0;


uint8_t crc_error = 0;
uint8_t framing_error = 0;

uint32_t fps_time = 0; //keeps track of when the last cycle was
uint8_t packets_in_counter = 0;  //counts up
uint8_t packets_in_per_second = 0; //saves the value

FastCRC8 CRC8;

void setup() {

  Serial.begin(57600);
  // The EL channels are on pins 2 through 9 on the ATMega328
  // Initialize the pins as outputs
  pinMode(2, OUTPUT);  // channel A
  pinMode(3, OUTPUT);  // channel B
  pinMode(4, OUTPUT);  // channel C
  pinMode(5, OUTPUT);  // channel D
  pinMode(6, OUTPUT);  // channel E
  pinMode(7, OUTPUT);  // channel F
  pinMode(8, OUTPUT);  // channel G
  pinMode(9, OUTPUT);  // channel H
  pinMode(13, OUTPUT); // status LED
  digitalWrite(13, HIGH);
}

void loop()
{
  //blank if no data for 4 seconds
  if (millis() - last_good_serial_data_time > 100) {
    for (uint8_t x = 2; x <= 9; x++) digitalWrite(x, LOW);
  }

  if (millis() - last_good_serial_data_time > 50) digitalWrite(13, LOW);
  else                                            digitalWrite(13, bitRead(millis(), 7)); 

  //fire once per second
  if (millis() - fps_time > 1000) {
    packets_in_per_second = packets_in_counter;
    packets_in_counter = 0;
    fps_time = millis();
  }

  SerialUpdate();
}

void SerialUpdate(void) {
  while (Serial.available()) {

    //read in a byte
    incoming_raw_buffer[incoming_index] = Serial.read();

    //check for end of packet
    if (incoming_raw_buffer[incoming_index] == 0x00) {

      //try to decode
      uint8_t decoded_length = COBSdecode(incoming_raw_buffer, incoming_index, incoming_decoded_buffer);

      //check length of decoded data (cleans up a series of 0x00 bytes)
      if (decoded_length > 0) receivePacket(incoming_decoded_buffer, decoded_length);

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
  //format of packet is
  //ABCDEFGH PACKET_INDEX CRC

  //increment packet stats counter
  if (packets_in_counter < 255)   packets_in_counter++;

  //check for framing errors
  if (size != 3) {
    framing_error++;
  }
  else {
    //check for crc errors
    uint8_t crc = CRC8.maxim(buffer, size - 1);
    if (crc != buffer[size - 1]) {
      crc_error++;
    }
    else {

      if (size == 3) {

        //keep track of when the last valid packet came in
        last_good_serial_data_time = millis();

        //write out all the el wire outputs
        for (uint8_t x = 0; x <= 8; x++) digitalWrite(x + 2, bitRead(buffer[0], x));

        //toggle the status light based on the 3rd digit of the packet counter

        switch (buffer[1]) {
          case SERIAL_CVG_UP_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_UP_BUTTON);
            break;
          case SERIAL_CVG_R1_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_R1_BUTTON);
            break;
          case SERIAL_CVG_R2_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_R2_BUTTON);
            break;
          case SERIAL_CVG_R3_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_R3_BUTTON);
            break;
          case SERIAL_CVG_R4_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_R4_BUTTON);
            break;
          case SERIAL_CVG_R5_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_R5_BUTTON);
            break;


          case SERIAL_CVG_DOWN_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_DOWN_BUTTON);
            break;
          case SERIAL_CVG_G1_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_G1_BUTTON);
            break;
          case SERIAL_CVG_G2_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_G2_BUTTON);
            break;
          case SERIAL_CVG_G3_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_G3_BUTTON);
            break;
          case SERIAL_CVG_G4_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_G4_BUTTON);
            break;
          case SERIAL_CVG_G5_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_G5_BUTTON);
            break;

          case SERIAL_CVG_OFF_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_OFF_BUTTON);
            break;
          case SERIAL_CVG_B1_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_B1_BUTTON);
            break;
          case SERIAL_CVG_B2_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_B2_BUTTON);
            break;
          case SERIAL_CVG_B3_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_B3_BUTTON);
            break;
          case SERIAL_CVG_B4_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_B4_BUTTON);
            break;
          case SERIAL_CVG_B5_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_B5_BUTTON);
            break;

          case SERIAL_CVG_ON_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_ON_BUTTON);
            break;
          case SERIAL_CVG_W_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_W_BUTTON);
            break;
          case SERIAL_CVG_FLASH_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_FLASH_BUTTON);
            break;
          case SERIAL_CVG_STROBE_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_STROBE_BUTTON);
            break;
          case SERIAL_CVG_FADE_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_FADE_BUTTON);
            break;
          case SERIAL_CVG_SMOOTH_BUTTON:
            IRLwrite<IR_NEC>(CVG_PIN, CVG_REMOTE_ADDRESS, CVG_SMOOTH_BUTTON);
            break;


          case SERIAL_SPOT_UP_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_UP_BUTTON);
            break;
          case SERIAL_SPOT_R1_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_R1_BUTTON);
            break;
          case SERIAL_SPOT_R2_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_R2_BUTTON);
            break;
          case SERIAL_SPOT_R3_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_R3_BUTTON);
            break;
          case SERIAL_SPOT_R4_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_R4_BUTTON);
            break;
          case SERIAL_SPOT_R5_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_R5_BUTTON);
            break;


          case SERIAL_SPOT_DOWN_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_DOWN_BUTTON);
            break;
          case SERIAL_SPOT_G1_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_G1_BUTTON);
            break;
          case SERIAL_SPOT_G2_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_G2_BUTTON);
            break;
          case SERIAL_SPOT_G3_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_G3_BUTTON);
            break;
          case SERIAL_SPOT_G4_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_G4_BUTTON);
            break;
          case SERIAL_SPOT_G5_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_G5_BUTTON);
            break;

          case SERIAL_SPOT_OFF_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_OFF_BUTTON);
            break;
          case SERIAL_SPOT_B1_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_B1_BUTTON);
            break;
          case SERIAL_SPOT_B2_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_B2_BUTTON);
            break;
          case SERIAL_SPOT_B3_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_B3_BUTTON);
            break;
          case SERIAL_SPOT_B4_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_B4_BUTTON);
            break;
          case SERIAL_SPOT_B5_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_B5_BUTTON);
            break;

          case SERIAL_SPOT_ON_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_ON_BUTTON);
            break;
          case SERIAL_SPOT_W_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_W_BUTTON);
            break;
          case SERIAL_SPOT_FLASH_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_FLASH_BUTTON);
            break;
          case SERIAL_SPOT_STROBE_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_STROBE_BUTTON);
            break;
          case SERIAL_SPOT_FADE_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_FADE_BUTTON);
            break;
          case SERIAL_SPOT_SMOOTH_BUTTON:
            IRLwrite<IR_NEC>(SPOT_PIN, SPOT_REMOTE_ADDRESS, SPOT_SMOOTH_BUTTON);
            break;

          case SERIAL_CVG_REPEAT_COMMAND:
            IRLwrite<IR_NEC>(CVG_PIN, REPEAT_ADDRESS, REPEAT_COMMAND);
            break;
          case SERIAL_SPOT_REPEAT_COMMAND:
            IRLwrite<IR_NEC>(SPOT_PIN, REPEAT_ADDRESS, REPEAT_COMMAND);
            break;

        }
        
      }
    }
  }
}


