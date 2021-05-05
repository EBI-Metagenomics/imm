#ifndef IMM_ARR3D_H
#define IMM_ARR3D_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/lprob.h"
#include <stdint.h>

struct imm_arr3d
{
    uint16_t strides[3];
    imm_float *data;
};

static inline unsigned imm_arr3d_len(struct imm_arr3d const *arr)
{
    return arr->strides[0] * arr->strides[1] * arr->strides[2];
}

IMM_API void imm_arr3d_init(struct imm_arr3d *arr, unsigned dim0, unsigned dim1,
                            unsigned dim2);

IMM_API void imm_arr3d_deinit(struct imm_arr3d *arr);

static inline void imm_arr3d_fill(struct imm_arr3d *arr, imm_float val)
{
    for (unsigned i = 0; i < imm_arr3d_len(arr); ++i)
        arr->data[i] = val;
}

static inline imm_float imm_arr3d_get(struct imm_arr3d const *arr,
                                      unsigned dim0, unsigned dim1,
                                      unsigned dim2)
{
    uint16_t const *s = arr->strides;
    return arr->data[dim0 * s[0] + dim1 * s[1] + dim2 * s[2]];
}

static inline int imm_arr3d_normalize(struct imm_arr3d const *arr)
{
    return imm_lprob_normalize(arr->data, imm_arr3d_len(arr));
}

static inline void imm_arr3d_set(struct imm_arr3d *arr, unsigned dim0,
                                 unsigned dim1, unsigned dim2, imm_float val)
{
    uint16_t const *s = arr->strides;
    arr->data[dim0 * s[0] + dim1 * s[1] + dim2 * s[2]] = val;
}

#endif
