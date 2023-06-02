#ifndef IMM_CODE_H
#define IMM_CODE_H

#include "compiler.h"
#include "export.h"
#include "state.h"
#include <assert.h>
#include <stdint.h>

struct imm_abc;
struct imm_code;
struct imm_seq;

struct imm_code
{
  uint16_t offset[IMM_STATE_MAX_SEQLEN + 2];
  uint16_t stride[IMM_STATE_MAX_SEQLEN];
  struct imm_abc const *abc;
};

IMM_API void imm_code_init(struct imm_code *, struct imm_abc const *);
unsigned imm_code_encode(struct imm_code const *, struct imm_seq const *);

imm_pure_template unsigned imm_code_translate(struct imm_code const *x,
                                              unsigned const value,
                                              unsigned const min_seq)
{
  assert(value >= x->offset[min_seq]);
  return value - x->offset[min_seq];
}

imm_pure_template unsigned imm_code_size(struct imm_code const *x,
                                         unsigned min_seq, unsigned max_seq)
{
  return (unsigned)(x->offset[max_seq + 1] - x->offset[min_seq]);
}

#endif
