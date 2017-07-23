// rf69_server.cpp
//
// Example program showing how to use RH_RF69 on Raspberry Pi
// Uses the bcm2835 library to access the GPIO pins to drive the RFM69 module
// Requires bcm2835 library to be already installed
// http://www.airspayce.com/mikem/bcm2835/
// Use the Makefile in this directory:
// cd example/raspi/rf69
// make
// sudo ./rf69_server
//
// Contributed by Charles-Henri Hallard based on sample RH_NRF24 by Mike Poublon

#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "RFM69.h"
#include "RPIHardware.h"

// define hardware used change to fit your need
// Uncomment the board you have, if not listed 
// uncommment custom board and set wiring tin custom section

// LoRasPi board 
// see https://github.com/hallard/LoRasPI
#define BOARD_LORASPI

// iC880A and LinkLab Lora Gateway Shield (if RF module plugged into)
// see https://github.com/ch2i/iC880A-Raspberry-PI
//#define BOARD_IC880A_PLATE

// Raspberri PI Lora Gateway for multiple modules 
// see https://github.com/hallard/RPI-Lora-Gateway
//#define BOARD_PI_LORA_GATEWAY

// Dragino Raspberry PI hat
// see https://github.com/dragino/Lora
//#define BOARD_DRAGINO_PIHAT

// Now we include RasPi_Boards.h so this will expose defined 
// constants with CS/IRQ/RESET/on board LED pins definition
//#include "../RasPiBoards.h"
#define RF_CS_PIN  RPI_V2_GPIO_P1_37 // Slave Select on CE0 so P1 connector pin #24
#define RF_IRQ_PIN RPI_V2_GPIO_P1_16 // IRQ on GPIO25 so P1 connector pin #22
#define RF_RST_PIN RPI_V2_GPIO_P1_33 // IRQ on GPIO22 so P1 connector pin #15
#define CS_PIN  RPI_V2_GPIO_P1_37 // Slave Select on GPIO26  so P1 connector pin #37
#define IRQ_PIN RPI_V2_GPIO_P1_16 // IRQ on GPIO23 so P1 connector pin #16
#define RST_PIN RPI_V2_GPIO_P1_33 // reset on GPIO13 so P1 connector pin #33

// Our RFM69 Configuration 
//#define RF_FREQUENCY  433.00
#define RF_FREQUENCY  915.00
#define RF_NODE_ID    0xFFFE  // We're node ID 1 (Gateway)
#define RF_GROUP_ID   200 // Moteino default is 100, I'm using 69 on all my house

// Create an instance of a driver
RFM69 rf69(RF_CS_PIN, RF_IRQ_PIN);
//RH_RF69 rf69(RF_CS_PIN);

//Flag for Ctrl-C
volatile sig_atomic_t force_exit = false;

void sig_handler(int sig)
{
  //printf("\n%s Break received, exiting!\n", __BASEFILE__);
  force_exit=true;
}
RPISPI RPISPI1(0,CS_PIN);
RPIHardware Device(&RPISPI1,0);

Hardware &Hardware::get() {
	return Device;
}

#ifndef OUTPUT
  #define OUTPUT BCM2835_GPIO_FSEL_OUTP
#endif

#ifndef INPUT
  #define INPUT BCM2835_GPIO_FSEL_INPT
#endif

void pinMode(unsigned char pin, unsigned char mode)
{
	if (mode == OUTPUT) {
		bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_OUTP);
	} else {
		bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_INPT);
	}
}

