#include "dcdarknet_app.h"
#include "display_device.h"

DCDarkNetApp::DCDarkNetApp() {

}

DisplayST7735 Display(128,160, DisplayST7735::PORTAIT);

ErrorType DCDarkNetApp::init() {
	ErrorType et;
	et = Display.init();
	return et;
}

void DCDarkNetApp::run() {

}
