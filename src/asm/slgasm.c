#include "asm/slgasm.h"
#include "vm/vm_opcodes.h"
#include "common/hash.h"
#include "common/bstrlib.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

enum slgasm_argtype {
    NoArgument, ByteArgument, WordArgument, LabelArgument
};

enum slgasm_InstructionType {
    Pivot, Label, Instruction
};

typedef struct _slgasm_instruction {
    enum slgasm_InstructionType irtype;
    union {
        bstring label;
        struct {    // instruction
            slgvm_ir op;
            enum slgasm_argtype argtype;
            union {
                slgvm_byte b;
                slgvm_word i;
                bstring reflabel;
            };
        };
    };
    int32_t addr;   // The eventual address of the instruction

    struct _slgasm_instruction *next;
    struct _slgasm_instruction *prev;
} slgasm_instruction;

typedef struct _slgasm_LabelEntry {
    bstring name;
    int32_t addr;
} slgasm_LabelEntry;

/*
    bin_digit -> '0' | '1'
    dec_digit -> '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
    hex_digit_lc -> dec_digit | 'a' | 'b' | 'c' | 'd' | 'e' | 'f
    hex_digit_uc -> dec_digit | 'A' | 'B' | 'C' | 'D' | 'E' | 'F
    hex_digit    -> hex_digit_lc | hex_digit_uc

    bin_number -> 'b' bin_digit*
    dec_number -> dec_digit*
    hex_number -> "0x" hex_digit*

    number -> bin_number | dec_number | hex_number

    irname -> "push" | "add" | "sub" | ...
    label  -> STRING

    instruction -> irname | irname number | irname label

    line -> label ':' | instruction | ''

    start -> line*
 */

static inline void skip_whitespace(char **str)
{
    size_t i = 0;
    char *tmp = *str;

    while (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == '\r' || tmp[i] == '\n') {
        i++;
    }

    tmp += i;
    *str = tmp;
}

static inline bool is_dec_digit(char c)
{
    return isdigit(c);
}

static inline bool is_hex_digit(char c)
{
    return is_dec_digit(c)
        || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f'
        || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F';
}

static inline bool is_bin_digit(char c)
{
    return c == '0' || c == '1';
}

static inline int hextodec(char c)
{
    switch (c) {
        case 'a':
        case 'A':
            return 10;
            break;
        case 'b':
        case 'B':
            return 11;
            break;
        case 'c':
        case 'C':
            return 12;
            break;
        case 'd':
        case 'D':
            return 13;
            break;
        case 'e':
        case 'E':
            return 14;
            break;
        case 'f':
        case 'F':
            return 15;
            break;
        default:
            return c - '0';
            break;
    }
}

static bool is_dec_number(bstring *str, uint32_t *value)
{
    bstring tmp = *str;
    int val = 0, pos = 0;

    if (is_dec_digit(bchar(tmp, pos))) {
        val = bchar(tmp, pos) - '0';
        pos++;

        while (is_dec_digit(bchar(tmp, pos))) {
            val = val * 10 + (bchar(tmp, pos) - '0');
            pos++;
        }

        *value = val;
        bassignmidstr(*str, tmp, 0, pos);

        return true;
    }

    return false;
}

static bool is_bin_number(bstring *str, uint32_t *value)
{
    bstring tmp = *str;
    int val = 0, pos = 0;

    if (bchar(tmp, pos) == 'b') {
        pos++;

        while (is_bin_digit(bchar(tmp, pos))) {
            val = val * 2 + (bchar(tmp, pos) - '0');
            pos++;
        }

        *value = val;
        bassignmidstr(*str, tmp, 0, pos);

        return true;
    }

    return false;
}

static bool is_hex_number(bstring *str, uint32_t *value)
{
    bstring tmp = *str;
    int val = 0, pos = 0;

    if (bchar(tmp, pos) == '0' && bchar(tmp, ++pos) == 'x') {
        pos++;

        while (is_hex_digit(bchar(tmp, pos))) {
            val += val * 16 + hextodec(bchar(tmp, pos));
            pos++;
        }

        *value = val;
        bassignmidstr(*str, tmp, 0, pos);

        return true;
    }

    return false;
}

