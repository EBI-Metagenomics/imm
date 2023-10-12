#ifndef IMM_STR_H
#define IMM_STR_H

#include "api.h"
#include "range.h"

struct imm_str
{
  unsigned size;
  char const *data;
};

IMM_API struct imm_str imm_str(char const *cstr);
IMM_API struct imm_str imm_str_slice(struct imm_str, struct imm_range);

#endif
