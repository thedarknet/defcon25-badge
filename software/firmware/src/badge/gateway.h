#ifndef DC_DARKNET_GATEWAY_H
#define DC_DARKNET_GATEWAY_H

#include "menus.h"

class Gateway: public StateBase {
public:
	Gateway();
	virtual ~Gateway();
	static const uint8_t REQUEST_REP_MSG = 1;
	static const uint8_t INT_REPONSE_MSG = 2;
protected:
	enum INTERNAL_STATE {
		REQUEST_REP,
		WAITING_FOR_REQUEST_ACK,
		RECEIVING,
		FINISHING
	};
	virtual ErrorType onInit(RunContext &rc);
	virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
private:
	INTERNAL_STATE InternalState;
	uint32_t Counter;
};

#endif
