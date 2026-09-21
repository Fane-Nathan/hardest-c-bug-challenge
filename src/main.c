#include "loader.h"
#include "noise.h"
#include "plan.h"
#include "store.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t parse_size(const char *s, size_t fallback) {
    char *end = NULL;
    unsigned long long v = strtoull(s, &end, 10);
    if (!s[0] || (end && *end != '\0')) {
        return fallback;
    }
    return (size_t)v;
}

int main(int argc, char **argv) {
    size_t generated = 10000;
    size_t noise_rounds = 50;
    size_t noise_width = 64;

    if (argc > 1) generated = parse_size(argv[1], generated);
    if (argc > 2) noise_rounds = parse_size(argv[2], noise_rounds);
    if (argc > 3) noise_width = parse_size(argv[3], noise_width);

    Store store;
    Plan plan;
    store_init(&store);
    plan_init(&plan);

    size_t alpha = store_intern(&store, "alpha");
    size_t beta  = store_intern(&store, "beta");
    size_t gamma = store_intern(&store, "gamma");

    plan_add(&plan, &store, alpha, 11);
    plan_add(&plan, &store, beta, 22);
    plan_add(&plan, &store, gamma, 33);

    /* An unrelated load phase grows the catalog substantially. */
    loader_expand_catalog(&store, generated);

    /* Make allocator layout less predictable before the cached plan runs. */
    noise_churn(noise_rounds, noise_width);

    plan_execute(&plan);

    const Entry *a = store_get_const(&store, alpha);
    const Entry *b = store_get_const(&store, beta);
    const Entry *g = store_get_const(&store, gamma);

    printf("catalog size: %zu\n", store_size(&store));
    printf("alpha=%" PRIu64 " beta=%" PRIu64 " gamma=%" PRIu64 "\n",
           a ? a->hits : 0,
           b ? b->hits : 0,
           g ? g->hits : 0);

    if (!a || !b || !g || a->hits != 11 || b->hits != 22 || g->hits != 33) {
        fputs("INVARIANT FAILURE: plan results are corrupt\n", stderr);
        plan_destroy(&plan);
        store_destroy(&store);
        return 1;
    }

    plan_destroy(&plan);
    store_destroy(&store);
    return 0;
}
