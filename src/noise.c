#include "noise.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void noise_churn(size_t rounds, size_t width) {
    if (width == 0) {
        return;
    }

    void **blocks = calloc(width, sizeof(*blocks));
    if (!blocks) {
        return;
    }

    for (size_t r = 0; r < rounds; ++r) {
        for (size_t i = 0; i < width; ++i) {
            size_t n = 32 + ((r * 131u + i * 17u) % 2048u);
            blocks[i] = malloc(n);
            if (blocks[i]) {
                memset(blocks[i], (int)((r + i) & 0xffu), n);
            }
        }

        for (size_t i = 0; i < width; ++i) {
            free(blocks[i]);
            blocks[i] = NULL;
        }
    }

    free(blocks);
}
