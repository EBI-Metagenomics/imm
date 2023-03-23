#ifndef IMM_ESEQ_H
#define IMM_ESEQ_H

#include "imm/matrixu16.h"

struct imm_seq;
struct imm_code;

struct imm_eseq
{
    struct imm_matrixu16 data;
    struct imm_code const *code;
};

void imm_eseq_del(struct imm_eseq const *);
void imm_eseq_reset(struct imm_eseq *, struct imm_code const *);
unsigned imm_eseq_get(struct imm_eseq const *, unsigned pos, unsigned len,
                      unsigned min_seq);
unsigned imm_eseq_len(struct imm_eseq const *);
void imm_eseq_init(struct imm_eseq *, struct imm_code const *);
int imm_eseq_setup(struct imm_eseq *, struct imm_seq const *);

#endif
