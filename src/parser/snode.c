#include <stdlib.h>

#include "parser/snode.h"

SNode *SNode_create(SNodeArray *sna, SNodeKind kind)
{
    SNode *node = malloc(sizeof(SNode));

    node->kind  = kind;
    node->id    = Vector_count(sna);

    Vector_insert(sna, node);

    return node;
}

SNode *SNode_init(SNodeKind kind)
{
    SNode *node = malloc(sizeof(SNode));
    node->kind  = kind;

    return node;
}

SNode *SNode_init_location()
{
    SNode *node = malloc(sizeof(SNode));
    node->kind  = SNODE_LOCATION;

    node->location = SNode_next_location();

    return node;
}

void SNodeArray_append(SNodeArray *sna, SNode *node)
{
    node->id = Vector_count(sna);

    Vector_insert(sna, node);
}

int SNode_next_location()
{
    static int loc = 0;

    return loc++;
}

SValueType SNode_get_type(SNode *node)
{
    if (node->kind == SNODE_CONST) {
        return node->sval.type;
    } else if (node->kind == SNODE_VAR || node->kind == SNODE_TMP) {
        return node->sym->type;
    }

    SValueType ltype, rtype;

    switch (node->op) {
        case SNODE_IADD:
        case SNODE_ISUB:
        case SNODE_IMUL:
        case SNODE_IDIV:
        case SNODE_IMOD:
            return SVALUE_INTEGER;
            break;
        case SNODE_NOT:
        case SNODE_AND:
        case SNODE_OR:
        case SNODE_LT:
        case SNODE_GT:
        case SNODE_EQ:
        case SNODE_NOTEQ:
            return SVALUE_BOOLEAN;
            break;
        case SNODE_ASSIGN:
            return SNode_get_type(node->arg);
        default:
            break;
    }

    return SVALUE_INVALID;
}


