#include "store.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void die_oom(void) {
    fputs("out of memory\n", stderr);
    exit(2);
}

static void store_reserve(Store *s, size_t need) {
    if (need <= s->cap) {
        return;
    }

    size_t new_cap = s->cap ? s->cap : 4;
    while (new_cap < need) {
        new_cap *= 2;
    }

    Entry *new_items = realloc(s->items, new_cap * sizeof(*new_items));
    if (!new_items) {
        die_oom();
    }

    s->items = new_items;
    s->cap = new_cap;
}

void store_init(Store *s) {
    s->items = NULL;
    s->len = 0;
    s->cap = 0;
}

void store_destroy(Store *s) {
    free(s->items);
    s->items = NULL;
    s->len = 0;
    s->cap = 0;
}

size_t store_intern(Store *s, const char *name) {
    for (size_t i = 0; i < s->len; ++i) {
        if (strcmp(s->items[i].name, name) == 0) {
            return i;
        }
    }

    store_reserve(s, s->len + 1);

    Entry *e = &s->items[s->len];
    e->id = (uint64_t)(s->len + 1);
    e->hits = 0;
    snprintf(e->name, sizeof(e->name), "%s", name);
    return s->len++;
}

Entry *store_get(Store *s, size_t index) {
    if (index >= s->len) {
        return NULL;
    }
    return &s->items[index];
}

const Entry *store_get_const(const Store *s, size_t index) {
    if (index >= s->len) {
        return NULL;
    }
    return &s->items[index];
}

size_t store_size(const Store *s) {
    return s->len;
}
