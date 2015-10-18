#include <stdlib.h>

#include "parser/symbol.h"

SymbolTable *SymbolTable_create(SymbolTable *parent)
{
    SymbolTable *st = (SymbolTable*) malloc(sizeof(SymbolTable));

    st->symbols       = HashTable_create(100);
    st->symbols->dtor = (HashTable_dtor_func_t) Symbol_destroy;

    st->parent   = parent;
    st->active   = true;

    return st;
}

void SymbolTable_destroy(SymbolTable *st)
{
    SymbolTable *tmp;

    for (SymbolTable *iter = st; iter != NULL; iter = tmp) {
        tmp = iter->parent;
        HashTable_destroy(iter->symbols);
        free(iter);
    }
}

bool SymbolTable_push(SymbolTable *st, bstring name, SValueType type)
{
    Symbol *sym = malloc(sizeof(Symbol));
    sym->name = bstrcpy(name);
    sym->location = SYMBOL_LOCATION_INVALID;
    sym->type = type;

    if (!HashTable_set(st->symbols, bdata(name), sym)) {
        return false;
    }

    return true;
}

Symbol *SymbolTable_find(SymbolTable *st, bstring name)
{
    Symbol *sym = NULL;
    sym = (Symbol*) HashTable_get(st->symbols, bdata(name));

    while (sym == NULL && st->parent != NULL) {
        st = st->parent;
        sym = (Symbol*) HashTable_get(st->symbols, bdata(name));
    }

    return sym;
}

int Symbol_get_location()
{
    static int regc = 0;

    return regc++;
}

void Symbol_destroy(Symbol *sym)
{
    bdestroy(sym->name);
    free(sym);
}
