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

serialPacket_t cmdBuf;

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
    case 0:  
      //attention packet
      //we need to stop the updating of the LEDs;
      PL_stop();
      FMEM_managedStart(0);
      //send replay
      Serial.println("RDY");
      break;
    
    case 1: //continuous data
      FMEM_managedWrite(outbuf,dataLen);
      //send replay
      Serial.println("NXT");
      break;

    case 2: //last packet
      //we can start the PL aganin
      PL_setStartAddress(0);
      PL_start();
      //send replay
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