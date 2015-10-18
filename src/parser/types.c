#include "parser/types.h"

char *SValueType_to_str(SValueType type)
{
    switch (type) {
        case SVALUE_INTEGER: return "int"; break;
        case SVALUE_DOUBLE:  return "double"; break;
        case SVALUE_BOOLEAN: return "bool"; break;
        default: return "unknown"; break;
    }
}

// @todo
bool SValueType_compatible(SValueType l, SValueType r) {
    return l == r;
}
