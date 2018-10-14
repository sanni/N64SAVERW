/*
 * utils.c
 *
 *  Created on: 25.08.2013
 *      Author: blacklotus
 */

#include "utils.h"

#define POLY 0x8408
/*
 //                                      16   12   5
 // this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
 // This works out to be 0x1021, but the way the algorithm works
 // lets us use 0x8408 (the reverse of the bit pattern).  The high
 // bit is always assumed to be set, thus we only use 16 bits to
 // represent the 17 bit value.
 */

uint16_t crc16(uint8_t *data_p, uint32_t length) {
	uint8_t i;
	uint32_t data;
	uint32_t crc = 0xffff;

	if (length == 0)
		return (~crc);

	do {
		for (i = 0, data = (uint32_t) 0xff & *data_p++; i < 8; i++, data >>= 1) {
			if ((crc & 0x0001) ^ (data & 0x0001))
				crc = (crc >> 1) ^ POLY;
			else
				crc >>= 1;
		}
	} while (--length);

	crc = ~crc;
	data = crc;
	crc = (crc << 8) | (data >> 8 & 0xff);

	return (crc);
}

void waitForABPress(_controller_data* keys) {
	controller_scan();
	*keys = (_controller_data) get_keys_down();

	while (!((*keys).c[0].err == ERROR_NONE && ((*keys).c[0].A || (*keys).c[0].B))) {
		delay(1); // For Reset Button to work
		controller_scan();
		*keys = (_controller_data) get_keys_down();
	}

}

void delay(unsigned long wait) {
	for (int w = 0; w < wait * (COUNTS_PER_SECOND / 1000); ++w) {
		asm("nop");
	}
}

