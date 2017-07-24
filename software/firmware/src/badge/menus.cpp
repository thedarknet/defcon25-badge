#include "stm32f3xx_hal.h"
#include "menus.h"
#include "display_device.h"
#include "GameOfLife.h"
#include "KeyStore.h"
#include "MessageState.h"
#include "Radio/RFM69.h"
#include <sha256.h>
#include "irmenu.h"
#include "SendMsgState.h"
#include "AddressState.h"
#include "3d/menu3d.h"
#include "leddc25.h"
#include <string.h>

#include "gateway.h"
//#include <tim.h>
//#include <uECC.h>
//#include <sha256.h>

RunContext::RunContext(DisplayST7735 *display, QKeyboard *kb, ContactStore *cs, RFM69 *r, LedDC25 *ledControl) :
		dp(display), GuiDisplay(display), KeyB(kb), CS(cs), Transciever(r), LedControl(ledControl) {

}

const GUI &RunContext::getGUI() {
	return GuiDisplay;
}

DisplayST7735 &RunContext::getDisplay() {
	return *dp;
}

QKeyboard &RunContext::getKB() {
	return *KeyB;
}

ContactStore &RunContext::getContactStore() {
	return *CS;
}

RFM69 &RunContext::getRadio() {
	return *Transciever;
}

LedDC25 &RunContext::getLedControl() {
	return *LedControl;
}

///////////////////////////

StateBase::StateBase() :
		StateData(0), TimesRunCalledAllTime(0), TimesRunCalledSinceLastReset(0), StateStartTime(0) {
}

ReturnStateContext StateBase::run(RunContext &rc) {
	++TimesRunCalledAllTime;
	ReturnStateContext sr(this);
	if (!hasBeenInitialized()) {
		TimesRunCalledSinceLastReset = 0;
		ErrorType et = init(rc);
		if (!et.ok()) {
			sr.NextMenuToRun = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), et.getMessage(),
					10000);
		}
	} else {
		++TimesRunCalledSinceLastReset;
		sr = onRun(rc);
		if (sr.NextMenuToRun != this) {
			shutdown();
		}
	}
	return sr;
}

StateBase::~StateBase() {
}

ErrorType StateBase::init(RunContext &rc) {
	ErrorType et = onInit(rc);
	if (et.ok()) {
		setState(INIT_BIT);
		StateStartTime = HAL_GetTick();
	}
	return et;
}
ErrorType StateBase::shutdown() {
	ErrorType et = onShutdown();
	clearState(INIT_BIT);
	StateStartTime = 0;
	return et;
}

uint32_t StateBase::timeInState() {
	return HAL_GetTick() - StateStartTime;
}

//=======================================================================
DisplayMessageState::DisplayMessageState(uint16_t timeInState, StateBase *nextState) :
		TimeInState(timeInState), NextState(nextState) {
}

DisplayMessageState::~DisplayMessageState() {
}

ErrorType DisplayMessageState::onInit(RunContext &rc) {
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	return ErrorType();
}

void DisplayMessageState::setMessage(const char *msg) {
	strncpy(&this->Message[0], msg, sizeof(this->Message));
}

ReturnStateContext DisplayMessageState::onRun(RunContext &rc) {
	rc.getDisplay().drawString(0, 10, &this->Message[0], RGBColor::WHITE, RGBColor::BLACK, 1, true);
	if (timeInState() > TimeInState) { //|| kb.getLastKeyReleased() != QKeyboard::NO_PIN_SELECTED) {
		return ReturnStateContext(StateFactory::getMenuState());
	}
	return ReturnStateContext(this);
}

ErrorType DisplayMessageState::onShutdown() {
	return ErrorType();
}

MenuState::MenuState() :
		StateBase(), MenuList("Main Menu", Items, 0, 0, 128, 160, 0, (sizeof(Items) / sizeof(Items[0]))) {
}

MenuState::~MenuState() {

}

const char *HasMessage = "DCDN Net Msgs *";
const char *NoHasMessage = "DCDN Net Msgs";