//Main Function
int main (int argc, const char* argv[] )
{
  unsigned long led_blink = 0;
  RPISPI1.init(SPI_1_3125MHZ,MSB,SPI_MODE0); 
  
  signal(SIGINT, sig_handler);
  //printf( "%s\n", __BASEFILE__);

  if (!bcm2835_init()) {
    //fprintf( stderr, "%s bcm2835_init() Failed\n\n", __BASEFILE__ );
    return 1;
  }
  
  printf( "RF69 CS=GPIO%d", RF_CS_PIN);

#ifdef RF_LED_PIN
  pinMode(RF_LED_PIN, OUTPUT);
  //digitalWrite(RF_LED_PIN, HIGH );
#endif

#ifdef RF_IRQ_PIN
  printf( ", IRQ=GPIO%d", RF_IRQ_PIN );
  // IRQ Pin input/pull down
  pinMode(RF_IRQ_PIN, INPUT);
  bcm2835_gpio_set_pud(RF_IRQ_PIN, BCM2835_GPIO_PUD_DOWN);
  // Now we can enable Rising edge detection
  bcm2835_gpio_ren(RF_IRQ_PIN);
#endif
  
#ifdef RF_RST_PIN
  printf( ", RST=GPIO%d", RF_RST_PIN );
  // Pulse a reset on module
  pinMode(RF_RST_PIN, OUTPUT);
  //digitalWrite(RF_RST_PIN, LOW );
  bcm2835_delay(150);
  //digitalWrite(RF_RST_PIN, HIGH );
  bcm2835_delay(100);
#endif

#ifdef RF_LED_PIN
  printf( ", LED=GPIO%d", RF_LED_PIN );
  //digitalWrite(RF_LED_PIN, LOW );
#endif

  if (!rf69.initialize(RF69_915MHZ,0xFFFE,200)) {
    fprintf( stderr, "\nRF69 module init failed, Please verify wiring/module\n" );
  } else {
    printf( "\nRF69 module seen OK!\r\n");


    
    // Defaults after init are 434.0MHz, +13dBm, no encryption, GFSK=250kbps, Fdev=250kHz

    // The default transmitter power is 13dBm. If you are using
    // High power version (RFM69HW or RFM69HCW) you need to set 
    // transmitter power to at least 14 dBm up to 20dBm
    //rf69.setTxPower(20); 

    // Now we change back to Moteino setting to be 
    // compatible with RFM69 library from lowpowerlabs 
    //rf69.setModemConfig( RH_RF69::FSK_MOTEINO);

    // set Network ID (by sync words)
    uint8_t syncwords[2];
    syncwords[0] = 0x2d;
    syncwords[1] = RF_GROUP_ID;
    //rf69.setSyncWords(syncwords, sizeof(syncwords));

    // Adjust Frequency
    //rf69.setFrequency( RF_FREQUENCY );

    // This is our Node ID
    //rf69.setThisAddress(RF_NODE_ID);
    //rf69.setHeaderFrom(RF_NODE_ID);
    
    printf("RF69 GroupID=%d, node #%d init OK @ %3.2fMHz\n", RF_GROUP_ID, RF_NODE_ID, RF_FREQUENCY );

    // Be sure to grab all node packet 
    // we're sniffing to display, it's a demo
    //rf69.setPromiscuous(true);

    // We're ready to listen for incoming message
    //rf69.setModeRx();
	 rf69.receiveDone();

    printf( " OK NodeID=%d @ %3.2fMHz\n", RF_NODE_ID, RF_FREQUENCY );
    printf( "Listening packet...\n" );

    //Begin the main body of code
    while (!force_exit) {
      
#ifdef RF_IRQ_PIN
      // We have a IRQ pin ,pool it instead reading
      // Modules IRQ registers from SPI in each loop
      
      // Rising edge fired ?
      if (bcm2835_gpio_eds(RF_IRQ_PIN)) {
        // Now clear the eds flag by setting it to 1
        bcm2835_gpio_set_eds(RF_IRQ_PIN);
		  RFM69::isr0();
        //printf("Packet Received, Rising event detect for pin GPIO%d\n", RF_IRQ_PIN);
#endif

        //if (rf69.available()) { 
#ifdef RF_LED_PIN
          led_blink = millis();
          //digitalWrite(RF_LED_PIN, HIGH);
#endif
          // Should be a message for us now
          uint8_t buf[61];
          uint8_t len  = sizeof(buf);
          uint16_t from = rf69.SENDERID;
          uint16_t to   = rf69.TARGETID;
          //uint8_t id   = rf69.headerId();
          //uint8_t flags= rf69.headerFlags();;
          int8_t rssi  = rf69.RSSI;
          
          //if (rf69.recv(buf, &len)) {
            printf("Packet[%02d] #%d => #%d %ddB: ", len, from, to, rssi);
            //printbuffer(buf, len);
          //} else {
            //Serial.print("receive failed");
          //}
          printf("\n");
        //}
        
#ifdef RF_IRQ_PIN
      }
#endif
      
#ifdef RF_LED_PIN
      // Led blink timer expiration ?
      if (led_blink && millis()-led_blink>200) {
        led_blink = 0;
        //ndigitalWrite(RF_LED_PIN, LOW);
      }
#endif
      // Let OS doing other tasks
      // For timed critical appliation you can reduce or delete
      // this delay, but this will charge CPU usage, take care and monitor
      bcm2835_delay(5);
    }
  }

#ifdef RF_LED_PIN
  //digitalWrite(RF_LED_PIN, LOW );
#endif
  //printf( "\n%s Ending\n", __BASEFILE__ );
  bcm2835_close();
  return 0;
}

