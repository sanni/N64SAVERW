ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
HEADERPATH = $(ROOTDIR)/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header
LINK_FLAGS = -G0 -L$(ROOTDIR)/lib -L$(ROOTDIR)/mips64-elf/lib -lff -ldragon -lc -lm -ldragonsys -lnosys -Tn64ld.x
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O1 -G0 -Wall -I$(ROOTDIR)/include -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

PROG_NAME = N64SAVERW

OBJS = main.o neo_2.o disk_io.o ram_io.o neo_2_asm.o N64FlashRAM.o N64SRAM.o utils.o 

$(PROG_NAME).v64: $(PROG_NAME).elf
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	$(N64TOOL) -b -l 2M -t "N64 Myth SAVE RW" -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME).v64 $(PROG_NAME).bin
	$(CHKSUM64PATH) $(PROG_NAME).v64

$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $(PROG_NAME).elf $(OBJS) $(LINK_FLAGS)

all: $(PROG_NAME).v64

N64FlashRAM.o : lib/N64FlashRAM.c lib/N64FlashRAM.h
	${CC} ${CFLAGS} -c lib/N64FlashRAM.c

N64SRAM.o : lib/N64SRAM.c lib/N64SRAM.h
	${CC} ${CFLAGS} -c lib/N64SRAM.c

utils.o : lib/utils.c lib/utils.h
	${CC} ${CFLAGS} -c lib/utils.c

clean:
	rm -f *.v64 *.elf *.o *.bin
