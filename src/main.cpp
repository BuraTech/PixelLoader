/* For testing over serial 
 Beginning of Transfer command 117 0 2 0 126 for start of transfer
 DATA transfer command: 117 0 2+pixelLen 1 <pixels> 126 for data after start
 END transfer command:  117 0 2 2 126 

pixel data 
10 reds, 0 index, 5sec: 
encoded:117 0 40 1 0xA 0 0 0 0x88 0x13 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 126

10 blues 0 index 5 sec, loop: 
encoded: 117 0 40 1 0xA 0x40 0 0 0x88 0x13 0 0 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 0 0 255 126
 
 */



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

uint8_t inputbuf[517]={"Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing softw!"};
int8_t stat;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  //while (1);
  PRT("Starting up");
  PL_init(0,9);
  
  PL_setStartAddress(0);
  //PL_start();

  cmdBuf.packet = rxCmdBuf;
  cmdBuf.packetMaxSize = RX_SERIALBUF_MAXSIZE;
  cmdBuf.rxBufPos = 0;

}




void PDL_process(uint8_t *inbuf, uint8_t *outbuf){
  uint16_t dataLen;
  uint8_t stat = decodePacket(inbuf, outbuf, &dataLen);

  frameHeader_t* h = (frameHeader_t*)&outbuf[1];

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
      cmdBuf.packet = inputbuf;
      cmdBuf.packetMaxSize = 517;
      //send replay
      Serial.println("RDY");
      break;
    
    case 1: //continuous data
      
      // Serial.print("Pixel Num");
      // Serial.println(h->ledNumFlags);
      // Serial.print("Duration");
      // Serial.println(h->duration);
      FMEM_managedWrite(&outbuf[1],dataLen-1);
      //send replay
      Serial.println("NXT");
      break;

    case 2: //last packet
      //we can start the PL aganin
      PL_setStartAddress(0);
      PL_start();
      cmdBuf.packet = rxCmdBuf;
      cmdBuf.packetMaxSize = RX_SERIALBUF_MAXSIZE;
      //send replay
      Serial.println("END");
      break;
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