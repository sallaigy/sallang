#include "parser/compile.h"
#include "parser/snode.h"
#include "common/vector.h"
#include "parser/types.h"
#include "parser/error.h"

#include <stdio.h>
#include <stdlib.h>

extern CompilerContext context;

static void compile_declaration_list(AstNode *ast, SNodeArray *sna);
static void compile_stmt_list(AstNode *ast, SNodeArray *sna);
static void compile_stmt_list(AstNode *ast, SNodeArray *sna);
static void compile_stmt(AstNode *ast, SNodeArray *sna);

static SNode *compile_expr(AstNode *ast, SNodeArray *sna);

bool compile(AstNode *ast, FILE *outfp)
{
    if (ast == NULL || ast->type != AST_ROOT) {
        trigger_error(0, "Invalid AstNode found. Stop.\n");
    }

    SNodeArray *sna = SNodeArray_create();

    compile_declaration_list(ast->children[0], sna);
    compile_stmt_list(ast->children[1], sna);

    for (size_t i = 0; i < sna->size; i++) {
        SNode *node = (SNode *) Vector_get(sna, i);
        if (node->kind == SNODE_VAR) {
            fprintf(outfp, "load %d\n", node->sym->location);
        } else if (node->kind == SNODE_TMP) {

        } else if (node->kind == SNODE_LOCATION) {
            fprintf(outfp, "L%d:\n", node->location);
        } else if (node->kind == SNODE_CONST) {
            fprintf(outfp, "push %d\n", node->sval.ival);
        } else {
            switch (node->op) {
                case SNODE_ASSIGN:  fprintf(outfp, "store %d\n", node->result->sym->location); break;
                case SNODE_IADD:    fprintf(outfp, "add\n");    break;
                case SNODE_ISUB:    fprintf(outfp, "sub\n");    break;
                case SNODE_IMUL:    fprintf(outfp, "mul\n");    break;
                case SNODE_IDIV:    fprintf(outfp, "div\n");    break;
                case SNODE_IMOD:    fprintf(outfp, "mod\n");    break;
                case SNODE_IPRINT:  fprintf(outfp, "print\n");  break;
                case SNODE_AND:     fprintf(outfp, "and\n");    break;
                case SNODE_OR:      fprintf(outfp, "or\n");     break;
                case SNODE_LT:      fprintf(outfp, "cmplt\n");  break;
                case SNODE_GT:      fprintf(outfp, "cmpgt\n");  break;
                case SNODE_EQ:      fprintf(outfp, "cmpeq\n");  break;
                case SNODE_NOT:     fprintf(outfp, "not\n");    break;
                case SNODE_NOTEQ:   fprintf(outfp, "cmpeq\n"); fprintf(outfp, "not\n"); break;
                case SNODE_JMP_FALSE:
                    fprintf(outfp, "jmpz L%d\n", node->arg->location);
                    break;
                case SNODE_GOTO:
                    fprintf(outfp, "goto L%d\n", node->arg->location);
                    break;
                default:
                    break;
            }
        }
    }

    fprintf(outfp, "halt\n");

    SNodeArray_destroy(sna);

    return true;
}

static void compile_declaration_list(AstNode *ast, SNodeArray *sna)
{
}

static void compile_stmt_list(AstNode *ast, SNodeArray *sna)
{
    List *list  = ast->list;

    LIST_FOREACH(list, first, next, current) {
        AstNode *node = (AstNode*) current->data;

        compile_stmt(node, sna);
    }

}

