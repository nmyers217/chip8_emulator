#include "config.h"

// NOTE: this is not its own translation unit, the operations are in here just
// for organization

void op_unkown(Chip8State* state, uint8_t* op) {
    printf("%02x %02x is an uknown operation!\n", op[0], op[1]);
}

// TODO: debug
static inline uint8_t op_0(Chip8State* state, uint8_t* op) {
    if (op[1] == 0xE0) {
        for (uint16_t i = 0; i < MEMORY_SIZE - DISPLAY_MEMORY_OFFSET; i++) {
            state->display[i] = 0;
        }
    } else if (op[1] == 0xEE) {
        state->pc = state->memory[state->sp];
        state->sp -= 2;
        if (state->sp < STACK_MEMORY_OFFSET) {
            return 1;
        }
    } else {
        printf("Operation 0NNN not supported!\n");
    }
    return 0;
}

static inline void op_1(Chip8State* state, uint16_t nnn) {
    state->pc = nnn - 2;
}

// TODO: debug
static inline void op_2(Chip8State* state, uint16_t nnn) {
    state->sp += 2;
    if (state->sp >= STACK_MEMORY_OFFSET + (MAX_STACK_FRAMES * 2)) {
        fprintf(stderr, "STACK OVERFLOW!\n");
        exit(EXIT_FAILURE);
    }
    state->memory[state->sp] = state->pc;
    state->pc = nnn;
}

static inline void op_3(Chip8State* state, uint8_t x, uint8_t nn) {
    if (state->v[x] == nn) {
        state->pc += 2;
    }
}

static inline void op_4(Chip8State* state, uint8_t x, uint8_t nn) {
    if (state->v[x] != nn) {
        state->pc += 2;
    }
}

static inline void op_5(Chip8State* state, uint8_t* op, uint8_t x, uint8_t y,
                        uint8_t n) {
    if (n == 0x0) {
        if (state->v[x] == state->v[y]) {
            state->pc += 2;
        }
    } else {
        op_unkown(state, op);
    }
}

static inline void op_6(Chip8State* state, uint8_t x, uint8_t nn) {
    state->v[x] = nn;
}

static inline void op_7(Chip8State* state, uint8_t x, uint8_t nn) {
    state->v[x] += nn;
}

static inline void op_8(Chip8State* state, uint8_t* op, uint8_t x, uint8_t y,
                        uint8_t n) {
    switch (n) {
        case 0x0: state->v[x] = state->v[y]; break;
        case 0x1: state->v[x] |= state->v[y]; break;
        case 0x2: state->v[x] &= state->v[y]; break;
        case 0x3: state->v[x] ^= state->v[y]; break;
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
            state->v[0xF] = state->v[x] & 1;
            state->v[x] >>= 1;
            break;
        case 0x7:
            state->v[0xF] = state->v[y] > state->v[x];
            state->v[x] = state->v[y] - state->v[x];
            break;
        case 0xE:
            state->v[0xF] = (state->v[x] >> 7) & 1;
            state->v[x] <<= 1;
            break;
        default: op_unkown(state, op);
    }
}

static inline void op_9(Chip8State* state, uint8_t* op, uint8_t x, uint8_t y,
                        uint8_t n) {

    if (n == 0x0) {
        if (state->v[x] != state->v[y]) {
            state->pc += 2;
        }
    } else {
        op_unkown(state, op);
    }
}

static inline void op_A(Chip8State* state, uint16_t nnn) { state->i = nnn; }

static inline void op_B(Chip8State* state, uint16_t nnn) {
    state->pc = nnn + state->v[0];
}

// TODO: debug
static inline void op_C(Chip8State* state, uint8_t x, uint8_t nn) {
    state->v[x] = (rand() % 255) & nn;
}

// TODO: debug
static inline void op_E(Chip8State* state, uint8_t* op, uint8_t x) {
    if (op[1] == 0x9E) {
        if (state->keys[KEY_TO_INDEX_MAPPING[state->v[x]]] == 1) {
            state->pc += 2;
        }
    } else if (op[1] == 0xA1) {
        if (state->keys[KEY_TO_INDEX_MAPPING[state->v[x]]] == 0) {
            state->pc += 2;
        }
    } else {
        op_unkown(state, op);
    }
}

static inline void op_F(Chip8State* state, uint8_t* op, uint8_t x) {
    switch (op[1]) {
        case 0x07: state->v[x] = state->delay; break;
        case 0x0A:
            // TODO: need to wait for a key press, this is platform specific
            op_unkown(state, op);
            break;
        case 0x15: state->delay = state->v[x]; break;
        case 0x18: state->sound = state->v[x]; break;
        case 0x1E: state->i += state->v[x]; break;
        case 0x29: {
            state->i = state->memory[state->v[x] * FONT_SPRITE_BYTE_COUNT];
            break;
        }
        case 0x33: {
            uint8_t n = state->v[x];
            uint8_t hundreds = n / 100;
            uint8_t tens = (n - (hundreds * 100)) / 10;
            uint8_t ones = n % 10;
            state->memory[state->i] = hundreds;
            state->memory[state->i + 1] = tens;
            state->memory[state->i + 2] = ones;
            break;
        }
        case 0x55: {
            for (uint8_t i = 0; i < 16; i++) {
                state->memory[state->i + i] = state->v[i];
            }
            break;
        }
        case 0x65: {
            for (uint8_t i = 0; i < 16; i++) {
                state->v[i] = state->memory[state->i + i];
            }
            break;
        }
        default: op_unkown(state, op);
    }
}
