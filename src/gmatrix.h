#ifndef IMM_GMATRIX_H
#define IMM_GMATRIX_H

#include "hide.h"
#include <stdlib.h>

#define MAKE_GMATRIX_STRUCT(S, T)                                                            \
    struct gmatrix_##S                                                                       \
    {                                                                                        \
        T*  data;                                                                            \
        int nrows;                                                                           \
        int ncols;                                                                           \
    };

#define MAKE_GMATRIX_CREATE(S, T)                                                            \
    static inline struct gmatrix_##S* gmatrix_##S##_create(int nrows, int ncols)             \
    {                                                                                        \
        struct gmatrix_##S* matrix = malloc(sizeof(struct gmatrix_##S));                     \
        matrix->data = malloc(sizeof(T) * ((size_t)(nrows * ncols)));                        \
        matrix->nrows = nrows;                                                               \
        matrix->ncols = ncols;                                                               \
        return matrix;                                                                       \
    }

#define MAKE_GMATRIX_GET(S, T)                                                               \
    static inline T* gmatrix_##S##_get(struct gmatrix_##S const* matrix, int r, int c)       \
    {                                                                                        \
        return matrix->data + (r * matrix->ncols + c);                                       \
    }

#define MAKE_GMATRIX_SET(S, T)                                                               \
    static inline void gmatrix_##S##_set(struct gmatrix_##S* matrix, int r, int c, T v)      \
    {                                                                                        \
        matrix->data[r * matrix->ncols + c] = v;                                             \
    }

#define MAKE_GMATRIX_SET_ALL(S, T)                                                           \
    static inline void gmatrix_##S##_set_all(struct gmatrix_##S* matrix, T v)                \
    {                                                                                        \
        for (int i = 0; i < matrix->nrows * matrix->ncols; ++i)                              \
            matrix->data[i] = v;                                                             \
    }

#define MAKE_GMATRIX_DESTROY(S, T)                                                           \
    static inline void gmatrix_##S##_destroy(struct gmatrix_##S* matrix)                     \
    {                                                                                        \
        if (!matrix)                                                                         \
            return;                                                                          \
                                                                                             \
        free(matrix->data);                                                                  \
        matrix->data = NULL;                                                                 \
        matrix->nrows = 0;                                                                   \
        matrix->ncols = 0;                                                                   \
        free(matrix);                                                                        \
    }

#endif
