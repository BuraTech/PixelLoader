#ifndef __FMEM_H
#define __FMEM_H

#include <Arduino.h>
#include <SPI.h>


void FMEM_init(int pin_number);

/* returns the entire status byte */
uint8_t FMEM_getStatus();

/* enable writing, necessary before any
erase or program funcitons */
void FMEM_enableWrite();

/* Erases the 4k sector that includes the target address */
int FMEM_eraseSector4K(uint32_t addr);

int FMEM_writePage(uint32_t addr, uint8_t* data, uint32_t len);

int FMEM_read(uint32_t addr, uint8_t* data, uint32_t len);

int FMEM_managedStart(uint32_t addr);

int FMEM_managedWrite(uint8_t* data, uint32_t len);

#endif