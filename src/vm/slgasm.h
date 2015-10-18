#ifndef _SALLANG_VM_SLGASM_H
#define _SALLANG_VM_SLGASM_H

#include "slgvm.h"

#include <stdio.h>

void slgasm_assemble(FILE *input, slgvm_ir *output, int32_t *plen);

#endif
