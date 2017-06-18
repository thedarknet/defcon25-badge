#ifndef  KEYBOARD_H
#define KEYBOARD_H

#include <stm32f3xx_hal.h>

class KeyBoardLetterCtx {
private:
	int16_t CursorPosition :10;
	int16_t Started :1;
	int16_t UnderBar :1;
	char *Buffer;
	char CurrentLetter;
	uint16_t BufferSize;
	uint32_t LastTimeLetterWasPushed;
	uint8_t LetterSelection;
	uint32_t LastBlinkTime;
	uint8_t LastPin;
	public:
	void processButtonPush(uint8_t button, const char *buttonLetters);
	bool isKeySelectionTimedOut();
	void timerStart();
	void timerStop();
	void incPosition();
	void decPosition();
	uint8_t getCursorPosition();
	void setCurrentLetterInBufferAndInc();
	void blinkLetter();
	KeyBoardLetterCtx();
	void resetChar();
	void finalize();
	void init(char *b, uint16_t s);
};

KeyBoardLetterCtx &getKeyboardContext();
class DialerMode;

class QKeyboard {
public:
	struct PinConfig {
		uint32_t Pin;
		uint32_t SamplePin;
		uint32_t GroupIndex;
		PinConfig() :
				Pin(0), SamplePin(0), GroupIndex(0) {
		}
		PinConfig(uint32_t pin, uint32_t sample, uint32_t gi) :
				Pin(pin), SamplePin(sample), GroupIndex(gi) {
		}
	};
	static const uint8_t NO_PIN_SELECTED = 0xFF;
	static const uint8_t NOT_A_NUMBER = 0xFF;
	static const uint8_t NO_LETTER_SELECTED = 0xFF;
	static const uint8_t TIMES_BUTTON_MUST_BE_HELD = 2;
	enum KEYS {
		ONE = 0
		, TWO = 1
		, THREE = 2
		, FOUR = 3
		, FIVE = 4
		, SIX = 5
		, SEVEN = 6
		, EIGHT = 7
		, NINE = 8
		, ZERO = 9
		, HOOK = 10
		, BACK = ONE
		, ENTER = HOOK
		, UP=TWO
		, DOWN=EIGHT
	};
public:
	QKeyboard(const PinConfig *pinConfig, uint8_t count);
	void scan();
	///last button pushed
	uint8_t getLastPinPushed();
	//last button pushed and held for at least TIMES_BUTTON_MUST_BE_HELD
	uint8_t getLastPinSeleted();
	uint8_t getLastKeyReleased();
	bool wasKeyReleased();
	void updateContext(KeyBoardLetterCtx &ctx);
	void reset();
	uint32_t getLastPinSelectedTick() {
		return LastPinSelectedTick;
	}
	char getNumberAsCharacter();
	void resetLastPinTick();
	bool isDialerMode();
	void setDialerMode(bool b);
protected:
	void setLetter();
private:
	const PinConfig *PC;
	uint8_t Count;
	uint8_t LastSelectedPin;
	uint8_t TimesLastPinSelected;
	uint8_t KeyJustReleased;
	uint32_t LastPinSelectedTick;
private:
	DialerMode *DM;
};

#endif
