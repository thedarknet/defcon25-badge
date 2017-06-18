#include "dcdarknet_app.h"
#include "display_device.h"
#include "stm32f3xx_hal.h"
#include "menus.h"
#include "Radio/RFM69.h"
#include "logger.h"
#include <tsc.h>
#include "Keyboard.h"
#include "irmenu.h"
#include "MessageState.h"
#include "leddc25.h"
//#include "bitarray.h"

DCDarkNetApp::DCDarkNetApp() :
		CurrentState(0) {

}

static const uint32_t TIME_BETWEEN_INITS = 100;

RFM69 Radio(RADIO_SPI3_NSS_Pin, RADIO_INTERRUPT_DIO0_EXTI4_Pin, true);
static const QKeyboard::PinConfig KBPins[] = {
		 {TSC_GROUP3_IO3,TSC_GROUP3_IO4, TSC_GROUP3_IDX} //1
		,{TSC_GROUP3_IO2,TSC_GROUP3_IO4, TSC_GROUP3_IDX} //2
		,{TSC_GROUP3_IO1,TSC_GROUP3_IO4, TSC_GROUP3_IDX} //3
		,{TSC_GROUP2_IO1,TSC_GROUP2_IO3, TSC_GROUP2_IDX} //4
		,{TSC_GROUP2_IO2,TSC_GROUP2_IO3, TSC_GROUP2_IDX} //5
		,{TSC_GROUP1_IO3,TSC_GROUP1_IO4, TSC_GROUP1_IDX} //6
		,{TSC_GROUP1_IO2,TSC_GROUP1_IO4, TSC_GROUP1_IDX} //7
		,{TSC_GROUP1_IO1,TSC_GROUP1_IO4, TSC_GROUP1_IDX} //8
		,{TSC_GROUP5_IO3,TSC_GROUP5_IO4, TSC_GROUP5_IDX} //9
		,{TSC_GROUP5_IO2,TSC_GROUP5_IO4, TSC_GROUP5_IDX} //0
		,{TSC_GROUP5_IO1,TSC_GROUP5_IO4, TSC_GROUP5_IDX} //hook
};

QKeyboard KB(&KBPins[0],sizeof(KBPins)/sizeof(KBPins[0]));

static const uint16_t SETTING_SECTOR = 57; //0x801C800
static const uint16_t FIRST_CONTACT_SECTOR = SETTING_SECTOR + 1;
static const uint16_t NUM_CONTACT_SECTOR = 64 - FIRST_CONTACT_SECTOR;
static const uint32_t MY_INFO_ADDRESS = 0x801FFD4; //2c needed

ContactStore MyContacts(SETTING_SECTOR, FIRST_CONTACT_SECTOR, NUM_CONTACT_SECTOR, MY_INFO_ADDRESS);

static void initFlash() {
}


static const uint32_t DISPLAY_WIDTH = 128;
static const uint32_t DISPLAY_HEIGHT = 160;
static const uint32_t DISPLAY_OPT_WRITE_ROWS = 2;
DisplayST7735 Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, DisplayST7735::PORTAIT);
uint16_t DrawBuffer[DISPLAY_WIDTH * DISPLAY_OPT_WRITE_ROWS]; //120 wide, 10 pixels high, 2 bytes per pixel
uint8_t DrawBufferRangeChange[DISPLAY_HEIGHT/DISPLAY_OPT_WRITE_ROWS+1];
DrawBufferNoBuffer NoBuffer(&Display,&DrawBuffer[0],DISPLAY_OPT_WRITE_ROWS);
static const uint8_t BITS_PER_PIXEL = 6;
uint8_t BackBuffer[((DISPLAY_WIDTH * DISPLAY_HEIGHT * BITS_PER_PIXEL)/8)+1];
DrawBuffer2D16BitColor DB2D16(DISPLAY_WIDTH,DISPLAY_HEIGHT,&BackBuffer[0],&DrawBuffer[0],DISPLAY_OPT_WRITE_ROWS,&DrawBufferRangeChange[0], &Display);
LedDC25 LedControl;

