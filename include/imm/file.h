#ifndef IMM_FILE_H
#define IMM_FILE_H

#include <stdint.h>
#include <stdio.h>

int     imm_file_seek(FILE* stream, int64_t offset, int origin);
int64_t imm_file_tell(FILE* stream);

#endif
