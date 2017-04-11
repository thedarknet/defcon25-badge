#ifndef BADGE_MENUS_H
#define BADGE_MENUS_H

#include "gui.h"
#include "KeyStore.h"

class StateBase;
class DisplayST7735;
class Menu3D;

struct ReturnStateContext {
	ReturnStateContext(StateBase *next, const ErrorType &er) :
			NextMenuToRun(next), Err(er) {
	}
	ReturnStateContext(StateBase *n) :
			NextMenuToRun(n), Err() {
	}
	StateBase *NextMenuToRun;
	ErrorType Err;
};

class RunContext {
public:
	RunContext(DisplayST7735 *display);
	DisplayST7735 &getDisplay();
	const GUI &getGUI();
private:
	DisplayST7735 *dp; //should just be DisplayDevice rather than specific display //TODO
	GUI GuiDisplay;
};

class StateBase {
public:
	StateBase();
	ReturnStateContext run(RunContext &rc);
	uint32_t timeInState();
	ErrorType shutdown();
	virtual ~StateBase();
protected:
	static const uint32_t INIT_BIT = 0x01;
	static const uint32_t DONT_RESET = 0x02;
	virtual ErrorType onInit()=0;
	virtual ReturnStateContext onRun(RunContext &rc)=0;
	virtual ErrorType onShutdown()=0;
	void setState(uint32_t n) {
		StateData |= n;
	}
	void clearState(uint32_t n) {
		StateData = (StateData & ~n);
	}
	bool hasBeenInitialized() {
		return (StateData & INIT_BIT) != 0;
	}
	bool shouldReset() {
		return (StateData & DONT_RESET) == 0;
	}
	uint32_t getTimesRunCalledAllTime() { return TimesRunCalledAllTime;}
	uint32_t getTimesRunCalledSinceLastReset() {return TimesRunCalledSinceLastReset;}
private:
	ErrorType init();
private:
	uint32_t StateData : 8;
	uint32_t TimesRunCalledAllTime : 24;
	uint32_t TimesRunCalledSinceLastReset;
	uint32_t StateStartTime;
};

class DisplayMessageState: public StateBase {
public:
	DisplayMessageState(uint16_t timeInState, StateBase *nextState);
	virtual ~DisplayMessageState();
	void setMessage(const char *msg);
	void setTimeInState(uint16_t t) {
		TimeInState = t;
	}
	void setNextState(StateBase *b) {
		NextState = b;
	}
	StateBase *getNextState() {
		return NextState;
	}
protected:
	virtual ErrorType onInit();
	virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
private:
	char Message[64];
	uint16_t TimeInState;
	StateBase *NextState;
};

class MenuState: public StateBase {
public:
	MenuState();
	virtual ~MenuState();
protected:
	virtual ErrorType onInit();
	virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
private:
	GUI_ListData MenuList;
	GUI_ListItemData Items[9];
};

class SettingState: public StateBase {
public:
	SettingState();
	virtual ~SettingState();
protected:
	virtual ErrorType onInit();
	virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
private:
	GUI_ListData SettingList;
	GUI_ListItemData Items[3];
	char AgentName[ContactStore::AGENT_NAME_LENGTH];
	uint8_t InputPos;
	uint8_t SubState;
};

class BadgeInfoState: public StateBase {
public:
	BadgeInfoState();
	virtual ~BadgeInfoState();
protected:
	virtual ErrorType onInit();
	virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
	const char *getRegCode();
private:
	GUI_ListData BadgeInfoList;
	GUI_ListItemData Items[9];
	char ListBuffer[9][64]; //height then width
	char RegCode[18];
};

//=============================
class StateFactory {
public:
	static bool init();
	static StateBase *getDisplayMessageState(StateBase *bm, const char *message, uint16_t timeToDisplay);
	static StateBase *getMenuState();
	static StateBase *getSettingState();
	static StateBase *getGameOfLifeState();
	static StateBase *getMenu3D();
	//static StateBase* getBadgeInfoState();

};

#endif
