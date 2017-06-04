#include "RPIHardware.h"
#include "RFM69.h"
#include <cstdio>

int main(int arc, char *agrv[]) {
	RFM69 Radio(false,3);
	bool b = Radio.initialize(RF69_915MHZ,1,1);
	if(b) {
		Radio.readAllRegs();
		printf("RSSI:  %d \n", Radio.readRSSI());
		printf("Gain:  %d \n", Radio.getCurrentGain());
		printf("ImpedenceLevel:  %d \n", Radio.getImpedenceLevel());
		printf("RSSIThreshHold:  %d \n", Radio.getRSSIThreshHold());
	} else {
		printf("init failed\n");
	}
	return 0;
}
