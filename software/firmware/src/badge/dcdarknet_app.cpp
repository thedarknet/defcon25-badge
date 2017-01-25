#include "dcdarknet_app.h"
#include "display_device.h"

DCDarkNetApp::DCDarkNetApp() {

}

DisplayST7735 Display(128,128);

ErrorType DCDarkNetApp::init() {
	ErrorType et;
	et = Display.init();
	return et;
}

