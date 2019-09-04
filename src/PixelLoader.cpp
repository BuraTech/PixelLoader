// for next version we will have to make this much more versatile
// as we cannot fit both communication and everyting in RAM if we dont reuse the space

#include "PixelLoader.h"

struct frame_t {
    frameHeader_t frame;
    uint8_t colArray[PL_MAX_PIXELS*3];
};

struct frame_t  m_frame;
long            m_lastTime = 0;
uint32_t        m_startAddress =0;
uint32_t        m_nextAddress = 0;
uint8_t         m_running  = 0;


Adafruit_NeoPixel m_pixels(PL_MAX_PIXELS, PL_PIN, NEO_GRB + NEO_KHZ800);

void PL_init(uint32_t addr, int memoryCSPin){
    FMEM_init(memoryCSPin);
    m_pixels.begin();
    PL_setStartAddress(addr);
}

void PL_setStartAddress(uint32_t addr){
    m_startAddress = addr;
    m_nextAddress = addr;
}

void PL_start(){
    m_running = 1;
    m_lastTime = millis();
    m_frame.frame.duration = 0;
    
}

void PL_stop(){
    m_running = 0;
}

void PL_applyFrame(struct frame_t* frame){
    uint16_t i=0;
    uint16_t pixCount = (frame->frame.ledNumFlags) & 0x1FF;
    uint8_t* colArray = frame->colArray;


    m_pixels.clear();
    for (i= frame->frame.index; i < pixCount; i++){
        uint8_t R = *colArray++;
        uint8_t G = *colArray++;
        uint8_t B = *colArray++;
        m_pixels.setPixelColor(i, m_pixels.Color(R,G,B));
    }

    m_pixels.show();

}

void PL_getNextFrame(struct frame_t*f){
    
    int pixNum;
    FMEM_read(m_nextAddress, (uint8_t*)f, 8);
    pixNum =  f->frame.ledNumFlags & 0x1FF;
    FMEM_read(m_nextAddress + 8, f->colArray, pixNum * 3);
    
    m_nextAddress += pixNum * 3 + 8;

    //check for the end of file 
    if (f->frame.ledNumFlags & PL_ENDFRAME_FLAG) m_running = 0;
    // check for loop reset
    if (f->frame.ledNumFlags & PL_LOOP_FLAG) m_nextAddress = m_startAddress;

    //prepare for the next frame
    Serial.print("Next ADR:");
    Serial.println(m_nextAddress);
   
}

void PL_tick(){
    if (((millis() - m_lastTime) > m_frame.frame.duration) && m_running ){
        //we need to load the new frame
        Serial.println("Next Frame");
        m_lastTime = millis();
        PL_getNextFrame(&m_frame);
        PL_applyFrame(&m_frame);
        Serial.println("Frame Applied");

    }
}

