#ifndef _NOISE_h
#define _NOISE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "popstar.h"

void Noise(uint8_t noise_version);

#endif