#ifndef _DCDARKNET_APP_H
#define _DCDARKNET_APP_H

#include "error_type.h"

class StateBase;

class DCDarkNetApp {
public:
	enum COMPONENTS_ITEMS {
		LCD = (1<<0),
		RADIO = (1<<1),
		IR = (1<<2),
		FLASH_MEM = (1<<3)
	};
	static const uint8_t AGENT_NETWORK = 1;
	static const uint8_t GATEWAY_NETWORK = 200;
	static const uint16_t GATEWAY_RADIO_ID = 0xFFFE;
public:
	DCDarkNetApp();
	uint32_t init();
	void run();
private:
	StateBase *CurrentState;
};

#endif