static void compile_stmt(AstNode *ast, SNodeArray *sna)
{
    if (ast->type == AST_PRINT_STMT) {
        SNode *expr_node = compile_expr(ast->children[0], sna);
        SNode *print_node = SNode_create(sna, SNODE_OP);
        print_node->op  = SNODE_IPRINT;
        print_node->arg = expr_node;

    } else if (ast->type == AST_LIST) {
        compile_stmt_list(ast, sna);
    } else if (ast->type == AST_IF) {
        SNode *cond = compile_expr(ast->children[0], sna);
        SValueType cond_type = SNode_get_type(cond);

        if (!SValueType_compatible(cond_type, SVALUE_BOOLEAN)) {
            trigger_error(
                ast->lineno,
                "Invalid if condition: expected '%s', got '%s'",
                SValueType_to_str(SVALUE_BOOLEAN),
                SValueType_to_str(cond_type)
            );
        }

        SNode *jmp_node = SNode_create(sna, SNODE_OP);
        jmp_node->op    = SNODE_JMP_FALSE;

        SNode *label    = SNode_init_location();
        jmp_node->arg   = label;

        compile_stmt(ast->children[1], sna);

        if (ast->children[2] != NULL) {
            SNode *end_label = SNode_init_location();

            SNode *goto_node = SNode_create(sna, SNODE_OP);
            goto_node->op  = SNODE_GOTO;
            goto_node->arg = end_label;

            SNodeArray_append(sna, label);
            compile_stmt(ast->children[2], sna);
            SNodeArray_append(sna, end_label);
        } else {
            SNodeArray_append(sna, label);
        }

    } else if (ast->type == AST_WHILE) {
        SNode *begin = SNode_init_location();
        SNode *end   = SNode_init_location();

        SNodeArray_append(sna, begin);
        SNode *cond = compile_expr(ast->children[0], sna);
        SValueType cond_type = SNode_get_type(cond);

        if (!SValueType_compatible(cond_type, SVALUE_BOOLEAN)) {
            trigger_error(
                ast->lineno,
                "Invalid if condition: expected '%s', got '%s'",
                SValueType_to_str(SVALUE_BOOLEAN),
                SValueType_to_str(cond_type)
            );
        }

        SNode *jmp_end = SNode_create(sna, SNODE_OP);
        jmp_end->op    = SNODE_JMP_FALSE;
        jmp_end->arg   = end;

        compile_stmt(ast->children[1], sna);

        SNode *goto_begin = SNode_create(sna, SNODE_OP);
        goto_begin->op    = SNODE_GOTO;
        goto_begin->arg   = begin;

        SNodeArray_append(sna, end);
    } else {
        compile_expr(ast, sna);
    }
}

