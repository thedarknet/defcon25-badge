//hardware abstraction class
#ifndef _RPIHardware_h
#define _RPIHardware_h

#include "Hardware.h"

class RPISPI : public SPI {
public:
	RPISPI(int channel, int cs);
	virtual ~RPISPI();
	virtual ErrorType onMasterListen();
	virtual ErrorType onSelect();
	virtual ErrorType onUnselect();
	virtual uint8_t onTransfer(uint8_t data);
private:
	int Channel;
	int CS;
};

class RPIHardware : public Hardware {
public:
	RPIHardware();
	virtual ~RPIHardware();
protected:
	virtual ErrorType onEnableIRQ();
	virtual ErrorType onDisableIRQ();
	virtual ErrorType onAttachInterrupt(int uniqueIRQ, void (*handler)(void), int mode);
};

//returns milliseconds since application start
//uint32_t millis();


#endif
