#ifndef MAKE_MATRIX_H
#define MAKE_MATRIX_H

#include "imm/error.h"
#include <stdint.h>
#include <stdlib.h>

#define MAKE_MATRIX_STRUCT(NAME, T)                                                                                    \
    struct NAME                                                                                                        \
    {                                                                                                                  \
        T*            data;                                                                                            \
        uint_fast32_t nrows;                                                                                           \
        uint_fast32_t ncols;                                                                                           \
    };

#define MAKE_MATRIX_CREATE(NAME, T)                                                                                    \
    static inline struct NAME* NAME##_create(uint_fast32_t const nrows, uint_fast32_t const ncols)                     \
    {                                                                                                                  \
        struct NAME* matrix = malloc(sizeof(struct NAME));                                                             \
        if (!matrix)                                                                                                   \
            return NULL;                                                                                               \
        matrix->data = malloc(sizeof(T) * (nrows * ncols));                                                            \
        matrix->nrows = nrows;                                                                                         \
        matrix->ncols = ncols;                                                                                         \
        return matrix;                                                                                                 \
    }

#define MAKE_MATRIX_GET(NAME, T)                                                                                       \
    static inline T NAME##_get(struct NAME const* matrix, uint_fast32_t r, uint_fast32_t c)                            \
    {                                                                                                                  \
        return matrix->data[r * matrix->ncols + c];                                                                    \
    }

#define MAKE_MATRIX_GET_PTR(NAME, T)                                                                                   \
    static inline T* NAME##_get_ptr(struct NAME const* matrix, uint_fast32_t r, uint_fast32_t c)                       \
    {                                                                                                                  \
        return matrix->data + (r * matrix->ncols + c);                                                                 \
    }

#define MAKE_MATRIX_GET_PTR_C(NAME, T)                                                                                 \
    static inline T const* NAME##_get_ptr_c(struct NAME const* matrix, uint_fast32_t r, uint_fast32_t c)               \
    {                                                                                                                  \
        return matrix->data + (r * matrix->ncols + c);                                                                 \
    }

#define MAKE_MATRIX_SET(NAME, T)                                                                                       \
    static inline void NAME##_set(struct NAME* matrix, uint_fast32_t r, uint_fast32_t c, T v)                          \
    {                                                                                                                  \
        matrix->data[r * matrix->ncols + c] = v;                                                                       \
    }

#define MAKE_MATRIX_SET_ALL(NAME, T)                                                                                   \
    static inline void NAME##_set_all(struct NAME* matrix, T v)                                                        \
    {                                                                                                                  \
        for (uint_fast32_t i = 0; i < matrix->nrows * matrix->ncols; ++i)                                              \
            matrix->data[i] = v;                                                                                       \
    }

#define MAKE_MATRIX_DESTROY(NAME)                                                                                      \
    static inline void NAME##_destroy(struct NAME const* matrix)                                                       \
    {                                                                                                                  \
        if (!matrix)                                                                                                   \
            return;                                                                                                    \
                                                                                                                       \
        free(matrix->data);                                                                                            \
        free((void*)matrix);                                                                                           \
    }

#define MAKE_MATRIX_NROWS(NAME)                                                                                        \
    static inline uint_fast32_t NAME##_nrows(struct NAME const* matrix) { return matrix->nrows; }

#define MAKE_MATRIX_NCOLS(NAME)                                                                                        \
    static inline uint_fast32_t NAME##_ncols(struct NAME const* matrix) { return matrix->ncols; }

#define MAKE_MATRIX_RESIZE(NAME, T)                                                                                    \
    static inline int NAME##_resize(struct NAME* matrix, uint_fast32_t const nrows, uint_fast32_t const ncols)         \
    {                                                                                                                  \
        void* ptr = realloc(matrix->data, sizeof(T) * (nrows * ncols));                                                \
        if (!ptr) {                                                                                                    \
            return IMM_OUTOFMEM;                                                                                       \
        }                                                                                                              \
        matrix->data = ptr;                                                                                            \
        matrix->nrows = nrows;                                                                                         \
        matrix->ncols = ncols;                                                                                         \
        return IMM_SUCCESS;                                                                                            \
    }

#endif
