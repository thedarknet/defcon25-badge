// ArduinoCompat/HardwareSPI.cpp
//
// Interface between Arduino-like SPI interface and STM32F4 Discovery and similar
// using STM32F4xx_DSP_StdPeriph_Lib_V1.3.0


#include "Hardware.h"

ErrorType SPI::masterListen() {
	return onMasterListen();
}

ErrorType SPI::select() {
	return onSelect();
}

ErrorType SPI::unselect() {
	return onUnselect();
}

uint8_t SPI::transfer(uint8_t data) {
	return onTransfer(data);
}

SPI::~SPI() {

}


Hardware::Hardware(SPI *one, SPI *two) : SPI1(one), SPI2(two) {
}

ErrorType Hardware::enableIRQ() {
	return onEnableIRQ();
}

ErrorType Hardware::disableIRQ() {
	return onDisableIRQ();
}

ErrorType Hardware::attachInterrupt(int uniqueIRQ, void (*handler)(void), int mode) {
	return onAttachInterrupt(uniqueIRQ, handler, mode);
}

SPI *Hardware::getSPI1() {
	return SPI1;
}

SPI *Hardware::getSPI2() {
	return SPI2;
}

Hardware::~Hardware() {

}

