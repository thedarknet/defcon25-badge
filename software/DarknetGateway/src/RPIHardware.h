//hardware abstraction class
#ifndef _RPIHardware_h
#define _RPIHardware_h

#include <bcm2835.h>
#include "Hardware.h"

class RPISPI : public SPI {
public:
	RPISPI(int channel, int cs);
	virtual ~RPISPI();
	virtual ErrorType onMasterListen();
	virtual ErrorType onSelect();
	virtual ErrorType onUnselect();
	virtual uint8_t onTransfer(uint8_t data);
	virtual bool onInit(SPIFrequency  divider, BitOrder  bitOrder, SPIMode dataMode);
	virtual bool onShutdown();
private:
	int Channel;
	int CS;
};

class RPIHardware : public Hardware {
public:
	RPIHardware(SPI *spi1, SPI *spi2);
	void checkIRQ();
	virtual ~RPIHardware();
protected:
	virtual ErrorType onEnableIRQ();
	virtual ErrorType onDisableIRQ();
	virtual ErrorType onAttachInterrupt(int uniqueIRQ, void (*handler)(void), int mode);
	virtual void onPinMode(uint8_t pin, PINMODE mode);
	virtual void onDigitalWrite(uint8_t pin, PINVAL val);
	virtual uint8_t onDigitalRead(uint8_t pin);
protected:
	uint8_t UniqueIRQ;
	void (*IRQHandler)();
	
};


#endif
