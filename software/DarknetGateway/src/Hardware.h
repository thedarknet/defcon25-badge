//hardware abstraction class
#ifndef _Hardware_h
#define _Hardware_h

#include <stdint.h>
#include <error_type.h>

typedef enum SPIFrequency {
	SPI_21_0MHZ = 0, /**< 21 MHz */
	SPI_10_5MHZ = 1, /**< 10.5 MHz */
	SPI_5_25MHZ = 2, /**< 5.25 MHz */
	SPI_2_625MHZ = 3, /**< 2.625 MHz */
	SPI_1_3125MHZ = 4, /**< 1.3125 MHz */
	SPI_656_25KHZ = 5, /**< 656.25 KHz */
	SPI_328_125KHZ = 6, /**< 328.125 KHz */
} SPIFrequency;

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

class SPI {
public:
	ErrorType masterListen();
	ErrorType select();
	ErrorType unselect();
	uint8_t transfer(uint8_t data);
protected:
	virtual ~SPI()=0;
	virtual ErrorType onMasterListen()=0;
	virtual ErrorType onSelect()=0;
	virtual ErrorType onUnselect()=0;
	virtual uint8_t onTransfer(uint8_t data)=0;
};

class Hardware {
public:
	static Hardware &get();
public:
	ErrorType enableIRQ();
	ErrorType disableIRQ();
	ErrorType attachInterrupt(int uniqueIRQ, void (*handler)(void), int mode);
	SPI *getSPI1();
	SPI *getSPI2();
	///returns milliseconds since application start
protected:
	Hardware(SPI *one, SPI *two);
	virtual ~Hardware()=0;
	virtual ErrorType onEnableIRQ()=0;
	virtual ErrorType onDisableIRQ()=0;
	virtual ErrorType onAttachInterrupt(int uniqueIRQ, void (*handler)(void), int mode)=0;
private:
	SPI *SPI1;
	SPI *SPI2;
};


#endif
