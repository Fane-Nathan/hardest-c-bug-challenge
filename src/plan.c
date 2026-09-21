#include "plan.h"

#include <stdio.h>
#include <stdlib.h>

static void plan_reserve(Plan *p, size_t need) {
    if (need <= p->cap) {
        return;
    }

    size_t new_cap = p->cap ? p->cap : 8;
    while (new_cap < need) {
        new_cap *= 2;
    }

    PlanOp *new_ops = realloc(p->ops, new_cap * sizeof(*new_ops));
    if (!new_ops) {
        fputs("out of memory\n", stderr);
        exit(2);
    }

    p->ops = new_ops;
    p->cap = new_cap;
}

void plan_init(Plan *p) {
    p->ops = NULL;
    p->len = 0;
    p->cap = 0;
}

void plan_destroy(Plan *p) {
    free(p->ops);
    p->ops = NULL;
    p->len = 0;
    p->cap = 0;
}

void plan_add(Plan *p, Store *s, size_t entry_index, uint64_t amount) {
    plan_reserve(p, p->len + 1);

    Entry *entry = store_get(s, entry_index);
    if (!entry) {
        fputs("invalid entry index\n", stderr);
        exit(3);
    }

    /*
     * PERFORMANCE NOTE:
     * Avoid repeated index lookup in the hot execution loop by caching the
     * direct Entry pointer here.
     */
    p->ops[p->len].cached_entry = entry;
    p->ops[p->len].amount = amount;
    ++p->len;
}

void plan_execute(Plan *p) {
    for (size_t i = 0; i < p->len; ++i) {
        PlanOp *op = &p->ops[i];
        op->cached_entry->hits += op->amount;
    }
}
