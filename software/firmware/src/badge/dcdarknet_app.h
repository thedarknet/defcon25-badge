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
public:
	DCDarkNetApp();
	uint32_t init();
	void run();
private:
	StateBase *CurrentState;
};

#endif
