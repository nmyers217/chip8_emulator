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

    // TODO: load the font into memory

    return s;
}

void free_chip8(Chip8State* state) {
    free(state->memory);
    free(state);
}

void tick(Chip8State* state) {
    // TODO: parse the instruction at pc
    // TODO: execute the instruction
}

int main(void) {
    Chip8State* s = init_chip8("lol", sizeof("lol"));
    free_chip8(s);
    s = NULL;
    return EXIT_SUCCESS;
}
