#ifndef _SALLANG_AST_H
#define _SALLANG_AST_H

#include <common/hash.h>
#include <common/bstrlib.h>
#include "parser/symbol.h"
#include "parser/types.h"

typedef enum AstNodeType {
    /* Special nodes */
    AST_ROOT,
    AST_LIST,
    AST_SVALUE,
    AST_TYPENAME,

    /* Unary nodes */
    AST_NAME,
    AST_PRINT_STMT,
/*
    AST_UNARY_PLUS,
    AST_UNARY_MINUS,
    AST_PREFIX_INCR,
    AST_PREFIX_DECR,
    AST_POSTFIX_INCR,
    AST_POSTFIX_DECR, */
    AST_UNARY_NOT,

    /* Binary nodes */
    AST_BINARY_ASSIGN,
    AST_BINARY_ADD,
    AST_BINARY_SUB,
    AST_BINARY_MUL,
    AST_BINARY_DIV,
    AST_BINARY_MOD,

    AST_BINARY_IS_EQ,
    AST_BINARY_IS_LT,
    AST_BINARY_IS_GT,
    AST_BINARY_IS_LTEQ,
    AST_BINARY_IS_GREQ,
    AST_BINARY_IS_NOTEQ,

    AST_BINARY_AND,
    AST_BINARY_OR,

    AST_IF,                 // condition, stmt_list, else_stmt_list
    AST_WHILE,

    /* Ternary nodes */
    AST_VAR_DECL,           // id, type, initial value
    AST_CONST_DECL,         // id, type, initial value
    AST_FUNCTION           // id, args, initial value

} AstNodeType;


#if 0

typedef struct _Symbol {
    enum SValueType type;

    SValue value;
} Symbol;

#endif


typedef struct _AstNode {
    AstNodeType type;
    union {
        List *list;                        // AST_LIST
        SValue value;                      // AST_VALUE
        bstring name;
        struct _AstNode *children[3];
    };
    int lineno;
} AstNode;

/* Node constructors */
AstNode *AstNode_create(int lineno, AstNodeType type, AstNode *c0, AstNode *c1, AstNode *c2);

AstNode *AstNode_create_list();
AstNode *AstNode_list_add(AstNode *list_node, AstNode *node);

AstNode *AstNode_create_decl(int lineno, AstNode *name, SValueType type);
AstNode *AstNode_create_value(SValue value);
AstNode *AstNode_create_id(const char *name);
AstNode *AstNode_create_var(AstNode *name, const char *type, AstNode *init_val);

void AstNode_destroy(AstNode *ast);

#endif