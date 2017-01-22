#include "display_device.h"

/////////////////////////////////////////////////////////////////////
// Generic base dipslay device
DisplayDevice::DisplayDevice(uint16_t w, uint16_t h) :
		width(w), height(h) {
}

DisplayDevice::~DisplayDevice() {

}


//////////////////////////////////////////////////////////////////
// ST7735 based device

enum PIN_PORTS {
	OLED_RESET
	, OLED_BACK_LIT
	, OLED_DATA_CMD
	, OLED_CHIP_SELECT
	, OLED_CLK
	, OLED_MOSI
};


DisplayST7735::DisplayST7735(uint16_t w, uint16_t h) : DisplayDevice(w,h) {

}

DisplayST7735::~DisplayST7735() {

}

ErrorType DisplayST7735::init() {
	ErrorType et;
	return et;
}


