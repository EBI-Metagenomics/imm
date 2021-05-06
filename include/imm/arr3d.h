#ifndef IMM_ARR3D_H
#define IMM_ARR3D_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/lprob.h"

struct imm_arr3d
{
    unsigned len;
    unsigned strides[3];
    imm_float *data;
};

static inline unsigned imm_arr3d_len(struct imm_arr3d const *arr)
{
    return arr->len;
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
                                      unsigned const idx[3])
{
    unsigned const *s = arr->strides;
    return arr->data[idx[0] * s[0] + idx[1] * s[1] + idx[2] * s[2]];
}

static inline int imm_arr3d_normalize(struct imm_arr3d const *arr)
{
    return imm_lprob_normalize(arr->data, imm_arr3d_len(arr));
}

static inline void imm_arr3d_set(struct imm_arr3d *arr, unsigned const idx[3],
                                 imm_float val)
{
    unsigned const *s = arr->strides;
    arr->data[idx[0] * s[0] + idx[1] * s[1] + idx[2] * s[2]] = val;
}

#endif
