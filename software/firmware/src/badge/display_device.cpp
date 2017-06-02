#include "display_device.h"
#include "stm32f3xx_hal.h"
#include "spi.h"
#include "gpio.h"
#include "logger.h"
#include "assert.h"
#include <string.h>
#include <diag/Trace.h>

const RGBColor RGBColor::BLACK(0, 0, 0);
const RGBColor RGBColor::RED(255, 0, 0);
const RGBColor RGBColor::GREEN(0, 255, 0);
const RGBColor RGBColor::BLUE(0, 0, 255);
const RGBColor RGBColor::WHITE(255, 255, 255);

bool RGBColor::operator==(const RGBColor &r) const {
	return (r.R == R && r.G == G && r.B == B);
}

bool RGBColor::operator!=(const RGBColor &r) const {
	return !((*this) == r);
}

/////////////////////////////////////////////////////////////////////
// Generic base display device
DisplayDevice::DisplayDevice(uint16_t w, uint16_t h, DisplayDevice::ROTATION r) :
		Width(w), Height(h), Rotation(r) {
}

DisplayDevice::~DisplayDevice() {

}

uint16_t DisplayDevice::getWidth() {
	return Width;
}
uint16_t DisplayDevice::getHeight() {
	return Height;
}
DisplayDevice::ROTATION DisplayDevice::getRotation() {
	return Rotation == 0 ? PORTAIT : LANDSCAPE;
}

///////////////////////////////////////////////////////////////////////////////////
DisplayST7735::PackedColor::PackedColor() :
		Color { 0 }, SizeInBytes(2) {
}

uint8_t *DisplayST7735::PackedColor::getPackedColorData() {
	return &Color[0];
}

uint8_t DisplayST7735::PackedColor::getSize() {
	return SizeInBytes;
}

///virtual ctor for converting RGB color into packed color class for the driver chip
DisplayST7735::PackedColor DisplayST7735::PackedColor::create(uint8_t pixelFormat, const RGBColor &c) {
	PackedColor pc;
	switch (pixelFormat) {
	case FORMAT_12_BIT:
		pc.SizeInBytes = 2;
		break;
	case FORMAT_16_BIT: {
		uint16_t tmp;
		tmp = (c.getR() & 0b11111) << 11;
		tmp |= (c.getG() & 0b111111) << 5;
		tmp |= (c.getB() & 0b11111);
		pc.Color[0] = tmp >> 8;
		pc.Color[1] = tmp & 0xFF;
		pc.SizeInBytes = 2;
	}
		break;
	case FORMAT_18_BIT:
		pc.Color[0] = c.getR() << 2;
		pc.Color[1] = c.getG() << 2;
		pc.Color[2] = c.getB() << 2;
		pc.SizeInBytes = 3;
		break;
	default:
		assert(false);
	}
	return pc;
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

DrawBufferNoBuffer::DrawBufferNoBuffer(DisplayST7735 *d, uint16_t *optimizedFillBuf, uint8_t rowsForDrawBuffer) :
		Display(d), SPIBuffer(optimizedFillBuf), RowsForDrawBuffer(rowsForDrawBuffer) {
}

bool DrawBufferNoBuffer::drawPixel(uint16_t x0, uint16_t y0, const RGBColor &color) {
	DisplayST7735::PackedColor pc = DisplayST7735::PackedColor::create(Display->getPixelFormat(), color);
	Display->setAddrWindow(x0, y0, x0, y0);
	Display->writeCmd(DisplayST7735::MEMORY_WRITE);
	return Display->writeNData(pc.getPackedColorData(), pc.getSize());
}

void DrawBufferNoBuffer::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor &color) {
	Display->setAddrWindow(x, y, w, h);
	Display->writeCmd(DisplayST7735::MEMORY_WRITE);
	DisplayST7735::PackedColor pc = DisplayST7735::PackedColor::create(Display->getPixelFormat(), color);

	uint16_t pcolor = *((uint16_t*) (pc.getPackedColorData()));
	uint16_t pixelCount = w * h;
	uint16_t maxAtOnce =
			pixelCount > (RowsForDrawBuffer * Display->getWidth()) ?
					(RowsForDrawBuffer * Display->getWidth()) : pixelCount;
	for (uint16_t i = 0; i < maxAtOnce; ++i) {
		SPIBuffer[i] = pcolor;
	}

	uint16_t pixelCopied = 0;
	do {
		Display->writeNData((uint8_t*) &SPIBuffer[0], maxAtOnce * sizeof(uint16_t));
		pixelCopied += maxAtOnce;

		if ((pixelCopied + maxAtOnce) > pixelCount) {
			maxAtOnce = pixelCount - pixelCopied;
		}
	} while (pixelCopied < pixelCount);
}

