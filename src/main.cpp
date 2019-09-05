#include <Arduino.h>

#include <PacketEncoder.h>

#include <PixelLoader.h>


#define PRT(x) Serial.println(x);
#define PRTVAL(des,val) Serial.print(des);Serial.println(val);
#define LED_PIN 6

#include "FMEM.h"

#define SERIAL_SPEED 250000
#define RX_SERIALBUF_MAXSIZE 20

uint8_t rxCmdBuf[RX_SERIALBUF_MAXSIZE];

struct serialPacket_t {
	uint8_t *packet;
	uint16_t packetMaxSize;
	uint16_t rxBufPos;
};

serialPacket_t cmdBuf;


// return	0 packet ready
//			1 no packet being received
//      2 packet being received
//			100 framing error
int8_t loadPacket(struct serialPacket_t *pkg) {
	uint8_t ch;
	int8_t stat = 1;		//no packet being received
	while (Serial.available()) {
    stat = 2;
		ch = Serial.read();
		if (ch == START_CHAR) {
     
      pkg->rxBufPos = 0;

		}else if (ch == END_CHAR) {
			if (pkg->packet[0] == START_CHAR) stat = 0; else stat = 100;
			pkg->packet[pkg->rxBufPos] = ch;
			pkg->rxBufPos = 0;
			break;
		}

		pkg->packet[pkg->rxBufPos] = ch;
		pkg->rxBufPos++;
		if (pkg->rxBufPos == pkg->packetMaxSize) pkg->rxBufPos = 0;

		
	}

	return stat;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  
  PRT("Starting up");
  
  PL_init(0,9);
  
  PL_setStartAddress(0);
  PL_start();

  cmdBuf.packet = rxCmdBuf;
  cmdBuf.packetMaxSize = RX_SERIALBUF_MAXSIZE;
  cmdBuf.rxBufPos = 0;
  
}

uint8_t inputbuf[515];
int8_t stat;


void PDL_process(uint8_t *inbuf, uint8_t *outbuf){
  uint16_t dataLen;
  uint8_t stat = decodePacket(inbuf, outbuf, &dataLen);
  Serial.print("Len: ");
  Serial.println(dataLen);
  Serial.print("BUF:");
  Serial.println(outbuf[0]);
  Serial.println(stat);
  if (stat == 0){

    switch (outbuf[0])
    {
    case 0:  //attention packet
      Serial.println("RDY");
      break;
    
    case 1: //continuous data
      Serial.println("NXT");
      break;

    case 2: //last packet
      Serial.println("END");

    default:
      break;
    }
  }

}


void loop() {

  PL_tick();

  stat = loadPacket(&cmdBuf);

  if (stat == 0){
      PDL_process(cmdBuf.packet, inputbuf);  
  } 
}