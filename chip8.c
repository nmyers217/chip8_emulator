#include <assert.h>
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

void tick(Chip8State* state) {
    uint8_t* op = state->memory[state->pc];
    uint8_t nib = op[0] >> 4;

    switch (nib) {
        case 0x0:
            if (op[1] == 0xE0) {
                // TODO: how do i clear the screen most efficiently?
            } else if (op[1] == 0xEE) {
                // TODO: debug this
                state->pc = state->memory[state->sp--];
            } else {
                printf("Operation 0NNN not supported!");
            }
            break;
        case 0x1:
            // TODO: debug this
            uint16_t nnn = ((op[0] & 0x0F) << 8) | op[1];
            state->pc = nnn;
            break;
        case 0x2:
            // TODO: debug this
            uint16_t nnn = ((op[0] & 0x0F) << 8) | op[1];
            state->memory[++state->sp] = state->pc;
            state->pc = nnn;
            break;
        case 0x3:
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            if (state->v[x] == op[1]) {
                state->pc += 2;
            }
            break;
        case 0x4:
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            if (state->v[x] != op[1]) {
                state->pc += 2;
            }
            break;
        case 0x5:
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
        case 0x6:
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            state->v[x] = op[1];
            break;
        case 0x7:
            // TODO: debug this
            uint8_t x = op[0] & 0x0F;
            assert(x >= 0 && x < 16);
            state->v[x] += op[1];
            break;
        case 0x8:
            // TODO: implement me
            break;
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
