#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static int failures = 0;

#define TEST_ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

#define TEST_SUMMARY() do { \
    if (failures) { \
        printf("%d Test(s) fehlgeschlagen\n", failures); \
    } else { \
        printf("Alle Tests bestanden!\n"); \
    } \
} while (0)

#endif
