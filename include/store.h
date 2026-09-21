#ifndef STORE_H
#define STORE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t id;
    uint64_t hits;
    char name[24];
} Entry;

typedef struct {
    Entry *items;
    size_t len;
    size_t cap;
} Store;

void store_init(Store *s);
void store_destroy(Store *s);
size_t store_intern(Store *s, const char *name);
Entry *store_get(Store *s, size_t index);
const Entry *store_get_const(const Store *s, size_t index);
size_t store_size(const Store *s);

#endif
