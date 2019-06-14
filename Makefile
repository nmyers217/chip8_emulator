.POSIX:
.SUFFIXES:
CC = cc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic

all: chip8_debug

chip8_debug:
	$(CC) $(CFLAGS) -g3 chip8.c -o chip8

chip8:
	$(CC) $(CFLAGS) -03 chip8.c -o chip8

disassembler_debug: disassembler.c
	$(CC) $(CFLAGS) -g3 disassembler.c -o disassembler

disassembler:
	$(CC) $(CFLAGS) -O3 disassembler.c -o disassembler

clean:
	rm -f disassembler chip8
