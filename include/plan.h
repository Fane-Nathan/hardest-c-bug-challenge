#ifndef PLAN_H
#define PLAN_H

#include <stddef.h>
#include <stdint.h>
#include "store.h"

typedef struct {
    Entry *cached_entry;   /* Intentionally looks like a harmless cache. */
    uint64_t amount;
} PlanOp;

typedef struct {
    PlanOp *ops;
    size_t len;
    size_t cap;
} Plan;

void plan_init(Plan *p);
void plan_destroy(Plan *p);
void plan_add(Plan *p, Store *s, size_t entry_index, uint64_t amount);
void plan_execute(Plan *p);

#endif
