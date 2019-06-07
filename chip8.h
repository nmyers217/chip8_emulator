#pragma once

#include <stdint.h>

// Chip8 programs have 4096 bytes of memory
uint16_t const MEMORY_SIZE = 1024 * 4;

// This font data needs to be loaded into memory for programs to use
#define FONT_SPRITE_COUNT 16
#define FONT_SPRITE_BYTE_COUNT 5
uint8_t const FONT_DATA[FONT_SPRITE_COUNT][FONT_SPRITE_BYTE_COUNT] = {
    {0b11110000, 0b10010000, 0b10010000, 0b10010000, 0b11110000},
    {0b00100000, 0b01100000, 0b00100000, 0b00100000, 0b01110000},
    {0b11110000, 0b00010000, 0b11110000, 0b10000000, 0b11110000},
    {0b11110000, 0b00010000, 0b11110000, 0b00010000, 0b11110000},
    {0b10010000, 0b10010000, 0b11110000, 0b00010000, 0b00010000},
    {0b11110000, 0b10000000, 0b11110000, 0b00010000, 0b11110000},
    {0b11110000, 0b10000000, 0b11110000, 0b10010000, 0b11110000},
    {0b11110000, 0b00010000, 0b00100000, 0b01000000, 0b01000000},
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b11110000},
    {0b11110000, 0b10010000, 0b11110000, 0b00010000, 0b11110000},
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b10010000},
    {0b11100000, 0b10010000, 0b11100000, 0b10010000, 0b11100000},
    {0b11110000, 0b10000000, 0b10000000, 0b10000000, 0b11110000},
    {0b11100000, 0b10010000, 0b10010000, 0b10010000, 0b11100000},
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b11110000},
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b10000000}};

// These constants are the memory map
uint16_t const FONT_MEMORY_OFFSET = 0x0;
uint16_t const PROGRAM_MEMORY_OFFSET = 0x200;
uint16_t const STACK_MEMORY_OFFSET = 0xEA0;
uint16_t const DISPLAY_MEMORY_OFFSET = 0xF00;

// The screen dimensions
uint8_t const DISPLAY_WIDTH_BITS = 64;
uint8_t const DISPLAY_HEIGHT_BITS = 32;

// Constants for the keyboard
//
// NOTE: the keyboard was originally layed out like this:
// +---+---+---+---+
// | 1 | 2 | 3 | C |
// +---+---+---+---+
// | 4 | 5 | 6 | D |
// +---+---+---+---+
// | 7 | 8 | 9 | E |
// +---+---+---+---+
// | A | 0 | B | F |
// +---+---+---+---+
#define KEY_COUNT 0x10
// Maps a given hex key code to an index we can use to look it up
uint8_t const KEY_TO_INDEX_MAPPING[KEY_COUNT] = {
    13, // 0x0
    0,  // 0x1
    1,  // 0x2
    2,  // 0x3
    4,  // 0x4
    5,  // 0x5
    6,  // 0x6
    8,  // 0x7
    9,  // 0x8
    10, // 0x9
    12, // 0xA
    14, // 0xB
    3,  // 0xC
    7,  // 0xD
    11, // 0xE
    15  // 0xF
};

// The internal state of a chip8 program
typedef struct Chip8State {
    uint8_t v[16];           // 16 8bit registers
    uint8_t keys[KEY_COUNT]; // The state of the keypad
    uint16_t i;              // The address register
    uint16_t sp;             // Stack pointer
    uint16_t pc;             // Program counter
    uint8_t delay;           // Delay timer register
    uint8_t sound;           // Sound timer register
    uint8_t* memory;         // A pointer to the 4kb of memory
    uint8_t* display;        // A pointer to the start of the display memory
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
 * Process a single operation
 */
void process_op(Chip8State* state);

/**
 * Advance the state forward one tick and process the required amount of
 * operations
 */
void tick(Chip8State* state);
