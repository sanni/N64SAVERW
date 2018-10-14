// standard c includes
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// n64 specific included
#include <libdragon.h>
#include "myth.c"

// save game related
#include "lib/N64FlashRAM.h"
#include "lib/N64SRAM.h"
#include "lib/utils.h"

#define DEFAULT_CONTROLLER 0

// Save Type Size in Byte
#define _4K_EEPROM 512
#define _16K_EEPROM 2048
#define _256K_SRAM 32768
#define _256K_CPAK 32768
#define _1M_FLASHRAM 131072

// variables
uint32_t eepSize;
uint32_t savefilesize;

// SD Speed
extern unsigned int sd_speed;
extern unsigned int fast_flag;

// Buffer
static uint8_t buffer1[_1M_FLASHRAM];
static uint8_t buffer2[_1M_FLASHRAM];
static uint8_t buffer3[_1M_FLASHRAM];
uint8_t buffer4[_256K_SRAM] = {};

// Variables for the menu
int choice = 0;
int num_options = 5;
char* options[] = { "Read Cart Save", "Write to SD", "Read from SD", "Erase Cart Save", "Write Cart Save"};

// SD Fat
FATFS SDFatFs;
FIL SDFile;
UINT ts;

// Filename on SD
XCHAR wname[24];

// SD Error
int error = 0;

//1st Savegame Type
uint8_t type = 1;

// Testfile to write to SD
static unsigned char __attribute__((aligned(16))) tmpBuf[_1M_FLASHRAM];

// Neo_2.c
extern void neo2_enable_sd(void);

// print menu
void printmenu(char* title, char* items[], int num_items, int choice, int type){
  console_clear();
  printf(title); 
  if(fast_flag && !sd_speed) printf(" + ");
  else printf(" - ");
  if(type==1) printf("Flashram Type 1");
  if(type==2) printf("Flashram Type 2");
  if(type==3) printf("4K Eeprom");
  if(type==4) printf("16K Eeprom");
  if(type==5) printf("SRAM");
  if(type==6) printf("CPAK");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("Use DPAD and A to select Option\n");
  printf("Press Z to change Save Type\n");
  printf("\n");
  for (unsigned char i = 0; i < num_items; i++) {
	if(i == choice)
	  printf(" ");
    printf(items[i]);
	printf("\n");
  }
  printf("\n");
  printf("\n");
  if((type==1) || (type==2)) {
    if(choice==0) printf("Reads Flashram to Buffer\n");
    if(choice==1) printf("Writes Buffer to SD\n");
    if(choice==2) printf("Reads save.fla from SD to Buffer\n");
    if(choice==3) printf("Erases Flashram\n");
    if(choice==4) printf("Writes Buffer to Flashram\n");
  }	
  if((type==3) || (type==4)) {
    if(choice==0) printf("Reads Eeprom to Buffer\n");
    if(choice==1) printf("Writes Buffer to SD\n");
    if(choice==2) printf("Reads save.eep from SD to Buffer\n");
    if(choice==3) printf("Overwrites Eeprom with 0x00\n");
    if(choice==4) printf("Writes Buffer to Eeprom\n");
  }	
  if(type==5) {
    if(choice==0) printf("Reads SRAM to Buffer\n");
    if(choice==1) printf("Writes Buffer to SD\n");
    if(choice==2) printf("Reads save.sra from SD to Buffer\n");
    if(choice==3) printf("Overwrites SRAM with 0x00\n");
    if(choice==4) printf("Writes Buffer to SRAM\n");
  }	
   if(type==6) {
    if(choice==0) printf("Reads CPAK to Buffer\n");
    if(choice==1) printf("Writes Buffer to SD\n");
    if(choice==2) printf("Reads save.mpk from SD to Buffer\n");
    if(choice==3) printf("Overwrites CPAK with 0x00\n");
    if(choice==4) printf("Writes Buffer to CPAK\n");
  }	
  printf("\n");
  printf("\n");
  printf("2015 sanni - Thanks to \n");
  printf("Shaun Taylor Dr.neo ChillyWilly \n");
  printf("Conle themanbehindcurtain \n");
}

