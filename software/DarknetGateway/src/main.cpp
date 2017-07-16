#include "RPIHardware.h"
#include "RFM69.h"
#include <cstdio>
#include <signal.h>
#include <curl/curl.h>
#include <cstring>
#include <tbb/include/tbb/concurrent_queue.h>
#include <tbb/include/tbb/task_scheduler_init.h>
#include <tbb/include/tbb/task.h>

volatile sig_atomic_t force_exit = false;

#define CS_PIN  RPI_V2_GPIO_P1_37 // Slave Select on GPIO26  so P1 connector pin #37
#define IRQ_PIN RPI_V2_GPIO_P1_16 // IRQ on GPIO23 so P1 connector pin #16
#define RST_PIN RPI_V2_GPIO_P1_33 // reset on GPIO13 so P1 connector pin #33

RPISPI RPISPI1(0,CS_PIN);
RPIHardware Device(&RPISPI1,0);

Hardware &Hardware::get() {
	return Device;
}

void sig_handler(int sig) {
  printf("\n%d Break received, exiting!\n", sig);
  force_exit=true;
}

struct HandledCurlData {
	size_t Size;
	HandledCurlData(size_t s) : Size(s) {}
};

size_t handleCurl(void *ptr, size_t size, size_t nmemb, HandledCurlData *hcd) {
	hcd->Size+=size*nmemb;
	printf("in handle: %s\n", (char *)ptr);
	return size*nmemb;
}

#define MAX_DATA_LEN 62
class  WorkItem {
protected:
	enum COMMAND {
		GET_REP = 0xFF12
		, EPICS_COMPLETED = 0xFF13
		, RANDOM = 0xFF14
	};
public:
	WorkItem(const uint8_t *d, uint8_t dataSize, uint16_t sid, int16_t rssi)
		: MsgSize(dataSize), SenderID(sid), RSSI(rssi), RS() {
		memcpy(&Data[0],d,dataSize);
	}
	bool isValid() {
		if(MsgSize>3) {
			RS.Command = *((int*)&Data[0]);
			switch(RS.Command) {
			case GET_REP: EPICS_COMPLETED:
				return true;
				break;
			}
		}
		return false;
	}
	bool doWork() {
		switch(RS.Command) {
			case GET_REP: 
					RS.RetVal1 = 10;	
				break;
			case EPICS_COMPLETED:
					RS.RetVal1 = 100;	
				break;
		}
		return RS.Success;
	}
	void *getResultBuffer() {
		return &RS;
	}
	uint32_t getResultSize() {
		return sizeof(RS);
	}

	uint16_t getSender() {
		return SenderID;
	}
	
	struct ResponseStruct {
		uint32_t Command;
		uint8_t  Success;
		uint32_t RetVal1;
		ResponseStruct() : Command(0), Success(0), RetVal1(0) {}
	} __attribute__((packed));

	void getResponse() {
		RS.Success=1;
#if 0
		HandledCurlData handledCurlData(0);
		CURL *hnd = curl_easy_init();
		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(hnd, CURLOPT_URL, "https://echo.getpostman.com/post");
		curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, handleCurl);
		curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &handledCurlData);
 
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "postman-token: 62c5f82f-64e3-0407-e673-7d26ed68e109");
		headers = curl_slist_append(headers, "cache-control: no-cache");
		headers = curl_slist_append(headers, "content-type: text/plain");
		curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "Duis posuere volutpat.");
		CURLcode ret = curl_easy_perform(hnd);
		curl_easy_cleanup(hnd);
#endif
	}
private:
	//request data
	uint8_t Data[MAX_DATA_LEN];
	uint8_t MsgSize;
	uint16_t SenderID;
	int16_t RSSI;
	//response data
	ResponseStruct RS;
};

tbb::concurrent_queue<WorkItem*> OutGoingWorkQueue;

class WorkTask : public tbb::task {
public:
	WorkTask(WorkItem *wi) : WI(wi) {}
	tbb::task *execute() {
		WI->getResponse();
		return null;
	}
private:
	WorkItem *WI;
};

int main(int arc, char *agrv[]) {
	signal(SIGINT, sig_handler);

	tbb::task_scheduler_init tsi;

	RPISPI1.init(SPI_1_3125MHZ,MSB,SPI_MODE0);
	RFM69 Radio(false,IRQ_PIN);
	bool b = Radio.initialize(RF69_915MHZ,0xFFFE,1);
	if(b) {
		Radio.readAllRegs();
		printf("RSSI:  %d \n", Radio.readRSSI());
		printf("Gain:  %d \n", Radio.getCurrentGain());
		printf("ImpedenceLevel:  %d \n", Radio.getImpedenceLevel());
		printf("RSSIThreshHold:  %d \n", Radio.getRSSIThreshHold());
	} else {
		printf("init failed\n");
	}

	curl_global_init(CURL_GLOBAL_ALL);
	while(!force_exit) {
		if(Radio.receiveDone()) {
			WorkItem wi((const uint8_t*)&Radio.DATA[0],Radio.DATALEN, Radio.SENDERID, Radio.RSSI);
			if(wi.isValid()) {
				
			}
		}
		WorkItem *wi = 0;
		int count=0;
		while(Radio.canSend() && count++<10 && OutGoingWorkQueue.try_pop(wi)) {
			Radio.send(wi->getSender(),wi->getResultBuffer(),wi->getResultSize());
		}	
	}
	curl_global_cleanup();
	return 0;
}
