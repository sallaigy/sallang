#include <stdlib.h>
#include "slg.h"
#include <common/list.h>

List *List_create(List_dtor_func_t dtor)
{
    List *list = malloc(sizeof(List));

    if (list == NULL) {
        return NULL;
    }

    list->count = 0;
    list->dtor  = dtor;

    list->first = NULL;
    list->last  = NULL;

    return list;
}

void List_append(List *list, void *data)
{
    ListNode *node = malloc(sizeof(ListNode));

    if (node == NULL) {
        return;
    }

    node->data = data;
    node->prev = list->last;
    node->next = NULL;

    if (list->last != NULL) {
        list->last->next = node;
    } else {
        list->first = node;
    }

    list->last = node;
    list->count++;
}

void List_prepend(List *list, void *data)
{
    ListNode *node = malloc(sizeof(ListNode));

    if (node == NULL) {
        return;
    }

    node->data = data;
    node->next = list->first;
    node->prev = NULL;

    if (list->first != NULL) {
        list->first->prev = node;
    } else {
        list->last = node;
    }

    list->first = node;
    list->count++;
}

void List_clean(List *list)
{
    if (list == NULL || list->dtor == NULL) {
        return;
    }

    LIST_FOREACH(list, first, next, curr) {
        list->dtor(curr->data);
    }
}

void List_destroy(List *list)
{
    if (list == NULL) {
        return;
    }

    LIST_FOREACH(list, first, next, curr) {
        if (curr->prev) {
            free(curr->prev);
        }
    }

    free(list->last);
    free(list);
}