//Read Eeprom
void readeeprom(uint8_t type){

  if(type==3){ 
    eepSize = _4K_EEPROM;
	printf("Reading 4K EEPROM into Buffer 1\n");
  }
  if(type==4){
    eepSize = _16K_EEPROM;
	printf("Reading 16K EEPROM into Buffer 1\n");
  }
   
  for (int i = 0; i < eepSize / 8; ++i) {
    eeprom_read(i, buffer3 + (i * 8));
  }
  
  printf("Reading again into Buffer 3\n");
  for (int i = 0; i < eepSize / 8; ++i) {
    eeprom_read(i, buffer1 + (i * 8));
  }

  printf("Comparing both Buffers\n");
  bool equal = true;
  bool newerror = true;
  int numerror = 0;
  for (int i = 0; i < eepSize; ++i) {
    if (buffer3[i] != buffer1[i]) {
      if (newerror) {
        printf("First Error at %d\n", i);
        newerror = false;
      }
      numerror++;
      equal = false;
      //break;
    }
  }
  printf("\n");
  if (equal) {
    printf("Reading was successful\n");
  }
  else {
    printf("Buffers don't match\n");
    printf("Number of Errors: %d\n", numerror);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n"); 
}

//Write Eeprom
void writeeeprom(uint8_t type){

  if(type==3){ 
    eepSize = _4K_EEPROM;
	printf("Writing Buffer 3 to 4K EEPROM\n");
  }
  if(type==4){
    eepSize = _16K_EEPROM;
	printf("Writing Buffer 3 to 16K EEPROM\n");
  }
   
  for (int i = 0; i < eepSize / 8; ++i) {
    eeprom_write(i, buffer3 + (i * 8));
	delay(20);
  }
  
  printf("Verifying\n");
  for (int i = 0; i < eepSize / 8; ++i) {
    eeprom_read(i, buffer1 + (i * 8));
  }

  bool equal = true;
  bool newerror = true;
  int numerror = 0;
  for (int i = 0; i < eepSize; ++i) {
    if (buffer3[i] != buffer1[i]) {
      if (newerror) {
        printf("First Error at %d\n", i);
        newerror = false;
      }
      numerror++;
      equal = false;
      //break;
    }
  }
  printf("\n");
  if (equal) {
    printf("Writing was successful\n");
  }
  else {
    printf("Writing failed to verify\n");
    printf("Number of Errors: %d\n", numerror);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n"); 
}

//Read Flashram
void readflashram(uint8_t type) {
  printf("Reading Flashram into Buffer 1\n");
  FRAM_Init();
  for (uint32_t i = 0; i < 8; ++i) {
    FRAM_ReadBank(buffer1 + (i * 16384), i, type);
    delay(20);
  }

  printf("Reading Flashram into Buffer 3\n");
  FRAM_Init();
  for (uint32_t i = 0; i < 8; ++i) {
    FRAM_ReadBank(buffer3 + (i * 16384), i, type);
    delay(20);
  }

  printf("Comparing both Buffers\n");
  bool equal = true;
  bool newerror = true;
  int numerror = 0;
  for (int i = 0; i < _1M_FLASHRAM; ++i) {
    if (buffer3[i] != buffer1[i]) {
      if (newerror) {
        printf("First Error at %d\n", i);
        newerror = false;
      }
      numerror++;
      equal = false;
      //break;
    }
  }
  printf("\n");
  if (equal) {
    printf("Reading was successful\n");
  }
  else {
    printf("Buffers don't match\n");
    printf("Number of Errors: %d\n", numerror);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");  
}

//Read Sram
void readsram(void){

  printf("Reading SRAM into Buffer 3\n");
  SRAM_read(buffer3, 0, _256K_SRAM);

  printf("Waiting 1 second...\n");
  delay(1000);

  printf("Reading 256K SRAM into Buffer 1\n");
  SRAM_read(buffer1, 0, _256K_SRAM);
  delay(20);

  printf("Comparing Buffers\n");
  bool equal = true;
  for (int i = 0; i < _256K_SRAM; ++i) {
    if (buffer1[i] != buffer3[i]) {
	  printf("buffer1[]: %u, buffer3[]: %u, i: %d\n", buffer1[i], buffer3[i], i);
	  equal = false;
	  break;
	}
  }

  if (!equal) {
    printf("Buffers don't match\n");
  } 
  else {
    printf("Buffers match\n");
  }
}

//Write Sram
void writesram(void){
  printf("Writing Buffer to SRAM\n");
  SRAM_write(buffer3, 0, _256K_SRAM);
  delay(20);
  
  printf("Reading again\n");
  SRAM_read(buffer1, 0, _256K_SRAM);
  delay(20);

  printf("Comparing Buffers\n");
  bool equal = true;
  for (int i = 0; i < _256K_SRAM; ++i) {
    if (buffer1[i] != buffer3[i]) {
      printf("buffer1[]: %u, buffer3[]: %u, i: %d\n", buffer1[i], buffer3[i], i);
      equal = false;
      break;
    }
  }

  if (equal) {
    printf("Writing successful\n");
  } 
  else {
    printf("Buffers don't match\n");
  }
}

//Erase Sram
void erasesram(void){
  printf("Erasing SRAM\n");
  SRAM_write(buffer4, 0, _256K_SRAM);
  delay(20);
 
  printf("Reading again\n");
  SRAM_read(buffer1, 0, _256K_SRAM);
  delay(20);
  
  int empty = 0;
  int notempty = 0;

  for (int i = 0; i < _256K_SRAM; ++i) {
    if (buffer1[i] == 0x00)
      empty++;
    else
      notempty++;
  }
  
  if (empty == _256K_SRAM) {
    printf("Erasing succeeded\n");
  }
  else {
    printf("Erasing failed. %d bytes are not empty\n", notempty);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");   
}

//Erase Eeprom
void eraseeeprom(uint8_t type){
  if(type==3){ 
    eepSize = _4K_EEPROM;
	printf("Erasing 4K EEPROM\n");
  }
  if(type==4){
    eepSize = _16K_EEPROM;
	printf("Erasing 16K EEPROM\n");
  }
  for (int i = 0; i < eepSize / 8; ++i) {
    eeprom_write(i, buffer4 + (i * 8));
	delay(20);
  }
 
  for (int i = 0; i < eepSize / 8; ++i) {
    eeprom_read(i, buffer1 + (i * 8));
  }
  
  int empty = 0;
  int notempty = 0;

  for (int i = 0; i < eepSize; ++i) {
    if (buffer1[i] == 0x00)
      empty++;
    else
      notempty++;
  }
  
  if (empty == eepSize) {
    printf("Erasing succeeded\n");
  }
  else {
    printf("Erasing failed. %d bytes are not empty\n", notempty);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");  
}
  
//Erase Flashram
void eraseflashram(uint8_t type) {
  printf("Erasing Flashram\n");

  FRAM_Init();

  for (uint32_t i = 0; i < 8; ++i) {
    FRAM_EraseBank(i);
    delay(20);
  }

  //Erase Flashram a 2nd time just to be sure
  delay(500);
  
  FRAM_Init();
  
  for (uint32_t i = 0; i < 8; ++i) {
    FRAM_EraseBank(i);
    delay(20);
  }

  printf("Erasing done\n");
  printf("\n");
  delay(500);

  printf("Testing if FlashRam is empty\n");
  printf("Reading 1M FlashRAM into Buffer 1\n");

  FRAM_Init();
  for (uint32_t i = 0; i < 8; ++i) {
    FRAM_ReadBank(buffer1 + (i * 16384), i, type);
    delay(20);
  }

  printf("\n");
  delay(500);

  int empty = 0;
  int notempty = 0;

  for (int i = 0; i < _1M_FLASHRAM; ++i) {
    if (buffer1[i] == 0xFF)
      empty++;
    else
      notempty++;
  }
  
  if (empty == _1M_FLASHRAM) {
    printf("Erasing succeeded\n");
  }
  else {
    printf("Erasing failed. %d bytes are not empty\n", notempty);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");  
}

//Write Flashram
void writeflashram(uint8_t type) {
  printf("Writing Buffer 3 to Flashram\n");

  FRAM_Init();
  for (uint32_t i = 0; i < 8; ++i) {
    FRAM_WriteBank(buffer3 + (i * 16384), i);
    delay(20);
  }

  delay(250);

  printf("Verifying\n");
  FRAM_Init();
  for (uint32_t i = 0; i < 8; ++i) {
    FRAM_ReadBank(buffer1 + (i * 16384), i, type);
    delay(20);
  }

  delay(250);
  
  bool equal = true;
  bool newerror = true;
  int numerror = 0;
  for (int i = 0; i < _1M_FLASHRAM; ++i) {
    if (buffer3[i] != buffer1[i]) {
      if (newerror) {
        printf("First Error at %d\n", i);
        newerror = false;
      }
      numerror++;
      equal = false;
      //break;
    }
  }
  printf("\n");
  if (equal) {
    printf("Writing was successful\n");
  }
  else {
    printf("Buffers don't match\n");
    printf("Number of Errors: %d\n", numerror);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");    
}

//Read CPAK
void readcpak(void){
  //Check accessories
  get_accessories_present();
  
  //Continue if Controller Pak found
  if(identify_accessory(DEFAULT_CONTROLLER) == ACCESSORY_MEMPAK){
	printf("Reading 256K CPAK into Buffer 3\n");
    for( int j = 0; j < 128; j++ ) {
      read_mempak_sector( DEFAULT_CONTROLLER, j, &buffer3[j * MEMPAK_BLOCK_SIZE]  );
    }
    printf("Waiting 1 second...\n");
    delay(1000);

    printf("Reading 256K CPAK into Buffer 1\n");
    for( int j = 0; j < 128; j++ ) {
      read_mempak_sector( DEFAULT_CONTROLLER, j, &buffer1[j * MEMPAK_BLOCK_SIZE]  );
    }
    delay(20);

    printf("Comparing Buffers\n");
    bool equal = true;
    for (int i = 0; i < _256K_CPAK; ++i) {
      if (buffer1[i] != buffer3[i]) {
	    printf("buffer1[]: %u, buffer3[]: %u, i: %d\n", buffer1[i], buffer3[i], i);
	    equal = false;
	    break;
	  }
    }

    if (!equal) {
      printf("Buffers don't match\n");
    } 
    else {
      printf("Buffers match\n");
    }
  }
  //Controller Pak not found
  else {
    printf("Error: CPAK not found\n");
  }
}

//Write CPAK
void writecpak(void){
  //Check accessories
  get_accessories_present();
  
  //Continue if Controller Pak found
  if(identify_accessory(DEFAULT_CONTROLLER) == ACCESSORY_MEMPAK){
    printf("Writing Buffer 3 to CPAK\n");
	
	for( int j = 0; j < 128; j++ ) {
      write_mempak_sector( DEFAULT_CONTROLLER, j, &buffer3[j * MEMPAK_BLOCK_SIZE]  );
    }
	
    delay(20);
    printf("Reading again\n");
   
    for( int j = 0; j < 128; j++ ) {
      read_mempak_sector( DEFAULT_CONTROLLER, j, &buffer1[j * MEMPAK_BLOCK_SIZE]  );
    }
    delay(20);

  printf("Comparing Buffers\n");
  bool equal = true;
  for (int i = 0; i < _256K_CPAK; ++i) {
    if (buffer1[i] != buffer3[i]) {
      printf("buffer1[]: %u, buffer3[]: %u, i: %d\n", buffer1[i], buffer3[i], i);
      equal = false;
      break;
    }
  }

  if (equal) {
    printf("Writing successful\n");
  } 
  else {
    printf("Buffers don't match\n");
  }
   }
  //Controller Pak not found
  else {
    printf("Error: CPAK not found\n");
  }
}

//Erase CPAK
void erasecpak(void){
 
  //Check accessories
  get_accessories_present();
  
  //Continue if Controller Pak found
  if(identify_accessory(DEFAULT_CONTROLLER) == ACCESSORY_MEMPAK){
    printf("Erasing CPAK\n");
	
	for( int j = 0; j < 128; j++ ) {
      write_mempak_sector( DEFAULT_CONTROLLER, j, &buffer4[j * MEMPAK_BLOCK_SIZE]  );
    }
	
    delay(20);
    printf("Reading again\n");
   
    for( int j = 0; j < 128; j++ ) {
      read_mempak_sector( DEFAULT_CONTROLLER, j, &buffer1[j * MEMPAK_BLOCK_SIZE]  );
    }
    delay(20);
  
  int empty = 0;
  int notempty = 0;

  for (int i = 0; i < _256K_CPAK; ++i) {
    if (buffer1[i] == 0x00)
      empty++;
    else
      notempty++;
  }
  
  if (empty == _256K_CPAK) {
    printf("Erasing succeeded\n");
  }
  else {
    printf("Erasing failed. %d bytes are not empty\n", notempty);
  }
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");   
 }
  //Controller Pak not found
  else {
    printf("Error: CPAK not found\n");
  }
}

//Write savefile to SD
void writesavetosd(uint8_t type) {
  printf("Writing file to SD\n");
   
  if((type==1) || (type==2)) {
    savefilesize = _1M_FLASHRAM;
    // copy filename 
    c2wstrcpy(wname, "save.fla");
  }	
  if(type==3) {
    savefilesize = _4K_EEPROM;
    // copy filename 
    c2wstrcpy(wname, "save.eep");
  }	
   if(type==4) {
    savefilesize = _16K_EEPROM;
    // copy filename 
    c2wstrcpy(wname, "save.eep");
  }	
  if(type==5) {
    savefilesize = _256K_SRAM;
    // copy filename 
    c2wstrcpy(wname, "save.sra");
  }	
  if(type==6) {
    savefilesize = _256K_CPAK;
    // copy filename 
    c2wstrcpy(wname, "save.mpk");
  }	
  
  // open file on sd, write some stuff in it then read it again
  if (f_open(&SDFile, wname, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {

    f_write(&SDFile, buffer3, savefilesize, &ts);
    f_close(&SDFile);

    printf("Saved to SD card\n");

    if (f_open(&SDFile, wname, FA_OPEN_EXISTING | FA_READ) == FR_OK) {

      printf("Reading from SD into Buffer 2\n");
      f_read(&SDFile, buffer2, savefilesize, &ts);

      printf("Comparing Buffer 2 to Buffer 3\n");
      bool equal = true;
      bool newerror = true;
      int numerror = 0;
      for (int i = 0; i < savefilesize; ++i) {
        if (buffer3[i] != buffer2[i]) {
          if (newerror) {
            printf("First Error at %d\n", i);
            newerror = false;
          }
          numerror++;
          equal = false;
          //break;
        }
      }
	  printf("\n");
      if (equal) {
        printf("Writing was successful\n");
      }
      else {
        printf("Buffers don't match\n");
        printf("Number of Errors: %d\n", numerror);
      }
    }
    else
      printf("Could not open file\n");
  }
  else
    printf("Could not create file\n");
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");  
}

// Reading save from SD
void readsavefromsd(uint8_t type) {
  printf("Reading from SD into Buffer 3\n");

  if((type==1) || (type==2)) {
    savefilesize = _1M_FLASHRAM;
    // copy filename 
    c2wstrcpy(wname, "save.fla");
  }	
  if(type==3) {
    savefilesize = _4K_EEPROM;
    // copy filename 
    c2wstrcpy(wname, "save.eep");
  }	
   if(type==4) {
    savefilesize = _16K_EEPROM;
    // copy filename 
    c2wstrcpy(wname, "save.eep");
  }	
  if(type==5) {
    savefilesize = _256K_SRAM;
    // copy filename 
    c2wstrcpy(wname, "save.sra");
  }	
  if(type==6) {
    savefilesize = _256K_CPAK;
    // copy filename 
    c2wstrcpy(wname, "save.mpk");
  }	
  
  // open file on sd, write some stuff in it then read it again
  if (f_open(&SDFile, wname, FA_OPEN_EXISTING | FA_READ) == FR_OK) {

    f_read(&SDFile, buffer3, savefilesize, &ts);
    f_close(&SDFile);

    if (f_open(&SDFile, wname, FA_OPEN_EXISTING | FA_READ) == FR_OK) {

      printf("Reading a second time into Buffer 2\n");
      f_read(&SDFile, buffer2, savefilesize, &ts);

      printf("Comparing both Buffers\n");
      bool equal = true;
      bool newerror = true;
      int numerror = 0;
      for (int i = 0; i < savefilesize; ++i) {
        if (buffer3[i] != buffer2[i]) {
          if (newerror) {
            printf("First Error at %d\n", i);
            newerror = false;
          }
          numerror++;
          equal = false;
          //break;
        }
      }
      if (equal) {
		printf("\n");
        printf("Reading was successful\n");
      }
      else {
	    printf("\n");
        printf("Buffers don't match\n");
        printf("Number of Errors: %d\n", numerror);
      }
    }
    else
      printf("Could not open save.fla\n");
  }
  else
    printf("Could not open save.fla\n");
  printf("\n");
  printf("\n");
  printf("Press A or B to continue\n");  	
}

// test the sd
void sdTest(void){
	// copy filename to thingy
	c2wstrcpy(wname, "/.test");	
		
	// open file on sd, write some stuff in it then read it again
	if (f_open(&SDFile, wname, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
			
		for (int i=0; i<_1M_FLASHRAM; i+=8192){
			for (int j=0; j<8192; j+=4)
				*(int*)&tmpBuf[j] = (i+j);
			f_write(&SDFile, tmpBuf, 8192, &ts);
		}
		f_close(&SDFile);
       
		if (f_open(&SDFile, wname, FA_OPEN_EXISTING | FA_READ) == FR_OK){
			for (int i=0; i<_1M_FLASHRAM; i+=8192){
				f_read(&SDFile, tmpBuf, 8192, &ts);
				for (int j=0; j<8192; j+=4)
					if (*(int*)&tmpBuf[j] != (i+j))
						error = 1;
			}
			f_lseek(&SDFile, 0);
			f_close(&SDFile);
		}	
		else 
			error = 1;
	f_unlink(wname); // delete test file		
	}	
	else
		error = 1;	

	if(error){	
		printf("ERROR: NEO SD Access failed\n");
		while(1);
	}	
}

// Program start
int main(void) {

  // Enable interrupts (on the CPU)
  init_interrupts();
  
  // Initialize peripherals
  console_init();
  controller_init();
  
  // Enable SD
  neo2_enable_sd();

  // set card type
  cardType = 0x0000;

  /* mount SD
  if (f_mount(1, &SDFatFs)) {
	printf("Couldn't mount SD card");
    while (1);
  }

  // make MMC current drive
  f_chdrive(1);
  */
	
  // Mount the SD
  mount_card();
	
  // Test the sd card
  sdTest();
  
  // Main loop
  while (1) {
  
	// scan controller
    controller_scan();
    _controller_data keys = (_controller_data) get_keys_down();
	delay(1);
	
	//printmenu
    printmenu("N64 Save R/W", options, num_options, choice, type);
	
    // wait until user makes his choice
    while (1) {
  
      // scan controller
      controller_scan();
      _controller_data keys = (_controller_data) get_keys_down();
	  delay(1);
	   
	  // if dpad up is pressed select choice above current selection 
      if ( keys.c[DEFAULT_CONTROLLER].up ) {
     
        if (choice == 0)
          choice = num_options - 1;
        else
          choice--;
		//print menu to show current selection  
	    printmenu("N64 Save R/W", options, num_options, choice, type);
      }
		
	  // if dpad down is pressed select choice below current selection 	
      if ( keys.c[DEFAULT_CONTROLLER].down ) {
	
        choice = (choice + 1) % num_options;
		//print menu to show current selection
	    printmenu("N64 Save R/W", options, num_options, choice, type);  
      }
      
	  // if Z is pressed change Flashtype
	  if (keys.c[DEFAULT_CONTROLLER].Z) {
        if(type==1)
			type = 2;
		else if(type==2)	
			type = 3;
		else if(type==3)	
			type = 4;
		else if(type==4)	
			type = 5;
		else if(type==5)	
			type = 6;
		else if(type==6)	
			type = 1;
		//print menu to show current selection
	    printmenu("N64 Save R/W", options, num_options, choice, type);  	
      }
	  
	  // if A is pressed exit while loop 
	  if (keys.c[DEFAULT_CONTROLLER].A) {
        break;
      }
    }
	
    // execute program part based on users choice
    switch (choice)
    {
      case 0:
        //Read save
		console_clear();
        if((type==1)||(type==2)) readflashram(type);
		if((type==3)||(type==4)) readeeprom(type);
		if(type==5) readsram();
		if(type==6) readcpak();
        waitForABPress(&keys);
        break;

      case 1:
        //Write savefile to SD
		console_clear();
        writesavetosd(type);
        waitForABPress(&keys);
        break;

      case 2:
        //Read savefile from SD
		console_clear();
        readsavefromsd(type);
        waitForABPress(&keys);
        break;

      case 3:
        //Erase save
		console_clear();
        if((type==1)||(type==2)) eraseflashram(type);
        if((type==3)||(type==4)) eraseeeprom(type);
		if(type==5) erasesram();
		if(type==6) erasecpak();
        waitForABPress(&keys);
        break;

      case 4:
        //Write save
		console_clear();
        if((type==1)||(type==2)) writeflashram(type);
		if((type==3)||(type==4)) writeeeprom(type);
		if(type==5) writesram();
		if(type==6) writecpak();
        waitForABPress(&keys);
        break;
    }    
  }
}