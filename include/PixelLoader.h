#ifndef __PIXELLOADER_H__
#define __PIXELLOADER_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <FMEM.h>

#define PL_MAX_PIXELS 16
#define PL_PIN         6
#define PL_ENDFRAME_FLAG (1<<15)
#define PL_LOOP_FLAG     (1<<14)

struct frameHeader_t {
    uint16_t ledNumFlags; //3 Flags are in the top 3 bits the rest is for the len
    uint16_t index;
    uint32_t duration;
};



void PL_init(uint32_t addr, int memoryCSPin);

void PL_tick();

void PL_setStartAddress(uint32_t addr);

void PL_start();

void PL_stop();

#endif