#ifndef IMM_STR_H
#define IMM_STR_H

#include "compiler.h"
#include "range.h"

struct imm_str
{
  int size;
  char const *data;
};

struct imm_str imm_str(char const *cstr);
struct imm_str imm_str_slice(struct imm_str, struct imm_range);

#endif
