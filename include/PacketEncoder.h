#ifndef __PACKETENCODEER_H
#define __PACKETENCODEER_H

#include <stdio.h>

#define START_CHAR  0x75
#define ESCAPE_CHAR 0x70
#define XOR_CHAR	0x20

/* packet is escaped for framing and after the start char we have len*/
/* we can have max 256 bytes in a pacet as it is supposed to go into a page */
/* 0x75 <len1><len2> DATA*/
/* returns the len of the entire packet including the start and len*/
int encodePacket(uint8_t* bufIn, uint8_t* bufOut, int lenIn);

/* input packet including start and len and output buffer*/
int decodePacket(uint8_t* bufIn, uint8_t* bufOut);


#endif //__PACKETENCODEER_H