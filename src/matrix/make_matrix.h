#ifndef MATRIX_MAKE_MATRIX_H
#define MATRIX_MAKE_MATRIX_H

#include "common/common.h"

#define MAKE_MATRIX_STRUCT(NAME, T)                                            \
    struct NAME                                                                \
    {                                                                          \
        T *data;                                                               \
        unsigned rows;                                                         \
        unsigned cols;                                                         \
    };

#define MAKE_MATRIX_INIT(NAME, T)                                              \
    static inline void NAME##_init(struct NAME *matrix, unsigned rows,         \
                                   unsigned cols)                              \
    {                                                                          \
        matrix->data = xmalloc(sizeof(T) * (rows * cols));                     \
        matrix->rows = rows;                                                   \
        matrix->cols = cols;                                                   \
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
    static inline struct NAME *NAME##_resize(struct NAME *matrix,              \
                                             unsigned rows, unsigned cols)     \
    {                                                                          \
        matrix->data = xrealloc(matrix->data, sizeof(T) * (rows * cols));      \
        matrix->rows = rows;                                                   \
        matrix->cols = cols;                                                   \
        return matrix;                                                         \
    }

#endif
