#include "Keyboard.h"
#include <tsc.h>
#include <string.h>

KeyBoardLetterCtx KCTX;

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

QKeyboard::QKeyboard(PinConfig *pinConfig, uint8_t count) :
		PC(pinConfig), Count(count), LastSelectedPin(NO_PIN_SELECTED), TimesLastPinSelected(0), KeyJustReleased(
				NO_PIN_SELECTED), LastPinSelectedTick(
				HAL_GetTick()), LightAll(true) {

}

void QKeyboard::resetLastPinTick() {
	LastPinSelectedTick = HAL_GetTick();
}

void QKeyboard::setAllLightsOn(bool b) {
	LightAll = b;
}

void QKeyboard::scan() {
	uint8_t selectedPin = NO_PIN_SELECTED;
	HAL_TSC_IODischarge(&htsc, ENABLE);
	HAL_Delay(100);
#if 1
	for (int r = 0; r < Count && selectedPin == NO_PIN_SELECTED; ++r) {
		TSC_IOConfigTypeDef conf;
		conf.ChannelIOs = PC[r].Pin;
		conf.SamplingIOs = PC[r].SamplePin;
		conf.ShieldIOs = 0;
		if (HAL_OK == HAL_TSC_IOConfig(&htsc, &conf)) {
			if (HAL_OK == HAL_TSC_Start(&htsc)) {
				if (HAL_OK == HAL_TSC_PollForAcquisition(&htsc)) {
					uint32_t value = HAL_TSC_GroupGetValue(&htsc, PC[r].GroupIndex);
					if (value < 250) {
						selectedPin = r;
						LastPinSelectedTick = HAL_GetTick();
					}
				} else {
					//ERRMSG("Error during Poll");
				}
				if (HAL_OK != HAL_TSC_Stop(&htsc)) {
					//ERRMSG("Error during stop");
				}
			} else {
				//ERRMSG("Error during start");
			}
		}
	}
#else
	TSC_IOConfigTypeDef conf;
	conf.ChannelIOs = TSC_GROUP5_IO3; //TSC_GROUP3_IO2; //PC[r].Pin;
	conf.SamplingIOs = TSC_GROUP5_IO4;//PC[r].SamplePin;
	conf.ShieldIOs = 0;
	if (HAL_OK == HAL_TSC_IOConfig(&htsc, &conf)) {
		if (HAL_OK == HAL_TSC_Start(&htsc)) {
			if (HAL_OK == HAL_TSC_PollForAcquisition(&htsc)) {
				uint32_t value = HAL_TSC_GroupGetValue(&htsc, TSC_GROUP5_IDX); //PC[r].GroupIndex);
				if (value < 250) {
					selectedPin = 8; //r;
					LastPinSelectedTick = HAL_GetTick();
				}
			} else {
				//ERRMSG("Error during Poll");
			}
			if (HAL_OK != HAL_TSC_Stop(&htsc)) {
				//ERRMSG("Error during stop");
			}
		} else {
			//ERRMSG("Error during start");
		}
	}
	conf.ChannelIOs = TSC_GROUP5_IO2; //PC[r].Pin;
	conf.SamplingIOs = TSC_GROUP5_IO4;//PC[r].SamplePin;
	conf.ShieldIOs = 0;
	if (HAL_OK == HAL_TSC_IOConfig(&htsc, &conf)) {
		if (HAL_OK == HAL_TSC_Start(&htsc)) {
			if (HAL_OK == HAL_TSC_PollForAcquisition(&htsc)) {
				uint32_t value = HAL_TSC_GroupGetValue(&htsc, TSC_GROUP5_IDX); //PC[r].GroupIndex);
				if (value < 250) {
					selectedPin = 9; //r;
					LastPinSelectedTick = HAL_GetTick();
				}
			} else {
				//ERRMSG("Error during Poll");
			}
			if (HAL_OK != HAL_TSC_Stop(&htsc)) {
				//ERRMSG("Error during stop");
			}
		} else {
			//ERRMSG("Error during start");
		}
	}
	conf.ChannelIOs = TSC_GROUP5_IO1; //PC[r].Pin;
	conf.SamplingIOs = TSC_GROUP5_IO4;//PC[r].SamplePin;
	conf.ShieldIOs = 0;
	if (HAL_OK == HAL_TSC_IOConfig(&htsc, &conf)) {
		if (HAL_OK == HAL_TSC_Start(&htsc)) {
			if (HAL_OK == HAL_TSC_PollForAcquisition(&htsc)) {
				uint32_t value = HAL_TSC_GroupGetValue(&htsc, TSC_GROUP5_IDX); //PC[r].GroupIndex);
				if (value < 250) {
					selectedPin = 10; //r;
					LastPinSelectedTick = HAL_GetTick();
				}
			} else {
				//ERRMSG("Error during Poll");
			}
			if (HAL_OK != HAL_TSC_Stop(&htsc)) {
				//ERRMSG("Error during stop");
			}
		} else {
			//ERRMSG("Error during start");
		}
	}

#endif
//}
	HAL_TSC_IODischarge(&htsc, DISABLE);

	if (selectedPin == LastSelectedPin) {
		TimesLastPinSelected++;
		if (KeyJustReleased != NO_PIN_SELECTED && selectedPin == NO_PIN_SELECTED) {
			KeyJustReleased = NO_PIN_SELECTED;
		}
	} else {
		KeyJustReleased = LastSelectedPin;
		LastSelectedPin = selectedPin;
		TimesLastPinSelected = 0;
	}
	if (getAllLightsOn()) {
		HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED9_GPIO_Port, LED9_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED10_GPIO_Port, LED10_Pin, GPIO_PIN_SET);
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
			current = "PQRS7";
			break;
		case 7:
			current = "TUV8";
			break;
		case 8:
			current = "WXYZ9";
			break;
		case 9:
			current = "##+";
			break;
		case 10:
			current = "0 \b";
			break;
		}
		if (getLastKeyReleased() < 11) {
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
