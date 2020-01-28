#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

#include "free.h"
#include <stdlib.h>

#define MAKE_MATRIX_STRUCT(S, T)                                                              \
    struct matrix_##S                                                                         \
    {                                                                                         \
        T*       data;                                                                        \
        unsigned nrows;                                                                       \
        unsigned ncols;                                                                       \
    };

#define MAKE_MATRIX_CREATE(S, T)                                                              \
    static inline struct matrix_##S* matrix_##S##_create(unsigned nrows, unsigned ncols)      \
    {                                                                                         \
        struct matrix_##S* matrix = malloc(sizeof(struct matrix_##S));                        \
        matrix->data = malloc(sizeof(T) * (nrows * ncols));                                   \
        matrix->nrows = nrows;                                                                \
        matrix->ncols = ncols;                                                                \
        return matrix;                                                                        \
    }

#define MAKE_MATRIX_GET(S, T)                                                                 \
    static inline T* matrix_##S##_get(struct matrix_##S const* matrix, unsigned r,            \
                                      unsigned c)                                             \
    {                                                                                         \
        return matrix->data + (r * matrix->ncols + c);                                        \
    }

#define MAKE_MATRIX_GET_C(S, T)                                                               \
    static inline T const* matrix_##S##_get_c(struct matrix_##S const* matrix, unsigned r,    \
                                              unsigned c)                                     \
    {                                                                                         \
        return matrix->data + (r * matrix->ncols + c);                                        \
    }

#define MAKE_MATRIX_SET(S, T)                                                                 \
    static inline void matrix_##S##_set(struct matrix_##S* matrix, unsigned r, unsigned c,    \
                                        T v)                                                  \
    {                                                                                         \
        matrix->data[r * matrix->ncols + c] = v;                                              \
    }

#define MAKE_MATRIX_SET_ALL(S, T)                                                             \
    static inline void matrix_##S##_set_all(struct matrix_##S* matrix, T v)                   \
    {                                                                                         \
        for (unsigned i = 0; i < matrix->nrows * matrix->ncols; ++i)                          \
            matrix->data[i] = v;                                                              \
    }

#define MAKE_MATRIX_DESTROY(S, T)                                                             \
    static inline void matrix_##S##_destroy(struct matrix_##S const* matrix)                  \
    {                                                                                         \
        if (!matrix)                                                                          \
            return;                                                                           \
                                                                                              \
        free_c(matrix->data);                                                                 \
        free_c(matrix);                                                                       \
    }

#endif
