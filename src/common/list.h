#ifndef _SALLANG_LIST_H
#define _SALLANG_LIST_H

#include "slg.h"

typedef struct _ListNode {

    struct _ListNode *prev;
    struct _ListNode *next;

    void *data;

} ListNode;

typedef void (*List_dtor_func_t)(void *);

typedef struct _List {

    ListNode *first;
    ListNode *last;

    size_t count;

    List_dtor_func_t dtor;

} List;

List *List_create(List_dtor_func_t dtor);

void List_append(List *list, void *data);
void List_prepend(List *list, void *data);

void List_clean(List *list);
void List_destroy(List *list);

#define LIST_FOREACH(L, S, M, V) ListNode *_node = NULL; \
    ListNode *V = NULL; \
    for (V = _node = L->S; _node != NULL; V = _node = _node->M)

#endif