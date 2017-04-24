// 
// 
// 

#include "oled.h"
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include "FastLED.h"

Adafruit_SSD1306 oledd(OLED_DC, OLED_RESET, OLED_CS);


void oled_init(void) {
	oledd.begin(SSD1306_SWITCHCAPVCC);
	oledd.setRotation(2);
	oledd.display();
	oledd.setTextSize(1);
	oledd.setTextColor(WHITE);
	oledd.setTextWrap(false);

}

void oled_update(void) {

	oledd.clearDisplay();
	oledd.setCursor(24, 12);

	for (uint8_t x = 0; x < 24; x++) {
		for (uint8_t y = 0; y < 16; y++) {
		
			if (Background_Array[x][y].getLuma() >= 50) {  //This sets where decayed pixels disappear from HUD
				oledd.drawPixel(x,y, WHITE);
			}
		}
	}

	oledd.print("hi");
	oledd.display();
}