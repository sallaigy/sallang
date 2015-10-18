#ifndef _SALLANG_PARSER_TYPES_H
#define _SALLANG_PARSER_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "common/bstrlib.h"

typedef enum _SValueType {
    SVALUE_AUTO,
    SVALUE_INTEGER,
    SVALUE_DOUBLE,
    SVALUE_BOOLEAN,
    SVALUE_STRING,

    SVALUE_INVALID
} SValueType;

typedef struct _SValue {
    SValueType type;
    union {
        uint32_t ival;        // SVALUE_INTEGER, SVALUE_BOOLEAN
        double   dval;        // SVALUE_DOUBLE
        bstring  sval;        // SVALUE_STRING
    };
} SValue;

char *SValueType_to_str(SValueType type);
bool SValueType_compatible(SValueType l, SValueType r);

#endif