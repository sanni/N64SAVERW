#include <libdragon.h>
#include "N64FlashRAM.h"
#include "N64SRAM.h"
#include "utils.h"

#include <string.h>

#define PI_DMAFromSRAM SRAM_read
#define PI_DMAToSRAM SRAM_write

void FRAM_Init(void) {
	SRAM_Init();

	fram_cmd(FRAM_EXECUTE_CMD);
	delay(10);

	fram_cmd(FRAM_EXECUTE_CMD);
	delay(10);

	fram_cmd(FRAM_STATUS_MODE_CMD);
	delay(10);

}

void FRAM_EraseBank(uint32_t bankNo) {

	bankNo = ((bankNo * 128) + 128) - 1;

	uint8_t erased = 2;
	while (erased) {
		fram_cmd(fram_set_addr(FRAM_ERASE_OFFSET_CMD, bankNo));
		delay(1);
		fram_cmd(FRAM_ERASE_MODE_CMD);
		delay(1);
		fram_cmd(FRAM_EXECUTE_CMD);
		delay(10);

		erased--;
	}
}

// Offset is in groups of 128 bytes.
void FRAM_SetWritingOffset(uint32_t offset) {
	fram_cmd(fram_set_addr(FRAM_WRITE_OFFSET_CMD, offset));
	delay(1);
}

void FRAM_WriteBlock(uint8_t *src, uint32_t blockNo) {
	fram_cmd(FRAM_WRITE_MODE_CMD);
	delay(1);
	PI_DMAToSRAM(src, 0, 128);
	while (dma_busy()) ;
	FRAM_SetWritingOffset(blockNo);
	fram_cmd(FRAM_EXECUTE_CMD);
	delay(1);
}

void FRAM_WriteSlot(uint8_t *src, uint32_t slotNo) {
	uint32_t startBlock = slotNo * 8;

	for (uint8_t block = 0; block < 8; block++) {
		FRAM_WriteBlock(src + (block * 128), startBlock + block);
	}
}

void FRAM_WriteBank(uint8_t *src, uint32_t bankNo) {
	uint32_t startSlot = bankNo * 16;

	for (uint8_t slot = 0; slot < 16; slot++) {
		FRAM_WriteSlot(src + (slot * 1024), startSlot + slot);
	}
}

void FRAM_ReadBlock(uint8_t *dst, uint32_t blockNo, uint8_t type) {
	uint32_t blockOffset = blockNo * 128;

	FRAM_Read(dst, blockOffset, 128, type);
}

void FRAM_ReadSlot(uint8_t *dst, uint32_t slotNo, uint8_t type) {
	uint8_t buffer[1024];
	uint32_t startBlock = slotNo * 8;

	for (uint8_t block = 0; block < 8; block++) {
		FRAM_ReadBlock(buffer + (block * 128), startBlock + block, type);
	}

	memcpy(dst, buffer, 1024);
}

void FRAM_ReadBank(uint8_t *dst, uint32_t bankNo, uint8_t type) {
	uint8_t buffer[16 * 1024];
	uint32_t startSlot = bankNo * 16;

	for (uint8_t slot = 0; slot < 16; slot++) {
		FRAM_ReadSlot(buffer + (slot * 1024), startSlot + slot, type);
	}
	memcpy(dst, buffer, 16 * 1024);
}

void FRAM_Read(uint8_t *dst, uint32_t offset, uint32_t size, uint8_t type) {
	fram_cmd(FRAM_READ_MODE_CMD); // Enable read mode
	PI_DMAFromSRAM(dst, (offset/type), size);
	while (dma_busy()) ;
}

void FRAM_Status(uint64_t *status) {
	fram_cmd(FRAM_STATUS_MODE_CMD); // Enable status mode
	delay(1);

	PI_DMAFromSRAM(status, 0, 8);
	while (dma_busy()) ;
}

uint8_t FRAM_Detect(void) {
	uint64_t status;

	FRAM_Status(&status);

	status >>= 48;

	if (status == 0x1111) return 1;
	else return 0;
}