void DrawBufferNoBuffer::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor &color) {
	DisplayST7735::PackedColor pc = DisplayST7735::PackedColor::create(Display->getPixelFormat(), color);

	uint16_t pcolor = *((uint16_t*) (pc.getPackedColorData()));
	uint16_t pixelCount = h;
	uint16_t maxAtOnce =
			pixelCount > (RowsForDrawBuffer * Display->getWidth()) ?
					(RowsForDrawBuffer * Display->getWidth()) : pixelCount;
	for (uint16_t i = 0; i < maxAtOnce; ++i) {
		SPIBuffer[i] = pcolor;
	}

	uint16_t pixelCopied = 0;
	Display->setAddrWindow(x, y, x, y + h - 1);
	Display->writeCmd(DisplayST7735::MEMORY_WRITE);
	do {
		Display->writeNData((uint8_t*) &SPIBuffer[0], maxAtOnce * sizeof(uint16_t));
		pixelCopied += maxAtOnce;

		if ((pixelCopied + maxAtOnce) > pixelCount) {
			maxAtOnce = pixelCount - pixelCopied;
		}
	} while (pixelCopied < pixelCount);
}

void DrawBufferNoBuffer::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
	DisplayST7735::PackedColor pc = DisplayST7735::PackedColor::create(Display->getPixelFormat(), color);

	uint16_t pcolor = *((uint16_t*) (pc.getPackedColorData()));
	for (uint16_t i = 0; i < w; ++i) {
		SPIBuffer[i] = pcolor;
	}

	Display->setAddrWindow(x, y, x + w - 1, y);
	Display->writeCmd(DisplayST7735::MEMORY_WRITE);
	Display->writeNData((uint8_t*) &SPIBuffer[0], w * sizeof(uint16_t));
}

void DrawBufferNoBuffer::swap() {
	//do nothing
}

DrawBufferNoBuffer::~DrawBufferNoBuffer() {

}

DrawBuffer2D16BitColor::DrawBuffer2D16BitColor(uint8_t w, uint8_t h, uint8_t *backBuffer, uint16_t *spiBuffer,
		uint8_t rowsForDrawBuffer, DisplayST7735 *d) :
		Width(w), Height(h), BufferSize(w * h), BackBuffer(backBuffer), SPIBuffer(spiBuffer), RowsForDrawBuffer(
				rowsForDrawBuffer), DrawBlocksChanged(0), Display(d) {
}

DrawBuffer2D16BitColor::~DrawBuffer2D16BitColor() {
}

bool DrawBuffer2D16BitColor::drawPixel(uint16_t x, uint16_t y, const RGBColor &color) {
	uint8_t c = deresColor(color);
	BackBuffer[(y * Width) + x] = c;
	DrawBlocksChanged |= (1 << (y / RowsForDrawBuffer));
	return true;
}

void DrawBuffer2D16BitColor::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor &color) {
	uint8_t c = deresColor(color);
	for (int i = y; i < (h + y); ++i) {
		memset(&BackBuffer[(i * Display->getWidth()) + x], c, w);
		DrawBlocksChanged |= (1 << (i / RowsForDrawBuffer));
	}
}

void DrawBuffer2D16BitColor::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor &color) {
	uint8_t c = deresColor(color);
	for (int i = y; i < (h + y); ++i) {
		BackBuffer[(i * Display->getWidth()) + x] = c;
		DrawBlocksChanged |= (1 << (i / RowsForDrawBuffer));
	}
}

void DrawBuffer2D16BitColor::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
	uint8_t c = deresColor(color);
	for(int i=x;i<(x+w);++i) {
		BackBuffer[(y*Display->getWidth())+i] = c;
	}
	DrawBlocksChanged|=(1<<(y/RowsForDrawBuffer));
}

