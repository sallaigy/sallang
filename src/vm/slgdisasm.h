#ifndef _SALLANG_VM_SLGDISASM_H
#define _SALLANG_VM_SLGDISASM_H

#include "slgvm.h"

#include <stdio.h>

void slgasm_disassemble(slgvm_ir *input, uint32_t plen, FILE *output);

#endif