static bool is_number_scalar(bstring *str, uint32_t *value)
{
    uint32_t tmpval;

    if (is_hex_number(str, &tmpval) || is_dec_number(str, &tmpval) || is_bin_number(str, &tmpval)) {
        *value = tmpval;
        return true;
    }

    return false;
}

static bool is_irname(bstring *str, slgvm_ir *kind, enum slgasm_argtype *argtype)
{
    bstring tmp = *str;
    bstring irname_tmp = bfromcstralloc(5, "");

    #define SLGASM_IRNAME_SEARCH(OPCODE, IRNAME, ARGTYPE)                   \
        bassigncstr(irname_tmp, IRNAME);                                    \
        if (BSTR_ERR != binstr(tmp, 0, irname_tmp)) {                       \
            *kind = OPCODE;                                                 \
            *argtype = ARGTYPE;                                             \
            *str = bmidstr(tmp, blength(irname_tmp), 255);                  \
            bdestroy(irname_tmp);                                           \
            return true;                                                    \
        }

    SLGASM_IRNAME_SEARCH(SLG_NOP,       "nop",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_PUSH,      "push",     WordArgument)
    SLGASM_IRNAME_SEARCH(SLG_STORE,     "store",    ByteArgument)
    SLGASM_IRNAME_SEARCH(SLG_LOAD,      "load",     ByteArgument)
    SLGASM_IRNAME_SEARCH(SLG_ADD,       "add",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_SUB,       "sub",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_MUL,       "mul",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_AND,       "and",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_OR,        "or",       NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_XOR,       "xor",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_LSL,       "lsl",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_LSR,       "lsr",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_NOT,       "not",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_INC,       "inc",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_DEC,       "dec",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_DIV,       "div",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_MOD,       "mod",      NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_CMPEQ,     "cmpeq",    NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_CMPGT,     "cmpgt",    NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_CMPLT,     "cmplt",    NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_CMPGTEQ,   "cmpgteq",  NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_CMPLTEQ,   "cmplteq",  NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_PRINT,     "print",    NoArgument)
    SLGASM_IRNAME_SEARCH(SLG_GOTO,      "goto",     LabelArgument)
    SLGASM_IRNAME_SEARCH(SLG_JMPZ,      "jmpz",     LabelArgument)
    SLGASM_IRNAME_SEARCH(SLG_JMPNZ,     "jmpnz",    LabelArgument)
    SLGASM_IRNAME_SEARCH(SLG_HALT,      "halt",     NoArgument)

    #undef SLGASM_IRNAME_SEARCH

    bdestroy(irname_tmp);

    return false;
}

static bool is_label(bstring *str, bstring result)
{
    size_t i = 0;
    bstring tmp = *str;

    // Labels are strings which do not start with a number
    if (is_dec_digit(bchar(tmp, 0))) {
        return false;
    }

    while (
        bchar(tmp, i) != '\0' &&
        bchar(tmp, i) != '\n' &&
        bchar(tmp, i) != '\r' &&
        bchar(tmp, i) != ' '  &&
        bchar(tmp, i) != '\t' &&
        bchar(tmp, i) != ':'  &&
        bchar(tmp, i) != ';') {
        i++;
    }

    if (i == 0) {
        return false;
    }

    bassignmidstr(result, tmp, 0, i);

    bassignmidstr(*str, tmp, i, blength(result));

    return true;
}

static bool is_instruction(bstring *str, slgasm_instruction *ir)
{
    bstring tmp       = *str;
    bstring label_tmp = bfromcstr("");

    bltrimws(tmp);

    if (is_irname(&tmp, &ir->op, &ir->argtype)) {
        ir->irtype = Instruction;
        bltrimws(tmp);
        if (is_number_scalar(&tmp, &ir->i)) {
            // empty
        } else if (is_label(&tmp, label_tmp)) {
            ir->reflabel = bfromcstr("");
            bassign(ir->reflabel, label_tmp);
        }

        *str = tmp;

        bdestroy(label_tmp);

        return true;
    }

    bdestroy(label_tmp);

    return false;
}

