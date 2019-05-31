CC = gcc
CCFLAGS = -Wall

all: disassembler_debug

disassembler_debug: disassembler.c
	$(CC) $(CCFLAGS) -g disassembler.c -o disassembler

disassembler_all:
	$(CC) $(CCFLAGS) -O3 disassembler.c -o disassembler

clean:
	rm -f disassembler
