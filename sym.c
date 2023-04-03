#include "sym.h"
#include "array_size.h"
#include "likely.h"

void imm_sym_init(struct imm_sym *sym)
{
  for (unsigned i = 0; i < imm_array_size(sym->idx); ++i)
    sym->idx[i] = IMM_SYM_NULL_IDX;
}

unsigned imm_sym_id(char c) { return IMM_SYM_ID(c); }

char imm_sym_char(unsigned id) { return IMM_SYM_CHAR(id); }

unsigned imm_sym_idx(struct imm_sym const *sym, unsigned id)
{
  return sym->idx[id];
}

void imm_sym_set_idx(struct imm_sym *sym, unsigned id, unsigned idx)
{
  sym->idx[id] = (uint8_t)idx;
}

bool imm_sym_valid_char(char c)
{
  return imm_likely(c >= IMM_SYM_FIRST_CHAR && c <= IMM_SYM_LAST_CHAR);
}

bool imm_sym_valid_id(unsigned id)
{
  return imm_sym_valid_char(imm_sym_char(id));
}
