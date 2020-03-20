#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

#include "free.h"
#include <stdlib.h>

#define MAKE_MATRIX_STRUCT(NAME, T)                                                           \
    struct NAME                                                                               \
    {                                                                                         \
        T*       data;                                                                        \
        unsigned nrows;                                                                       \
        unsigned ncols;                                                                       \
    };

#define MAKE_MATRIX_CREATE(NAME, T)                                                           \
    static inline struct NAME* NAME##_create(unsigned const nrows, unsigned const ncols)      \
    {                                                                                         \
        struct NAME* matrix = malloc(sizeof(struct NAME*));                                   \
        matrix->data = malloc(sizeof(T) * (nrows * ncols));                                   \
        matrix->nrows = nrows;                                                                \
        matrix->ncols = ncols;                                                                \
        return matrix;                                                                        \
    }

#define MAKE_MATRIX_GET(NAME, T)                                                              \
    static inline T* NAME##_get(struct NAME const* matrix, unsigned r, unsigned c)            \
    {                                                                                         \
        return matrix->data + (r * matrix->ncols + c);                                        \
    }

#define MAKE_MATRIX_GET_C(NAME, T)                                                            \
    static inline T const* NAME##_get_c(struct NAME const* matrix, unsigned r, unsigned c)    \
    {                                                                                         \
        return matrix->data + (r * matrix->ncols + c);                                        \
    }

#define MAKE_MATRIX_SET(NAME, T)                                                              \
    static inline void NAME##_set(struct NAME* matrix, unsigned r, unsigned c, T v)           \
    {                                                                                         \
        matrix->data[r * matrix->ncols + c] = v;                                              \
    }

#define MAKE_MATRIX_SET_ALL(NAME, T)                                                          \
    static inline void NAME##_set_all(struct NAME* matrix, T v)                               \
    {                                                                                         \
        for (unsigned i = 0; i < matrix->nrows * matrix->ncols; ++i)                          \
            matrix->data[i] = v;                                                              \
    }

#define MAKE_MATRIX_DESTROY(NAME)                                                             \
    static inline void NAME##_destroy(struct NAME const* matrix)                              \
    {                                                                                         \
        if (!matrix)                                                                          \
            return;                                                                           \
                                                                                              \
        imm_free(matrix->data);                                                                 \
        imm_free(matrix);                                                                       \
    }

#define MAKE_MATRIX_NROWS(NAME)                                                               \
    static inline unsigned NAME##_nrows(struct NAME const* matrix) { return matrix->nrows; }

#define MAKE_MATRIX_NCOLS(NAME)                                                               \
    static inline unsigned NAME##_ncols(struct NAME const* matrix) { return matrix->ncols; }

#define MAKE_MATRIX_RESIZE(NAME, T)                                                           \
    static inline void NAME##_resize(struct NAME* matrix, unsigned const nrows,               \
                                     unsigned const ncols)                                    \
    {                                                                                         \
        matrix->data = realloc(matrix->data, sizeof(T) * (nrows * ncols));                    \
        matrix->nrows = nrows;                                                                \
        matrix->ncols = ncols;                                                                \
    }

#endif
