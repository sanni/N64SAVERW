/*
 * utils.h
 *
 *  Created on: 25.08.2013
 *      Author: blacklotus
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <libdragon.h>

uint16_t crc16(uint8_t *data_p, uint32_t length);
void waitForABPress(_controller_data* keys);
void delay(unsigned long wait);

#endif /* UTILS_H_ */
