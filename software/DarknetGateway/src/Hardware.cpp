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

bool SPI::init(SPIFrequency  divider, BitOrder  bitOrder, SPIMode dataMode) {
	return onInit(divider, bitOrder, dataMode);
}

bool SPI::shutdown() {
	return onShutdown();
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

void Hardware::pinMode(uint8_t pin, PINMODE mode) {
	onPinMode(pin,mode);
}

void Hardware::digitalWrite(uint8_t pin, Hardware::PINVAL val) {
	onDigitalWrite(pin,val);
}

uint8_t Hardware::digitalRead(uint8_t pin) {
	return onDigitalRead(pin);
}

SPI *Hardware::getSPI1() {
	return SPI1;
}

SPI *Hardware::getSPI2() {
	return SPI2;
}

Hardware::~Hardware() {

}

