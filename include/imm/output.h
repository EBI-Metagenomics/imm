#ifndef IMM_OUTPUT_H
#define IMM_OUTPUT_H

#include "imm/export.h"

struct imm_profile;
struct imm_output;

IMM_API int                imm_output_close(struct imm_output* output);
IMM_API struct imm_output* imm_output_create(char const* filepath);
IMM_API int                imm_output_destroy(struct imm_output* output);
IMM_API int                imm_output_write(struct imm_output* output, struct imm_profile const* prof);

#endif
