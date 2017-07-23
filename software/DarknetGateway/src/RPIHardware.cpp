
#include "RPIHardware.h"
#include <chrono>
#include <cstdio>
#include <cerrno>

RPISPI::RPISPI(int channel, int cs) : Channel(channel), CS(cs) {
}

RPISPI::~RPISPI() {
	shutdown();
}

ErrorType RPISPI::onMasterListen() {
	return unselect();
}

ErrorType RPISPI::onSelect() {
	//todo add this to a interface for GPIO
	bcm2835_gpio_write(CS,0);
	return ErrorType();
}

ErrorType RPISPI::onUnselect()  {
	//todo add this to a interface for GPIO
	bcm2835_gpio_write(CS,1);
	return ErrorType();
}

uint8_t RPISPI::onTransfer(uint8_t data) {
	return bcm2835_spi_transfer(data);
}

bool RPISPI::onInit(SPIFrequency  divider, BitOrder  bitOrder, SPIMode dataMode) {
	if(bcm2835_init()) {
		bcm2835_gpio_fsel(CS,BCM2835_GPIO_FSEL_OUTP);
		// We control CS line manually don't assert CEx line!
		bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
		//todo map from generic to RPI specific
		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);
		bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
		bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
		bcm2835_spi_begin();
		return true;
	}
	return false;
}

bool RPISPI::onShutdown() {
	bcm2835_spi_end();
	return true;
}


RPIHardware::RPIHardware(SPI *one, SPI* two) : Hardware(one,two) {
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
	UniqueIRQ = uniqueIRQ;
	IRQHandler = handler;
	pinMode(UniqueIRQ, INPUT);
	bcm2835_gpio_set_pud(UniqueIRQ, BCM2835_GPIO_PUD_DOWN);
	// Now we can enable Rising edge detection
	bcm2835_gpio_ren(UniqueIRQ);
	return ErrorType();
}

void RPIHardware::checkIRQ() {
	if (bcm2835_gpio_eds(UniqueIRQ)) {
		// Now clear the eds flag by setting it to 1
		bcm2835_gpio_set_eds(UniqueIRQ);
		printf("Rising event detect for pin GPIO%d\n", UniqueIRQ);
		(*IRQHandler)();
    }
}

void RPIHardware::onPinMode(uint8_t pin, Hardware::PINMODE mode) {
	if(mode==Hardware::OUTPUT) {
		 bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_OUTP);
	} else {
		 bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_INPT);
	}
}

void RPIHardware::onDigitalWrite(uint8_t pin, Hardware::PINVAL val) {
	bcm2835_gpio_write(pin,val);
}

uint8_t RPIHardware::onDigitalRead(uint8_t pin) {
	return bcm2835_gpio_lev(pin);
}

static auto ApplicationStartTime = std::chrono::system_clock::now();

//returns milliseconds since application start
uint32_t Hardware::millis() {
	auto diff = std::chrono::system_clock::now()-ApplicationStartTime;
	return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
}

