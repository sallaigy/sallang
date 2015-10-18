#ifndef _SALLANG_COMPILE_H
#define _SALLANG_COMPILE_H

#include <stdio.h>

#include <parser/ast.h>
#include "parser/symbol.h"
#include "common/vector.h"

/**
 * Compiler context: global variables for compilation
 */
typedef struct _CompilerContext {

    AstNode     *ast_root;
    SymbolTable *symbol_table;
    /* Intermediate representation */
    Vector      *irs;

} CompilerContext;

/**
 * Compiles the Abstract Syntax Tree given by ast, and outputs the assembly code into fp.
 *
 * @param  ast The syntax tree to compile
 * @param  fp  The resulting file
 *
 * @return
 */
bool compile(AstNode *ast, FILE *fp);

#endif
