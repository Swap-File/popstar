#include "popstar.h"
#include "el.h"

uint8_t EL_Mode = EL_OFF;
uint8_t EL_Mode_last = EL_Mode;
uint8_t EL_animation = 0;
uint8_t EL_data;

uint32_t el_ani_time = 0;

void update_el_state(void) {

		if (background_mode >= BACKGROUND_FFT_FIRST && background_mode <= BACKGROUND_NOISE_LAST) EL_Mode = EL_FFT;
		else if (background_mode >= BACKGROUND_ANI_FIRST && background_mode <= BACKGROUND_ANI_LAST) EL_Mode = EL_ANI;

		if (EL_Mode == EL_OFF) {
			if (EL_animation == EL_ANI_STOP)  EL_data = 0;
		}
		else if (EL_Mode == EL_ANI) {
			if (EL_animation == EL_ANI_STOP) EL_data = 0xFF;
		}
		else if (EL_Mode == EL_FFT) {
			if (EL_animation == EL_ANI_STOP) {
				EL_data = 0;
				for (uint8_t i = 0; i < 8; i++) {
					if (millis() - FFTdisplayValueMax16time[i] < 150)	bitSet(EL_data, i);
				}
			}
		}
	
		if (millis() - el_ani_time > 100) {
			switch (EL_animation) {
			case EL_ANI_BOOT: //state 10
				EL_data = 0b00000001;
				EL_animation--;
				break;
			case 9:
				EL_data = 0b00000011;
				EL_animation--;
				break;
			case 8:
				EL_data = 0b00000111;
				EL_animation--;
				break;
			case 7:
				EL_data = 0b00001111;
				EL_animation--;
				break;
			case 6:
				EL_data = 0b00011111;
				EL_animation--;
				break;
			case 5:
				EL_data = 0b00111111;
				EL_animation--;
				break;
			case 4:
				EL_data = 0b01111111;
				EL_animation--;
				break;
			case 3:
				EL_data = 0b11111111;
				EL_animation--;
				break;
			case 2:
				EL_animation = EL_ANI_STOP;
				break;

			case EL_ANI_RIGHT: //state 20
				EL_data = 0b11000001;
				EL_animation--;
				break;
			case 19:
				EL_data = 0b11000010;
				EL_animation--;
				break;
			case 18:
				EL_data = 0b11000100;
				EL_animation--;
				break;
			case 17:
				EL_data = 0b11001000;
				EL_animation--;
				break;
			case 16:
				EL_data = 0b11010000;
				EL_animation--;
				break;
			case 15:
				EL_data = 0b11100000;
				EL_animation--;
				break;
			case 14:
				EL_animation = EL_ANI_STOP;
				break;

			case EL_ANI_LEFT:
				EL_data = 0b11100000;
				EL_animation--;
				break;
			case 29:
				EL_data = 0b11010000;
				EL_animation--;
				break;
			case 28:
				EL_data = 0b11001000;
				EL_animation--;
				break;
			case 27:
				EL_data = 0b11000100;
				EL_animation--;
				break;
			case 26:
				EL_data = 0b11000010;
				EL_animation--;
				break;
			case 25:
				EL_data = 0b11000001;
				EL_animation--;
				break;
			case 24:
				EL_animation = EL_ANI_STOP;
				break;
			}
			el_ani_time = millis();
		}


	EL_Mode_last = EL_Mode;
}