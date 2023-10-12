#include "eseq.h"
#include "code.h"

void imm_eseq_init(struct imm_eseq *eseq, struct imm_code const *code)
{
  imm_matrixu16_init(&eseq->data, 1, 1);
  eseq->code = code;
}

void imm_eseq_cleanup(struct imm_eseq *eseq)
{
  imm_matrixu16_cleanup(&eseq->data);
}

struct imm_abc const *imm_eseq_abc(struct imm_eseq const *eseq)
{
  return eseq->code->abc;
}

unsigned imm_eseq_size(struct imm_eseq const *eseq)
{
  return eseq->data.rows - 1;
}

int imm_eseq_setup(struct imm_eseq *eseq, struct imm_seq const *seq)
{
  unsigned ncols = IMM_STATE_MAX_SEQLEN + 1;
  int rc = 0;

  if ((rc = imm_matrixu16_resize(&eseq->data, imm_seq_size(seq) + 1, ncols)))
    return rc;

  for (unsigned i = 0; i <= imm_seq_size(seq); ++i)
  {
    for (unsigned len = 0; len < ncols; ++len)
    {
      if (i + len > imm_seq_size(seq)) break;

      struct imm_seq t = imm_seq_slice(seq, imm_range(i, i + len));
      unsigned code = imm_code_encode(eseq->code, &t);
      imm_matrixu16_set(&eseq->data, i, len, (uint16_t)code);
    }
  }
  return rc;
}

struct imm_eseq imm_eseq_slice(struct imm_eseq *x, struct imm_range r)
{
  assert(r.start + imm_range_size(r) <= x->data.rows);
  uint16_t *ptr = x->data.data + r.start;
  struct imm_matrixu16 m = {ptr, imm_range_size(r), x->data.cols};
  return (struct imm_eseq){m, x->code};
}
