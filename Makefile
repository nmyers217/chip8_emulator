CC = gcc
CCFLAGS = -Wall

all: chip8

chip8: chip8.c
	$(CC) $(CCFLAGS) -O3 chip8.c -o chip8

debug: chip8.c
	$(CC) $(CCFLAGS) -g chip8.c -o chip8

clean:
	rm -f chip8
