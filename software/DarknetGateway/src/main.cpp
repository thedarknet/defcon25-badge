#include "RPIHardware.h"
#include "RFM69.h"
#include <cstdio>
#include <signal.h>
#include <curl/curl.h>

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

int main(int arc, char *agrv[]) {
	signal(SIGINT, sig_handler);
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


	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);
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
	curl_global_cleanup();
	return 0;
}
