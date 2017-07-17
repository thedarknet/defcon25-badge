#include <stdint.h>
#include <gpio.h>
#include <main.h>
#include "menus.h"
#include "MessageState.h"
#include "leddc25.h"

const LedDC25::PinConfig LedDC25::PC[] = {
		{ LED_STATUS_Pin, LED_STATUS_GPIO_Port }
		, { LED1_Pin, LED1_GPIO_Port }
		, { LED2_Pin, LED2_GPIO_Port }
		, { LED3_Pin, LED3_GPIO_Port }
		, { LED4_Pin, LED4_GPIO_Port }
		, { LED5_Pin, LED5_GPIO_Port }
		, { LED6_Pin, LED6_GPIO_Port }
		, { LED7_Pin, LED7_GPIO_Port }
		, { LED8_Pin, LED8_GPIO_Port }
		, { LED9_Pin, LED9_GPIO_Port }
		, { LED10_Pin, LED10_GPIO_Port }
};

LedDC25::LedDC25() :
		LedState(0), DanceType(NONE), DanceTypeData(0), lastBlinkTime(0), lastDialerTime(0), DialerData(0), Counter(0) {

}

LedDC25::~LedDC25() {

}

void LedDC25::setAllOn() {
	LedState |= ALL;
}

void LedDC25::setAllOff() {
	LedState = 0;
}

void LedDC25::setLedOn(LED_ID ledid) {
	setInternalLedOn(ledid);
}

void LedDC25::setLedOff(LED_ID ledid) {
	SetInternalLedOff(ledid);
}

bool LedDC25::isLedOn(LED_ID ledid) {
	return (LedState & ledid) == ledid;
}

void LedDC25::setDanceType(LED_DANCE_TYPE t) {
	setDanceType(t, 0);
}

void LedDC25::setInternalLedOn(uint32_t t) {
	LedState |= t;
}

void LedDC25::SetInternalLedOff(uint32_t t) {
	LedState &= ~t;
}

void LedDC25::setDanceType(LED_DANCE_TYPE t, uint8_t data) {
	DanceType = t;
	DanceTypeData = data;
	lastBlinkTime = 0;
	lastDialerTime = 0;
	DialerData = 0;
	Counter = 0;
}

#define BLINK_TIME 350
#define DIALER_BACK_TIME 50

void LedDC25::process() {
	switch (DanceType) {
		case NONE:
			{
			if (StateFactory::getMessageState()->hasNewMessage()) {
				if (HAL_GetTick() - lastBlinkTime > BLINK_TIME) {
					HAL_GPIO_TogglePin(PC[LED_STATUS_OFFSET].Port, PC[LED_STATUS_OFFSET].Pin);
					lastBlinkTime = HAL_GetTick();
				}
			} else {
				if (isLedOn(LED_STATUS))
					HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
			}

		}
			break;
		case DIALER:
			{
			for (int i = 1; i < LED_COUNT; i++) {
				if (i - 1 <= DialerData) {
					setInternalLedOn((1 << i));
				} else {
					SetInternalLedOff((1 << i));
				}
			}
			if (HAL_GetTick() - lastDialerTime > 200) {
				lastDialerTime = HAL_GetTick();
				DialerData++;
				if (DialerData > DanceTypeData) {
					DialerData = 0;
					DanceType = NONE;
					SetInternalLedOff(ALL);
					return;
				}
			}
		}
			break;
		case CLOCK_WISE_CIRCLE:
			if (Counter < 0)
				Counter = 9;
			if ((HAL_GetTick() - lastBlinkTime) > DialerData) {
				lastBlinkTime = HAL_GetTick();
				setAllOff();
				setInternalLedOn(1 << Counter);
				--Counter;
			}
			break;
		case COUNTER_CLOCK_WISE_CIRCLE:
			if (Counter > 9)
				Counter = 0;
			if ((HAL_GetTick() - lastBlinkTime) > DialerData) {
				lastBlinkTime = HAL_GetTick();
				setAllOff();
				setInternalLedOn(1 << Counter);
				++Counter;
			}
			break;

		case INIT:
			{
			if (Counter < 12) {
				if (Counter < 4) {
					if ((HAL_GetTick() - lastBlinkTime) > BLINK_TIME) {
						lastBlinkTime = HAL_GetTick();
						if ((Counter & 0x1) == 0) {
							setAllOn();
						} else {
							setAllOff();
						}
						++Counter;
					}
				} else {
					if ((HAL_GetTick() - lastBlinkTime) > DIALER_BACK_TIME) {
						lastBlinkTime = HAL_GetTick();
						if ((Counter & 0x1) == 0) {
							for (int i = 0; i < LED_COUNT; i++) {
								if (i <= DialerData) {
									setInternalLedOn((1 << i));
								} else {
									SetInternalLedOff((1 << i));
								}
							}
							if (DialerData == LED_COUNT - 1) {
								++Counter;
							} else {
								++DialerData;
							}
						} else {
							for (int i = 0; i < LED_COUNT; i++) {
								if (i <= DialerData) {
									setInternalLedOn((1 << i));
								} else {
									SetInternalLedOff((1 << i));
								}
							}
							if (DialerData == 0) {
								++Counter;
							} else {
								--DialerData;
							}
						}
					}
				}
			} else if (Counter < 14) {
				if ((HAL_GetTick() - lastBlinkTime) > BLINK_TIME) {
					lastBlinkTime = HAL_GetTick();
					if ((Counter & 0x1) == 0) {
						setAllOn();
					} else {
						setAllOff();
					}
					++Counter;
				}
			} else {
				setDanceType(NONE);
			}
		}
			break;
	}

	if (isLedOn(LED_1))
		HAL_GPIO_WritePin(PC[LED_1_OFFSET].Port, PC[LED_1_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_1_OFFSET].Port, PC[LED_1_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_2))
		HAL_GPIO_WritePin(PC[LED_2_OFFSET].Port, PC[LED_2_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_2_OFFSET].Port, PC[LED_2_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_3))
		HAL_GPIO_WritePin(PC[LED_3_OFFSET].Port, PC[LED_3_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_3_OFFSET].Port, PC[LED_3_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_4))
		HAL_GPIO_WritePin(PC[LED_4_OFFSET].Port, PC[LED_4_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_4_OFFSET].Port, PC[LED_4_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_5))
		HAL_GPIO_WritePin(PC[LED_5_OFFSET].Port, PC[LED_5_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_5_OFFSET].Port, PC[LED_5_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_6))
		HAL_GPIO_WritePin(PC[LED_6_OFFSET].Port, PC[LED_6_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_6_OFFSET].Port, PC[LED_6_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_7))
		HAL_GPIO_WritePin(PC[LED_7_OFFSET].Port, PC[LED_7_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_7_OFFSET].Port, PC[LED_7_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_8))
		HAL_GPIO_WritePin(PC[LED_8_OFFSET].Port, PC[LED_8_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_8_OFFSET].Port, PC[LED_8_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_9))
		HAL_GPIO_WritePin(PC[LED_9_OFFSET].Port, PC[LED_9_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_9_OFFSET].Port, PC[LED_9_OFFSET].Pin, GPIO_PIN_RESET);
	if (isLedOn(LED_0))
		HAL_GPIO_WritePin(PC[LED_0_OFFSET].Port, PC[LED_0_OFFSET].Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PC[LED_0_OFFSET].Port, PC[LED_0_OFFSET].Pin, GPIO_PIN_RESET);
}
