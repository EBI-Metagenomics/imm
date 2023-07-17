#ifndef IMM_ESEQ_H
#define IMM_ESEQ_H

#include "api.h"
#include "code.h"
#include "compiler.h"
#include "matrixu16.h"

struct imm_abc;
struct imm_code;
struct imm_seq;

struct imm_eseq
{
  struct imm_matrixu16 data;
  struct imm_code const *code;
};

IMM_API void imm_eseq_init(struct imm_eseq *, struct imm_code const *);
IMM_API void imm_eseq_cleanup(struct imm_eseq *);
IMM_API struct imm_abc const *imm_eseq_abc(struct imm_eseq const *);
IMM_API unsigned imm_eseq_size(struct imm_eseq const *);
IMM_API int imm_eseq_setup(struct imm_eseq *, struct imm_seq const *);

imm_pure_template unsigned imm_eseq_get(struct imm_eseq const *x, unsigned pos,
                                        unsigned size, unsigned min_seq)
{
  uint16_t val = imm_matrixu16_get(&x->data, pos, size);
  return imm_code_translate(x->code, val, min_seq);
}

#endif
