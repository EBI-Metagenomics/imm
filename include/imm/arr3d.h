#ifndef IMM_ARR3D_H
#define IMM_ARR3D_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/log.h"
#include "imm/lprob.h"

struct imm_arr3d
{
    unsigned shape[3];
    unsigned strides[3];
    imm_float *data;
};

static inline unsigned imm_arr3d_len(struct imm_arr3d const *arr)
{
    return arr->shape[0] * arr->shape[1] * arr->shape[2];
}

IMM_API void imm_arr3d_init(struct imm_arr3d *arr, unsigned dim0, unsigned dim1,
                            unsigned dim2);

IMM_API void imm_arr3d_deinit(struct imm_arr3d *arr);

static inline void imm_arr3d_fill(struct imm_arr3d *arr, imm_float val)
{
    for (unsigned i = 0; i < imm_arr3d_len(arr); ++i)
        arr->data[i] = val;
}

static inline unsigned __imm_arr3d_idx(struct imm_arr3d const *arr,
                                       unsigned const idx[3])
{
    unsigned const *s = arr->strides;
    return idx[0] * s[0] + idx[1] * s[1] + idx[2] * s[2];
}

static inline imm_float imm_arr3d_get(struct imm_arr3d const *arr,
                                      unsigned const idx[3])
{
    IMM_BUG(__imm_arr3d_idx(arr, idx) >= imm_arr3d_len(arr));
    return arr->data[__imm_arr3d_idx(arr, idx)];
}

static inline int imm_arr3d_normalize(struct imm_arr3d const *arr)
{
    return imm_lprob_normalize(imm_arr3d_len(arr), arr->data);
}

static inline void imm_arr3d_set(struct imm_arr3d *arr, unsigned const idx[3],
                                 imm_float val)
{
    IMM_BUG(__imm_arr3d_idx(arr, idx) >= imm_arr3d_len(arr));
    arr->data[__imm_arr3d_idx(arr, idx)] = val;
}

#endif
