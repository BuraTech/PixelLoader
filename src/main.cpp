#include <Arduino.h>
#include <PixelLoader.h>

#define PRT(x) Serial.println(x);
#define PRTVAL(des,val) Serial.print(des);Serial.println(val);
#define LED_PIN 6

#include "FMEM.h"



uint8_t charTest[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has s!";
uint8_t charRead[256];


uint8_t Strip1[] = {255,0,0,
                    255,0,0,
                    255,0,0,
                    255,0,0,
                    255,0,0,
                    255,0,0,
                    255,0,0,
                    255,0,0,
                    255,0,0,
                    255,0,0,
                    };

uint8_t Strip2[] = {0,255,0,
                    0,255,0,
                    0,255,0,
                    0,255,0,
                    0,255,0,
                    0,255,0,
                    0,255,0,
                    0,255,0,
                    0,255,0,
                    0,255,0,
                    };

uint8_t Strip3[] = {0,0,255,
                    0,0,255,
                    0,0,255,
                    0,0,255,
                    0,0,255,
                    0,0,255,
                    0,0,255,
                    0,0,255,
                    0,0,255,
                    0,0,255,
                    };

frameHeader_t f1;
frameHeader_t f2;

uint8_t tmp[256];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  
  PRT("Starting up");
  
  PL_init(0,9);

  f1.duration = 5000;
  f1.index = 0;
  f1.ledNumFlags = 10;

  f2.duration = 5000;
  f2.index = 0;
  f2.ledNumFlags = PL_LOOP_FLAG + 10;

  // loading the colors

  //  FMEM_eraseSector4K(0);
  //  FMEM_writePage(0,(uint8_t*)&f1,8);
  //  FMEM_writePage(8,Strip1,sizeof(Strip1));

  //  FMEM_writePage(8+sizeof(Strip1),(uint8_t*)&f1,8);
  //  FMEM_writePage(16+sizeof(Strip1),Strip3,sizeof(Strip3));

  //  FMEM_writePage(16+2*sizeof(Strip1),(uint8_t*)&f2,8);
  //  FMEM_writePage(24+2*sizeof(Strip2),Strip2,sizeof(Strip2));



  FMEM_read(0,tmp,256);
  for (int i=0; i<256;i++){
    Serial.print(tmp[i]);
    Serial.print(" ");
  }
  

 //  while(1);
  PL_setStartAddress(0);
  PL_start();
}

uint8_t inputbuf[515];
int cnt =0;

void loop() {

  PL_tick();

  // put your main code here, to run repeatedly:
  //PRTVAL("status:",FMEM_getStatus());
  //delay(5000);

  // PRTVAL("Erasing:",FMEM_eraseSector4K(0));
  // PRTVAL("Status writing:",FMEM_writePage(256,charTest,256)); 

  // PRTVAL("Reading",FMEM_read(256,charRead,256));

  //Serial.write(charRead,256);

  //while (1);
  
  // if (Serial.available()){
  //   inputbuf[cnt++] = Serial.read();
  //   if (cnt == 515){
  //      Serial.write(inputbuf,cnt);
  //      cnt =0;
  //   }
  // }  



  // pixels.clear();
  // for (uint16_t i=0; i<m_pixNumber; i++){

  //   pixels.setPixelColor(i, pixels.Color(0,150,150));

  //   pixels.show();

  //   delay(1000);
  // }

}