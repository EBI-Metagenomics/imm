#ifndef IMM_LIP_H
#define IMM_LIP_H

#include "lip/1darray/1darray.h"
#include "lip/file/file.h"
#include "lip/lip.h"

#ifdef IMM_DOUBLE_PRECISION
#define XLIP_1DARRAY_FLOAT LIP_1DARRAY_F64
#else
#define XLIP_1DARRAY_FLOAT LIP_1DARRAY_F32
#endif

#endif
