#ifndef IMM_SYM_H
#define IMM_SYM_H

#include "api.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum imm_sym_type
{
  IMM_SYM_NULL = 0,
  IMM_SYM_NORMAL = 1,
  IMM_SYM_ANY = 2,
};

#define IMM_SYM_FIRST_CHAR '!'
#define IMM_SYM_LAST_CHAR '~'

#define IMM_SYM_NULL_ID UINT8_MAX
#define IMM_SYM_NULL_IDX UINT8_MAX

#define IMM_SYM_ID(c) ((unsigned)(c - IMM_SYM_FIRST_CHAR))
#define IMM_SYM_CHAR(x) ((char)(x + IMM_SYM_FIRST_CHAR))

#define IMM_SYM_SIZE ((size_t)((IMM_SYM_LAST_CHAR - IMM_SYM_FIRST_CHAR) + 1))

struct imm_sym
{
  uint8_t idx[IMM_SYM_SIZE];
};

IMM_API void imm_sym_init(struct imm_sym *);
IMM_API unsigned imm_sym_id(char c);
IMM_API char imm_sym_char(unsigned id);
IMM_API unsigned imm_sym_idx(struct imm_sym const *, unsigned id);
IMM_API void imm_sym_set_idx(struct imm_sym *, unsigned id, unsigned idx);
IMM_API bool imm_sym_valid_char(char c);
IMM_API bool imm_sym_valid_id(unsigned id);

#endif