static SNode *compile_expr(AstNode *ast, SNodeArray *sna)
{
    if (ast->type == AST_SVALUE) {
        SNode *node = SNode_create(sna, SNODE_CONST);
        node->sval = ast->value;

        return node;
    } else if (ast->type == AST_NAME) {
        Symbol *sym = SymbolTable_find(context.symbol_table, ast->name);

        if (NULL == sym) {
            trigger_error(
                ast->lineno,
                "Use of undeclared variable '%s'.",
                bdata(ast->name)
            );
        }

        if (sym->location == SYMBOL_LOCATION_INVALID) {
            trigger_error(
                ast->lineno,
                "Use of unassigned variable '%s'.",
                bdata(ast->name)
            );
        }

        SNode *node = SNode_create(sna, SNODE_VAR);
        node->sym = sym;

        return node;
    } else if (ast->type == AST_BINARY_ASSIGN) {
        if (ast->children[0]->type != AST_NAME) {
            trigger_error(
                ast->lineno,
                "Expected variable as left hand side of assignment"
            );
        }

        Symbol *sym = SymbolTable_find(context.symbol_table, ast->children[0]->name);

        if (NULL == sym) {
            trigger_error(
                ast->lineno,
                "Use of undeclared variable '%s'.",
                bdata(ast->children[0]->name)
            );
        }

        if (sym->location == SYMBOL_LOCATION_INVALID) {
            sym->location = Symbol_get_location();
        }

        SNode *res = SNode_init(SNODE_VAR);
        res->sym   = sym;

        SNode *rhs = compile_expr(ast->children[1], sna);

        SValueType ltype = SNode_get_type(res);
        SValueType rtype = SNode_get_type(rhs);

        if (!SValueType_compatible(ltype, rtype)) {
            trigger_error(
                ast->lineno,
                "Type mismatch: Expected '%s', found '%s'.",
                SValueType_to_str(ltype),
                SValueType_to_str(rtype)
            );
        }

        SNode *node = SNode_create(sna, SNODE_OP);
        node->op  = SNODE_ASSIGN;
        node->arg = rhs;
        node->result = res;

        return node;
    }

    SNode *lhs = compile_expr(ast->children[0], sna);
    SValueType ltype = SNode_get_type(lhs);

    switch (ast->type) {
        case AST_UNARY_NOT:
            if (!SValueType_compatible(ltype, SVALUE_BOOLEAN)) {
                trigger_error(
                    ast->lineno,
                    "Type mismatch: Expected '%s', found '%s'.",
                    SValueType_to_str(SVALUE_BOOLEAN),
                    SValueType_to_str(ltype)
                );
            }

            SNode *node = SNode_create(sna, SNODE_OP);
            node->op = SNODE_NOT;
            node->arg = lhs;

            return node;
        default:
            break;
    }

    SNode *rhs = compile_expr(ast->children[1], sna);
    SValueType rtype = SNode_get_type(rhs);

    SNode *node = SNode_create(sna, SNODE_OP);

    switch (ast->type) {
        case AST_BINARY_ADD:
            if (
                SValueType_compatible(ltype, SVALUE_INTEGER)
                    &&
                SValueType_compatible(rtype, SVALUE_INTEGER)
            ) {
                node->op = SNODE_IADD;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator +",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_SUB:
            if (
                SValueType_compatible(ltype, SVALUE_INTEGER)
                    &&
                SValueType_compatible(rtype, SVALUE_INTEGER)
            ) {
                node->op = SNODE_ISUB;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator -",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_MUL:
            if (
                SValueType_compatible(ltype, SVALUE_INTEGER)
                    &&
                SValueType_compatible(rtype, SVALUE_INTEGER)
            ) {
                node->op = SNODE_IMUL;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator *",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_MOD:
            if (
                SValueType_compatible(ltype, SVALUE_INTEGER)
                    &&
                SValueType_compatible(rtype, SVALUE_INTEGER)
            ) {
                node->op = SNODE_IMOD;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator %%",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }

            if (rhs->kind == SNODE_CONST && rhs->sval.ival == 0) {
                trigger_warning(
                    ast->lineno,
                    "Division by zero"
                );
            }
            break;
        case AST_BINARY_DIV:
            if (
                SValueType_compatible(ltype, SVALUE_INTEGER)
                    &&
                SValueType_compatible(rtype, SVALUE_INTEGER)
            ) {
                node->op = SNODE_IDIV;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator /",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }

            if (rhs->kind == SNODE_CONST && rhs->sval.ival == 0) {
                trigger_warning(
                    ast->lineno,
                    "Division by zero"
                );
            }
            break;
        case AST_BINARY_IS_EQ:
            if (
                (SValueType_compatible(ltype, SVALUE_INTEGER) && SValueType_compatible(rtype, SVALUE_INTEGER))
                    ||
                (SValueType_compatible(ltype, SVALUE_BOOLEAN) && SValueType_compatible(rtype, SVALUE_BOOLEAN))
            ) {
                node->op = SNODE_EQ;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator ==",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_IS_NOTEQ:
            if (
                (SValueType_compatible(ltype, SVALUE_INTEGER) && SValueType_compatible(rtype, SVALUE_INTEGER))
                    ||
                (SValueType_compatible(ltype, SVALUE_BOOLEAN) && SValueType_compatible(rtype, SVALUE_BOOLEAN))
            ) {
                node->op = SNODE_NOTEQ;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator !=",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_IS_GT:
            if (
                SValueType_compatible(ltype, SVALUE_INTEGER)
                    &&
                SValueType_compatible(rtype, SVALUE_INTEGER)
            ) {
                node->op = SNODE_GT;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator >",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_IS_LT:
            if (
                SValueType_compatible(ltype, SVALUE_INTEGER)
                    &&
                SValueType_compatible(rtype, SVALUE_INTEGER)
            ) {
                node->op = SNODE_LT;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator <",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_AND:
            if (
                SValueType_compatible(ltype, SVALUE_BOOLEAN)
                    &&
                SValueType_compatible(rtype, SVALUE_BOOLEAN)
            ) {
                node->op = SNODE_AND;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator &&",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        case AST_BINARY_OR:
            if (
                SValueType_compatible(ltype, SVALUE_BOOLEAN)
                    &&
                SValueType_compatible(rtype, SVALUE_BOOLEAN)
            ) {
                node->op = SNODE_OR;
            } else {
                trigger_error(
                    ast->lineno,
                    "Cannot apply type pair '%s' and '%s' for binary operator ||",
                    SValueType_to_str(ltype),
                    SValueType_to_str(rtype)
                );
            }
            break;
        default:
            trigger_error(ast->lineno, "Compiler error.\n");
            break;
    }

    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}
