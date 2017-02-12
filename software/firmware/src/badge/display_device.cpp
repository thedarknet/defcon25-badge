#include "display_device.h"
#include "stm32f3xx_hal.h"
#include "spi.h"
#include "gpio.h"
#include "logger.h"
#include "assert.h"
#include <string.h>
#include <diag/Trace.h>

/////////////////////////////////////////////////////////////////////
// Generic base display device
DisplayDevice::DisplayDevice(uint16_t w, uint16_t h) :
		width(w), height(h) {
}

DisplayDevice::~DisplayDevice() {

}

//////////////////////////////////////////////////////////////////
// ST7735 based device
/*
 enum PIN_PORTS {
 OLED_RESET = OLED_RESET_Pin
 , OLED_PORT_RESET = OLED_RESET_GPIO_Port
 , OLED_BACK_LIT = OLED_BACK_LIT_Pin
 , OLED_PORT_BACK_LIT = OLED_BACK_LIT_GPIO_Port
 , OLED_DATA_CMD = OLED_DATA_CMD_Pin
 , OLED_PORT_DATA_CMD =OLED_DATA_CMD_GPIO_Port
 , OLED_CHIP_SELECT = OLED_SPI2_CS_Pin
 , OLED_PORT_CHIP_SELECT = OLED_SPI2_CS_GPIO_Port
 , OLED_CLK = OLED_SPI2_SCK_Pin
 , OLED_MOSI = OLED_SPI2_MOSI_Pin
 };
 */

DisplayST7735::DisplayST7735(uint16_t w, uint16_t h) :
		DisplayDevice(w, h), PixelFormat(0), MemoryAccessControl(0) {

}

DisplayST7735::~DisplayST7735() {

}

//#define MADCTLGRAPHICS 0x6
//#define MADCTLBMP      0x2
//#define ST7735_CASET 0x2A
///#define ST7735_RASET 0x2B
//#define ST7735_MADCTL 0x36
//#define ST7735_RAMWR 0x2C
//#define ST7735_RAMRD 0x2E
//#define ST7735_COLMOD 0x3A

//#define MADVAL(x) (((x) << 5) | 8)
//static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS);

struct sCmdBuf {
	uint8_t command;   // ST7735 command byte
	uint8_t delay;     // ms delay after
	uint8_t len;       // length of parameter data
	uint8_t data[16];  // parameter data
};


static const struct sCmdBuf initializers[] = {
		// SWRESET Software reset
		{ DisplayST7735::SWRESET, 150, 0, 0 },
		// SLPOUT Leave sleep mode
		{ DisplayST7735::SLEEP_OUT, 150, 0, 0 },
		// FRMCTR1, FRMCTR2 Frame Rate configuration -- Normal mode, idle
		// frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
		{ DisplayST7735::FRAME_RATE_CONTROL_FULL_COLOR, 0, 3, { 0x01, 0x2C, 0x2B } },
		{ DisplayST7735::FRAME_RATE_CONTROL_IDLE_COLOR, 0, 3, { 0x01, 0x2C, 0x2B } },
		// FRMCTR3 Frame Rate configureation -- partial mode
		{ DisplayST7735::FRAME_RATE_CONTROL_PARTIAL_FULL_COLOR, 0, 6, { 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D } },
		// INVCTR Display inversion (no inversion)
		{ DisplayST7735::DISPLAY_INVERSION_CONTROL, 0, 1, { 0x07 } },
		// PWCTR1 Power control -4.6V, Auto mode
		{ DisplayST7735::POWER_CONTROL_1, 0, 3, { 0xA2, 0x02, 0x84 } },
		// PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
		{ DisplayST7735::POWER_CONTROL_2, 0, 1, { 0xC5 } },
		// PWCTR3 Power control, opamp current smal, boost frequency
		{ DisplayST7735::POWER_CONTROL_3, 0, 2, { 0x0A, 0x00 } },
		// PWCTR4 Power control, BLK/2, opamp current small and medium low
		{ DisplayST7735::POWER_CONTROL_4, 0, 2, { 0x8A, 0x2A } },
		// PWRCTR5, VMCTR1 Power control
		{ DisplayST7735::POWER_CONTROL_5, 0, 2, { 0x8A, 0xEE } }, { 0xC5, 0, 1, { 0x0E } },
		// INVOFF Don't invert display
		{ DisplayST7735::DISPLAY_INVERSION_OFF, 0, 0, 0 },
		// Memory access directions. row address/col address, bottom to top refesh (10.1.27)
		{ DisplayST7735::MEMORY_DATA_ACCESS_CONTROL, 0, 1, { 0 } },
		// Color mode 18 bit (10.1.30
		//011 12 bit/pixel, 101 16 bit/pixel, 110 18 bit/pixel, 111 not used
		{ DisplayST7735::INTERFACE_PIXEL_FORMAT, 0, 1, { 0b101 } },
		// Column address set 0..127
		{ DisplayST7735::COLUMN_ADDRESS_SET, 0, 4, { 0x00, 0x00, 0x00, 0x7F } },
		// Row address set 0..159
		{ DisplayST7735::ROW_ADDRESS_SET, 0, 4, { 0x00, 0x00, 0x00, 0x9F } },
		// GMCTRP1 Gamma correction
		{ 0xE0, 0, 16,
				{ 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10 } },
		// GMCTRP2 Gamma Polarity corrction
		{ 0xE1, 0, 16,
				{ 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10 } },
		// DISPON Display on
		{ DisplayST7735::DISPLAY_ON, 100, 0, 0 },
		// NORON Normal on
		{ DisplayST7735::NORMAL_DISPLAY_MODE_ON, 10, 0, 0 },
		// End
		{ 0, 0, 0, 0 } };

