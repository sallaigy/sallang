#include <stdlib.h>
#include "slg.h"
#include "vector.h"

Vector *Vector_create()
{
    Vector *vec = malloc(sizeof(Vector));

    SLG_TRY(vec != NULL, error);

    vec->size = 0;
    vec->capacity = VECTOR_INITIAL_CAPACITY;
    vec->data = malloc(sizeof(void*) * vec->capacity);

    SLG_TRY(vec->data != NULL, error);

    return vec;

error:
    if (vec != NULL) {
        free(vec);
    }

    return NULL;
}

static inline void Vector_resize(Vector *vec, size_t newsize)
{
    void **tmp;
    size_t i;

    vec->capacity = newsize;
    tmp = malloc(sizeof(void*) * vec->capacity);

    for (i = 0; i < vec->size; i++) {
        tmp[i] = vec->data[i];
    }

    free(vec->data);
    vec->data = tmp;
}

void Vector_insert(Vector *vec, void *value)
{
    if (vec->size == vec->capacity) {
        Vector_resize(vec, vec->capacity * (int) VECTOR_EXPAND_RATE);
    }

    vec->data[vec->size] = value;
    vec->size++;
}

void Vector_clean(Vector *vec, Vector_dtor_func_t dtor)
{
    for (size_t i = 0; i < vec->size; i++) {
        dtor(vec->data[i]);
    }
}

void Vector_destroy(Vector *vec)
{
    free(vec->data);
    free(vec);
}
