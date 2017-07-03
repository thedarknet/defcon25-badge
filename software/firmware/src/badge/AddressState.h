#ifndef ADDRESS_STATE_H
#define ADDRESS_STATE_H

#include "menus.h"

class AddressState: public StateBase {
public:
	AddressState();
	virtual ~AddressState();
	void resetSelection();
protected:
	virtual ErrorType onInit(RunContext &rc);
	virtual ReturnStateContext onRun(RunContext &rc);
	virtual ErrorType onShutdown();
	void setNext4Items(uint16_t startAt, RunContext &rc);
private:
	GUI_ListData AddressList;
	GUI_ListItemData Items[4];
	ContactStore::Contact CurrentContactList[4];
	GUI_ListData ContactDetails;
	GUI_ListItemData DetailItems[5];
	char RadioIDBuf[12];
	char PublicKey[56];
	char SignatureKey[108];
	uint8_t Index;
	GUI_ListData *DisplayList;
};


#endif
