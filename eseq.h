#ifndef IMM_ESEQ_H
#define IMM_ESEQ_H

#include "code.h"
#include "compiler.h"
#include "matrixu16.h"

struct imm_seq;
struct imm_code;

struct imm_eseq
{
  struct imm_matrixu16 data;
  struct imm_code const *code;
};

void imm_eseq_cleanup(struct imm_eseq const *);
void imm_eseq_reset(struct imm_eseq *, struct imm_code const *);
unsigned imm_eseq_len(struct imm_eseq const *);
void imm_eseq_init(struct imm_eseq *, struct imm_code const *);
int imm_eseq_setup(struct imm_eseq *, struct imm_seq const *);

TEMPLATE unsigned imm_eseq_get(struct imm_eseq const *x, unsigned const pos,
                               unsigned const len, unsigned const min_seq)
{
  uint16_t val = imm_matrixu16_get(&x->data, pos, len);
  return imm_code_translate(x->code, val, min_seq);
}

#endif
