#ifndef _SALLANG_VM_VM_OPCODES_H
#define _SALLANG_VM_VM_OPCODES_H

/**
 * SLGVM: The sallang virtual machine.
 *
 * X, Y: 4 byte integers
 * D, R: 1 byte integers
 *
 * S[0]: The top of the stack
 * S[N]: The Nth element from the top element in the stack
 *
 * R[N]: The value of local variable register N
 * PC: Program counter register
 */
#define SLG_NOP     0x00    /* Do nothing */

/* Data manipulations */

#define SLG_PUSH    0x01    /* PUSH(X):  S[0] <-- X     */
#define SLG_STORE   0x02    /* STORE(X): R[X] <-- S[0]  */
#define SLG_LOAD    0x03    /* LOAD(X):  S[0] <-- R[X]  */
#define SLG_POP     0x04    /* POP(): Remove the top element from the stack  */

/* Arithmetics */

#define SLG_ADD     0x20    /* ADD():   S(1) <-- S(0) + S(1); */
#define SLG_SUB     0x21    /* SUB():   S(1) <-- S(0) - S(1); */
#define SLG_MUL     0x22    /* MUL():   S(1) <-- S(0) * S(1); */
#define SLG_AND     0x23    /* AND():   S(1) <-- S(0) & S(1); */
#define SLG_OR      0x24    /* OR():    S(1) <-- S(0) | S(1); */
#define SLG_XOR     0x25    /* XOR():   S(1) <-- S(0) ^ S(1); */
#define SLG_LSL     0x26    /* LSL():   S(1) <-- S(0) << S(1); */
#define SLG_LSR     0x27    /* LSR():   S(1) <-- S(0) >> S(1); */
#define SLG_NOT     0x28    /* NOT():   S[1] <-- ~S[0] */
#define SLG_INC     0x29    /* INC():   S[1] <-- S[0] + 1 */
#define SLG_DEC     0x2A    /* DEC():   S[1] <-- S[0] - 1 */
#define SLG_DIV     0x2B    /* DIV():   S[1] <-- S[0] / S[1] */
#define SLG_MOD     0x2C    /* MOD():   S[1] <-- S[0] % S[1] */
#define SLG_CMPEQ   0x2D
#define SLG_CMPGT   0x2E
#define SLG_CMPLT   0X30
#define SLG_CMPGTEQ 0x31
#define SLG_CMPLTEQ 0x32

/* IO */

#define SLG_PRINT   0x50    /* PRINT(X):    Prints the top of the stack to stdout */

/* Branch instructions */
#define SLG_GOTO    0x60    /* Unconditional jump:  GOTO(X): PC <-- X */
#define SLG_JMPZ    0x61    /* Jump if zero:        JUMPZ(X):  IF (S[0] == 0) PC <-- X  */
#define SLG_JMPNZ   0x62    /* Jump if not zero:    JUMPNZ(X): IF (S[0] != 0) PC <-- X */
#define SLG_CALL    0x63    /* Call subroutine */

/* VM */

#define SLG_HALT    0xFF    /* Halts the virtual machine */

#endif
