.POSIX:
.SUFFIXES:
CC = cc
CFLAGS = -Wall -Wextra -ansi -pedantic

all: chip8_debug

chip8_debug:
	$(CC) $(CCFLAGS) -g3 chip8.c -o chip8

chip8:
	$(CC) $(CCFLAGS) -03 chip8.c -o chip8

disassembler_debug: disassembler.c
	$(CC) $(CCFLAGS) -g3 disassembler.c -o disassembler

disassembler:
	$(CC) $(CCFLAGS) -O3 disassembler.c -o disassembler

clean:
	rm -f disassembler chip8
