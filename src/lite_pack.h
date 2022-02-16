#ifndef IMM_LITE_PACK_H
#define IMM_LITE_PACK_H

#include "lite_pack/1darray/1darray.h"
#include "lite_pack/io/file.h"
#include "lite_pack/lite_pack.h"

#ifdef IMM_DOUBLE_PRECISION
#define XLIP_1DARRAY_FLOAT LIP_1DARRAY_F64
#else
#define XLIP_1DARRAY_FLOAT LIP_1DARRAY_F32
#endif

#endif
