#ifndef MAKE_MATRIX_H
#define MAKE_MATRIX_H

#include "free.h"
#include <stdint.h>
#include <stdlib.h>

#define MAKE_MATRIX_STRUCT(NAME, T)                                                                \
    struct NAME                                                                                    \
    {                                                                                              \
        T*            data;                                                                        \
        uint_fast16_t nrows;                                                                       \
        uint_fast16_t ncols;                                                                       \
    };

#define MAKE_MATRIX_CREATE(NAME, T)                                                                \
    static inline struct NAME* NAME##_create(uint_fast16_t const nrows, uint_fast16_t const ncols) \
    {                                                                                              \
        struct NAME* matrix = malloc(sizeof(struct NAME));                                         \
        matrix->data = malloc(sizeof(T) * (nrows * ncols));                                        \
        matrix->nrows = nrows;                                                                     \
        matrix->ncols = ncols;                                                                     \
        return matrix;                                                                             \
    }

#define MAKE_MATRIX_GET(NAME, T)                                                                   \
    static inline T NAME##_get(struct NAME const* matrix, uint_fast16_t r, uint_fast16_t c)        \
    {                                                                                              \
        return matrix->data[r * matrix->ncols + c];                                                \
    }

#define MAKE_MATRIX_GET_PTR(NAME, T)                                                               \
    static inline T* NAME##_get_ptr(struct NAME const* matrix, uint_fast16_t r, uint_fast16_t c)   \
    {                                                                                              \
        return matrix->data + (r * matrix->ncols + c);                                             \
    }

#define MAKE_MATRIX_GET_PTR_C(NAME, T)                                                             \
    static inline T const* NAME##_get_ptr_c(struct NAME const* matrix, uint_fast16_t r,            \
                                            uint_fast16_t c)                                       \
    {                                                                                              \
        return matrix->data + (r * matrix->ncols + c);                                             \
    }

#define MAKE_MATRIX_SET(NAME, T)                                                                   \
    static inline void NAME##_set(struct NAME* matrix, uint_fast16_t r, uint_fast16_t c, T v)      \
    {                                                                                              \
        matrix->data[r * matrix->ncols + c] = v;                                                   \
    }

#define MAKE_MATRIX_SET_ALL(NAME, T)                                                               \
    static inline void NAME##_set_all(struct NAME* matrix, T v)                                    \
    {                                                                                              \
        for (uint_fast16_t i = 0; i < matrix->nrows * matrix->ncols; ++i)                          \
            matrix->data[i] = v;                                                                   \
    }

#define MAKE_MATRIX_DESTROY(NAME)                                                                  \
    static inline void NAME##_destroy(struct NAME const* matrix)                                   \
    {                                                                                              \
        if (!matrix)                                                                               \
            return;                                                                                \
                                                                                                   \
        free_c(matrix->data);                                                                      \
        free_c(matrix);                                                                            \
    }

#define MAKE_MATRIX_NROWS(NAME)                                                                    \
    static inline uint_fast16_t NAME##_nrows(struct NAME const* matrix) { return matrix->nrows; }

#define MAKE_MATRIX_NCOLS(NAME)                                                                    \
    static inline uint_fast16_t NAME##_ncols(struct NAME const* matrix) { return matrix->ncols; }

#define MAKE_MATRIX_RESIZE(NAME, T)                                                                \
    static inline void NAME##_resize(struct NAME* matrix, uint_fast16_t const nrows,               \
                                     uint_fast16_t const ncols)                                    \
    {                                                                                              \
        matrix->data = realloc(matrix->data, sizeof(T) * (nrows * ncols));                         \
        matrix->nrows = nrows;                                                                     \
        matrix->ncols = ncols;                                                                     \
    }

#endif
