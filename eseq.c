#include "eseq.h"
#include "code.h"

void imm_eseq_init(struct imm_eseq *eseq, struct imm_code const *code)
{
  imm_matrixi16_init(&eseq->data, 1, 1);
  eseq->code = code;
}

void imm_eseq_cleanup(struct imm_eseq *eseq)
{
  imm_matrixi16_cleanup(&eseq->data);
}

struct imm_abc const *imm_eseq_abc(struct imm_eseq const *eseq)
{
  return eseq->code->abc;
}

int imm_eseq_size(struct imm_eseq const *eseq) { return eseq->data.rows - 1; }

int imm_eseq_setup(struct imm_eseq *eseq, struct imm_seq const *seq)
{
  int ncols = IMM_STATE_MAX_SEQSIZE + 1;
  int rc = 0;

  if ((rc = imm_matrixi16_resize(&eseq->data, imm_seq_size(seq) + 1, ncols)))
    return rc;

  for (int i = 0; i <= imm_seq_size(seq); ++i)
  {
    for (int size = 0; size < ncols; ++size)
    {
      if (i + size > imm_seq_size(seq)) break;

      struct imm_seq t = imm_seq_slice(seq, imm_range(i, i + size));
      int code = imm_code_encode(eseq->code, &t);
      imm_matrixi16_set(&eseq->data, i, size, (int16_t)code);
    }
  }
  return rc;
}

struct imm_eseq imm_eseq_slice(struct imm_eseq const *x, struct imm_range r)
{
  assert(r.start + imm_range_size(r) <= x->data.rows);
  int16_t *ptr = x->data.data + r.start;
  struct imm_matrixi16 m = {ptr, imm_range_size(r), x->data.cols};
  return (struct imm_eseq){m, x->code};
}
