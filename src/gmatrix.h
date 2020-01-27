#ifndef IMM_GMATRIX_H
#define IMM_GMATRIX_H

#include "free.h"
#include <stdlib.h>

#define MAKE_GMATRIX_STRUCT(S, T)                                                             \
    struct gmatrix_##S                                                                        \
    {                                                                                         \
        T*       data;                                                                        \
        unsigned nrows;                                                                       \
        unsigned ncols;                                                                       \
    };

#define MAKE_GMATRIX_CREATE(S, T)                                                             \
    static inline struct gmatrix_##S* gmatrix_##S##_create(unsigned nrows, unsigned ncols)    \
    {                                                                                         \
        struct gmatrix_##S* matrix = malloc(sizeof(struct gmatrix_##S));                      \
        matrix->data = malloc(sizeof(T) * (nrows * ncols));                                   \
        matrix->nrows = nrows;                                                                \
        matrix->ncols = ncols;                                                                \
        return matrix;                                                                        \
    }

#define MAKE_GMATRIX_GET(S, T)                                                                \
    static inline T* gmatrix_##S##_get(struct gmatrix_##S const* matrix, unsigned r,          \
                                       unsigned c)                                            \
    {                                                                                         \
        return matrix->data + (r * matrix->ncols + c);                                        \
    }

#define MAKE_GMATRIX_GET_C(S, T)                                                              \
    static inline T const* gmatrix_##S##_get_c(struct gmatrix_##S const* matrix, unsigned r,  \
                                               unsigned c)                                    \
    {                                                                                         \
        return matrix->data + (r * matrix->ncols + c);                                        \
    }

#define MAKE_GMATRIX_SET(S, T)                                                                \
    static inline void gmatrix_##S##_set(struct gmatrix_##S* matrix, unsigned r, unsigned c,  \
                                         T v)                                                 \
    {                                                                                         \
        matrix->data[r * matrix->ncols + c] = v;                                              \
    }

#define MAKE_GMATRIX_SET_ALL(S, T)                                                            \
    static inline void gmatrix_##S##_set_all(struct gmatrix_##S* matrix, T v)                 \
    {                                                                                         \
        for (unsigned i = 0; i < matrix->nrows * matrix->ncols; ++i)                          \
            matrix->data[i] = v;                                                              \
    }

#define MAKE_GMATRIX_DESTROY(S, T)                                                            \
    static inline void gmatrix_##S##_destroy(struct gmatrix_##S const* matrix)                \
    {                                                                                         \
        if (!matrix)                                                                          \
            return;                                                                           \
                                                                                              \
        free_c(matrix->data);                                                                 \
        free_c(matrix);                                                                       \
    }

#endif
