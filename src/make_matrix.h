#ifndef MAKE_MATRIX_H
#define MAKE_MATRIX_H

#include "reallocf.h"
#include <stdlib.h>

#define MAKE_MATRIX_INIT(NAME, T)                                              \
    static inline int NAME##_init(struct NAME *matrix, unsigned rows,          \
                                  unsigned cols)                               \
    {                                                                          \
        matrix->data = malloc(sizeof(T) * (rows * cols));                      \
        if (!matrix->data) return 1;                                           \
        matrix->rows = rows;                                                   \
        matrix->cols = cols;                                                   \
        return 0;                                                              \
    }

#define MAKE_MATRIX_EMPTY(NAME, T)                                             \
    static inline void NAME##_empty(struct NAME *matrix)                       \
    {                                                                          \
        matrix->data = NULL;                                                   \
        matrix->rows = 0;                                                      \
        matrix->cols = 0;                                                      \
    }

#define MAKE_MATRIX_GET(NAME, T)                                               \
    static inline T NAME##_get(struct NAME const *matrix, unsigned r,          \
                               unsigned c)                                     \
    {                                                                          \
        return matrix->data[r * matrix->cols + c];                             \
    }

#define MAKE_MATRIX_GET_PTR(NAME, T)                                           \
    static inline T *NAME##_get_ptr(struct NAME const *matrix, unsigned r,     \
                                    unsigned c)                                \
    {                                                                          \
        return matrix->data + (r * matrix->cols + c);                          \
    }

#define MAKE_MATRIX_GET_PTR_C(NAME, T)                                         \
    static inline T const *NAME##_get_ptr_c(struct NAME const *matrix,         \
                                            unsigned r, unsigned c)            \
    {                                                                          \
        return matrix->data + (r * matrix->cols + c);                          \
    }

#define MAKE_MATRIX_SET(NAME, T)                                               \
    static inline void NAME##_set(struct NAME *matrix, unsigned r, unsigned c, \
                                  T v)                                         \
    {                                                                          \
        matrix->data[r * matrix->cols + c] = v;                                \
    }

#define MAKE_MATRIX_SET_ALL(NAME, T)                                           \
    static inline void NAME##_set_all(struct NAME *matrix, T v)                \
    {                                                                          \
        for (unsigned i = 0; i < matrix->rows * matrix->cols; ++i)             \
            matrix->data[i] = v;                                               \
    }

#define MAKE_MATRIX_DEINIT(NAME)                                               \
    static inline void NAME##_deinit(struct NAME const *matrix)                \
    {                                                                          \
        free(matrix->data);                                                    \
    }

#define MAKE_MATRIX_NROWS(NAME)                                                \
    static inline unsigned NAME##_nrows(struct NAME const *matrix)             \
    {                                                                          \
        return matrix->rows;                                                   \
    }

#define MAKE_MATRIX_NCOLS(NAME)                                                \
    static inline unsigned NAME##_ncols(struct NAME const *matrix)             \
    {                                                                          \
        return matrix->cols;                                                   \
    }

#define MAKE_MATRIX_RESIZE(NAME, T)                                            \
    static inline int NAME##_resize(struct NAME *matrix, unsigned rows,        \
                                    unsigned cols)                             \
    {                                                                          \
        matrix->data = reallocf(matrix->data, sizeof(T) * (rows * cols));      \
        if (!matrix->data && rows * cols > 0) return 1;                        \
        matrix->rows = rows;                                                   \
        matrix->cols = cols;                                                   \
        return 0;                                                              \
    }

#define MAKE_MATRIX(name, type)                                                \
    MAKE_MATRIX_INIT(name, type)                                               \
    MAKE_MATRIX_EMPTY(name, type)                                              \
    MAKE_MATRIX_GET(name, type)                                                \
    MAKE_MATRIX_GET_PTR(name, type)                                            \
    MAKE_MATRIX_GET_PTR_C(name, type)                                          \
    MAKE_MATRIX_SET(name, type)                                                \
    MAKE_MATRIX_DEINIT(name)                                                   \
    MAKE_MATRIX_NROWS(name)                                                    \
    MAKE_MATRIX_NCOLS(name)                                                    \
    MAKE_MATRIX_RESIZE(name, type)

#endif
