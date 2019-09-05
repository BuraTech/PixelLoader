#include "PacketEncoder.h"


int8_t encodePacket(uint8_t* bufIn, uint8_t* bufOut, int lenIn) {
	int len = 0;
	uint8_t chr = 0;
	uint8_t* dp = bufOut + 1;
	*bufOut++ = START_CHAR;	// start indication
	*bufOut++ = 0;	// space for escape character indication
	*bufOut++ = 0;	// space for the len character - 1 
	while (lenIn-- > 0) {
		chr = *bufIn++;
		if ((chr == START_CHAR) || (chr == ESCAPE_CHAR) || (chr == END_CHAR)) {
			*bufOut++ = ESCAPE_CHAR;
			*bufOut++ = chr ^ XOR_CHAR;
			len += 2;
		}
		else {
			*bufOut++ = chr;
			len++;
		}
	}
	//write the end of packet
	*bufOut++ = END_CHAR;
	len++;
	//write the len
	*dp++ = len >> 8;
	*dp = len;

	return len + HEADER_SIZE-1;
}

int8_t decodePacket(uint8_t* bufIn, uint8_t* bufOut, uint16_t* len_p) {
	int stat = 0;
	int len = 0;
	uint8_t chr;
	if (*bufIn++ == START_CHAR) {
		len = *bufIn++ << 8;
		len += *bufIn++;
		if (len == 0) 
			stat = -2;
		else {
			*len_p = len - 1;
			do {
				chr = *bufIn++;
				len--;

				//check for end first otehrwise we cannot transfer END char
				if (chr == END_CHAR) {
					break;
				}

				// check for escape chars
				if (chr == ESCAPE_CHAR) {
					chr = (*bufIn++) ^ XOR_CHAR;
					len--;
				}
				*bufOut++ = chr;
			} while ((len > 0));

			if (len != 0) stat = -3;
		}
	}
	else {
		stat = -1;
	}
	return stat;
}

