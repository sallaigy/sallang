#include "vm/slgvm.h"
#include "vm/vm_opcodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static inline uint32_t slgvm_program_fetch(slgvm_core *vm, size_t n)
{
    int val = 0;

    for (int i = 0; i < n; i++) {
        val = val << 8;
        val += vm->program[vm->pc++];
    }

    return val;
}

static inline void vm_debug_dump(FILE *fp, slgvm_core *vm)
{
#if 1
    return;
#endif

    fprintf(fp, "[SLGVM](debug) @0x%08x STACK=[", vm->pc);
    for (size_t i = 0; i <= vm->sp + 1; i++) {
        fprintf(fp, "%d ", vm->stack[i]);
    }
    fprintf(fp, "] REG=[");
    for (size_t i = 0; i < SLGVM_REGISTER_COUNT; i++) {
        fprintf(fp, "%d ", vm->r[i]);
    }
    fprintf(fp, "];\n");
}

void slgvm_execute(slgvm_core *vm)
{
    /* Initialize stack and registers */
    size_t i;

    for (i = 0; i < SLGVM_STACK_SIZE; i++) {
        vm->stack[i] = 0;
    }

    for (i = 0; i < SLGVM_REGISTER_COUNT; i++) {
        vm->r[i] = 0;
    }

    vm->pc = 0;
    vm->sp = -1;
    vm->fp = -1;

    bool halt = false;

    slgvm_word x, y;

    while (!halt) {
        uint8_t op = (slgvm_ir) slgvm_program_fetch(vm, 1);

        switch (op) {
            case SLG_NOP:   /* Do nothing */
                vm_debug_dump(stderr, vm);
                break;
            case SLG_PUSH:  /* Push a constant to the top of the stack */
                vm->stack[++vm->sp] = slgvm_program_fetch(vm, 4);
                vm_debug_dump(stderr, vm);
                break;
            case SLG_STORE: /* Store the top of the stack into a register */
                x = slgvm_program_fetch(vm, 1);
                vm->r[x] = vm->stack[vm->sp--];
                vm_debug_dump(stderr, vm);
                break;
            case SLG_LOAD:  /* Push a register value onto the stack */
                x = slgvm_program_fetch(vm, 1);
                vm->stack[++vm->sp] = vm->r[x];
                vm_debug_dump(stderr, vm);
                break;
            case SLG_ADD:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y + x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_SUB:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y - x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_MUL:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y * x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_AND:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y & x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_OR:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y | x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_XOR:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y ^ x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_LSL:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y << x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_LSR:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                vm->stack[--vm->sp] = y >> x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_NOT:
                vm->stack[vm->sp] = !vm->stack[vm->sp];
                vm_debug_dump(stderr, vm);
                break;
            case SLG_INC:
                vm->stack[vm->sp]++;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_DEC:
                vm->stack[vm->sp]--;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_DIV:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];
                if (y == 0) {
                    slgvm_error(vm, DivisionByZeroError, NULL);
                }

                vm->stack[--vm->sp] = y / x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_MOD:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];vm_debug_dump(stderr, vm);
                if (x == 0) {
                    slgvm_error(vm, DivisionByZeroError, NULL);
                }
                //fprintf(stderr, "x: %d, y: %d, x%%y = %d \n", x, y, x%y);

                vm->stack[--vm->sp] = y % x;
                vm_debug_dump(stderr, vm);

                break;
            case SLG_CMPGT:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];

                if (y > x) {
                    vm->stack[--vm->sp] = 1;
                } else {
                    vm->stack[--vm->sp] = 0;
                }
                vm_debug_dump(stderr, vm);
                break;
            case SLG_CMPLT:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];

                if (y < x) {
                    vm->stack[--vm->sp] = 1;
                } else {
                    vm->stack[--vm->sp] = 0;
                }
                vm_debug_dump(stderr, vm);
                break;
            case SLG_CMPGTEQ:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];

                if (y >= x) {
                    vm->stack[--vm->sp] = 1;
                } else {
                    vm->stack[--vm->sp] = 0;
                }
                vm_debug_dump(stderr, vm);
                break;
            case SLG_CMPLTEQ:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];

                if (y <= x) {
                    vm->stack[--vm->sp] = 1;
                } else {
                    vm->stack[--vm->sp] = 0;
                }
                vm_debug_dump(stderr, vm);
                break;
            case SLG_CMPEQ:
                x = vm->stack[vm->sp];
                y = vm->stack[vm->sp - 1];

                if (y == x) {
                    vm->stack[--vm->sp] = 1;
                } else {
                    vm->stack[--vm->sp] = 0;
                }
                vm_debug_dump(stderr, vm);
                break;
            case SLG_PRINT:
                fprintf(stdout, "%d\n", vm->stack[vm->sp]);
                vm_debug_dump(stderr, vm);
                break;
            case SLG_GOTO:  /* Unconditional jump */
                x = slgvm_program_fetch(vm, 4);
                vm->pc = x;
                vm_debug_dump(stderr, vm);
                break;
            case SLG_JMPZ:  /* Jump if zero */
                x = vm->stack[vm->sp];
                y = slgvm_program_fetch(vm, 4);
                if (x == 0) {
                    vm->pc = y;
                }
                vm_debug_dump(stderr, vm);
                break;
            case SLG_JMPNZ:  /* Jump if not zero */
                x = vm->stack[vm->sp];
                y = slgvm_program_fetch(vm, 4);
                if (x != 0) {
                    vm->pc = y;
                }
                vm_debug_dump(stderr, vm);
                break;
            case SLG_HALT:
                halt = true;
                break;
            default:
                slgvm_error(vm, InvalidInstructionError, NULL);
                break;
        }
    }
}

void slgvm_load_program(slgvm_core *vm, FILE *fp)
{
    int nRead;
    slgvm_ir *pi = vm->program;
    vm->plen = -1;

    do {
        nRead = fread(pi, sizeof(slgvm_ir), 1, fp);
        //fprintf(stderr, "%02x\n", *pi);
        pi++;
        vm->plen++;
    } while (nRead > 0);
}

void slgvm_error(slgvm_core *vm, slgvm_error_type error, const char *msg)
{
    switch (error) {
        case DivisionByZeroError:
            fprintf(stderr, "[SLGVM] DivisionByZeroError\n");
            fprintf(stderr, "[SLGVM] Division by zero at address 0x%08x.\n", vm->pc);
            break;
        case InvalidInstructionError:
            fprintf(stderr, "[SLGVM] InvalidInstructionError\n");
            fprintf(
                stderr,
                "[SLGVM] Invalid instruction 0x%2x at address 0x%08x.\n",
                vm->program[vm->pc],
                vm->pc
            );
            break;
        case VmError:
        default:
            fprintf(stderr, "[SLGVM] Virtual machine error.\n");
            break;
    }

    if (msg != NULL) { // If we got some additional info, print it.
        fprintf(stderr, msg);
    }

    fprintf(stderr, "[SLGVM] Exception occured. Stop.\n");

    exit(1);
}
