#include "loader.h"

#include <stdio.h>

void loader_expand_catalog(Store *s, size_t count) {
    char name[32];

    for (size_t i = 0; i < count; ++i) {
        snprintf(name, sizeof(name), "generated_%zu", i);
        (void)store_intern(s, name);
    }
}
