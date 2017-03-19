#ifndef _DCDARKNET_APP_H
#define _DCDARKNET_APP_H

#include "error_type.h"

class DCDarkNetApp {
public:
	DCDarkNetApp();
	ErrorType init();
	void run();
private:

};

#endif
