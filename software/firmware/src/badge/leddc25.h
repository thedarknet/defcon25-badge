#ifndef LEDDC25_H
#define LEDDC25_H

/*
 * @author cmdc0de
 * @date 6/12/2017
 *
 * Class to encapsulate controlling of LEDS on DC25 darknet badge
 */
class LedDC25 {
public:
	struct PinConfig {
		uint16_t Pin;
		GPIO_TypeDef *Port;
		PinConfig() :
				Pin(0), Port(0) {
		}
		PinConfig(uint16_t pin, GPIO_TypeDef *port) :
				Pin(pin), Port(port) {
		}
	};
public:
	enum LED_ID {
		LED_STATUS = (1 << 0)
		, LED_STATUS_OFFSET = 0
		, LED_1 = (1 << 1)
		, LED_1_OFFSET = 1
		, LED_2 = (1 << 2)
		, LED_2_OFFSET = 2
		, LED_3 = (1 << 3)
		, LED_3_OFFSET = 3
		, LED_4 = (1 << 4)
		, LED_4_OFFSET = 4
		, LED_5 = (1 << 5)
		, LED_5_OFFSET = 5
		, LED_6 = (1 << 6)
		, LED_6_OFFSET = 6
		, LED_7 = (1 << 7)
		, LED_7_OFFSET = 7
		, LED_8 = (1 << 8)
		, LED_8_OFFSET = 8
		, LED_9 = (1 << 9)
		, LED_9_OFFSET = 9
		, LED_0 = (1 << 10)
		, LED_0_OFFSET = 10
		, ALL = LED_STATUS | LED_1 | LED_2 | LED_3 | LED_4 | LED_5 | LED_6 | LED_7 | LED_8 | LED_9 | LED_0
				, LED_COUNT = 11
	};
	enum LED_DANCE_TYPE {
		NONE = 0
		, DIALER = 1
		, RANDOM = 2
		, INIT = 3
		, CLOCK_WISE_CIRCLE = 4
		, COUNTER_CLOCK_WISE_CIRCLE=5
		, TOTAL_DANCE_TYPES = 6
	};
public:
	LedDC25();
	~LedDC25();
	void setAllOn();
	void setAllOff();
	void setLedOn(LED_ID ledid);
	void setLedOff(LED_ID ledid);
	bool isLedOn(LED_ID ledid);
	void setDanceType(LED_DANCE_TYPE t);
	void setDanceType(LED_DANCE_TYPE t, uint8_t data);
	void process();
protected:
	void setInternalLedOn(uint32_t t);
	void SetInternalLedOff(uint32_t t);
private:
	uint16_t LedState;
	uint8_t DanceType;
	uint8_t DanceTypeData;
	static const PinConfig PC[LED_COUNT];
	uint32_t lastBlinkTime;
	uint32_t lastDialerTime;
	uint8_t DialerData;
	int8_t Counter;

};

#endif
