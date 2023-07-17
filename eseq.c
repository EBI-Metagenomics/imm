#include "eseq.h"
#include "code.h"
#include "subseq.h"

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

      struct imm_seq subseq = imm_subseq(seq, i, len);
      unsigned code = imm_code_encode(eseq->code, &subseq);
      imm_matrixu16_set(&eseq->data, i, len, (uint16_t)code);
    }
  }
  return rc;
}