ErrorType MenuState::onInit(RunContext &rc) {
	Items[0].id = 0;
	if (rc.getContactStore().getSettings().isNameSet()) {
		Items[0].text = (const char *) "Settings";
	} else {
		Items[0].text = (const char *) "Settings *";
	}
	Items[1].id = 1;
	Items[1].text = (const char *) "IR Pair";
	Items[2].id = 2;
	Items[2].text = (const char *) "Address Book";
	Items[3].id = 3;
	if (((MessageState *) StateFactory::getMessageState())->hasNewMessage()) {
		Items[3].text = HasMessage;
	} else {
		Items[3].text = NoHasMessage;
	}
	Items[4].id = 4;
	Items[4].text = (const char *) "3D";
	Items[5].id = 5;
	Items[5].text = (const char *) "Screen Saver";
	Items[6].id = 6;
	Items[6].text = (const char *) "Badge Info";
	Items[7].id = 7;
	Items[7].text = (const char *) "Radio Info";
	Items[8].id = 8;
	Items[8].text = (const char *) "KeyBoard Test";
	Items[9].id = 9;
	Items[9].text = (const char *) "Quest Dialing";
	Items[10].id = 10;
	Items[10].text = (const char *) "Gateway";
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	rc.getGUI().drawList(&this->MenuList);
	return ErrorType();
}

ReturnStateContext MenuState::onRun(RunContext &rc) {
	StateBase *nextState = this;
	uint8_t key = rc.getKB().getLastKeyReleased();

	switch (key) {
		case QKeyboard::UP: {
			if (MenuList.selectedItem == 0) {
				MenuList.selectedItem = sizeof(Items) / sizeof(Items[0]) - 1;
			} else {
				MenuList.selectedItem--;
			}
			break;
		}
		case QKeyboard::DOWN: {
			if (MenuList.selectedItem == (sizeof(Items) / sizeof(Items[0]) - 1)) {
				MenuList.selectedItem = 0;
			} else {
				MenuList.selectedItem++;
			}
			break;
		}
		case QKeyboard::BACK: {
			MenuList.selectedItem = 0;
		}
			break;
		case QKeyboard::ENTER: {
			switch (MenuList.selectedItem) {
				case 0:
					nextState = StateFactory::getSettingState();
					break;
				case 1:
					if (rc.getContactStore().getSettings().getAgentName()[0] != '\0') {
						nextState = StateFactory::getIRPairingState();
					} else {
						nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(),
								(const char *) "You must set your agent name first", 3000);
					}
					break;
				case 2:
					nextState = StateFactory::getAddressBookState();
					break;
				case 3:
					nextState = StateFactory::getMessageState();
					break;
				case 4:
					nextState = StateFactory::get3DState();
					break;
				case 5:
					nextState = StateFactory::getGameOfLifeState();
					break;
				case 6:
					nextState = StateFactory::getBadgeInfoState();
					break;
				case 7:
					nextState = StateFactory::getRadioInfoState();
					break;
				case 8:
					nextState = StateFactory::getKeyBoardTest();
					break;
				case 9:
					rc.getKB().setDialerMode(true);
					nextState = StateFactory::getKeyBoardTest();
					break;
				case 10:
					nextState = StateFactory::getGateway();
					break;
			}
		}
			break;
	}
	if (rc.getKB().wasKeyReleased() && key != 9) {
		rc.getGUI().drawList(&this->MenuList);
	}
	return ReturnStateContext(nextState);
}

ErrorType MenuState::onShutdown() {
	MenuList.selectedItem = 0;
	return ErrorType();
}

KeyBoardTest::KeyBoardTest() :
		LastKey(QKeyboard::NO_PIN_SELECTED), NumberDialed(), FinalHexHash(), Pos(0), SelectedNumber(NOT_A_NUMBER), ReleaseEnter(
				false) {

}

KeyBoardTest::~KeyBoardTest() {

}

