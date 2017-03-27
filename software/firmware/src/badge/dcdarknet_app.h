#ifndef _DCDARKNET_APP_H
#define _DCDARKNET_APP_H

#include "error_type.h"

class StateBase;

class DCDarkNetApp {
public:
	DCDarkNetApp();
	ErrorType init();
	void run();
private:
	StateBase *CurrentState;
};

#endif
