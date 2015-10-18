#include <stdio.h>
#include "vm/slgdisasm.h"
#include "vm/vm_opcodes.h"

static const char *slgasm_get_opcode_name(uint8_t opcode)
{
    #define SLGASM_OPCODE_NAME_CASE(OPCODE, NAME) \
        case OPCODE: return NAME; break;

    switch (opcode) {
        SLGASM_OPCODE_NAME_CASE(SLG_NOP,     "nop")
        SLGASM_OPCODE_NAME_CASE(SLG_PUSH,    "push")
        SLGASM_OPCODE_NAME_CASE(SLG_STORE,   "store")
        SLGASM_OPCODE_NAME_CASE(SLG_LOAD,    "load")
        SLGASM_OPCODE_NAME_CASE(SLG_ADD,     "add")
        SLGASM_OPCODE_NAME_CASE(SLG_SUB,     "sub")
        SLGASM_OPCODE_NAME_CASE(SLG_MUL,     "mul")
        SLGASM_OPCODE_NAME_CASE(SLG_AND,     "and")
        SLGASM_OPCODE_NAME_CASE(SLG_OR,      "or")
        SLGASM_OPCODE_NAME_CASE(SLG_XOR,     "xor")
        SLGASM_OPCODE_NAME_CASE(SLG_LSL,     "lsl")
        SLGASM_OPCODE_NAME_CASE(SLG_LSR,     "lsr")
        SLGASM_OPCODE_NAME_CASE(SLG_NOT,     "not")
        SLGASM_OPCODE_NAME_CASE(SLG_INC,     "inc")
        SLGASM_OPCODE_NAME_CASE(SLG_DEC,     "dec")
        SLGASM_OPCODE_NAME_CASE(SLG_DIV,     "div")
        SLGASM_OPCODE_NAME_CASE(SLG_MOD,     "mod")
        SLGASM_OPCODE_NAME_CASE(SLG_CMPEQ,   "cmpeq")
        SLGASM_OPCODE_NAME_CASE(SLG_CMPGT,   "cmpgt")
        SLGASM_OPCODE_NAME_CASE(SLG_CMPLT,   "cmplt")
        SLGASM_OPCODE_NAME_CASE(SLG_CMPGTEQ, "cmpgteq")
        SLGASM_OPCODE_NAME_CASE(SLG_CMPLTEQ, "cmplteq")
        SLGASM_OPCODE_NAME_CASE(SLG_PRINT,  "print")
        SLGASM_OPCODE_NAME_CASE(SLG_GOTO,   "goto")
        SLGASM_OPCODE_NAME_CASE(SLG_JMPZ,   "jmpz")
        SLGASM_OPCODE_NAME_CASE(SLG_JMPNZ,  "jmpnz")
        SLGASM_OPCODE_NAME_CASE(SLG_HALT,   "halt")
        default:
            return "__INVALID__";
            break;
    }
}

void slgasm_disassemble(slgvm_ir program[], uint32_t plen, FILE *fp)
{
    uint32_t i = 0;
    int val = 0;
    uint32_t addr = 0;

    while (i < plen) {
        uint8_t op = program[i];

        #define SLGASM_CASE_OPCODE(OPCODE)                      \
            case OPCODE:                                        \
                fprintf(                                        \
                    fp,                                         \
                    "%08X %02X          %s\n",                  \
                    i,                                          \
                    OPCODE,                                     \
                    slgasm_get_opcode_name(OPCODE)              \
                );                                              \
                break;

        #define SLGASM_CASE_OPCODE_8(OPCODE)                    \
            case OPCODE:                                        \
                addr = i;                                       \
                val = program[++i];                             \
                fprintf(                                        \
                    fp,                                         \
                    "%08X %02X%02X        %-7s0x%x\n",               \
                    addr,                                       \
                    OPCODE,                                     \
                    val,                                        \
                    slgasm_get_opcode_name(OPCODE),             \
                    val                                         \
                );                                              \
                break;

        #define SLGASM_CASE_OPCODE_32(OPCODE)                   \
            case OPCODE:                                        \
                addr = i;                                       \
                for (int j = 0; j < 4; j++) {                   \
                    val = val << 8;                             \
                    val += program[++i];                        \
                }                                               \
                fprintf(                                        \
                    fp,                                         \
                    "%08X %02X%08X  %-7s0x%x\n",                 \
                    addr,                                       \
                    OPCODE,                                     \
                    val,                                        \
                    slgasm_get_opcode_name(OPCODE),             \
                    val                                         \
                );                                              \
                break;


        switch (op) {
            SLGASM_CASE_OPCODE(SLG_NOP)

            SLGASM_CASE_OPCODE_32(SLG_PUSH)
            SLGASM_CASE_OPCODE_8(SLG_STORE)
            SLGASM_CASE_OPCODE_8(SLG_LOAD)

            SLGASM_CASE_OPCODE(SLG_ADD)
            SLGASM_CASE_OPCODE(SLG_SUB)
            SLGASM_CASE_OPCODE(SLG_MUL)
            SLGASM_CASE_OPCODE(SLG_AND)
            SLGASM_CASE_OPCODE(SLG_OR)
            SLGASM_CASE_OPCODE(SLG_XOR)
            SLGASM_CASE_OPCODE(SLG_LSL)
            SLGASM_CASE_OPCODE(SLG_LSR)
            SLGASM_CASE_OPCODE(SLG_NOT)
            SLGASM_CASE_OPCODE(SLG_INC)
            SLGASM_CASE_OPCODE(SLG_DEC)
            SLGASM_CASE_OPCODE(SLG_DIV)
            SLGASM_CASE_OPCODE(SLG_MOD)
            SLGASM_CASE_OPCODE(SLG_CMPEQ)
            SLGASM_CASE_OPCODE(SLG_CMPGT)
            SLGASM_CASE_OPCODE(SLG_CMPLT)
            SLGASM_CASE_OPCODE(SLG_CMPGTEQ)
            SLGASM_CASE_OPCODE(SLG_CMPLTEQ)

            SLGASM_CASE_OPCODE(SLG_PRINT)

            SLGASM_CASE_OPCODE_32(SLG_GOTO)
            SLGASM_CASE_OPCODE_32(SLG_JMPZ)
            SLGASM_CASE_OPCODE_32(SLG_JMPNZ)

            SLGASM_CASE_OPCODE(SLG_HALT)
            default:
                break;
        }

        i++;
    }
}