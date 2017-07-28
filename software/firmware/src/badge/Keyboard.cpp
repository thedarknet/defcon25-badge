#include "Keyboard.h"
#include <tsc.h>
#include <string.h>
#include "logger.h"

KeyBoardLetterCtx KCTX;

class DialerMode {
public:
	DialerMode() : StartPin(QKeyboard::NO_PIN_SELECTED), DialTimer(0) {}
	~DialerMode() {}
	uint8_t selectPin(uint8_t p) {
		if(StartPin==QKeyboard::NO_PIN_SELECTED) {
			if(p!=QKeyboard::ENTER) {
				StartPin = p;
				resetTimer();
			}
		} else {
			if(p!=QKeyboard::NO_PIN_SELECTED) {
				if(p==QKeyboard::ENTER && ((HAL_GetTick()-DialTimer)<2000)) {
					uint8_t retVal = StartPin;
					reset();
					return retVal;
				} else if(p<StartPin) {
					resetTimer();
				} else {
					reset();
				}
			}
		}
		return QKeyboard::NO_PIN_SELECTED;
	}
protected:
	void resetTimer() {
		DialTimer = HAL_GetTick();
	}
	void reset() {
		resetTimer();
		StartPin = QKeyboard::NO_PIN_SELECTED;
	}
private:
	uint8_t StartPin;
	uint32_t DialTimer;
};

DialerMode sDialerMode;

KeyBoardLetterCtx &getKeyboardContext() {
	return KCTX;
}

KeyBoardLetterCtx::KeyBoardLetterCtx() {
	init(0, 0);
}

void KeyBoardLetterCtx::processButtonPush(uint8_t button, const char *buttonLetters) {
	if (LastPin == button) {
		LetterSelection++;
		if (LetterSelection >= strlen(buttonLetters)) {
			LetterSelection = 0;
		}
	} else {
		LetterSelection = 0;
		if (Started) {
			setCurrentLetterInBufferAndInc();
		}
	}
	if (buttonLetters[LetterSelection] == '\b') {
		Buffer[CursorPosition] = '\0';
		decPosition();
		Buffer[CursorPosition] = '\0';
		decPosition();
		CurrentLetter = ' ';
		LastPin = QKeyboard::NO_PIN_SELECTED;
		timerStop();
	} else {
		CurrentLetter = buttonLetters[LetterSelection];
		LastPin = button;
		timerStart();
	}
}

bool KeyBoardLetterCtx::isKeySelectionTimedOut() {
	if (Started && (HAL_GetTick() - LastTimeLetterWasPushed) > 3000) {
		return true;
	}
	return false;
}
void KeyBoardLetterCtx::timerStart() {
	Started = true;
	LastTimeLetterWasPushed = HAL_GetTick();
}
void KeyBoardLetterCtx::timerStop() {
	Started = false;
}

void KeyBoardLetterCtx::decPosition() {
	if (--CursorPosition < 0) {
		CursorPosition = 0;
	}
}

void KeyBoardLetterCtx::finalize() {
	Buffer[CursorPosition] = CurrentLetter;
	Buffer[BufferSize - 1] = '\0';
}

void KeyBoardLetterCtx::incPosition() {
	if (++CursorPosition >= BufferSize) {
		CursorPosition = BufferSize - 1;
	}
}
uint8_t KeyBoardLetterCtx::getCursorPosition() {
	return CursorPosition;
}
void KeyBoardLetterCtx::setCurrentLetterInBufferAndInc() {
	if (CursorPosition >= 0) {
		Buffer[CursorPosition] = CurrentLetter;
	}
	incPosition();
}
void KeyBoardLetterCtx::blinkLetter() {
	uint32_t tmp = HAL_GetTick() / 500;
	if (tmp != LastBlinkTime) {
		LastBlinkTime = tmp;
		UnderBar = !UnderBar;
	}
	int16_t pos = CursorPosition;
	if (pos < 0)
		pos = 0;
	if (UnderBar) {
		Buffer[pos] = '_';
	} else {
		Buffer[pos] = CurrentLetter;
	}
}

void KeyBoardLetterCtx::resetChar() {
	LastPin = QKeyboard::NO_PIN_SELECTED;
	CurrentLetter = ' ';
}
void KeyBoardLetterCtx::init(char *b, uint16_t s) {
	Buffer = b;
	Started = false;
	UnderBar = true;
	CurrentLetter = ' ';
	BufferSize = s;
	CursorPosition = 0;
	LastTimeLetterWasPushed = 0;
	LetterSelection = 0;
	LastBlinkTime = 0;
	LastPin = QKeyboard::NO_PIN_SELECTED;
}

