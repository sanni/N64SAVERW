#ifndef _N64SRAM_h
#define _N64SRAM_h

#include <stdbool.h>
#include "N64FlashRAM.h"
#include <stdio.h>

#define PI_BASE_REG		0x04600000

/* PI dom2 latency (R/W): [7:0] domain 2 device latency */
#define PI_BSD_DOM2_LAT_REG	(PI_BASE_REG+0x24)    /* Domain 2 latency */

/* PI dom2 pulse width (R/W): [7:0] domain 2 device R/W strobe pulse width */
#define PI_BSD_DOM2_PWD_REG	(PI_BASE_REG+0x28)    /*   pulse width */

/* PI dom2 page size (R/W): [3:0] domain 2 device page size */
#define PI_BSD_DOM2_PGS_REG	(PI_BASE_REG+0x2C)    /*   page size */

/* PI dom2 release (R/W): [1:0] domain 2 device R/W release duration */
#define PI_BSD_DOM2_RLS_REG	(PI_BASE_REG+0x30)    /*   release duration */

#define SRAM_READ 0x3
#define SRAM_WRITE 0x2

void SRAM_Init(void);
void SRAM_read(void * dest, unsigned long offset, const unsigned long size);
void SRAM_write(void * src, unsigned long offset, const unsigned long size);
#endif
