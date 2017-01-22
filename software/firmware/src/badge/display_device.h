#ifndef DCDARKNET_DISPLAY_DEVICE_H
#define DCDARKNET_DISPLAY_DEVICE_H

#include "error_type.h"

class DisplayDevice {
public:
	DisplayDevice(uint16_t w, uint16_t h);
	virtual ErrorType init()=0;
	virtual ~DisplayDevice();
public:

private:
	uint16_t width;
	uint16_t height;
};

class DisplayST7735 : public DisplayDevice {
public:
	DisplayST7735(uint16_t w, uint16_t h);
	virtual ErrorType init();
	virtual ~DisplayST7735();
private:

};

#endif
