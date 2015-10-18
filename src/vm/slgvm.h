#ifndef _SALLANG_VM_SLGVM_H
#define _SALLANG_VM_SLGVM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SLGVM_STACK_SIZE        1024
#define SLGVM_REGISTER_COUNT    16
#define SLGVM_PROGRAM_LENGTH    65535

typedef uint8_t slgvm_ir;

typedef uint32_t slgvm_word;

typedef uint8_t  slgvm_byte;
typedef int8_t   slgvm_sbyte;
typedef uint16_t slgvm_ushort;
typedef int16_t  slgvm_short;
typedef uint32_t slgvm_uint;
typedef int32_t  slgvm_int;
typedef uint64_t slgvm_ulong;
typedef int64_t  slgvm_long;


typedef struct _slgvm_core {

    /* SLGVM stack, 1024 32-bit words */
    slgvm_word stack[SLGVM_STACK_SIZE];

    /* SLGVM general purpose registers, 16 32-bit words */
    slgvm_word r[SLGVM_REGISTER_COUNT];

    /* SLGVM real number registers, 16 32-bit words */
    slgvm_word d[SLGVM_REGISTER_COUNT];

    /* SLGVM program code */
    slgvm_ir program[SLGVM_PROGRAM_LENGTH];
    int32_t plen; /* Program length */

    /* Special registers */

    int32_t pc; /* Program counter */
    int32_t sp; /* Stack pointer   */
    int32_t fp; /* Frame pointer   */

    /* Status register */
    struct {
        unsigned int eq    : 1;
        unsigned int sign  : 1;
        unsigned int carry : 1;
    } status;

} slgvm_core;

void slgvm_init(slgvm_core *vm);
void slgvm_execute(slgvm_core *vm);
void slgvm_load_program(slgvm_core *vm, FILE *fp);

typedef enum _slgvm_error_type {
    DivisionByZeroError, InvalidInstructionError, StackOverflowError, VmError
} slgvm_error_type;

void slgvm_error(slgvm_core *vm, slgvm_error_type error, const char *msg);

#endif