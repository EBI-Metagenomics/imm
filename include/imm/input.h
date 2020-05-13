#ifndef IMM_INPUT_H
#define IMM_INPUT_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

struct imm_input;
struct imm_model;

struct imm_input_vtable
{
    struct imm_model const* (*read_block)(struct imm_input* input, uint8_t block_type);
};

struct imm_input
{
    FILE*       stream;
    char const* filepath;

    bool eof;

    struct imm_input_vtable vtable;
};

IMM_API struct imm_input*       imm_input_create(char const* filepath);
IMM_API int                     imm_input_destroy(struct imm_input const* input);
IMM_API bool                    imm_input_eof(struct imm_input const* input);
IMM_API struct imm_model const* imm_input_read(struct imm_input* input);

IMM_API struct imm_model const* __imm_input_read_block(struct imm_input* input, uint8_t block_type);

#endif
