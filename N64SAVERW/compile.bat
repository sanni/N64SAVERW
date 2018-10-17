@echo off
del *.v64
set N64_INST=C:/n64dev/mips64-elf
set Path=C:\n64dev\msys\bin
make -i
del *.elf *.o *.bin *.dfs *.expand
pause