#include "dcdarknet_app.h"
#include "display_device.h"
#include "stm32f3xx_hal.h"

DCDarkNetApp::DCDarkNetApp() {

}

DisplayST7735 Display(128,160, DisplayST7735::PORTAIT);

ErrorType DCDarkNetApp::init() {
	ErrorType et;
	et = Display.init();
	return et;
}

void DCDarkNetApp::run() {
	static uint32_t time = HAL_GetTick();
	static bool bStart = true;

	//temp
	if(HAL_GetTick()-time>6000) {
		Display.fillScreen(RGBColor::BLACK);
		Display.fillScreen(RGBColor(255, 0, 0));
		Display.drawString(0, 0, "goodbye", RGBColor::WHITE, RGBColor::BLACK, 3, false);
		time = HAL_GetTick();
	} else if (HAL_GetTick()-time>3000) {
		Display.fillScreen(RGBColor::BLACK);
		Display.fillScreen(RGBColor(255, 0, 0));
		Display.drawString(0, 0, "Hello", RGBColor::WHITE, RGBColor::BLACK, 3, false);
	} else if (bStart == true) {
		bStart = false;
		Display.fillScreen(RGBColor(0, 0, 255));
		Display.drawString(0, 0, "Hello", RGBColor::WHITE, RGBColor::BLACK, 3, false);
	}
	///
}
