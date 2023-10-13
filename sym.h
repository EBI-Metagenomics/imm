#ifndef IMM_SYM_H
#define IMM_SYM_H

#include "compiler.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum imm_sym_type : int
{
  IMM_SYM_NULL = 0,
  IMM_SYM_NORMAL = 1,
  IMM_SYM_ANY = 2,
};

#define IMM_SYM_FIRST_CHAR '!'
#define IMM_SYM_LAST_CHAR '~'

#define IMM_SYM_NULL_ID UINT8_MAX
#define IMM_SYM_NULL_IDX UINT8_MAX

#define IMM_SYM_ID(c) ((int)(c - IMM_SYM_FIRST_CHAR))
#define IMM_SYM_CHAR(x) ((char)(x + IMM_SYM_FIRST_CHAR))

#define IMM_SYM_SIZE ((IMM_SYM_LAST_CHAR - IMM_SYM_FIRST_CHAR) + 1)

struct imm_sym
{
  uint8_t idx[IMM_SYM_SIZE];
};

// clang-format off
IMM_API void imm_sym_init(struct imm_sym *);
IMM_API int  imm_sym_id(char c);
IMM_API char imm_sym_char(int id);
IMM_API int  imm_sym_idx(struct imm_sym const *, int id);
IMM_API void imm_sym_set_idx(struct imm_sym *, int id, int idx);
IMM_API bool imm_sym_valid_char(char c);
IMM_API bool imm_sym_valid_id(int id);
// clang-format on

#endif
