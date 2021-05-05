#include "imm/arr3d.h"
#include "common/common.h"

void imm_arr3d_init(struct imm_arr3d *arr, unsigned dim0, unsigned dim1,
                    unsigned dim2)
{
    unsigned len = dim0 * dim1 * dim2;
    arr->strides[0] = (uint16_t)dim0;
    arr->strides[1] = (uint16_t)dim1;
    arr->strides[2] = (uint16_t)dim2;
    arr->data = xmalloc(sizeof(*arr->data) * len);
}

void imm_arr3d_deinit(struct imm_arr3d *arr) { free(arr->data); }