uint32_t DCDarkNetApp::init() {

	uint32_t retVal = 0;
	ErrorType et;

	initFlash(); //only used for development

	GUI_ListItemData items[4];
	GUI_ListData DrawList((const char *) "Self Check", items, uint8_t(0), uint8_t(0), uint8_t(128), uint8_t(64),
			uint8_t(0), uint8_t(0));
	//DO SELF CHECK
	if ((et = Display.init(DisplayST7735::FORMAT_16_BIT, DisplayST7735::ROW_COLUMN_ORDER, &Font_6x10,&DB2D16)).ok()) {
		HAL_Delay(1000);
		items[0].set(0, "OLED_INIT");
		DrawList.ItemsCount++;
		retVal |= COMPONENTS_ITEMS::LCD;
	}
	GUI gui(&Display);
	gui.drawList(&DrawList);
	Display.swap();

	HAL_Delay(TIME_BETWEEN_INITS);
	if (MyContacts.init()) {
		items[1].set(2, "FLASH MEM INIT");
		retVal |= COMPONENTS_ITEMS::FLASH_MEM;
	} else {
		items[1].set(2, "FLASH FAILED");
	}
	DrawList.ItemsCount++;
	DrawList.selectedItem++;
	gui.drawList(&DrawList);
	Display.swap();
	HAL_Delay(TIME_BETWEEN_INITS);

	//test for IR??
	if (Radio.initialize(RF69_915MHZ, MyContacts.getMyInfo().getUniqueID())) {
		items[2].set(1, "RADIO INIT");
		Radio.setPowerLevel(31);
		retVal |= COMPONENTS_ITEMS::RADIO;
	} else {
		items[2].set(1, "RADIO FAILED");
	}

	DrawList.ItemsCount++;
	DrawList.selectedItem++;
	gui.drawList(&DrawList);
	Display.swap();
	HAL_Delay(TIME_BETWEEN_INITS);

	Display.fillScreen(RGBColor::BLACK);
	Display.drawString(0,10,"#dcdn17");
	Display.drawString(0,40,"><>");
	Display.drawString(0,50,"   Cyberez Inc");
	Display.swap();
	HAL_Delay(3000);

	((IRState *)StateFactory::getIRPairingState())->BeTheBob();
	CurrentState = StateFactory::getMenuState();
	KB.resetLastPinTick();
	LedControl.setDanceType(LedDC25::INIT);
	return retVal;
}

void DCDarkNetApp::run() {
	static uint32_t tick = HAL_GetTick();

	KB.scan();
	if(KB.isDialerMode()) {
		if(KB.wasKeyReleased()) {
			LedControl.setDanceType(LedDC25::DIALER,KB.getLastKeyReleased());
		}
	}

	RunContext rc(&Display, &KB,&MyContacts, &Radio, &LedControl);

	ReturnStateContext rsc = CurrentState->run(rc);
	Display.swap();

	if (rsc.Err.ok()) {
		if (CurrentState != rsc.NextMenuToRun) {
			//on state switches reset keyboard and give a 1 second pause on reading from keyboard.
			KB.reset();
		}
		if (CurrentState != StateFactory::getGameOfLifeState() && (tick > KB.getLastPinSelectedTick())
				&& (tick - KB.getLastPinSelectedTick()
						> (1000 * 60 * rc.getContactStore().getSettings().getScreenSaverTime()))) {
			CurrentState->shutdown();
			CurrentState = StateFactory::getGameOfLifeState();
		} else {
			CurrentState = rsc.NextMenuToRun;
		}
	} else {
		CurrentState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Run State Error....", 2000);
	}
	if (MyContacts.getSettings().isNameSet()) {
		((IRState *)StateFactory::getIRPairingState())->ListenForAlice(rc);
	}
	LedControl.process();

	static uint32_t lastSendTime = 0;
	if (tick - lastSendTime > 10) {
		lastSendTime = tick;
		if (Radio.receiveDone()) {
			if (Radio.TARGETID == RF69_BROADCAST_ADDR) {
				((MessageState *)StateFactory::getMessageState())->addRadioMessage((const char *) &Radio.DATA[0], Radio.DATALEN,
						RF69_BROADCAST_ADDR, Radio.RSSI);
			} else {
				((MessageState *)StateFactory::getMessageState())->addRadioMessage((const char *) &Radio.DATA[0], Radio.DATALEN,
						Radio.SENDERID, Radio.RSSI);
			}
#ifndef DONT_USE_ACK
			if (Radio.ACK_REQUESTED && Radio.SENDERID != RF69_BROADCAST_ADDR) {
				Radio.sendACK("ACK", 4);
			}
#endif
		}
	}
}
