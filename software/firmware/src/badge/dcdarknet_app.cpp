#include "dcdarknet_app.h"
#include "display_device.h"
#include "stm32f3xx_hal.h"
#include "menus.h"

DCDarkNetApp::DCDarkNetApp() :
		CurrentState(0) {

}

DisplayST7735 Display(128, 160, DisplayST7735::PORTAIT);

ErrorType DCDarkNetApp::init() {
	ErrorType et;
	et = Display.init();

	//blink status led a few times
	for (int i = 0; i < 5; i++) {
		HAL_GPIO_TogglePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin);
		HAL_GPIO_TogglePin(LED7_GPIO_Port, LED7_Pin);
		HAL_GPIO_TogglePin(LED8_GPIO_Port, LED8_Pin);
		HAL_GPIO_TogglePin(LED9_GPIO_Port, LED9_Pin);
		HAL_GPIO_TogglePin(LED10_GPIO_Port, LED10_Pin);
		HAL_Delay(500);
	}

#if 0
	uint32_t retVal = 0;
	initFlash();

	GUI_ListItemData items[4];
	GUI_ListData DrawList((const char *) "Self Check", items, uint8_t(0), uint8_t(0), uint8_t(128), uint8_t(64),
			uint8_t(0), uint8_t(0));
	//DO SELF CHECK
	if (gui_init()) {
		delay(1000);
		items[0].set(0, "OLED_INIT");
		DrawList.ItemsCount++;
		retVal |= COMPONENTS_ITEMS::OLED;
		gui_set_curList(&DrawList);
	}
	gui_draw();
	delay(TIME_BETWEEN_INITS);
	if (MyContacts.init()) {
		items[1].set(2, "FLASH MEM INIT");
		retVal |= COMPONENTS_ITEMS::FLASH_MEM;
	} else {
		items[1].set(2, "FLASH FAILED");
	}
	DrawList.ItemsCount++;
	gui_draw();
	delay(TIME_BETWEEN_INITS);

	//test for IR??
	if (Radio.initialize(RF69_915MHZ, getContactStore().getMyInfo().getUniqueID())) {
		items[2].set(1, "RADIO INIT");
		Radio.setPowerLevel(31);
		retVal |= COMPONENTS_ITEMS::RADIO;
	} else {
		items[2].set(1, "RADIO FAILED");
	}

	DrawList.ItemsCount++;
	delay(TIME_BETWEEN_INITS);
	gui_draw();
	delay(1000);
	StateFactory::getIRPairingState()->BeTheBob();
	CurrentState = StateFactory::getMenuState();
	KB.resetLastPinTick();
#endif
	//GUI gui(&Display);
	Display.drawString(0,10,(const char *)"#dcdn17",RGBColor::BLUE,RGBColor::BLACK,2,false);
	Display.drawString(20,80,(const char *)"Cyberez Inc",RGBColor::BLUE,RGBColor::BLACK,1,false);
	HAL_Delay(1000);

	//CurrentState = StateFactory::getMenuState();
	CurrentState = StateFactory::getMenu3D();
	return et;
}

void DCDarkNetApp::run() {
	static uint32_t time = HAL_GetTick();
	static bool bStart = true;

	//check to see if keyboard should be ignored
	//uint32_t tick = HAL_GetTick();
	//KB.scan();
	RunContext rc(&Display);

	ReturnStateContext rsc = CurrentState->run(rc);

	if (rsc.Err.ok()) {
		if (CurrentState != rsc.NextMenuToRun) {
			//on state switches reset keyboard and give a 1 second pause on reading from keyboard.
			//KB.reset();
		}
		//if (CurrentState != StateFactory::getGameOfLifeState() && (tick > KB.getLastPinSelectedTick())
		//		&& (tick - KB.getLastPinSelectedTick()
		//				> (1000 * 60 * getContactStore().getSettings().getScreenSaverTime()))) {
		//	CurrentState->shutdown();
		//	CurrentState = StateFactory::getGameOfLifeState();
		//} else {
		CurrentState = rsc.NextMenuToRun;
		//}
	} else {
		CurrentState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Run State Error....", 2000);
	}
#if 0
	if (getContactStore().getSettings().isNameSet()) {
		StateFactory::getIRPairingState()->ListenForAlice();
	}
	StateFactory::getMessageState()->blink();

	static uint32_t lastSendTime = 0;
	if (tick - lastSendTime > 10) {
		lastSendTime = tick;
		if (Radio.receiveDone()) {
			if (Radio.TARGETID == RF69_BROADCAST_ADDR) {
				StateFactory::getMessageState()->addRadioMessage((const char *) &Radio.DATA[0], Radio.DATALEN,
						RF69_BROADCAST_ADDR, Radio.RSSI);
			} else {
				StateFactory::getMessageState()->addRadioMessage((const char *) &Radio.DATA[0], Radio.DATALEN,
						Radio.SENDERID, Radio.RSSI);
			}
#ifndef DONT_USE_ACK
			if (Radio.ACK_REQUESTED && Radio.SENDERID != RF69_BROADCAST_ADDR) {
				Radio.sendACK("ACK", 4);
			}
#endif
		}
	}
#endif
}
