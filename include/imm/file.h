#ifndef IMM_FILE_H
#define IMM_FILE_H

#include "imm/export.h"
#include <stdint.h>
#include <stdio.h>

IMM_API int     imm_file_seek(FILE* stream, int64_t offset, int origin);
IMM_API int64_t imm_file_tell(FILE* stream);

#endif
