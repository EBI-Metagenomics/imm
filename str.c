#include "str.h"
#include <assert.h>
#include <limits.h>
#include <string.h>

struct imm_str imm_str(char const *cstr)
{
  assert(strlen(cstr) <= UINT_MAX);
  return (struct imm_str){(unsigned)strlen(cstr), cstr};
}

struct imm_str imm_str_slice(struct imm_str x, struct imm_range range)
{
  assert(range.start + imm_range_size(range) <= x.size);
  return (struct imm_str){imm_range_size(range), x.data + range.start};
}