bool DisplayST7735::writeCmd(uint8_t c) {
	return writeN(0, &c, sizeof(c));
}

bool DisplayST7735::writeNData(const uint8_t *data, int nbytes) {
	return writeN(1,data,nbytes);
}

bool DisplayST7735::writeN(char dc, const uint8_t *data, int nbytes) {
	if (dc == 1) { //dc 1=data 0 = control
		HAL_GPIO_WritePin(OLED_DATA_CMD_GPIO_Port, OLED_DATA_CMD_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(OLED_DATA_CMD_GPIO_Port, OLED_DATA_CMD_Pin, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(OLED_SPI2_CS_GPIO_Port, OLED_SPI2_CS_Pin, GPIO_PIN_RESET);
	if (HAL_OK != HAL_SPI_Transmit(&hspi2, const_cast<uint8_t*>(data), nbytes, 1000)) {
		return false;
	}
	HAL_GPIO_WritePin(OLED_SPI2_CS_GPIO_Port, OLED_SPI2_CS_Pin, GPIO_PIN_SET);
	return true;
}

bool DisplayST7735::write16Data(const uint16_t &data) {
	uint8_t buf[2];
	buf[0] = data>>8;
	buf[1] = data&0xFF;
	return writeN(1,&buf[0],sizeof(buf));
}

bool DisplayST7735::drawPixel(uint16_t x0, uint16_t y0, uint8_t r, uint8_t g, uint8_t b) {
	setAddrWindow(x0,y0,x0,y0);

	uint8_t data[3] = {0};
	uint8_t size = makeColor(r,g,b,data);
	writeCmd(MEMORY_WRITE);
	return writeNData(&data[0],size);
}

void DisplayST7735::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if((MemoryAccessControl&ROW_COLUMN_ORDER)==0) {
		writeCmd(COLUMN_ADDRESS_SET);
		write16Data(y0);
		write16Data(y1);

		writeCmd(ROW_ADDRESS_SET);
		write16Data(x0);
		write16Data(x1);

	} else {
		writeCmd(COLUMN_ADDRESS_SET);
		write16Data(x0);
		write16Data(x1);

		writeCmd(ROW_ADDRESS_SET);
		write16Data(y0);
		write16Data(y1);

	}
}

void ST7735_backLight(uint8_t on) {
	if (on)
		HAL_GPIO_WritePin(OLED_BACK_LIT_GPIO_Port, OLED_BACK_LIT_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(OLED_BACK_LIT_GPIO_Port, OLED_BACK_LIT_Pin, GPIO_PIN_SET);
}

uint8_t DisplayST7735::makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t out[3]) {
	uint16_t retVal = 0;
	switch(PixelFormat) {
	case FORMAT_12_BIT:
		break;
	case FORMAT_16_BIT:
		retVal = (r&0b11111)<<11;
		retVal |= (g&0b111111)<<5;
		retVal |= (b&0b11111);
		out[0] = retVal>>8;
		out[1] = retVal&0xFF;
		break;
	case FORMAT_18_BIT:
		out[0] = r<<2;
		out[1] = g<<2;
		out[2] = b<<2;
		break;
	default:
		assert(false);
	}
}

ErrorType DisplayST7735::init() {
	return init(FORMAT_16_BIT,ROW_COLUMN_ORDER);
}

ErrorType DisplayST7735::init(uint8_t pf, uint8_t madctl) {
	PixelFormat = pf;
	MemoryAccessControl = madctl;
	ErrorType et;

	//clear chip select
	HAL_GPIO_WritePin(OLED_SPI2_CS_GPIO_Port, OLED_SPI2_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

#if 0
	//serial interface, data packet contains just
	//transmission byte and control bit D/CX is transferred by the D/CX pin. If D/CX is “low”, the transmission byte is
	//interpreted as a command byte. If D/CX is “high”, the transmission byte is stored in the display data
	// RAM (memory write command), or command register as parameter.
	// Controlling reads = Page 29
	//
	unsigned char CMD = 0x1;
	//select chip
	HAL_GPIO_WritePin(OLED_SPI2_CS_GPIO_Port, OLED_SPI2_CS_Pin, GPIO_PIN_RESET);
	//make it a cmd (D/CX is low)
	HAL_GPIO_WritePin(OLED_DATA_CMD_GPIO_Port, OLED_DATA_CMD_Pin, GPIO_PIN_RESET);
	if(HAL_SPI_Transmit(&hspi2,&CMD,1,1000)==HAL_OK) {
		HAL_Delay(120);
		//still a command
		CMD=0x11;
		if(HAL_SPI_Transmit(&hspi2,&CMD,1,1000)==HAL_OK) {
			//still a command
			CMD=0x4;
			if(HAL_SPI_Transmit(&hspi2,&CMD,1,1000)==HAL_OK) {
				//HAL_GPIO_WritePin(OLED_SPI2_CS_GPIO_Port, OLED_SPI2_CS_Pin, GPIO_PIN_SET);
				//NOW release LCD to send data
				//HAL_GPIO_WritePin(OLED_DATA_CMD_GPIO_Port, OLED_DATA_CMD_Pin, GPIO_PIN_SET);
				//HAL_GPIO_WritePin(OLED_DATA_CMD_GPIO_Port, OLED_DATA_CMD_Pin, GPIO_PIN_RESET);
				unsigned char recBuf[1] = {100};
				HAL_SPI_Receive(&hspi2,&recBuf[0],1,500);
				HAL_GPIO_WritePin(OLED_SPI2_CS_GPIO_Port, OLED_SPI2_CS_Pin, GPIO_PIN_SET);
				darknet::LogManager::get();
				//trace_write((char*)&recBuf[0],3);
			}
		}
	}

#endif
	for (const sCmdBuf *cmd = initializers; cmd->command; cmd++) {
		writeCmd(cmd->command);
		if (cmd->len)
			writeNData(cmd->data, cmd->len);
		if (cmd->delay)
			HAL_Delay(cmd->delay);
	}

	uint16_t color = 0xFFFF;
	uint8_t hi = color >> 8, lo = color;
	uint16_t _width = 128;
	uint16_t _height = 160;
	uint16_t w = _width;
	uint16_t h = _height;
	uint16_t x = 0, y = 0;


	  // rudimentary clipping (drawChar w/big text requires this)
	  //if((x >= _width) || (y >= _height)) return;
	  if((x + w - 1) >= _width)  w = _width  - x;
	  if((y + h - 1) >= _height) h = _height - y;

	  setAddrWindow(0, 0, 127, 159);
	  writeCmd(MEMORY_WRITE);

	  for(y=h; y>0; y--) {
	    for(x=w; x>0; x--) {
	      writeNData(&hi,1);
	      writeNData(&lo,1);
	    }
	  }
	//for(int i=0;i<40;i++) {
	//	drawPixel(10+i,10+i,0xF,0,0);
	//}
	//for(int i=0;i<40;i++) {
	//	drawPixel(12+i,10,0xF,0,0);
	//}
	return et;
}

