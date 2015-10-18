#ifndef _SALLANG_COMPILE_SNODE_H
#define _SALLANG_COMPILE_SNODE_H

#include "common/vector.h"
#include "parser/symbol.h"
#include "parser/types.h"

typedef enum _SNodeKind {
    SNODE_VAR,
    SNODE_TMP,
    SNODE_CONST,
    SNODE_OP,
    SNODE_LOCATION,
} SNodeKind;

typedef enum _SNodeOp {

    SNODE_ASSIGN,

    SNODE_IPRINT,

    SNODE_NOT,
    SNODE_AND,
    SNODE_OR,
    SNODE_LT,
    SNODE_GT,
    SNODE_EQ,
    SNODE_NOTEQ,

    SNODE_JMP_FALSE,
    SNODE_GOTO,

    /* Binary operations for integers */
    SNODE_IADD,
    SNODE_ISUB,
    SNODE_IMUL,
    SNODE_IDIV,
    SNODE_IMOD,

    /* Binary operations for doubles */
    SNODE_DADD,
    SNODE_DSUB,
    SNODE_DMUL,
    SNODE_DDIV,


} SNodeOp;

typedef struct _SNode {

    uint32_t id;
    uint32_t flags;

    SNodeKind kind;

    union {
        SValue sval;  /* SNODE_CONST */
        Symbol *sym;  /* SNODE_VAR, SNODE_TMP */
        struct {      /* SNODE_OP */
            SNodeOp op;

            union {
                struct _SNode *arg;     /* Unary operations  */
                struct {                /* Binary operations */
                    struct _SNode *lhs;
                    struct _SNode *rhs;
                };
            };

            struct _SNode *result;
        };

        int location; /* SNODE_LOCATION */
    };

} SNode;

typedef Vector SNodeArray;

SNode *SNode_init(SNodeKind kind);
SNode *SNode_create(SNodeArray *sna, SNodeKind kind);

SNode *SNode_init_location();

void SNodeArray_append(SNodeArray *sna, SNode *node);

#define SNodeArray_create() Vector_create()
#define SNodeArray_destroy(S)       \
    Vector_clean(S, free);          \
    Vector_destroy(S)

void SNodeArray_append(SNodeArray *sna, SNode *node);

int SNode_next_location();


/**
 * Returns a node value's typename.
 *
 * If the node is a constant, its type is determined by the stored sval.
 * If the node is a variable or temporary, its type will be queried from the symbol table.
 * If the node is a operation node, its type will be based upon the operation.
 *
 * @param  node
 *
 * @return
 */
SValueType SNode_get_type(SNode *node);


#endif