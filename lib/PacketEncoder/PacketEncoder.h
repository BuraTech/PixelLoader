#ifndef __PACKETENCODEER_H
#define __PACKETENCODEER_H

#include <Arduino.h>
#include <stdio.h>

#define START_CHAR  0x75
#define ESCAPE_CHAR 0x70
#define XOR_CHAR	0x20
#define END_CHAR    0x7E
#define HEADER_SIZE 4


/* packet is escaped for framing and after the start char we have len*/
/* we can have max 256 bytes in a pacet as it is supposed to go into a page */
/* 0x75 <len1><len2> DATA*/
/* returns the len of the entire packet including the start and len*/
int8_t encodePacket(uint8_t* bufIn, uint8_t* bufOut, int lenIn);

/* input packet including start and len and output buffer*/
int8_t decodePacket(uint8_t* bufIn, uint8_t* bufOut, uint16_t* len_p);

#endif //__PACKETENCODEER_H