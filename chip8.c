#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"

Chip8State* init_chip8(uint8_t* const program_buffer, size_t program_size) {
    // We should stop if the program is too big
    assert(program_size < MEMORY_SIZE - PROGRAM_MEMORY_OFFSET);

    Chip8State* s = calloc(1, sizeof(Chip8State));

    s->memory = calloc(1, MEMORY_SIZE);
    s->pc = PROGRAM_MEMORY_OFFSET;
    s->sp = STACK_MEMORY_OFFSET;
    s->display = &s->memory[DISPLAY_MEMORY_OFFSET];

    memcpy(&s->memory[s->pc], program_buffer, program_size);
    memcpy(&s->memory[FONT_MEMORY_OFFSET], FONT_DATA, sizeof(FONT_DATA));

    return s;
}

void free_chip8(Chip8State* state) {
    free(state->memory);
    free(state);
}

// TODO: DEFINITLEY DEBUG THIS
void draw_sprite(Chip8State* state, uint8_t x, uint8_t y, uint8_t n) {
    // No collision by default
    state->v[0xF] = 0;

    for (uint8_t byte = 0; byte < n; byte++) {
        uint8_t sprite_byte = state->memory[state->i + byte];

        // Iterate the bits of sprite_byte from right to left
        for (uint8_t bit = 0; bit < 8; bit++) {
            uint8_t sprite_bit = (sprite_byte >> bit) & 0b1;

            // Pixel space coordinates
            uint8_t pixel_y = y + byte;
            uint8_t pixel_x = x + (7 - bit);

            // Bit space index
            uint8_t screen_bit_index = pixel_y * SCREEN_WIDTH + pixel_x;

            // Addressable byte index, and a bit offset within it
            uint8_t screen_byte_index = screen_bit_index / 8;
            uint8_t screen_byte_off = 7 - screen_bit_index % 8;

            // Actually address the byte and query the state of the bit
            uint8_t* screen_byte = &state->display[screen_byte_index];
            uint8_t screen_bit_state = (*screen_byte >> screen_byte_off) & 0b1;

            // Calculate the new byte and query the bit again
            uint8_t new_screen_byte =
                *screen_byte ^ (sprite_bit << screen_byte_off);
            uint8_t new_screen_bit_state =
                (new_screen_byte >> screen_byte_off) & 0b1;

            // Update the state
            if (state->v[0xF] == 0 && screen_bit_state == 1 &&
                new_screen_bit_state == 0) {
                state->v[0xF] = 1;
            }
            *screen_byte = new_screen_byte;
        }
    }
}

void process_op(Chip8State* state) {
    uint8_t* op = &state->memory[state->pc];
    uint8_t nib = op[0] >> 4;

    switch (nib) {
        case 0x0: {
            // TODO: debug this
            if (op[1] == 0xE0) {
                for (size_t i = DISPLAY_MEMORY_OFFSET; i < MEMORY_SIZE; i++) {
                    state->memory[i] = 0;
                }
            } else if (op[1] == 0xEE) {
                state->pc = state->memory[state->sp--];
            } else {
                printf("Operation 0NNN not supported!");
            }
            break;
        }
        case 0x1: {
            // TODO: debug this
            uint16_t nnn = ((op[0] & 0x0F) << 8) | op[1];
            state->pc = nnn;
            break;
        }
        case 0x2: {
            // TODO: debug this
            uint16_t nnn = ((op[0] & 0x0F) << 8) | op[1];
            state->memory[++state->sp] = state->pc;
            state->pc = nnn;
            break;
        }
        case 0x3: {
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            if (state->v[x] == op[1]) {
                state->pc += 2;
            }
            break;
        }
        case 0x4: {
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            if (state->v[x] != op[1]) {
                state->pc += 2;
            }
            break;
        }
        case 0x5: {
            // TODO: debug this
            uint8_t last_nib = op[1] & 0x0F;
            if (last_nib == 0x0) {
                uint8_t x = op[0] & 0x0F;
                uint8_t y = op[1] >> 4;
                assert(x >= 0 && x < 16 && y >= 0 && y < 16);
                if (state->v[x] == state->v[y]) {
                    state->pc += 2;
                }
            } else {
                printf("Unkown operation!");
            }
            break;
        }
        case 0x6: {
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            state->v[x] = op[1];
            break;
        }
        case 0x7: {
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            state->v[x] += op[1];
            break;
        }
        case 0x8: {
            // TODO: debug this
            uint8_t last_nib = op[1] & 0x0F;
            uint8_t x = op[0] & 0x0F;
            uint8_t y = op[1] >> 4;
            assert(x >= 0 && x < 16 && y >= 0 && y < 16);

            switch (last_nib) {
                case 0x0:
                    state->v[x] = state->v[y];
                    break;
                case 0x1:
                    state->v[x] |= state->v[y];
                    break;
                case 0x2:
                    state->v[x] &= state->v[y];
                    break;
                case 0x3:
                    state->v[x] ^= state->v[y];
                    break;
                case 0x4: {
                    uint16_t result = state->v[x] + state->v[y];
                    uint8_t carry = result >> 8;
                    uint8_t lower_byte = result & 0x00FF;
                    state->v[0xF] = carry;
                    state->v[x] = lower_byte;
                    break;
                }
                case 0x5:
                    state->v[0xF] = state->v[x] > state->v[y];
                    state->v[x] -= state->v[y];
                    break;
                case 0x6:
                    state->v[0xF] = state->v[x] & 0b1;
                    state->v[x] >>= 1;
                    break;
                case 0x7:
                    state->v[0xF] = state->v[y] > state->v[x];
                    state->v[x] = state->v[y] - state->v[x];
                    break;
                case 0xE:
                    state->v[0xF] = (state->v[x] >> 7) & 0b1;
                    state->v[x] <<= 1;
                    break;
                default:
                    printf("Unkown operation!");
            }

            break;
        }
        case 0x9: {
            // TODO: debug this
            uint8_t last_nib = op[1] & 0x0F;
            if (last_nib == 0x0) {
                uint8_t x = op[0] & 0x0F;
                uint8_t y = op[1] >> 4;
                assert(x >= 0 && x < 16 && y >= 0 && y < 16);
                if (state->v[x] != state->v[y]) {
                    state->pc += 2;
                }
            } else {
                printf("Unkown operation!");
            }
            break;
        }
        case 0xA: {
            // TODO: debug
            uint16_t nnn = ((op[0] & 0x0F) << 8) | op[1];
            state->i = nnn;
            break;
        }
        case 0xB: {
            // TODO: debug
            uint16_t nnn = ((op[0] & 0x0F) << 8) | op[1];
            state->pc = nnn + state->v[0];
            break;
        }
        case 0xC:
            // TODO: decide on a method for rng
            break;
        case 0xD: {
            uint8_t x = op[0] & 0x0F;
            uint8_t y = op[1] >> 4;
            assert(x >= 0 && x < 16 && y >= 0 && y < 16);
            uint8_t n = op[1] & 0x0F;
            draw_sprite(state, x, y, n);
            break;
        }
        default:
            printf("Unkown operation!");
    }
}

int main(void) {
    Chip8State* s = init_chip8("lol", sizeof("lol"));
    free_chip8(s);
    s = NULL;
    return EXIT_SUCCESS;
}