//////
// first check to see if we changed anything in the draw block, if not skip it
// if we did change something convert from our short hand notation to something the LCD will understand
//	then send to LCD
void DrawBuffer2D16BitColor::swap() {
	for (int h = 0; h < Height; h++) {
		if ((DrawBlocksChanged & (1 << (h / RowsForDrawBuffer))) != 0) {
			for (int w = 0; w < Width; w++) {
				uint32_t SPIY = h % RowsForDrawBuffer;
				SPIBuffer[(SPIY * Width) + w] = calcLCDColor(BackBuffer[(h * Width) + w]);
			}
			if (h != 0 && h % RowsForDrawBuffer == 0) {
				Display->setAddrWindow(0, h - RowsForDrawBuffer, Width, h);
				Display->writeCmd(DisplayST7735::MEMORY_WRITE);
				Display->writeNData((uint8_t*) &SPIBuffer[0], sizeof(SPIBuffer));
			}
		}
	}
	DrawBlocksChanged = 0;
}

uint16_t DrawBuffer2D16BitColor::calcLCDColor(uint8_t packedColor) {
	uint32_t rc = packedColor & RED_MASK >> 5;
	uint32_t gc = packedColor & GREEN_MASK >> 3;
	uint32_t bc = packedColor & BLUE_MASK;
	RGBColor lcdColor(rc * 36, gc * 85, bc * 36);
	return uint16_t(*DisplayST7735::PackedColor::create(Display->getPixelFormat(), lcdColor).getPackedColorData());
}
uint8_t DrawBuffer2D16BitColor::deresColor(const RGBColor &color) {
	uint32_t retVal = 0;
	retVal = (color.getR() / 36) << 5; //3 bits = 7, 36 is ~1/7 of 255
	retVal |= (color.getG() / 85) << 3;
	retVal |= (color.getB() / 36);
	return retVal;
}

////////////////////////////////////////////////////////

DisplayST7735::DisplayST7735(uint16_t w, uint16_t h, DisplayST7735::ROTATION r) :
		DisplayDevice(w, h, r), PixelFormat(0), MemoryAccessControl(0), CurrentTextColor(RGBColor::WHITE), CurrentBGColor(
				RGBColor::BLACK), CurrentFont(0) {

}

DisplayST7735::~DisplayST7735() {

}

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
		{ DisplayST7735::FRAME_RATE_CONTROL_FULL_COLOR, 0, 3, { 0x01, 0x2C, 0x2B } }, {
				DisplayST7735::FRAME_RATE_CONTROL_IDLE_COLOR, 0, 3, { 0x01, 0x2C, 0x2B } },
		// FRMCTR3 Frame Rate configuration -- partial mode
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
		{ DisplayST7735::MEMORY_DATA_ACCESS_CONTROL, 0, 1, { DisplayST7735::VERTICAL_REFRESH_ORDER } },
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
		// GMCTRP2 Gamma Polarity correction
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
	return writeN(1, data, nbytes);
}

