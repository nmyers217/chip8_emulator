#pragma once

#include <stdint.h>

// Chip8 programs have 4096 bytes of memory
uint16_t const MEMORY_SIZE = 1024 * 4;

// This font data needs to be loaded into memory for programs to use
uint8_t const FONT_DATA[16][5] = {
    {
        0b11110000,
        0b10010000,
        0b10010000,
        0b10010000,
        0b11110000
    },
    {
        0b00100000,
        0b01100000,
        0b00100000,
        0b00100000,
        0b01110000
    },
    {
        0b11110000,
        0b00010000,
        0b11110000,
        0b10000000,
        0b11110000
    },
    {
        0b11110000,
        0b00010000,
        0b11110000,
        0b00010000,
        0b11110000
    },
    {
        0b10010000,
        0b10010000,
        0b11110000,
        0b00010000,
        0b00010000
    },
    {
        0b11110000,
        0b10000000,
        0b11110000,
        0b00010000,
        0b11110000
    },
    {
        0b11110000,
        0b10000000,
        0b11110000,
        0b10010000,
        0b11110000
    },
    {
        0b11110000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01000000
    },
    {
        0b11110000,
        0b10010000,
        0b11110000,
        0b10010000,
        0b11110000
    },
    {
        0b11110000,
        0b10010000,
        0b11110000,
        0b00010000,
        0b11110000
    },
    {
        0b11110000,
        0b10010000,
        0b11110000,
        0b10010000,
        0b10010000
    },
    {
        0b11100000,
        0b10010000,
        0b11100000,
        0b10010000,
        0b11100000
    },
    {
        0b11110000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b11110000
    },
    {
        0b11100000,
        0b10010000,
        0b10010000,
        0b10010000,
        0b11100000
    },
    {
        0b11110000,
        0b10000000,
        0b11110000,
        0b10000000,
        0b11110000
    },
    {
        0b11110000,
        0b10000000,
        0b11110000,
        0b10000000,
        0b10000000
    }
};

// These constants are the memory map
uint16_t const FONT_MEMORY_OFFSET = 0x0;
uint16_t const PROGRAM_MEMORY_OFFSET = 0x200;
uint16_t const STACK_MEMORY_OFFSET = 0xEA0;
uint16_t const DISPLAY_MEMORY_OFFSET = 0xF00;

typedef struct Chip8State {
    uint8_t v[16];    // 16 8bit registers
    uint16_t i;       // The address register
    uint16_t sp;      // Stack pointer
    uint16_t pc;      // Program counter
    uint8_t delay;    // Delay timer register
    uint8_t sound;    // Sound timer register
    uint8_t* memory;  // A pointer to the 4kb of memory
    uint8_t* display; // A pointer to the start of the display memory
} Chip8State;

/**
 * Initialize the chip8 hardware for the program in program_buffer
 */
Chip8State* init_chip8(uint8_t* const program_buffer, size_t program_size);

/**
 * Free the resources used by the chip8 state
 */
void free_chip8(Chip8State* state);

/**
 * Advance the state forward one tick
 */
void tick(Chip8State* state);
