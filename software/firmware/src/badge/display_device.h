#ifndef DCDARKNET_DISPLAY_DEVICE_H
#define DCDARKNET_DISPLAY_DEVICE_H

#include "error_type.h"

/*
 * @Author cmdc0de
 * @date:  2/13/17
 *
 * Convient RGB color class
 */
class RGBColor {
public:
	static const RGBColor BLACK;
	static const RGBColor RED;
	static const RGBColor GREEN;
	static const RGBColor BLUE;
	static const RGBColor WHITE;
public:
	RGBColor(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {}
	uint16_t getR() const {return R;}
	uint16_t getG() const {return G;}
	uint16_t getB() const {return B;}
	bool operator==(const RGBColor &r) const;
	bool operator!=(const RGBColor &r) const;
private:
	uint8_t R,G,B;
};

/*
 * @author cmdc0de
 * @date:  2/13/17
 *
 * Base class for display devices
 * 	not sure this is actually needed as you create an instance of the Display7735 class not this
 */
class DisplayDevice {
public:
	enum ROTATION {
		PORTAIT = 0,
		LANDSCAPE = 1
	};
public:
	DisplayDevice(uint16_t w, uint16_t h, ROTATION r);
	ErrorType init();
	virtual ~DisplayDevice();
public:
	uint16_t getWidth();
	uint16_t getHeight();
	ROTATION getRotation();
protected:
private:
	uint16_t Width;
	uint16_t Height;
	uint32_t Rotation : 1;
};

/*
 * @author cmdc0de
 *
 * Big thank you to adafruit as the interface of this st7735 driver was inspired by their GFX libraries.
 */
class DisplayST7735 : public DisplayDevice {
protected:
	///Used to convert from the RGB Color class to something the driver chip understands
	class PackedColor {
	public:
		PackedColor();
		uint8_t *getPackedColorData();
		uint8_t getSize();
	public:
		static PackedColor create(uint8_t pixelFormat, const RGBColor &c);
	private:
		uint8_t Color[3];
		uint8_t SizeInBytes;
	};

public:
	///parameter to Memory Data access control command
	enum MEMORY_DATA_ACCESS_CONTROL_BITS {
		BOT_TO_TOP 					= 0b1000000 //Bottom to Top (When MADCTL B7=1), 0 = top to bottom
		, COLUMN_ORDER 				= 0b01000000 //right to left, 0 = left to right
		, ROW_COLUMN_ORDER 			= 0b00100000 //Row then column, 0 = column then row
		, VERTICAL_REFRESH_ORDER 	= 0b00010000 //lcd referesh bot to top, 0 = top to bot
		, RGB_ORDER					= 0b00001000 // 1 = BGR, 0 = RGB
		, LCD_HORIZONTAL_REFERESH	= 0b00000100 // 1 = right to left, 0 = left to right
		};
	///enumeration of pixel formats for interface pixel command
	enum PIXEL_FORMAT {
		FORMAT_12_BIT 		= 0b011
		, FORMAT_16_BIT 	= 0b101
		, FORMAT_18_BIT 	= 0b110
	};
	///all LCD commands
	enum LCD_COMMANDS {
		//No OP
		NOP = 0x0
		//If Software Reset is applied during Sleep In mode, it will be necessary to wait 120msec before sending next command
		//The display module loads all default values to the registers during 120msec
		//If Software Reset is applied during Sleep Out or Display On Mode, it will be necessary to wait 120msec before sending next command
		, SWRESET = 0x1
		//	This read byte returns 24-bit display identification information.
		//-The 1st parameter is dummy data
		//-The 2nd parameter (ID17 to ID10): LCD modules manufacturer ID.
		//-The 3rd parameter (ID26 to ID20): LCD module/driver version ID
		//-The 4th parameter (ID37 to UD30): LCD module/driver ID.
		//-Commands RDID1/2/3(DAh, DBh, DCh) read data correspond to the parameters 2,3,4 of the command 04h, respectively.
		, READ_DISPLAY_ID = 0x4
		//
		//
		//
		, READ_DISPLAY_STATUS = 0x9
		//  Byte 1:
		//		DUMMY
		//	Byte 2:
		//		Bit 7;
		//			Booster Voltage Status: 1 = on, 0 = off
		//		Bit 6:
		//			Idle Mode On/Off: 1= on, 0 = off
		//		Bit 5:
		//			Partial Mode On/Off: 1= on, 0 = off
		//		Bit 4:
		//			Sleep In/Out:  1= on, 0 = off
		//		Bit 3:
		//			Display Normal Mode On/Off: 1= on, 0 = off
		//		Bit 2:
		//			Display On/Off:  1= on, 0 = off
		, READ_DISPLAY_POWER_MODE = 0xa
		/*
		Bit Description
			Bit7: (MY) (Row order)
				1 = Bottom to Top (When MADCTL B7=1)
				0 = Top to Bottom (When MADCTL B7=0)
			Bit6: (MX) (column order)
				1 = Right to Left (When MADCTL B6=1)
				0 = Left to Right (When MADCTL B6=0)
			Bit5: Row/Column Order
				1 = Row/column exchange (MV=1)
				0 = Normal (MV=0)
			Bit4: Vertical Refresh order
				1 =LCD Refresh Bottom to Top
				0 =LCD Refresh Top to Bottom
			Bit3: RGB order
				1 =BGR, 0=RGB
			Bit2: LCD horizontal refresh direction control
				1 = LCD horizontal refresh Left to right
				0 = LCD horizontal refresh right to left
			Bit1:
				Not Used
			Bit0:
				Not Used
		 */
		//Read Display MADCTL
		, READ_DISPLAY_MEMORY_ACCESS_CONTROL = 0xb
		//Read Display Pixel Format
		// Bits2-0
		// 	011 = 12 bits / pixel
		//	101 = 16 bits / pixel
		//  110 = 18 bits / pixel
		// Default on hardware reset is 18 bits / pixel, does not change from last set on software reset
		, READ_DISPLAY_PIXEL_FORMAT = 0xc
		// Read Display Image Mode
		//	Bit 5: Inversion On/Off
		//		Inversion On/Off: 1 = on, 0 = off
		//	bit 2-0: Gamma Curve Selection
		//		000 = GC0, 001 = GC1, 010 = GC2, 011 = GC3, 100 to 111 = Not defined
		, READ_DISPLAY_IMAGE_MODE = 0xd
		//Read Display Signal Mode
		//	bit 7:
		//		Tearing Effect Line On/Off: 1 = on, 0 = off
		//	bit 6:
		//		Tearing effect line mode: 1 = mode 2, 0 = mode 1
		, READ_DISPLAY_SIGNAL_MODE = 0xe
		//Sleep In
		//	Sleep In Mode can only be exit by the Sleep Out
		//	When IC is in Sleep Out or Display On mode, it is necessary to wait 120msec
		//		This is the mode the display is started in, in any reset mode
		, SLEEP_IN = 0x10
		//Sleep Out
		//  delay for 120ms after command sent
		, SLEEP_OUT = 0x11
		//Partial Display Mode On
		// No idea why you would use this
		, PARTIAL_DISPLAY_MODE_ON = 0x12
		// Normal Display Mode On
		//Normal display mode on means Partial mode off.
		, NORMAL_DISPLAY_MODE_ON = 0x13
		// Display Inversion Off
		, DISPLAY_INVERSION_OFF = 0x20
		// Display Inversion On
		, DISPLAY_INVERSION_ON = 0x21
		// Gamma Set
		//	Byte 1: â€œ000â€� = GC0, â€œ001â€� = GC1, â€œ010â€� = GC2, â€œ011â€� = GC3, â€�100â€� to â€œ111â€� = Not defined
		, GAMMA_SET = 0x26
		//Display Off, exit by sending display_on
		, DISPLAY_OFF = 0x28
		// display on
		, DISPLAY_ON = 0x29
		// Column Address Set
		//	Byte 1: X Start: Hi Byte
		//	Byte 2: X Start: Lo Byte
		//	Byte 3: X End: Hi Byte
		//	Byte 4: X End: Lo Byte
		, COLUMN_ADDRESS_SET = 0x2a
		//Row Address Set
		//	Byte 1: Y Start = Hi Byte
		//	Byte 2: Y Start: Lo Byte
		//	Byte 3: Y End: Hi Byte
		//	Byte 4: Y End: Lo Byte
		, ROW_ADDRESS_SET = 0x2b
		// Memory Write
		//	128x160x18-bit memory can be written by this command  Memory range: (0000h, 0000h) -> (007Fh, 09Fh)
		, MEMORY_WRITE = 0x2c
		// Color Setting for 4K, 65K and 262K
		//	This command is used to define the LUT for 12bits-to-16bits / 16-bit-to- 18bits color depth conversations.
		//	128-Bytes must be written to the LUT regardless of the color mode. Only the values in Section 9.18 are referred.
		//	In this condition, 4K-color (4-4-4) and 65K-color(5-6-5) data input are transferred 6(R)-6(G)-6(B) through RGB LUT table
		, COLOLR_SETTING = 0x2d
		// Memory Read
		//	This command is used to transfer data from frame memory to MCU
		//	When this command is accepted, the column register and the row register are reset to the Start Column/Start
		//		Row positions.
		//	The Start Column/Start Row positions are different in accordance with MADCTL setting.
		//	Then D[17:0] is read back from the frame memory and the column register and the row register
		//	Frame Read can be cancelled by sending any other command.
		//	The data color coding is fixed to 18-bit in reading function. Please see section 9.8 â€œData color codingâ€� for color
		//	coding (18-bit cases), when there is used 8, 9, 16 and 18-bit data lines for image data.
		, MEMORY_READ = 0x2e
		// Partial Area
		//	Byte 1: X Start: Hi Byte
		//	Byte 2: X Start: Lo Byte
		//	Byte 3: X End: Hi Byte
		//	Byte 4: X End: Lo Byte
		, PARIAL_AREA = 0x30
		//Tearing Effect Line OFF
		//	This command is used to turn OFF (Active Low) the Tearing Effect output signal from the TE signal line
		, TEARING_EFFORT_LINE_OFF = 0x34
		// Tearing Effect Line ON
		, TEARING_EFFORT_LINE_ON = 0x35
		// Memory Data Access Control (MADCTL)
		/*
		 * 	Bit7: (MY) (Row order)
				1 = Bottom to Top (When MADCTL B7=1)
				0 = Top to Bottom (When MADCTL B7=0)
			Bit6: (MX) (column order)
				1 = Right to Left (When MADCTL B6=1)
				0 = Left to Right (When MADCTL B6=0)
			Bit5: Row/Column Order
				1 = Row/column exchange (MV=1)
				0 = Normal (MV=0)
			Bit4: Vertical Refresh order
				1 =LCD Refresh Bottom to Top
				0 =LCD Refresh Top to Bottom
			Bit3: RGB order
				1 =BGR, 0=RGB
			Bit2: LCD horizontal refresh direction control
				0 = LCD horizontal refresh Left to right
				1 = LCD horizontal refresh right to left
			Bit1:
				Not Used
			Bit0:
				Not Used
		Section 10.1.27
		*/
		, MEMORY_DATA_ACCESS_CONTROL = 0x36
		//Idle Mode Off
		//	This command is used to recover from Idle mode on
		, IDLE_MODE_OFF = 0x38
		// Idle Mode On
		//	This command is used to enter into Idle mode on.
		//	There will be no abnormal visible effect on the display mode change transition.
		//	In the idle on mode,
		//		1. Color expression is reduced. The primary and the secondary colors using MSB of each R,G and B in the Frame
		//		Memory, 8 color depth data is displayed.
		//		2. 8-Color mode frame frequency is applied.
		//		3. Exit from IDMON by Idle Mode Off (38h) command
		, IDLE_MODE_ON = 0x39
		//Interface Pixel Format
		// 	Bit 7-3: 0
		//	Bit 2-0: 011 12 bit/pixel, 101 16 bit/pixel, 110 18 bit/pixel, 111 not used
		, INTERFACE_PIXEL_FORMAT = 0x3a
		// Read ID1 Value
		//	Byte 1: dummy
		//	Byte 2: LCD moduleâ€™s manufacturer ID. Bits 17:10
		, READ_ID1 = 0xda
		// Read ID2 Value
		//	Byte 1: dummy
		//	Byte 2:
		//		Bit 7: ignore:
		//		Bites 6-0:  LCD moduleâ€™s manufacturer ID. Bits 26-20
		, READ_ID2 = 0xdb
		// Read ID3 Value
		//	Byte 1: dummy
		//	Byte 2:
		//		Bites 6-0:  LCD moduleâ€™s manufacturer ID. Bits 37-30
		, READ_ID3 = 0xdc
		// Frame Rate Control: In normal mode/ Full colors
		//	Byte 1:
		//		Bit3-0: RTNA
		//	Byte 2:
		//		Bit5-0: FPA
		//	Byte 3:
		//		Bit5-0: BPA
		// Frame rate = Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA))
		// fosc = 625kHz
		, FRAME_RATE_CONTROL_FULL_COLOR = 0xb1
		// Frame Rate Control (In Idle mode/ 8-colors)
		//	same params as FRAME_RATE_CONTROL_FULL_COLOR
		, FRAME_RATE_CONTROL_IDLE_COLOR = 0xb2
		// Frame Rate Control (In Partial mode/ full colors)
		//	Set the frame frequency of the Partial mode/ full colors.
		// 	- 1st parameter to 3rd parameter are used in dot inversion mode.
		//		params same as FRAME_RATE_CONTROL_FULL_COLOR
		//	- 4th parameter to 6th parameter are used in line inversion mode.
		//		params saem as FRAME_RATE_CONTROL_FULL_COLOR
		, FRAME_RATE_CONTROL_PARTIAL_FULL_COLOR = 0xb3
		//  Display Inversion Control
		//	Bit 2: Inversion setting in full Colors normal mode
		//		0 = dot inversion, 1 = line inversion
		//	Bit 1: Inversion setting in Idle mode
		//		0 = dot inversion, 1 = line inversion
		//	Bit 0: Inversion setting in full Colors partial mode
		//		0 = dot inversion, 1 = line inversion
		, DISPLAY_INVERSION_CONTROL = 0xb4
		// Power control 1
		//	Byte 1: AVDD voltage
		//	Byte 2: GVDD voltage
		//  Byte 3: MODE = 00 = 2x, 01 = 3x, 10 = auto, 11 = 3x
		, POWER_CONTROL_1 = 0xc0
		// power control 2
		, POWER_CONTROL_2 = 0xc1
		// power control 3
		//	Set the amount of current in Operational amplifier in normal mode/full colors
		//	byte 1: AP & SAO
		//  byte 2: Set the Booster circuit Step-up cycle in Normal mode/ full colors.
		, POWER_CONTROL_3 = 0xc2
		// power control 4
		//	Set the amount of current in Operational amplifier in Idle mode/8 colors
		, POWER_CONTROL_4 = 0xc3
		// power control 5
		//	Set the amount of current in Operational amplifier in partial / full color
		, POWER_CONTROL_5 = 0xc4
		//VCOM voltage
		, VCOM_1 = 0xc5
		// VCOM_2 - can reduce flicker
		, VCOM_2 = 0xc7
		//Write ID2 Value
		//	allows you to write LCD Module ID...hrm
		//	Byte 1: bits 6-0: bits 6-0 of id2
		, WRITE_ID2 = 0xd1
		// Write ID3 Value
		//	allows you to write LCD Module ID
		//	Byte 1: bites7-0: bites 7-0 for id3
		, WRITE_ID3 = 0xd2
		// NVM Control Status
		// bit 6: VMF_EN 1 = Command C7h enable ; 0 = Command C7h disable
		// bit 5: ID2_EN 1 = Command D1h enable ; 0 = Command D1h disable
		// bit 0: EXT_R Read: extension command status, 1 for enable, 0 for disable.
		, NVM_CONTROL_STATAUS = 0xd9
		// NVM Read Command
		// 	reads 2 bytes
		, NVM_READ_COMMAND = 0xde
		//NVM Write Command
		//	2 bytes:
		//		1 = NVM_CMD[7:0] : Select to Program/Erase ; Program command : 3Ah ; Erase command : C5h
		//		2 = A5 (must be)
		, NVM_WRTIE_COMMAND = 0xdf
		// Gamma (+ polarity) Correction Characteristics Setting
		//	10.2.17
		, GAMMA_POS_CONTROL = 0xe0
		// Gamma - polarity Correction Characteristics Setting
		//	10.2.18
		, GAMMA_NEG_CONTROL = 0xe1
		//
	};
public:
	DisplayST7735(uint16_t w, uint16_t h, ROTATION r);
	virtual ErrorType init();
	ErrorType init(uint8_t pf, uint8_t mac);
	virtual ~DisplayST7735();
	bool drawPixel(uint16_t x0, uint16_t y0, const RGBColor &color);
	void setMemoryAccessControl(uint8_t macctl);
	void fillRec(int16_t x, int16_t y, int16_t w, int16_t h, const RGBColor &color);
	void fillScreen(const RGBColor &color);
	void setPixelFormat(uint8_t pf);
	void drawVerticalLine(int16_t x, int16_t y, int16_t h);
	void drawVerticalLine(int16_t x, int16_t y, int16_t h, const RGBColor &color);
	void drawHorizontalLine(int16_t x, int16_t y, int16_t w);
	void drawHorizontalLine(int16_t x, int16_t y, int16_t w, const RGBColor &color);
	uint32_t drawString(uint16_t x, uint16_t y, char *pt);
	uint32_t drawString(uint16_t x, uint16_t y, char *pt, const RGBColor &textColor);
	uint32_t drawString(uint16_t x, uint16_t y, char *pt, const RGBColor &textColor, const RGBColor &bgColor, uint8_t size);
	void drawCharAtPosition(int16_t x, int16_t y, char c, const RGBColor &textColor, const RGBColor &bgColor, uint8_t size);
	void setTextColor(const RGBColor &t);
	void setBackgroundColor(const RGBColor &t);
	const RGBColor &getTextColor();
	const RGBColor &getBackgroundColor();
	void setBackLightOn(bool on);
protected:
	bool writeCmd(uint8_t c);
	bool writeNData(const uint8_t *data, int nbytes);
	bool write16Data(const uint16_t &data);
	void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	bool writeN(char dc, const uint8_t *data, int nbytes);
	PackedColor makeColor(const RGBColor &c);
private:
	uint8_t 	PixelFormat;
	uint8_t  	MemoryAccessControl;
	RGBColor	CurrentTextColor;
	RGBColor	CurrentBGColor;
};

#endif