bool DisplayST7735::writeN(char dc, const uint8_t *data, int nbytes) {
	if (dc == 1) { //dc 1=data 0 = control
		HAL_GPIO_WritePin(LCD_DATA_CMD_GPIO_Port, LCD_DATA_CMD_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_DATA_CMD_GPIO_Port, LCD_DATA_CMD_Pin, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	if (HAL_OK != HAL_SPI_Transmit(&hspi2, const_cast<uint8_t*>(data), nbytes, 1000)) {
		return false;
	}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	return true;
}

bool DisplayST7735::write16Data(const uint16_t &data) {
	uint8_t buf[2];
	buf[0] = data >> 8;
	buf[1] = data & 0xFF;
	return writeN(1, &buf[0], sizeof(buf));
}

void DisplayST7735::swap() {
	FB->swap();
}

bool DisplayST7735::drawPixel(uint16_t x0, uint16_t y0, const RGBColor &color) {
#if 0
	PackedColor pc = PackedColor::create(PixelFormat, color);
	setAddrWindow(x0, y0, x0, y0);
	writeCmd(MEMORY_WRITE);
	return writeNData(pc.getPackedColorData(), pc.getSize());
#else
	return FB->drawPixel(x0, y0, color);
#endif
}

void DisplayST7735::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if ((MemoryAccessControl & ROW_COLUMN_ORDER) == 0) {
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

void DisplayST7735::setBackLightOn(bool on) {
	if (on)
		HAL_GPIO_WritePin(LCD_BACK_LIT_GPIO_Port, LCD_BACK_LIT_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_BACK_LIT_GPIO_Port, LCD_BACK_LIT_Pin, GPIO_PIN_RESET);
}

DisplayST7735::PackedColor DisplayST7735::makeColor(const RGBColor &rgb) {
	return PackedColor::create(PixelFormat, rgb);
}

void DisplayST7735::setMemoryAccessControl(uint8_t macctl) {
	if (macctl != MemoryAccessControl) {
		MemoryAccessControl = macctl;
		writeCmd(MEMORY_DATA_ACCESS_CONTROL);
		writeNData(&MemoryAccessControl, 1);
	}
}

void DisplayST7735::setPixelFormat(uint8_t pf) {
	if (PixelFormat != pf) {
		PixelFormat = pf;
		writeCmd(INTERFACE_PIXEL_FORMAT);
		writeNData(&pf, 1);
	}
}

void DisplayST7735::setFont(const FontDef_t *font) {
	CurrentFont = font;
}

ErrorType DisplayST7735::init(uint8_t pf, uint8_t madctl, const FontDef_t *defaultFont, DisplayST7735::FrameBuf *fb) {
	ErrorType et;
	setFrameBuffer(fb);
	setFont(defaultFont);
	setBackLightOn(true);
	//ensure pixel format
	setPixelFormat(pf);
	//ensure memory access control format
	setMemoryAccessControl(madctl);

	//clear chip select
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

#if 0
	//serial interface, data packet contains just
	//transmission byte and control bit D/CX is transferred by the D/CX pin. If D/CX is "low", the transmission byte is
	//interpreted as a command byte. If D/CX is "high", the transmission byte is stored in the display data
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

	fillScreen(RGBColor::BLACK);
	return et;
}

void DisplayST7735::fillScreen(const RGBColor &color) {
	fillRec(0, 0, getWidth(), getHeight(), color);
}

// Draw a filled rectangle at the given coordinates with the given width, height, and color.
// Input: x     horizontal position of the top left corner of the rectangle, columns from the left edge
//        y     vertical position of the top left corner of the rectangle, rows from the top edge
//        w     horizontal width of the rectangle
//        h     vertical height of the rectangle
//        color appropriated packed color, which can be produced by PackColor::create()
// Output: none
void DisplayST7735::fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor &color) {
	//PackedColor pc = PackedColor::create(PixelFormat, color);

	if ((x >= getWidth()) || (y >= getHeight()))
		return;
	if ((x + w - 1) >= getWidth())
		w = getWidth() - x;
	if ((y + h - 1) >= getHeight())
		h = getHeight() - y;

	getFrameBuffer()->fillRec(x, y, w, h, color);
}

void DisplayST7735::drawRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor &color) {
	drawHorizontalLine(x, y, w, color);
	drawVerticalLine(x, y, h, color);
	drawHorizontalLine(x, y + h >= getHeight() ? getHeight() - 1 : y + h, w, color);
	drawVerticalLine(x + w, y, h, color);
}

const uint8_t *DisplayST7735::getFontData() {
	return CurrentFont->data;
}

// Input: x         horizontal position of the top left corner of the character, columns from the left edge
//        y         vertical position of the top left corner of the character, rows from the top edge
//        c         character to be printed
//        textColor 16-bit color of the character
//        bgColor   16-bit color of the background
//        size      number of pixels per character pixel (e.g. size==2 prints each pixel of font as 2x2 square)
// Output: none
void DisplayST7735::drawCharAtPosition(int16_t x, int16_t y, char c, const RGBColor &textColor, const RGBColor &bgColor,
		uint8_t size) {
	uint8_t line; // vertical column of pixels of character in font
	int32_t i, j;
	if ((x >= getWidth()) || // Clip right
			(y >= getHeight()) || // Clip bottom
			((x + 5 * size - 1) < 0) || // Clip left
			((y + 8 * size - 1) < 0))   // Clip top
		return;

	for (i = 0; i < CurrentFont->FontWidth; i++) {
		if (i == CurrentFont->FontWidth - 1)
			line = 0x0;
		else
			line = getFontData()[(c * CurrentFont->CharBytes) + i];
		for (j = 0; j < 8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
					drawPixel(x + i, y + j, textColor);
				else {  // big size
					fillRec(x + (i * size), y + (j * size), size, size, textColor);
				}
			} else if (bgColor != textColor) {
				if (size == 1) // default size
					drawPixel(x + i, y + j, bgColor);
				else {  // big size
					fillRec(x + i * size, y + j * size, size, size, bgColor);
				}
			}
			line >>= 1;
		}
	}
}

