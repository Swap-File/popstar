#include "I2Cdev.h"
#include <FastCRC.h>
#include <cobs.h>
#include "MPU6050_6Axis_MotionApps20.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

FastCRC8 CRC8;

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

//serial com data
#define INCOMING_BUFFER_SIZE 128
uint8_t incoming_raw_buffer[INCOMING_BUFFER_SIZE];
uint8_t incoming_index = 0;
uint8_t incoming_decoded_buffer[INCOMING_BUFFER_SIZE];

uint8_t packets_in_counter;
volatile bool mpuInterrupt = false; 

void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {

  //Serial.println(F("Joining I2C Bus..."));
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  //Serial.println(F("Initializing Serial Port..."));
  Serial.begin(57600);

  //Serial.println(F("Initializing MPU6050 devices..."));
  mpu.initialize();

  //Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(69);
  mpu.setYGyroOffset(-30);
  mpu.setZGyroOffset(14);
  mpu.setXAccelOffset(-1129); // 1688 factory default for my test chip
  mpu.setYAccelOffset(1774); // 1688 factory default for my test chip
  mpu.setZAccelOffset(1828); // 1688 factory default for my test chip


  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    //Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    //Not using Interrupts for IMU, let the FIFO buffer handle it and poll
    //Color sensor interrupts are more important.
    // enable Arduino interrupt detection
    //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);

    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    //Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else {
    // ERROR!
  }
}

void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {

  }

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    //Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  }
  else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    //mpu.dmpGetAccel(&aa, fifoBuffer);
    //mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    //mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);

#ifdef TESTING
    Serial.print("ypr\t");
    Serial.print(ypr[0] * 180 / M_PI);
    Serial.print("\t");
    Serial.print(ypr[1] * 180 / M_PI);
    Serial.print("\t");
    Serial.println(ypr[2] * 180 / M_PI);

#endif


    byte raw_buffer[22];

    raw_buffer[0] = ((((int16_t)(ypr[0] * 18000 / M_PI)) >> 8) & 0xff);
    raw_buffer[1] = ((((int16_t)(ypr[0] * 18000 / M_PI)) >> 0) & 0xff);
    raw_buffer[2] = ((((int16_t)(ypr[1] * 18000 / M_PI)) >> 8) & 0xff);
    raw_buffer[3] = ((((int16_t)(ypr[1] * 18000 / M_PI)) >> 0) & 0xff);
    raw_buffer[4] = ((((int16_t)(ypr[2] * 18000 / M_PI)) >> 8) & 0xff);
    raw_buffer[5] = ((((int16_t)(ypr[2] * 18000 / M_PI)) >> 0) & 0xff);

    raw_buffer[6] = CRC8.maxim(raw_buffer, 6);
    
    //prep buffer completely
    uint8_t encoded_buffer[23];  //one extra to hold cobs data
    uint8_t encoded_size = COBSencode(raw_buffer,7, encoded_buffer);
    //send out data from last cycle
    Serial.write(encoded_buffer, encoded_size);
    Serial.write(0x00);

    digitalWrite(13, bitRead(packets_in_counter++, 3));
    
  }
}
