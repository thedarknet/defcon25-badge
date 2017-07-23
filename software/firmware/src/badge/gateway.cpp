#include "gateway.h"

#include "Radio/RFM69.h"
#include "dcdarknet_app.h"

Gateway::Gateway() :
		InternalState(REQUEST_REP), Counter(0) {

}

Gateway::~Gateway() {

}

ErrorType Gateway::onInit(RunContext &rc) {
	InternalState = REQUEST_REP;
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	return ErrorType();
}

struct GatewayRepMessage {
	const uint8_t MessageID;
	uint16_t RadioID;
	char AgentName[ContactStore::AGENT_NAME_LENGTH];
	GatewayRepMessage() :
			MessageID(Gateway::REQUEST_REP_MSG), RadioID(0), AgentName() {
	}
}  __attribute__((packed));

struct IntGatewayResponse {
	const uint8_t MessageID;
	uint8_t Success;
	int32_t Result;
	IntGatewayResponse() :
			MessageID(0), Success(false), Result(0) {
	}
}  __attribute__((packed));

ReturnStateContext Gateway::onRun(RunContext &rc) {
	StateBase *nextState = this;
	switch (InternalState) {
		case REQUEST_REP: {
			rc.getRadio().setNetwork(DCDarkNetApp::GATEWAY_NETWORK);
			GatewayRepMessage grm;
			grm.RadioID = rc.getContactStore().getMyInfo().getUniqueID();
			strcpy(&grm.AgentName[0], rc.getContactStore().getSettings().getAgentName());
			rc.getRadio().send(DCDarkNetApp::GATEWAY_RADIO_ID, &grm, sizeof(grm), true);
			rc.getDisplay().drawString(2, 10, "Request sent \nto Gateway.", RGBColor::WHITE, RGBColor::BLACK, 1, true);
			rc.getDisplay().drawString(2, 40, "Waiting for ack\n from Gateway.", RGBColor::WHITE, RGBColor::BLACK, 1,
					true);
			InternalState = WAITING_FOR_REQUEST_ACK;
		}
			break;
		case WAITING_FOR_REQUEST_ACK: {
			if (rc.getRadio().receiveDone()) {
				if (rc.getRadio().ACKReceived(DCDarkNetApp::GATEWAY_RADIO_ID)) {
					InternalState = RECEIVING;
				}
			} else if (this->getTimesRunCalledSinceLastReset() > 2000) {
				nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(),
						"No response from Gateway", 3000);
			}
		}
			break;
		case RECEIVING: {
			if (rc.getRadio().receiveDone()) {
				if (rc.getRadio().DATALEN == sizeof(IntGatewayResponse)
						&& rc.getRadio().SENDERID == DCDarkNetApp::GATEWAY_RADIO_ID) {
					IntGatewayResponse *grrm = (IntGatewayResponse *) &rc.getRadio().DATA[0];
					char buf[48];
					sprintf(&buf[0], "You have %d reputation points", grrm->Result);
					rc.getDisplay().fillScreen(RGBColor::BLUE);
					rc.getDisplay().drawString(2, 30, &buf[0], RGBColor::BLACK, RGBColor::BLUE, 2, true);
					Counter = getTimesRunCalledSinceLastReset() + 100;
					InternalState = FINISHING;
				}
			} else if (this->getTimesRunCalledSinceLastReset() > 10000) {
				nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(),
						"No response from Gateway", 3000);
			}
		}
			break;
		case FINISHING: {
			if (Counter < getTimesRunCalledSinceLastReset()) {
				nextState = StateFactory::getMenuState();
			}
		}
			break;
	}
	if (rc.getKB().wasKeyReleased()) {
		nextState = StateFactory::getMenuState();
	}
	if (nextState != this) {
		rc.getRadio().setNetwork(DCDarkNetApp::AGENT_NETWORK);
	}
	return ReturnStateContext(nextState);
}

ErrorType Gateway::onShutdown() {
	return ErrorType();
}

