#include "dt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

List *list_append(List *list, void *data) {
    List *new = malloc(sizeof(List));
    new->data = data;
    new->next = NULL;
    if (list == NULL) {
        return new;
    }
    List *last = list;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = new;
    return list;
}
