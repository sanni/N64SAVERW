#ifndef __N64_FLASHRAM__
#define __N64_FLASHRAM__

#include <stdbool.h>
#include "N64FlashRAM.h"

/* Units:
 * BLOCK = 128 bytes
 * SLOT = 8 * BLOCKS; 960 bytes. the last block holds only 64bytes, instead of 128.
 * BANK = 16 * SLOTS; 15360 bytes.
 *
 * There are 1024 BLOCKs
 * 128 SLOTs
 * 8 BANKs
 */

#define FRAM_STATUS_REG	0xA8000000
#define FRAM_COMMAND_REG 0xA8010000

#define fram_cmd(cmd)	io_write(FRAM_COMMAND_REG, cmd)
#define fram_get_status()	io_read(FRAM_STATUS_REG)
#define fram_set_addr(cmd, addr)	(cmd | (addr & 0xFFFF))

#define FRAM_EXECUTE_CMD		0xD2000000
#define FRAM_STATUS_MODE_CMD	0xE1000000
#define FRAM_ERASE_OFFSET_CMD	0x4B000000
#define FRAM_WRITE_OFFSET_CMD	0xA5000000
#define FRAM_ERASE_MODE_CMD		0x78000000
#define FRAM_WRITE_MODE_CMD		0xB4000000
#define FRAM_READ_MODE_CMD		0xF0000000

void FRAM_Init(void);

// Erase bank
// BEWARE: Before a block can be written, it MUST be erased.
// Flashram bits can only be "turned off", once a bit is turned off, the only way
// to turn it back on, is erasing the whole bank in which it resides.
void FRAM_EraseBank(uint32_t bankNo);

// Write 128 bytes (64 if last slot of slot) into block.
void FRAM_WriteBlock(uint8_t *src, uint32_t blockNo);

// Write 960 bytes in the slot.
void FRAM_WriteSlot(uint8_t *src, uint32_t slotNo);

// Write 15360 bytes in bank.
void FRAM_WriteBank(uint8_t *src, uint32_t bankNo);

// Read 128 bytes from block (if block is last of slot, last 64b will contain garbage)
void FRAM_ReadBlock(uint8_t *dst, uint32_t blockNo, uint8_t type);

// Read 960 bytes from the slot.
void FRAM_ReadSlot(uint8_t *dst, uint32_t slotNo, uint8_t type);

// Read 15360 bytes in bank.
void FRAM_ReadBank(uint8_t *dst, uint32_t bankNo, uint8_t type);

// Generic read in flashram. Offset must be even.
void FRAM_Read(uint8_t *dst, uint32_t offset, uint32_t size, uint8_t type);

void FRAM_Status(uint64_t *status);

// 1 if FRAM is found, 0 if not
uint8_t FRAM_Detect(void);

#endif
