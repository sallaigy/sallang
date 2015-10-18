#ifndef _SALLANG_VECTOR_H
#define _SALLANG_VECTOR_H

#include "slg.h"

#define VECTOR_INITIAL_CAPACITY 10
#define VECTOR_EXPAND_RATE 2

typedef void (*Vector_dtor_func_t)(void *);

typedef struct _Vector {

    void **data;

    size_t size;
    size_t capacity;

} Vector;

Vector *Vector_create();

static inline bool Vector_set(Vector *vec, size_t idx, void *value)
{
    if (idx > vec->size) {
        return false;
    }

    vec->data[idx] = value;

    return true;
}

static inline void *Vector_get(Vector *vec, size_t idx)
{
    if (idx > vec->size) {
        return NULL;
    }

    return vec->data[idx];
}

#define Vector_last(V) ((V->data[v->size - 1]))
#define Vector_first(V) ((V->data[0]))
#define Vector_count(V) ((V->size))
#define Vector_capacity(V) ((V->capacity))

void Vector_insert(Vector *vec, void *value);
void Vector_remove_at(Vector *vec, size_t idx);

void Vector_merge(Vector *target, const Vector *vec);

void Vector_clean(Vector *vec, Vector_dtor_func_t dtor);
void Vector_destroy(Vector *vec);

#define VECTOR_FOR(V, I) \
    for (size_t I = 0; i < V->size; I++)

#endif