QKeyboard::QKeyboard(const PinConfig *pinConfig, uint8_t count) :
		PC(pinConfig), Count(count), LastSelectedPin(NO_PIN_SELECTED), TimesLastPinSelected(0), KeyJustReleased(
				NO_PIN_SELECTED), LastPinSelectedTick(
				HAL_GetTick()), DM(0) {

}

void QKeyboard::resetLastPinTick() {
	LastPinSelectedTick = HAL_GetTick();
}

char QKeyboard::getNumberAsCharacter() {
	switch (getLastKeyReleased()) {
		case ONE:
			return '1';
		case TWO:
			return '2';
		case THREE:
			return '3';
		case FOUR:
			return '4';
		case FIVE:
			return '5';
		case SIX:
			return '6';
		case SEVEN:
			return '7';
		case EIGHT:
			return '8';
		case NINE:
			return '9';
		case ZERO:
			return '0';
		default:
			return '*';
	}
}

bool QKeyboard::isDialerMode() {
	return DM!=0;
}

void QKeyboard::setDialerMode(bool b) {
	if(b) {
		DM = &sDialerMode;
	} else {
		DM = 0;
	}
}

void QKeyboard::scan() {
	uint8_t selectedPin = NO_PIN_SELECTED;
	HAL_TSC_IODischarge(&htsc, ENABLE);
	HAL_Delay(50);

	for (int r = 0; r < Count && selectedPin == NO_PIN_SELECTED; ++r) {
		TSC_IOConfigTypeDef conf;
		conf.ChannelIOs = PC[r].Pin;
		conf.SamplingIOs = PC[r].SamplePin;
		conf.ShieldIOs = 0;
		if (HAL_OK == HAL_TSC_IOConfig(&htsc, &conf)) {
			if (HAL_OK == HAL_TSC_Start(&htsc)) {
				if (HAL_OK == HAL_TSC_PollForAcquisition(&htsc)) {
					uint32_t value = HAL_TSC_GroupGetValue(&htsc, PC[r].GroupIndex);
					if (value < 252) {
						selectedPin = r;
					}
				} else {
					ERRMSG("Error during Poll");
				}
				if (HAL_OK != HAL_TSC_Stop(&htsc)) {
					ERRMSG("Error during stop");
				}
			} else {
				ERRMSG("Error during start");
			}
		}
	}
	HAL_TSC_IODischarge(&htsc, DISABLE);

	if (selectedPin == LastSelectedPin) {
		TimesLastPinSelected++;
		if (KeyJustReleased != NO_PIN_SELECTED && selectedPin == NO_PIN_SELECTED) {
			KeyJustReleased = NO_PIN_SELECTED;
		}
	} else {
		LastPinSelectedTick = HAL_GetTick();
		if (isDialerMode()) {
			KeyJustReleased = DM->selectPin(selectedPin);
			LastSelectedPin = selectedPin;
		} else {
			KeyJustReleased = getLastPinSeleted(); //LastSelectedPin;
			LastSelectedPin = selectedPin;
			TimesLastPinSelected = 0;
		}
	}
}

uint8_t QKeyboard::getLastKeyReleased() {
	return KeyJustReleased;
}

bool QKeyboard::wasKeyReleased() {
	return KeyJustReleased != NO_PIN_SELECTED;
}

void QKeyboard::reset() {
	TimesLastPinSelected = 0;
	LastSelectedPin = NO_PIN_SELECTED;
}

void QKeyboard::updateContext(KeyBoardLetterCtx &ctx) {
	if (ctx.isKeySelectionTimedOut() && !wasKeyReleased()) {
		ctx.setCurrentLetterInBufferAndInc();
		ctx.resetChar();
		ctx.timerStop();
	} else if (wasKeyReleased()) {
		const char *current = 0;
		switch (getLastKeyReleased()) {
			case 0:
				current = ".,?1";
				break;
			case 1:
				current = "ABC2";
				break;
			case 2:
				current = "DEF3";
				break;
			case 3:
				current = "GHI4";
				break;
			case 4:
				current = "JKL5";
				break;
			case 5:
				current = "MNO6";
				break;
			case 6:
				current = "PRS7";
				break;
			case 7:
				current = "TUV8";
				break;
			case 8:
				current = "WXY9";
				break;
				//case 9:
				//	current = "##+";
				//	break;
			case 9:
				current = "Z0 \b";
				break;
		}
		if (getLastKeyReleased() < 10) {
			ctx.processButtonPush(getLastKeyReleased(), current);
		}
	}
	ctx.blinkLetter();
}

uint8_t QKeyboard::getLastPinPushed() {
	return LastSelectedPin;
}

uint8_t QKeyboard::getLastPinSeleted() {
	if (LastSelectedPin != QKeyboard::NO_PIN_SELECTED && TimesLastPinSelected >= QKeyboard::TIMES_BUTTON_MUST_BE_HELD) {
		return LastSelectedPin;
	}
	return QKeyboard::NO_PIN_SELECTED;
}
