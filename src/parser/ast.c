#include <parser/ast.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/hash.h>
#include <common/list.h>
#include <parser/compile.h>

extern CompilerContext context;

AstNode *AstNode_create(int lineno, AstNodeType type, AstNode *c0, AstNode *c1, AstNode *c2)
{
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = type;
    node->children[0] = c0;
    node->children[1] = c1;
    node->children[2] = c2;

    node->lineno = lineno;

    return node;
}

AstNode *AstNode_create_list()
{
    // @todo nullptr checks
    AstNode *node = malloc(sizeof(AstNode));

    node->type = AST_LIST;
    node->list = List_create( (List_dtor_func_t) AstNode_destroy);

    return node;
}

AstNode *AstNode_list_add(AstNode *list_node, AstNode *node)
{
    List *l = (List *) list_node->list;
    List_append(l, node);

    return list_node;
}

AstNode *AstNode_create_value(SValue value)
{
    AstNode *node = malloc(sizeof(AstNode));

    node->type  = AST_SVALUE;
    node->value = value;

    return node;
}

AstNode *AstNode_create_id(const char *name)
{
    // @todo nullptr checks
    AstNode *node = malloc(sizeof(AstNode));
    node->type = AST_NAME;
    node->name = bfromcstr(name);

    return node;
}

AstNode *AstNode_create_decl(int lineno, AstNode *name, SValueType type)
{
    AstNode *node = malloc(sizeof(AstNode));
    node->type = AST_VAR_DECL;
    node->children[0] = name;

    SymbolTable_push(context.symbol_table, name->name, type);

    return node;
}

AstNode *AstNode_create_var(AstNode *name, const char *type, AstNode *init_val)
{
    // @todo nullptr checks
    AstNode *node = malloc(sizeof(AstNode));

    node->type    = AST_VAR_DECL;

    return node;
}

void AstNode_destroy(AstNode *ast)
{
    if (ast == NULL) {
        return;
    }

    switch (ast->type) {
        case AST_ROOT:
            AstNode_destroy(ast->children[0]);
            AstNode_destroy(ast->children[1]);
            break;
        case AST_LIST:
            List_clean(ast->list);
            List_destroy(ast->list);
            break;
        case AST_SVALUE:
            break;
        case AST_TYPENAME:
            break;
        case AST_NAME:
            bdestroy(ast->name);
            break;
        case AST_PRINT_STMT:
            AstNode_destroy(ast->children[0]);
            break;
        case AST_BINARY_ASSIGN:
        case AST_BINARY_ADD:
        case AST_BINARY_SUB:
        case AST_BINARY_MUL:
        case AST_BINARY_DIV:
        case AST_BINARY_MOD:
        case AST_BINARY_IS_EQ:
        case AST_BINARY_IS_LT:
        case AST_BINARY_IS_GT:
        case AST_BINARY_IS_LTEQ:
        case AST_BINARY_IS_GREQ:
        case AST_BINARY_IS_NOTEQ:
        case AST_BINARY_AND:
        case AST_BINARY_OR:
            AstNode_destroy(ast->children[0]);
            AstNode_destroy(ast->children[1]);
            break;
        case AST_VAR_DECL:
            break;
        case AST_CONST_DECL:
            break;
        default:
            break;
    }

    free(ast);
}
