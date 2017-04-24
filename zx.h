// zx.h

#ifndef _ZX_h
#define _ZX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void zx_init(void);
void zx_update(void);
#endif

