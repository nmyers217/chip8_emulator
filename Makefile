CC = gcc
CCFLAGS = -Wall

all: chip8_debug

chip8_debug:
	$(CC) $(CCFLAGS) -g chip8.c -o chip8

chip8:
	$(CC) $(CCFLAGS) -03 chip8.c -o chip8

disassembler_debug: disassembler.c
	$(CC) $(CCFLAGS) -g disassembler.c -o disassembler

disassembler:
	$(CC) $(CCFLAGS) -O3 disassembler.c -o disassembler

clean:
	rm -f disassembler chip8
