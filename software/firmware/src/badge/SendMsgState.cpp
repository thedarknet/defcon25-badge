#include "SendMsgState.h"
#include "Radio/RFM69.h"

SendMsgState::SendMsgState() :
		StateBase(), RadioID(0), AgentName(0), MsgBuffer(), InternalState(TYPE_MESSAGE) {

}
SendMsgState::~SendMsgState() {

}
void SendMsgState::setContactToMessage(const uint16_t radioID, const char *agentName) {
	RadioID = radioID;
	AgentName = agentName;
}

ErrorType SendMsgState::onInit(RunContext &rc) {
	if (shouldReset()) {
		memset(&MsgBuffer[0], 0, sizeof(MsgBuffer));
		getKeyboardContext().init(&MsgBuffer[0], sizeof(MsgBuffer));
	} else {
		clearState(DONT_RESET);
	}
	InternalState = TYPE_MESSAGE;
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	return ErrorType();
}

ReturnStateContext SendMsgState::onRun(RunContext &rc) {
	StateBase *nextState = this;
	switch (InternalState) {
	case TYPE_MESSAGE: {
		rc.getDisplay().drawString(0,10,(const char *)"Send Message: ");
		rc.getDisplay().drawString(0,20,&MsgBuffer[0]);
		//keyboard entry
		rc.getKB().updateContext(getKeyboardContext());
		uint8_t pin = rc.getKB().getLastKeyReleased();
		if (pin == QKeyboard::ENTER) { //return has been pushed
			getKeyboardContext().finalize();
			InternalState = INTERNAL_STATE::CONFIRM_SEND;
		}
	}
		break;
	case CONFIRM_SEND: {
		rc.getDisplay().drawStringOnLine(1,(const char *)"Send by pressing return (hook).");
		rc.getDisplay().drawStringOnLine(3,(const char *)"Cancel by pressing back (1).");
		//gui_lable_multiline(&MsgBuffer[0], 0, 30, 128, 64, 0, 0);
		uint8_t pin = rc.getKB().getLastKeyReleased();
		if (pin == QKeyboard::BACK) {
			InternalState = TYPE_MESSAGE;
		} else if (pin == QKeyboard::ENTER) {
			InternalState = SENDING;
		}
	}
		break;
	case SENDING: {
		static char buf[32];
		sprintf(&buf[0], "Sending Message to: %s", AgentName);
		rc.getDisplay().drawString(0,10,&buf[0]);
#ifdef DONT_USE_ACK
		rc.getRadio().send(RadioID, &MsgBuffer[0], strlen(&MsgBuffer[0]), false);
		nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Message Sent!", 5000);
#else
		//TODO get ack working
		if (getRadio().sendWithRetry(RadioID, &MsgBuffer[0], strlen(&MsgBuffer[0]), 1, 400)) {
			nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Message Sent Successfully!",
					5000);
		} else {
			static char ResultBuf[64];
			sprintf(&ResultBuf[0],"Failed to send: \nAddress: %d\n N: %s", RadioID,AgentName);
			nextState = StateFactory::getDisplayMessageState(StateFactory::getSendMessageState(),
					&ResultBuf[0], 8000);
			setState(DONT_RESET);
		}
#endif
	}
		break;
	}
	return ReturnStateContext(nextState);
}

ErrorType SendMsgState::onShutdown() {
	if (shouldReset()) {
		RadioID = 0;
		AgentName = 0;
	}
	return ErrorType();
}
