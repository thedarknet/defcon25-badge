
#include "RPIHardware.h"
#include <chrono>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <cstdio>
#include <cerrno>

static const uint32_t SPI1_CS = 6; //pin 22

RPISPI::RPISPI(int channel, int cs) : Channel(channel), CS(cs) {
	if(-1==wiringPiSPISetup(Channel,18000000)) {
		printf("error: %d\n",errno);
	}
	pinMode(OUTPUT,CS); //pin 22
}

RPISPI::~RPISPI() {

}

ErrorType RPISPI::onMasterListen() {
	return unselect();
}

ErrorType RPISPI::onSelect() {
	digitalWrite(CS,0);
	return ErrorType();
}

ErrorType RPISPI::onUnselect()  {
	digitalWrite(CS,1);
	return ErrorType();
}

uint8_t RPISPI::onTransfer(uint8_t data) {
	if(wiringPiSPIDataRW(Channel, &data,1)!=-1) {
		return data;	
	}
	return 0;
}

RPISPI RPISPI1(0,SPI1_CS);

RPIHardware::RPIHardware() : Hardware(&RPISPI1,0) {
	wiringPiSetup();
}

RPIHardware::~RPIHardware() {

}

ErrorType RPIHardware::onEnableIRQ()  {
	return ErrorType();
}

ErrorType RPIHardware::onDisableIRQ() {
	return ErrorType();
}


ErrorType RPIHardware::onAttachInterrupt(int uniqueIRQ, void (*handler)(void), int mode) {
	return ErrorType();
}


static auto ApplicationStartTime = std::chrono::system_clock::now();

RPIHardware TheHardware;
Hardware &Hardware::get() {
	return TheHardware;
}

/*
//returns milliseconds since application start
uint32_t millis() {
	auto diff = std::chrono::system_clock::now()-ApplicationStartTime;
	return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
}
*/

