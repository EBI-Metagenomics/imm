#ifndef IMM_OUTPUT_H
#define IMM_OUTPUT_H

#include "imm/export.h"
#include <stdio.h>

struct imm_model;

struct imm_output
{
    FILE*       stream;
    char const* filepath;
};

IMM_API struct imm_output* imm_output_create(char const* filepath);
IMM_API int                imm_output_destroy(struct imm_output const* output);
IMM_API int imm_output_write(struct imm_output* output, struct imm_model const* model);

#endif