ErrorType KeyBoardTest::onInit(RunContext &rc) {
	LastKey = QKeyboard::NO_PIN_SELECTED;
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	memset(&NumberDialed[0], 0, sizeof(NumberDialed));
	memset(&FinalHexHash[0], 0, sizeof(FinalHexHash));
	Pos = 0;
	SelectedNumber = NOT_A_NUMBER;
	rc.getLedControl().setAllOff();
	if (rc.getKB().isDialerMode()) {
		//rc.getDisplay().drawString(0, 10, (const char*) "Hold Hook");
	} else {
		rc.getDisplay().drawString(0, 10, (const char*) "Hook then 1 to exit");
	}
	return ErrorType();
}

//http://www.11points.com/Movies/11_Memorable_555_Phone_Numbers_From_Movies_and_TV
#ifndef ALPHA
#define TOTAL_NUMBERS 7
static const char *Numbers[] = {
		"411" //uh
		, "911" //duh
		, "8675309" //Jenny
		, "8743221" //KRUX
		, "5552368" //Ghostbusters (555-2368)
		, "5554823" //back to the future
		, "5554220" //Hackers
};
#define MAX_NUMBER_LEN 7
#else
#define TOTAL_NUMBERS 1
static const char *Numbers[] = {
		"5555555"
};
#define MAX_NUMBER_LEN 7
#endif

ReturnStateContext KeyBoardTest::onRun(RunContext &rc) {
	StateBase *nextState = this;
	uint8_t key = QKeyboard::NO_PIN_SELECTED;
	if (rc.getKB().isDialerMode()) {
		if (Pos == MAX_NUMBER_LEN || (SelectedNumber != NOT_A_NUMBER && strlen(Numbers[SelectedNumber]) == Pos)) {
			rc.getDisplay().drawString(0, 100, "Hit 1 to reset");
			if (FinalHexHash[0] == '\0') {
				if (SelectedNumber == NOT_A_NUMBER) {
					rc.getLedControl().setDanceType(LedDC25::COUNTER_CLOCK_WISE_CIRCLE, 75);
				} else {
					rc.getLedControl().setDanceType(LedDC25::CLOCK_WISE_CIRCLE, 75);
				}
				uint8_t mhash[SHA256_HASH_SIZE] = { 0 };
				ShaOBJ HCtx;
				sha256_init(&HCtx);
				sha256_add(&HCtx, rc.getContactStore().getMyInfo().getPublicKey(), ContactStore::PUBLIC_KEY_LENGTH);
				sha256_add(&HCtx, (const unsigned char *) &NumberDialed[0], strlen((const char *) &NumberDialed[0]));
				sha256_digest(&HCtx, &mhash[0]);
				sprintf((char *) &FinalHexHash[0], "%02x%02x%02x%02x%02x%02x%02x%02x", mhash[0], mhash[1], mhash[2],
						mhash[3], mhash[4], mhash[5], mhash[6], mhash[7]);
			} else {
				rc.getDisplay().drawString(0, 120, "Send To Daemon: ");
				rc.getDisplay().drawString(0, 130, (const char *) &FinalHexHash[0]);
			}
			if (rc.getKB().getLastPinPushed() == QKeyboard::ONE) {
				rc.getLedControl().setDanceType(LedDC25::NONE);
				clearState(INIT_BIT);
			}
		} else {
			if (rc.getKB().getLastPinPushed() == QKeyboard::ENTER && !ReleaseEnter) {
				ReleaseEnter = true;
				LastKey = rc.getKB().getLastKeyReleased();
				NumberDialed[Pos] = rc.getKB().getNumberAsCharacter();
				SelectedNumber = NOT_A_NUMBER;
				for (int i = 0; i < TOTAL_NUMBERS; ++i) {
					if (strstr(&Numbers[i][0], (const char *) &NumberDialed[0]) == &Numbers[i][0]) {
						SelectedNumber = i;
						break;
					}
				}
				++Pos;
			} else if (rc.getKB().getLastPinPushed() == QKeyboard::ENTER) {
				if (HAL_GetTick() - rc.getKB().getLastPinSelectedTick() > 2000) {
					rc.getKB().setDialerMode(false);
					rc.getLedControl().setDanceType(LedDC25::NONE);
					nextState = StateFactory::getMenuState();
				}
			} else if (rc.getKB().getLastPinPushed() != QKeyboard::ENTER) {
				ReleaseEnter = false;
			}
		}
		key = rc.getKB().getLastPinPushed();
		char buf[24];
		rc.getDisplay().fillRec(0, 20, 128, 20, RGBColor::BLACK);
		if (LastKey == QKeyboard::NO_PIN_SELECTED) {
			sprintf(&buf[0], "Dialer Number:  N/A");
		} else {
			int displayNumber = (LastKey == 9 ? 0 : LastKey + 1);
			sprintf(&buf[0], "Dialer Number:  %d", displayNumber);
		}
		rc.getDisplay().drawString(0, 20, &buf[0]);
		rc.getDisplay().fillRec(0, 30, 128, 10, RGBColor::BLACK);
		if (key == QKeyboard::ENTER) {
			sprintf(&buf[0], "Current Number: Hook");
		} else if (key == QKeyboard::NO_PIN_SELECTED) {
			sprintf(&buf[0], "Current Number: N/A");
		} else {
			int displayNumber = (key == 9 ? 0 : key + 1);
			sprintf(&buf[0], "Current Number: %d", displayNumber);
		}
		rc.getDisplay().drawString(0, 30, &buf[0]);
		sprintf(&buf[0], "Dialing:");
		rc.getDisplay().drawString(0, 50, &buf[0]);
		sprintf(&buf[0], "Dialing: %s", &NumberDialed[0]);
		rc.getDisplay().fillRec(0, 70, 128, 10, RGBColor::BLACK);
		if (SelectedNumber != NOT_A_NUMBER) {
			rc.getDisplay().drawString(0, 70, &buf[0], RGBColor::BLUE);
		} else {
			rc.getDisplay().drawString(0, 70, &buf[0], RGBColor::RED);
		}
	} else {
		key = rc.getKB().getLastPinPushed();
		if (LastKey == 10 && key == 0) {
			rc.getKB().setDialerMode(false);
			nextState = StateFactory::getMenuState();
		}
		if (LastKey != key) {
			LastKey = key;
			rc.getDisplay().fillRec(0, 20, 128, 20, RGBColor::BLACK);
			char buf[16];
			rc.getDisplay().fillRec(0, 30, 128, 10, RGBColor::BLACK);
			int displayNumber = (key == 9 ? 0 : key + 1);
			sprintf(&buf[0], "pushed: %d", displayNumber);
			rc.getDisplay().drawString(0, 30, &buf[0]);
		}
	}
	return ReturnStateContext(nextState);
}

