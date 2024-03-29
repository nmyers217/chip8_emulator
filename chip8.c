#include "config.h"

// NOTE: include the operations file so that it can be in the same TU
#include "operations.c"

Chip8State* init_chip8(uint8_t* const program_buffer, size_t program_size) {
    // We should stop if the program is too big
    if (program_size >= (uint16_t)(MEMORY_SIZE - PROGRAM_MEMORY_OFFSET)) {
        fprintf(stderr, "This program is too large!");
        exit(EXIT_FAILURE);
    }

    Chip8State* s = calloc(1, sizeof(Chip8State));

    if (s == NULL) {
        fprintf(stderr, "Could not initialize memory!\n");
        exit(EXIT_FAILURE);
    }

    s->memory = calloc(1, MEMORY_SIZE);

    if (s->memory == NULL) {
        fprintf(stderr, "Could not initialize memory!\n");
        exit(EXIT_FAILURE);
    }
    s->pc = PROGRAM_MEMORY_OFFSET;
    s->sp = STACK_MEMORY_OFFSET;
    s->display = &s->memory[DISPLAY_MEMORY_OFFSET];

    memcpy(&s->memory[s->pc], program_buffer, program_size);
    memcpy(&s->memory[FONT_MEMORY_OFFSET], FONT_DATA, sizeof(FONT_DATA));

    return s;
}

void free_chip8(Chip8State* state) {
    free(state->memory);
    state->memory = NULL;
    state->display = NULL;
    free(state);
}

// TODO: clipping and wrapping?
void draw_sprite(Chip8State* state, uint8_t x, uint8_t y, uint8_t n) {
    // No collision by default
    state->v[0xF] = 0;

    for (uint8_t byte = 0; byte < n; byte++) {
        uint8_t sprite_byte = state->memory[state->i + byte];

        // Iterate the bits of sprite_byte from right to left
        for (uint8_t bit = 0; bit < 8; bit++) {
            uint8_t sprite_bit = (sprite_byte >> bit) & 1;

            // Pixel space coordinates
            uint8_t pixel_y = state->v[y] + byte;
            uint8_t pixel_x = state->v[x] + (7 - bit);

            // Bit space index
            uint16_t screen_bit_index = pixel_y * DISPLAY_WIDTH_BITS + pixel_x;

            // Addressable byte index, and a bit offset within it
            uint16_t screen_byte_index = screen_bit_index / 8;
            uint8_t screen_byte_off = 7 - screen_bit_index % 8;

            // Actually address the byte and query the state of the bit
            uint8_t* screen_byte = &state->display[screen_byte_index];
            uint8_t screen_bit_state = (*screen_byte >> screen_byte_off) & 1;

            // Calculate the new byte and query the bit again
            uint8_t new_screen_byte =
                *screen_byte ^ (sprite_bit << screen_byte_off);
            uint8_t new_screen_bit_state =
                (new_screen_byte >> screen_byte_off) & 1;

            // Update the state
            if (state->v[0xF] == 0 && screen_bit_state == 1 &&
                new_screen_bit_state == 0) {
                state->v[0xF] = 1;
            }
            *screen_byte = new_screen_byte;
        }
    }
}

void print_display(Chip8State* state) {
    for (uint16_t byte_i = 0; byte_i < MEMORY_SIZE - DISPLAY_MEMORY_OFFSET;
         byte_i++) {
        uint8_t byte = state->display[byte_i];

        if (byte_i % (DISPLAY_WIDTH_BITS / 8) == 0) {
            printf("\n");
        }

        for (uint8_t bit_i = 0; bit_i < 8; bit_i++) {
            uint8_t bit = (byte >> (7 - bit_i)) & 1;
            printf("%c", bit ? '#' : ' ');
        }
    }
    printf("\n");
}

uint8_t process_op(Chip8State* state) {
    if (state->pc < PROGRAM_MEMORY_OFFSET || state->pc >= STACK_MEMORY_OFFSET) {
        fprintf(stderr, "The pc %04x is not valid!", state->pc);
        exit(EXIT_FAILURE);
    }

    uint8_t* op = &state->memory[state->pc];

    uint8_t op_code = op[0] >> 4;

    uint16_t nnn = ((op[0] & 0x0F) << 8) | op[1];
    uint8_t x = op[0] & 0x0F;
    uint8_t y = op[1] >> 4;
    uint8_t n = op[1] & 0x0F;

    printf("Processing pc %04x: %02x %02x\n", state->pc, op[0], op[1]);

    switch (op_code) {
        case 0x0:
            if (op_0(state, op)) {
                return 1;
            }
            break;
        case 0x1: op_1(state, nnn); break;
        case 0x2: op_2(state, nnn); break;
        case 0x3: op_3(state, x, op[1]); break;
        case 0x4: op_4(state, x, op[1]); break;
        case 0x5: op_5(state, op, x, y, n); break;
        case 0x6: op_6(state, x, op[1]); break;
        case 0x7: op_7(state, x, op[1]); break;
        case 0x8: op_8(state, op, x, y, n); break;
        case 0x9: op_9(state, op, x, y, n); break;
        case 0xA: op_A(state, nnn); break;
        case 0xB: op_B(state, nnn); break;
        case 0xC: op_C(state, x, op[1]); break;
        case 0xD: draw_sprite(state, x, y, n); break;
        case 0xE: op_E(state, op, x); break;
        case 0xF: op_F(state, op, x); break;
        default: op_unkown(state, op);
    }

    state->pc += 2;

    return 0;
}

int main(int32_t argc, char const* argv[]) {
    if (!argv[1]) {
        fprintf(stderr, "Usage: %s <chip8_rom.ch8>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    uint32_t fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    uint8_t* buffer = malloc(fsize);
    fread(buffer, fsize, 1, fp);
    fclose(fp);

    printf("Loaded rom: %s\n", argv[1]);

    Chip8State* s = init_chip8(buffer, fsize);
    free(buffer);
    buffer = NULL;

    uint8_t is_over = 0;
    while (!is_over) {
        is_over = process_op(s);
        print_display(s);
    }

    free_chip8(s);
    s = NULL;

    return EXIT_SUCCESS;
}