static bool is_label_location(bstring *str, slgasm_instruction *ir, HashTable *labels)
{
    bstring tmp = *str;
    bstring label = bfromcstr("");

    bltrimws(tmp);

    if (is_label(&tmp, label)) {
        if (bchar(tmp, 0) == ':') {
            bassignmidstr(*str, label, 0, blength(label));
            ir->irtype = Label;
            ir->label = bstrcpy(label);

            HashTable_set(labels, bdata(ir->label), ir);

            bdestroy(label);

            return true;
        }
    }

    bdestroy(label);

    return false;
}

void slgasm_assemble(FILE *fp, slgvm_ir *output, int32_t *plen)
{
    bstring line;
    HashTable *labels = HashTable_create(100);

    slgasm_instruction *irs_head = malloc(sizeof(slgasm_instruction));
    slgasm_instruction *irs = irs_head;

    slgasm_LabelEntry label_refs[100];

    size_t pc = 0, irc = 0, lec = 0;
    int lineno = 0;

    /* Parse the given file */
    while ((line = bgets((bNgetc) fgetc, fp, '\n')) != NULL) {
        lineno++;
        bltrimws(line);

        if (line->slen == 0) {
            continue;
        }

        if (bchar(line, 0) == '#') {
            continue;
        }

        if (is_instruction(&line, irs) || is_label_location(&line, irs, labels)) {
            irs->next = malloc(sizeof(slgasm_instruction));
            irc++;
            irs = irs->next;
        }

        bltrimws(line);

        if (BSTR_ERR != bstrchr(line, '#')) {
            continue;
        }
        // else {
        //     fprintf(stderr, "ERROR: Parse error at line %d, near '%s'.\n", lineno, bdata(line));
        //     exit(EXIT_FAILURE);
        // }
    }

    bdestroy(line);

    /* Code generation */
    for (slgasm_instruction *iter = irs_head; iter != NULL; iter = iter->next) {
        slgasm_instruction curr = *iter;
        if (curr.irtype == Instruction) {
            output[pc++] = curr.op;

            if (curr.argtype == WordArgument) {
                output[pc++] = (curr.i >> 24) & 0xFF;
                output[pc++] = (curr.i >> 16) & 0xFF;
                output[pc++] = (curr.i >> 8)  & 0xFF;
                output[pc++] = curr.i & 0xFF;

                curr.addr = pc;
            } else if (curr.argtype == ByteArgument) {
                output[pc++] = curr.b;
                curr.addr = pc;
            } else if (curr.argtype == LabelArgument) {
                label_refs[lec].name = bstrcpy(curr.reflabel);
                label_refs[lec++].addr = pc;

                output[pc++] = 0x00;
                output[pc++] = 0x00;
                output[pc++] = 0x00;
                output[pc++] = 0x00;
            }
        } else if (curr.irtype == Label) {
            slgasm_instruction *label_ir = HashTable_get(labels, bdata(curr.label));
            if (NULL != label_ir) {
                label_ir->addr = pc;
            }
        }
    }

    /* Resolve labels */
    for (size_t i = 0; i < lec; i++) {
        char *label_name = bdata(label_refs[i].name);
        slgasm_instruction *label_ir = HashTable_get(labels, label_name);

        bdestroy(label_refs[i].name);
        if (NULL == label_ir) {
            fprintf(
                stderr,
                "ERROR: Reference to unknown label %s.\n",
                label_name
            );
        }

        int32_t ref_addr = label_refs[i].addr;
        output[ref_addr++] = (label_ir->addr >> 24) & 0xFF;
        output[ref_addr++] = (label_ir->addr >> 16) & 0xFF;
        output[ref_addr++] = (label_ir->addr >> 8)  & 0xFF;
        output[ref_addr++] = label_ir->addr & 0xFF;
    }

    *plen = pc;

    HashTable_destroy(labels);
}
