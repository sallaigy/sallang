#ifndef _SALLANG_PARSER_SYMBOL_H
#define _SALLANG_PARSER_SYMBOL_H

#include "common/bstrlib.h"
#include "common/hash.h"
#include "parser/types.h"

#define SYMBOL_LOCATION_INVALID -1

typedef struct _Symbol {
    SValueType type;
    bstring name;

    uint32_t attributes;
    int location;
} Symbol;

typedef struct _SymbolTable {
    /* Symbols stored in this table */
    HashTable *symbols;

    /* Reference to the parent scope */
    struct _SymbolTable *parent;

    /* Whether this scope is active or not */
    bool active;

} SymbolTable;

SymbolTable *SymbolTable_create(SymbolTable *parent);

bool SymbolTable_push(SymbolTable *st, bstring name, SValueType type);
bool SymbolTable_push_symbol(SymbolTable *st, Symbol *sym);
Symbol *SymbolTable_find(SymbolTable *st, bstring name);

void SymbolTable_destroy(SymbolTable *st);

int Symbol_get_location();

void Symbol_destroy(Symbol *sym);

#endif