void DisplayST7735::setTextColor(const RGBColor &t) {
	CurrentTextColor = t;
}

void DisplayST7735::setBackgroundColor(const RGBColor &t) {
	CurrentBGColor = t;
}

const RGBColor &DisplayST7735::getTextColor() {
	return CurrentTextColor;
}

const RGBColor &DisplayST7735::getBackgroundColor() {
	return CurrentBGColor;
}

uint32_t DisplayST7735::drawStringOnLine(uint8_t line, const char *msg) {
	return drawString(0, getFont()->FontHeight * line, msg, RGBColor::WHITE, RGBColor::BLACK, 1, true);
}

uint32_t DisplayST7735::drawString(uint16_t x, uint16_t y, const char *pt) {
	return drawString(x, y, pt, CurrentTextColor);
}

uint32_t DisplayST7735::drawString(uint16_t x, uint16_t y, const char *pt, const RGBColor &textColor) {
	return drawString(x, y, pt, textColor, CurrentBGColor, 1, false);
}

uint32_t DisplayST7735::drawString(uint16_t xPos, uint16_t yPos, const char *pt, const RGBColor &textColor,
		const RGBColor &backGroundColor, uint8_t size, bool lineWrap) {
#if 1
	uint16_t currentX = xPos;
	uint16_t currentY = yPos;
	const char *orig = pt;

	while (*pt) {
		if ((currentX > getWidth() && !lineWrap) || currentY > getHeight()) {
			return pt - orig;
		} else if (currentX > getWidth() && lineWrap) {
			currentX = 0;
			currentY += CurrentFont->FontHeight * size;
			drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
			currentX += CurrentFont->FontWidth;
		} else if (*pt == '\n' || *pt == '\r') {
			currentY += CurrentFont->FontHeight * size;
			currentX = 0;
		} else {
			drawCharAtPosition(currentX, currentY, *pt, textColor, backGroundColor, size);
			currentX += CurrentFont->FontWidth * size;
		}
		pt++;
	}
	return (pt - orig);  // number of characters printed
#else
	uint32_t count = 0;
	//TODO clean this up
	if (yPos > 15)//15*10 = 150
	return 0;

	uint16_t w = 6 * size;
	uint16_t h = 10 * size;

	while (*pt) {
		drawCharAtPosition(xPos * w, yPos * h, *pt, textColor, backGroundColor, size);
		pt++;
		xPos = xPos + 1;
		if (xPos > 20)
		return count;
		count++;
	}
	return count;  // number of characters printed
#endif
}

void DisplayST7735::drawVerticalLine(int16_t x, int16_t y, int16_t h) {
	drawVerticalLine(x, y, h, CurrentTextColor);
}

// Draw a vertical line at the given coordinates with the given height and color.
// A vertical line is parallel to the longer side of the rectangular display
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        h     vertical height of the line
//		color	RGB color of line
void DisplayST7735::drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor &color) {
	// safety
	if ((x >= getWidth()) || (y >= getHeight()))
		return;
	if ((y + h - 1) >= getHeight())
		h = getHeight() - y;
	getFrameBuffer()->drawVerticalLine(x, y, h, color);
}

void DisplayST7735::drawHorizontalLine(int16_t x, int16_t y, int16_t w) {
	return drawHorizontalLine(x, y, w, CurrentTextColor);
}

// Draw a horizontal line at the given coordinates with the given width and color.
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        w     horizontal width of the line
//		Color is the RGBColor
void DisplayST7735::drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor& color) {
	//safey
	if ((x >= getWidth()) || (y >= getHeight()))
		return;
	if ((x + w - 1) >= getWidth())
		w = getWidth() - x;

	getFrameBuffer()->drawHorizontalLine(x, y, w, color);
}

