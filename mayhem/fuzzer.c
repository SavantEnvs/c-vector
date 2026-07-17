/* libFuzzer harness for c-vector (header-only dynamic array).
 *
 * Ported from the original mayhem/fuzzer.c on archive/original-master, repaired:
 *  - guards against empty input (the old harness read data[0] unconditionally),
 *  - no reads of uninitialized stack values (values now come from the input),
 *  - no per-exec printf flooding,
 *  - the checksum accumulator is unsigned (defined wraparound), so summing
 *    the harness's own pushed values can no longer overflow a signed int and
 *    abort the harness itself (QA #1077),
 * while preserving the same cvector API surface the old harness drove:
 * push_back / pop_back / iterate / erase / insert / copy / reserve /
 * for_each destructor free on a char* vector.
 */
#define CVECTOR_LOGARITHMIC_GROWTH
#include "cvector.h"
#include "cvector_utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef cvector_free_each_and_free
#define cvector_free_each_and_free(vec, func) \
    do {                                      \
        cvector_for_each((vec), (func));      \
        cvector_free((vec));                  \
    } while (0)
#endif

/* Convert one byte to its 8-char binary string (heap-allocated). */
static char *convert(uint8_t a) {
    char *buf = malloc(9);
    int i;
    if (!buf) {
        return NULL;
    }
    buf[8] = 0;
    for (i = 0; i <= 7; i++) {
        buf[7 - i] = ((a >> i) & 0x01) + '0';
    }
    return buf;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    cvector_vector_type(int) v           = NULL;
    cvector_vector_type(int) a           = NULL;
    cvector_vector_type(int) b           = NULL;
    cvector_vector_type(int) c           = NULL;
    cvector_vector_type(char *) str_vect = NULL;
    size_t i;
    volatile unsigned int sink = 0;

    if (size < 1) {
        return 0;
    }

    /* push/pop driven by the input bytes */
    cvector_push_back(v, (int)size);
    cvector_push_back(v, (int)data[0]);
    cvector_pop_back(v);

    for (i = 0; i < size; i++) {
        cvector_push_back(v, (int)data[i] << (i % 24));
    }
    sink += (unsigned int)cvector_capacity(v);
    sink += (unsigned int)cvector_size(v);

    /* iterator style */
    if (v) {
        int *it;
        for (it = cvector_begin(v); it != cvector_end(v); ++it) {
            sink += *it;
        }
    }
    /* index style */
    if (v) {
        for (i = 0; i < cvector_size(v); ++i) {
            sink += v[i];
        }
    }
    cvector_free(v);

    /* erase / insert */
    cvector_push_back(a, (int)data[0]);
    cvector_push_back(a, 1);
    cvector_push_back(a, 5);
    cvector_push_back(a, 4);
    cvector_pop_back(a);
    cvector_push_back(a, 5);
    cvector_erase(a, 1);
    cvector_erase(a, 0);
    cvector_insert(a, 0, 1);

    /* copy */
    if (a) {
        cvector_copy(a, b);
        for (i = 0; i < cvector_size(b); ++i) {
            sink += a[i];
        }
    }
    cvector_free(a);

    if (b) {
        cvector_insert(b, 0, 0);
        cvector_insert(b, 2, 4);
        cvector_insert(b, 2, 2);
        cvector_insert(b, 3, 3);
        for (i = 0; i < cvector_size(b); ++i) {
            sink += b[i];
        }
    }
    cvector_free(b);

    /* reserve semantics (growing and non-shrinking) */
    cvector_reserve(c, 100);
    cvector_push_back(c, 10);
    cvector_reserve(c, 10);
    for (i = 0; i < size && i < 200; ++i) {
        cvector_push_back(c, (int)data[i]);
    }
    cvector_shrink_to_fit(c);
    sink += (unsigned int)cvector_capacity(c);
    cvector_free(c);

    /* char* vector with per-element free */
    cvector_push_back(str_vect, convert(data[0]));
    cvector_push_back(str_vect, strdup("Hello world"));
    cvector_push_back(str_vect, strdup("Good  bye world"));
    cvector_push_back(str_vect, strndup((const char *)data, size));
    if (str_vect) {
        for (i = 0; i < cvector_size(str_vect); ++i) {
            if (str_vect[i]) {
                sink += (unsigned int)strlen(str_vect[i]);
            }
        }
    }
    cvector_free_each_and_free(str_vect, free);

    (void)sink;
    return 0;
}
