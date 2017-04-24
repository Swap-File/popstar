#include <FastCRC.h>
#include <cobs.h> //cobs encoder and decoder 

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
}

void loop()
{
  //blank if no data for 4 seconds
  if (millis() - last_good_serial_data_time > 100) {
    for (uint8_t x = 2; x <= 9; x++) digitalWrite(x, LOW);
    digitalWrite(13, LOW);
  }

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
      //keep track of when the last valid packet came in
      last_good_serial_data_time = millis();
      
      //write out all the el wire outputs
      for (uint8_t x = 0; x <= 8; x++) digitalWrite(x+2, bitRead(buffer[0], x));

      //toggle the status light based on the 3rd digit of the packet counter
      digitalWrite(13, bitRead(buffer[1], 3));
    }
  }
}