ErrorType KeyBoardTest::onShutdown() {
	return ErrorType();
}

SettingState::SettingState() :
		StateBase(), SettingList((const char *) "MENU", Items, 0, 0, 128, 160, 0, sizeof(Items) / sizeof(Items[0])), InputPos(
				0), SubState(0) {

	memset(&AgentName[0], 0, sizeof(AgentName));
	Items[0].id = 0;
	Items[0].text = (const char *) "Set Agent Name";
	//Items[1].id = 1;
	//Items[1].text = (const char *) "Set Screen Saver";
	Items[1].id = 1;
	Items[1].text = (const char *) "Screen Saver Time";
	Items[1].setShouldScroll();
	Items[2].id = 2;
	Items[2].text = (const char *) "Reset Badge Contacts";
	Items[2].setShouldScroll();
}

SettingState::~SettingState() {

}

ErrorType SettingState::onInit(RunContext &rc) {
	SubState = 0;
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	rc.getGUI().drawList(&SettingList);
	return ErrorType();
}

ReturnStateContext SettingState::onRun(RunContext &rc) {
	uint8_t key = rc.getKB().getLastKeyReleased();
	StateBase *nextState = this;
	switch (SubState) {
		case 0:
			switch (key) {
				case QKeyboard::UP: {
					if (SettingList.selectedItem == 0) {
						SettingList.selectedItem = sizeof(Items) / sizeof(Items[0]) - 1;
					} else {
						SettingList.selectedItem--;
					}
					break;
				}
				case QKeyboard::DOWN: {
					if (SettingList.selectedItem == (sizeof(Items) / sizeof(Items[0]) - 1)) {
						SettingList.selectedItem = 0;
					} else {
						SettingList.selectedItem++;
					}
					break;
				}
				case QKeyboard::BACK: {
					nextState = StateFactory::getMenuState();
				}
					break;
				case QKeyboard::ENTER: {
					SubState = SettingList.selectedItem + 100;
					rc.getDisplay().fillScreen(RGBColor::BLACK);
					switch (SubState) {
						case 100:
							memset(&AgentName[0], 0, sizeof(AgentName));
							getKeyboardContext().init(&AgentName[0], sizeof(AgentName));
							rc.getDisplay().drawString(0, 10, (const char*) "Current agent name:");
							if (*rc.getContactStore().getSettings().getAgentName() == '\0') {
								rc.getDisplay().drawString(0, 20, (const char *) "NOT SET");
							} else {
								rc.getDisplay().drawString(0, 20, rc.getContactStore().getSettings().getAgentName());
							}
							rc.getDisplay().drawString(0, 30, (const char*) "Set agent name:");
							break;
						case 101:
							rc.getDisplay().drawString(0, 10, (const char*) "Time until badge\ngoes to sleep:",
									RGBColor::WHITE, RGBColor::BLACK, 1, true);
							InputPos = rc.getContactStore().getSettings().getScreenSaverTime();
							break;
						case 102:
							rc.getKB().reset();
							rc.getDisplay().drawString(0, 10, (const char*) "ERASE ALL\nCONTACTS?");
							rc.getDisplay().drawString(0, 30, (const char*) "Touch 1 to Cancel");
							rc.getDisplay().drawString(0, 34, (const char*) "Touch Hook to do it");
							break;
					}
				}
					break;
				default:
					break;
			}
			break;

		case 100:
			rc.getKB().updateContext(getKeyboardContext());
			if (rc.getKB().getLastKeyReleased() == QKeyboard::ENTER && AgentName[0] != '\0' && AgentName[0] != ' '
					&& AgentName[0] != '_') {
				AgentName[ContactStore::AGENT_NAME_LENGTH - 1] = '\0';
				getKeyboardContext().finalize();
				//done
				if (rc.getContactStore().getSettings().setAgentname(&AgentName[0])) {
					nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Save Successful",
							2000);
				} else {
					nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Save FAILED!",
							4000);
				}
			} else {
				rc.getDisplay().drawString(0, 40, &AgentName[0]);
			}
			break;
		case 101:
			if (rc.getKB().getLastKeyReleased() == QKeyboard::ENTER) {
				if (rc.getContactStore().getSettings().setScreenSaverTime(InputPos)) {
					nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Setting saved",
							2000);
				} else {
					nextState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Save FAILED!",
							4000);
				}
			} else if (rc.getKB().getLastKeyReleased() != QKeyboard::NO_PIN_SELECTED) {
				InputPos = rc.getKB().getLastKeyReleased() + 1;
				if (InputPos > 8) {
					InputPos = 8;
				} else if (InputPos < 1) {
					InputPos = 1;
				}
			}
			sprintf(&AgentName[0], "%d Minutes", InputPos);
			rc.getDisplay().drawString(0, 40, &AgentName[0]);
			break;
		case 102:
			if (rc.getKB().getLastKeyReleased() == QKeyboard::BACK) {
				nextState = StateFactory::getMenuState();
			} else if (rc.getKB().getLastKeyReleased() == QKeyboard::ENTER) {
				rc.getContactStore().resetToFactory();
				StateFactory::getAddressBookState()->resetSelection();
				nextState = StateFactory::getMenuState();
			}
			break;
	}
	if (SubState < 100 && rc.getKB().wasKeyReleased()) {
		rc.getGUI().drawList(&SettingList);
	}
	return ReturnStateContext(nextState);
}

