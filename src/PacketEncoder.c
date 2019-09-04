#include "PacketEncoder.h"


int encodePacket(uint8_t* bufIn, uint8_t* bufOut, int lenIn) {
	int len = 0;
	uint8_t chr = 0;
	uint8_t* dp = bufOut+1;
	*bufOut++ = START_CHAR;	// start indication
	*bufOut++ = 0;	// space for escape character indication
	*bufOut++ = 0;	// space for the len character - 1 
	while (lenIn-- > 0) {
		chr = *bufIn++;
		if ((chr == START_CHAR) || (chr == ESCAPE_CHAR)) {
			*bufOut++ = ESCAPE_CHAR;
			*bufOut++ = chr ^ XOR_CHAR;
			len += 2;
		} else {
			*bufOut++ = chr;
			len++;
		}
	}
	//write the len
	*dp++ = len >> 8;
	*dp = len;

	return len+3;
}

int decodePacket(uint8_t* bufIn, uint8_t* bufOut) {
	int stat = 0;
	int len = 0;
	uint8_t chr;
	if (*bufIn++ == START_CHAR) {
		len = *bufIn++ << 8;
		len += *bufIn++;
		while (len-- > 0) {
			chr = *bufIn++;
			// check for escape chars
			if (chr == ESCAPE_CHAR) {
				chr = (*bufIn++) ^ XOR_CHAR;
			}
			*bufOut++ = chr;
		}
	}
	else {
		stat = -1;
	}
	return stat;
}
