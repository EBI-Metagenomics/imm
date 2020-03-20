#ifndef IMM_BUG_H
#define IMM_BUG_H

#include <stdio.h>
#include <stdlib.h>

#define IMM_BUG(cond)                                                                             \
    do {                                                                                      \
        if (!(cond))                                                                          \
            break;                                                                            \
        fprintf(stderr, "BUG: %s: %s: %d: %s\n", __FILE__, __func__, __LINE__, #cond);        \
        fflush(stderr);                                                                       \
        exit(1);                                                                              \
    } while (0)

#endif