ErrorType SettingState::onShutdown()
{
	InputPos = 0;
	memset(&AgentName[0], 0, sizeof(AgentName));
	return ErrorType();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////

BadgeInfoState::BadgeInfoState()
:
		StateBase(), BadgeInfoList("Badge Info:", Items, 0, 0, 128, 160, 0, (sizeof(Items) / sizeof(Items[0]))), RegCode() {

	memset(&RegCode, 0, sizeof(RegCode));
}

BadgeInfoState::~BadgeInfoState()
{

}

const char *BadgeInfoState::getRegCode(ContactStore &cs) {
	if (RegCode[0] == 0) {
		ShaOBJ hashObj;
		sha256_init(&hashObj);
		sha256_add(&hashObj, cs.getMyInfo().getPrivateKey(), ContactStore::PRIVATE_KEY_LENGTH);
		uint16_t id = cs.getMyInfo().getUniqueID();
		sha256_add(&hashObj, (uint8_t *) &id, sizeof(id));
		uint8_t rH[SHA256_HASH_SIZE];
		sha256_digest(&hashObj, &rH[0]);
		sprintf(&RegCode[0], "%02x%02x%02x%02x%02x%02x%02x%02x", rH[0], rH[1], rH[2], rH[3], rH[4], rH[5], rH[6],
				rH[7]);
	}
	return &RegCode[0];
}

static const char *VERSION = "dc25.1.1";

ErrorType
BadgeInfoState::onInit(RunContext & rc)
		{
	memset(&ListBuffer[0], 0, sizeof(ListBuffer));
	sprintf(&ListBuffer[0][0], "Name: %s", rc.getContactStore().getSettings().getAgentName());
	sprintf(&ListBuffer[1][0], "Num contacts: %u", rc.getContactStore().getSettings().getNumContacts());
	sprintf(&ListBuffer[2][0], "REG: %s", getRegCode(rc.getContactStore()));
	sprintf(&ListBuffer[3][0], "UID: %u", rc.getContactStore().getMyInfo().getUniqueID());
	uint8_t *pCP = rc.getContactStore().getMyInfo().getCompressedPublicKey();
	sprintf(&ListBuffer[4][0],
			"PK: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			pCP[0], pCP[1], pCP[2], pCP[3], pCP[4], pCP[5], pCP[6], pCP[7], pCP[8], pCP[9], pCP[10], pCP[11],
			pCP[12],
			pCP[13], pCP[14], pCP[15], pCP[16], pCP[17], pCP[18], pCP[19], pCP[20], pCP[21], pCP[22], pCP[23],
			pCP[24]);
	sprintf(&ListBuffer[5][0], "DEVID: %lu", HAL_GetDEVID());
	sprintf(&ListBuffer[6][0], "REVID: %lu", HAL_GetREVID());
	sprintf(&ListBuffer[7][0], "HAL Version: %lu", HAL_GetHalVersion());
	sprintf(&ListBuffer[8][0], "SVer: %s", VERSION);

	for (uint32_t i = 0; i < (sizeof(Items) / sizeof(Items[0])); i++) {
		Items[i].text = &ListBuffer[i][0];
		Items[i].id = i;
		Items[i].setShouldScroll();
	}
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	rc.getGUI().drawList(&BadgeInfoList);
	return ErrorType();
}

ReturnStateContext
BadgeInfoState::onRun(RunContext & rc)
		{
	StateBase *nextState = this;
	uint8_t key = rc.getKB().getLastKeyReleased();
	switch (key) {
		case QKeyboard::UP: {
			if (BadgeInfoList.selectedItem == 0) {
				BadgeInfoList.selectedItem = sizeof(Items) / sizeof(Items[0]) - 1;
			} else {
				BadgeInfoList.selectedItem--;
			}
			break;
		}
		case QKeyboard::DOWN: {
			if (BadgeInfoList.selectedItem == (sizeof(Items) / sizeof(Items[0]) - 1)) {
				BadgeInfoList.selectedItem = 0;
			} else {
				BadgeInfoList.selectedItem++;
			}
			break;
		}
		case QKeyboard::ENTER:
			case QKeyboard::BACK:
			nextState = StateFactory::getMenuState();
			break;
	}
	if (rc.getKB().wasKeyReleased()
			|| (Items[BadgeInfoList.selectedItem].shouldScroll() && getTimesRunCalledSinceLastReset() % 5 == 0)) {
		rc.getGUI().drawList(&BadgeInfoList);
	}
	return ReturnStateContext(nextState);
}

ErrorType
BadgeInfoState::onShutdown()
{
	return ErrorType();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RadioInfoState::RadioInfoState()
:
		StateBase(), RadioInfoList("Radio Info:", Items, 0, 0, 128, 160, 0, (sizeof(Items) / sizeof(Items[0]))), Items(), ListBuffer() {

}

RadioInfoState::~RadioInfoState()
{

}

ErrorType
RadioInfoState::onInit(RunContext & rc)
		{
	memset(&ListBuffer[0], 0, sizeof(ListBuffer));
	for (uint32_t i = 0; i < (sizeof(Items) / sizeof(Items[0])); i++) {
		Items[i].text = &ListBuffer[i][0];
	}
	sprintf(&ListBuffer[0][0], "Frequency: %lu", (rc.getRadio().getFrequency() / (1000 * 1000)));
	sprintf(&ListBuffer[1][0], "RSSI: %d", rc.getRadio().readRSSI());
	sprintf(&ListBuffer[2][0], "RSSI Threshold: %u", rc.getRadio().getRSSIThreshHold());
	sprintf(&ListBuffer[3][0], "Gain: %u", rc.getRadio().getCurrentGain());
	sprintf(&ListBuffer[4][0], "Temp: %u", rc.getRadio().readTemperature());
	sprintf(&ListBuffer[5][0], "Impedance: %u", rc.getRadio().getImpedenceLevel());
	rc.getDisplay().fillScreen(RGBColor::BLACK);
	rc.getGUI().drawList(&RadioInfoList);
	return ErrorType();
}

ReturnStateContext
RadioInfoState::onRun(RunContext & rc)
		{
	StateBase *nextState = this;
	uint8_t pin = rc.getKB().getLastKeyReleased();
	if (pin == 9) {
		nextState = StateFactory::getMenuState();
	}
	return ReturnStateContext(nextState);
}

ErrorType
RadioInfoState::onShutdown()
{
	return ErrorType();
}

//============================================================
static DisplayMessageState Display_Message_State(3000, 0);
static MenuState MenuState;
static SettingState TheSettingState;
static BadgeInfoState TheBadgeInfoState;
static GameOfLife TheGameOfLifeState;
//EventState TheEventState;
static KeyBoardTest TheKeyBoardTest;
static MessageState TheMessageState;
static RadioInfoState TheRadioInfoState;
static IRState TheIRState(2000, 5);
static SendMsgState TheMsgState;
static AddressState TheAddressState;
static Menu3D The3DMenu;
static Gateway TheGateway;

Menu3D *StateFactory::get3DState() {
	return &The3DMenu;
}

SendMsgState* StateFactory::getSendMessageState() {
	return &TheMsgState;
}

AddressState* StateFactory::getAddressBookState() {
	return &TheAddressState;
}

StateBase *StateFactory::getDisplayMessageState(StateBase *bm, const char *message, uint16_t timeToDisplay) {
	Display_Message_State.setMessage(message);
	Display_Message_State.setNextState(bm);
	Display_Message_State.setTimeInState(timeToDisplay);
	return &Display_Message_State;
}

StateBase * StateFactory::getRadioInfoState()
{
	return &TheRadioInfoState;
}

StateBase * StateFactory::getMenuState()
{
	return &MenuState;
}

StateBase * StateFactory::getSettingState()
{
	return &TheSettingState;
}

StateBase * StateFactory::getBadgeInfoState()
{
	return &TheBadgeInfoState;
}

StateBase * StateFactory::getGameOfLifeState()
{
	return &TheGameOfLifeState;
}

StateBase * StateFactory::getKeyBoardTest()
{
	return &TheKeyBoardTest;
}

MessageState * StateFactory::getMessageState()
{
	return &TheMessageState;
}

StateBase * StateFactory::getIRPairingState()
{
	return &TheIRState;
}

StateBase *StateFactory::getGateway() {
	return &TheGateway;
}
