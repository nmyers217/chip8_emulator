#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


// Chip8 programs start at this memory location
// IMPORTANT: this MUST be an even number
uint16_t const PROGRAM_START_MEMORY = 0x200;


/**
 * Print out the disassembled instruction at the program counter
 *
 * Instruction set is available here:
 * https://github.com/mattmikolay/chip-8/wiki/CHIP‐8-Instruction-Set
 */
void DisassembleInstruction(uint8_t const* program, uint16_t pc)
{
    assert(pc % 2 == 0);

    // An operation is 2 big endian bytes
    uint8_t const* op = &program[pc];
    uint8_t nibble = op[0] >> 4;

    printf("%04x %-2s %02x %02x %-2s ", pc, "", op[0], op[1], "");

    switch (nibble) {
        case 0x0:
            if (op[0] == 0x00 && op[1] == 0xE0) {
                printf("%s", "CLEAR");
            } else if (op[0] == 0x00 && op[1] == 0xEE) {
                printf("%s", "RET");
            } else {
                // NOTE: This code is not longer in use
                printf("Uninmplemented!");
            }
            break;
        case 0x1:
            printf("%-8s %01x%02x", "JUMP", op[0] & 0x0F, op[1]);
            break;
        case 0x2:
            printf("%-8s %01x%02x", "CALL", op[0] & 0x0F, op[1]);
            break;
        case 0x3:
            printf("%-8s V%01x, %02x", "SKIP==", op[0] & 0x0F, op[1]);
            break;
        case 0x4:
            printf("%-8s V%01x, %02x", "SKIP!=", op[0] & 0x0F, op[1]);
            break;
        case 0x5:
            printf("%-8s V%01x, V%01x", "SKIP==", op[0] & 0x0F, op[1] >> 4);
            break;
        case 0x6:
            printf("%-8s V%01x, %02x", "MOVE", op[0] & 0x0F, op[1]);
            break;
        case 0x7:
            printf("%-8s V%01x, %02x", "ADD", op[0] & 0x0F, op[1]);
            break;
        case 0x8: {
            uint8_t flag = op[1] & 0x0F;
            uint8_t x = op[0] & 0x0F;
            uint8_t y = op[1] >> 4;

            switch (flag) {
                case 0x0:
                    printf("%-8s V%01x, V%01x", "MOVE", x, y);
                    break;
                case 0x1:
                    printf("%-8s V%01x, V%01x", "OR", x, y);
                    break;
                case 0x2:
                    printf("%-8s V%01x, V%01x", "AND", x, y);
                    break;
                case 0x3:
                    printf("%-8s V%01x, V%01x", "XOR", x, y);
                    break;
                case 0x4:
                    printf("%-8s V%01x, V%01x", "ADD", x, y);
                    break;
                case 0x5:
                    printf("%-8s V%01x, V%01x", "SUB", x, y);
                    break;
                case 0x6:
                    printf("%-8s V%01x, V%01x", "SHIFTR", x, y);
                    break;
                case 0x7:
                    printf("%-8s V%01x, V%01x", "SUB", y, x);
                    break;
                case 0xE:
                    printf("%-8s V%01x, V%01x", "SHIFTL", x, y);
                    break;
                default:
                    printf("%01x not handled yet!", nibble);
            }

            break;
        }
        default:
            printf("%01x not handled yet!", nibble);
    }


    printf("\n");
}


int main(int arc, char* argv[]) {
    if (argv[1] == NULL) {
        printf("Usage: disassembler <chip8_program>\n");
        exit(EXIT_SUCCESS);
    }

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    uint32_t fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    uint8_t* buffer = malloc(fsize + PROGRAM_START_MEMORY);
    fread(buffer + PROGRAM_START_MEMORY, fsize, 1, fp);
    fclose(fp);

    uint16_t pc = PROGRAM_START_MEMORY;
    while (pc < fsize + PROGRAM_START_MEMORY) {
        DisassembleInstruction(buffer, pc);
        pc += 2;
    }

    free(buffer);

    return EXIT_SUCCESS;
}

