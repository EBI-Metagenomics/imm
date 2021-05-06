#include "imm/arr3d.h"
#include "common/common.h"

void imm_arr3d_init(struct imm_arr3d *arr, unsigned dim0, unsigned dim1,
                    unsigned dim2)
{
    arr->shape[0] = dim0;
    arr->shape[1] = dim1;
    arr->shape[2] = dim2;
    arr->strides[0] = dim1 * dim2;
    arr->strides[1] = dim2;
    arr->strides[2] = 1;
    arr->data = xmalloc(sizeof(*arr->data) * imm_arr3d_len(arr));
}

void imm_arr3d_deinit(struct imm_arr3d *arr) { free(arr->data); }
