#include "FMEM.h"

int m_pin;
uint8_t m_initalized = 0;
uint32_t m_nexAddress =0;
#define CS_START digitalWrite(m_pin, LOW);
#define CS_STOP  digitalWrite(m_pin, HIGH);

#define FMEM_CMD_READ               0x03
#define FMEM_CMD_WRITE_ENABLE       0x06
#define FMEM_CMD_WRITE_DISABLE      0x04
#define FMEM_CMD_READ_STATUS        0x05
#define FMEM_CMD_SECTOR_ERASE       0x20
#define FMEM_CMD_CHIP_ERASE         0x60
#define FMEM_CMD_PAGE_PROGRAM       0x02

void FMEM_init(int pin_number){
    
    static uint8_t initalized = 0;
    if (initalized) return;
    m_pin = pin_number;
    pinMode (m_pin, OUTPUT);
    CS_STOP;

    SPI.begin();
    SPI.setClockDivider(1);

    initalized = 1;

}
/* returns is the device busy, a single status read */
uint8_t FMEM_isBusy(){
    CS_START;
    uint8_t stat = SPI.transfer(FMEM_CMD_READ_STATUS);
    stat = SPI.transfer(0xFF);
    CS_STOP;

    return stat & 1;
}

uint8_t FMEM_getStatus(){
    CS_START;
    uint8_t stat = SPI.transfer(FMEM_CMD_READ_STATUS);
    stat = SPI.transfer(0xFF);
    CS_STOP;

    return stat;
}

/* waits untilt the flash is 
done or untils timeput is done */
uint8_t FMEM_waitUntilDone(uint16_t ms_timeout){
    CS_START;
    int done = 0;
    int stat = 0;
    SPI.transfer(FMEM_CMD_READ_STATUS);
    while ((ms_timeout-->0) && (!done)) {
        stat = SPI.transfer(0xFF);
        done = !(stat & 0x1);
        delay(1);
    }
    CS_STOP;
    return !done;
}


void FMEM_enableWrite(){
    //we should make sure that the device isnt doing something currently
    CS_START;
    SPI.transfer(FMEM_CMD_WRITE_ENABLE);
    CS_STOP;
}

int FMEM_eraseSector4K(uint32_t addr){
    int stat;
    if (FMEM_waitUntilDone(1000)==0){
        FMEM_enableWrite();
    
        CS_START;
        SPI.transfer(FMEM_CMD_SECTOR_ERASE);
        addr = addr & 0xFFF000; //only 24bits minus the last 4k 
        SPI.transfer(addr>>16);
        SPI.transfer(addr>>8);
        SPI.transfer(addr);
        CS_STOP;

        stat = FMEM_waitUntilDone(200);
    } else {
        stat = -1;
    }
    return stat;

}

/* writes elements up to 256 or until the end of the page */
/* function returns the number of bytes writen or -1 in case of an error*/

uint32_t FMEM_writePage(uint32_t addr, uint8_t* data, uint32_t len){
    int stat;
    if ((addr & 0xFF) + len > 256) len = 256 - (addr & 0xFF);

    if (FMEM_waitUntilDone(1000)==0){
        FMEM_enableWrite();
        
        CS_START;
        SPI.transfer(FMEM_CMD_PAGE_PROGRAM);
        SPI.transfer(addr>>16);
        SPI.transfer(addr>>8);
        SPI.transfer(addr);

        for (uint32_t i =0; i<len; i++){
            SPI.transfer(*data++);
        }
        CS_STOP;
        stat = FMEM_waitUntilDone(2);
        if (!stat) len = -1;

    } else {
        len = -1;
    }
    return len;

}

int FMEM_read(uint32_t addr, uint8_t* data, uint32_t len){
    int stat;
     if (FMEM_waitUntilDone(1000)==0){
         CS_START;
         SPI.transfer(FMEM_CMD_READ);
         SPI.transfer(addr>>16);
         SPI.transfer(addr>>8);
         SPI.transfer(addr);
         for (uint32_t i=0; i<len; i++){
             *data++ = SPI.transfer(0xFF);
         }
         CS_STOP;
        stat = 0;   //finisehd successfully
     } else {
        stat = -1;   //timed out waiting for the flash to recover from busy
     }

    return stat;
}

int FMEM_managedStart(uint32_t addr)
{
    m_nexAddress = addr;
}

int FMEM_managedWrite(uint8_t* data, uint32_t len)
{
    int16_t stat = 0;
    do {
        //check if we are at the beginning of a sector
        if (m_nexAddress & 0xFFF == 0){
            Serial.println("Erasing sector");
            FMEM_eraseSector4K(m_nexAddress);
        }
        stat = FMEM_writePage(m_nexAddress,data,len);
        if (stat > 0){
            m_nexAddress +=stat;
            data += stat;
            len -= stat;

            Serial.print("Written bytes:");
            Serial.println(stat);
            Serial.print("Next Address:");
            Serial.println(m_nexAddress);
            Serial.print("Remaining len:");
            Serial.println(len);
        }

    } while (stat > 0);
}