#include <libdragon.h>
#include "N64SRAM.h"
#include "regsinternal.h"
#include <string.h>

static volatile struct PI_regs_s * const PI_regs = (struct PI_regs_s *) 0xa4600000;

// Inits PI for sram transfer
void SRAM_Init(void) {
	io_write(PI_BSD_DOM2_LAT_REG, 0x05);
	io_write(PI_BSD_DOM2_PWD_REG, 0x0C);
	io_write(PI_BSD_DOM2_PGS_REG, 0x0D);
	io_write(PI_BSD_DOM2_RLS_REG, 0x02);
}

void SRAM_read(void * dest, unsigned long offset, const unsigned long size) {
	//volatile uint8_t inblock_temp[size];

	//data_cache_hit_writeback_invalidate(dest, size);
	disable_interrupts();
	while (dma_busy())
		;

	//memcpy(UncachedAddr(dest), dest, size);

	SRAM_Init();

	while (dma_busy())
		;
	MEMORY_BARRIER();
	PI_regs->status = SRAM_READ;
	MEMORY_BARRIER();
	PI_regs->ram_address = dest;
	MEMORY_BARRIER();
	PI_regs->pi_address = (0xA8000000 + offset) & 0x1FFFFFFF;
	MEMORY_BARRIER();
	while (dma_busy())
		;
	data_cache_hit_writeback_invalidate(dest, size);

	MEMORY_BARRIER();
	PI_regs->write_length = size - 1;
	MEMORY_BARRIER();
	while (dma_busy())
		;

	//memcpy(dest, UncachedAddr(dest), size);

	//while (dma_busy())
		//;

	enable_interrupts();
}

/*
 * size: must be multiple of 4
 */
/*void SRAM_Read(void * dest, unsigned long offset, const unsigned long blockSize, unsigned long size) {
 volatile uint8_t read1[blockSize];
 volatile uint8_t read2[blockSize];
 volatile uint8_t read3[blockSize];
 volatile uint8_t read4[blockSize];
 volatile uint8_t read5[blockSize];
 bool goodRead;
 uint8_t tries;
 int j;
 for (int i = 0; i < size / blockSize; ++i) {
 goodRead = false;
 tries = 0;
 j = 0;

 while (!goodRead && tries < MAX_READ_TRIES) {
 tries++;
 SRAM_ReadBlock(&read1, offset + (i * blockSize), blockSize);
 SRAM_ReadBlock(&read2, offset + (i * blockSize), blockSize);
 SRAM_ReadBlock(&read3, offset + (i * blockSize), blockSize);
 SRAM_ReadBlock(&read4, offset + (i * blockSize), blockSize);
 SRAM_ReadBlock(&read5, offset + (i * blockSize), blockSize);

 // check if all read bytes are equal
 for (j = 0; j < blockSize; ++j) {
 if (!((read1[j] == read2[j]) && (read2[j] == read3[j]) && (read3[j] == read4[j])
 && (read4[j] == read5[j]))) {
 break;
 } else {
 if (j == (blockSize - 1)) {
 goodRead = true;
 memcpy(dest + (i * blockSize), &read1, blockSize);
 while (dma_busy())
 ;
 }
 }
 }
 }
 if (!(tries < MAX_READ_TRIES)) {
 printf("max rd i: %d, of: %lu, bs: %lu\n ", i, offset, blockSize);
 if ((blockSize % 8) == 0) {
 const uint8_t halfBlockSize = blockSize / 2;
 volatile uint8_t halfBlock1[halfBlockSize];
 volatile uint8_t halfBlock2[halfBlockSize];

 SRAM_Read(&halfBlock1, offset + (i * blockSize), halfBlockSize, halfBlockSize);
 SRAM_Read(&halfBlock2, offset + (i * blockSize) + halfBlockSize, halfBlockSize, halfBlockSize);

 memcpy(dest + (i * blockSize), &halfBlock1, halfBlockSize);
 memcpy(dest + (i * blockSize) + halfBlockSize, &halfBlock2, halfBlockSize);
 while (dma_busy())
 ;
 } else {

 printf("no rec i: %d, of: %lu, bs: %lu\n", i, offset, blockSize);
 printf("r1: ");
 for (int k = 0; k < blockSize; k++)
 printf("%02X", read1[k]);
 printf("\n");
 printf("r2: ");
 for (int k = 0; k < blockSize; k++)
 printf("%02X", read2[k]);
 printf("\n");
 printf("r3: ");
 for (int k = 0; k < blockSize; k++)
 printf("%02X", read3[k]);
 printf("\n");
 printf("r4: ");
 for (int k = 0; k < blockSize; k++)
 printf("%02X", read4[k]);
 printf("\n");
 printf("r5: ");
 for (int k = 0; k < blockSize; k++)
 printf("%02X", read5[k]);
 printf("\n");

 printf("\nPress A or B to continue\n\n");
 _controller_data keys = (_controller_data) get_keys_down();
 waitForABPress(&keys);

 if (blockSize == 4) {
 uint32_t beaf = 0xDEADBEEF;
 memcpy(dest + (i * blockSize), &beaf, 4);

 } else {
 memset(dest + (i * blockSize), 0, blockSize);
 }

 }

 }
 }
 }*/

/*
 * size: must be multiple of 4
 */
/*void SRAM_Write(void * src, unsigned long offset, const unsigned long blockSize, unsigned long size) {
 for (int i = 0; i < size / blockSize; ++i) {
 SRAM_WriteBlock(src + (i * blockSize), offset + (i * blockSize), blockSize);
 }
 }*/

void SRAM_write(void * src, unsigned long offset, const unsigned long size) {
	//volatile uint8_t src[size];

	//data_cache_hit_writeback_invalidate(src, size);

	disable_interrupts();
	while (dma_busy())
		;

	//memcpy(UncachedAddr(src), src, size);

	SRAM_Init();

	while (dma_busy())
		;
	MEMORY_BARRIER();
	PI_regs->status = SRAM_WRITE;
	MEMORY_BARRIER();
	PI_regs->ram_address = src;
	MEMORY_BARRIER();
	PI_regs->pi_address = (0xA8000000 + offset) & 0x1FFFFFFF;
	MEMORY_BARRIER();
	while (dma_busy())
		;
	data_cache_hit_writeback_invalidate(src, size);

	MEMORY_BARRIER();
	PI_regs->read_length = size - 1;
	MEMORY_BARRIER();
	while (dma_busy())
		;

	enable_interrupts();
}
