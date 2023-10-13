#include "code.h"
#include "abc.h"
#include "seq.h"

static void calc_strides(int16_t *stride, int abc_size)
{
  int last = IMM_STATE_MAX_SEQSIZE - 1;
  stride[last] = 1;
  while (last > 0)
  {
    --last;
    stride[last] = (int16_t)(stride[last + 1] * abc_size);
  }
}

static void calc_offsets(int16_t *offset, int abc_size)
{
  int n = abc_size;
  int i = 0;
  offset[i] = 0;
  /* +1 for sentinel */
  while (i < IMM_STATE_MAX_SEQSIZE + 1)
  {
    ++i;
    offset[i] = (int16_t)(offset[i - 1] * n + 1);
  }
}

void imm_code_init(struct imm_code *code, struct imm_abc const *abc)
{
  code->abc = abc;
  calc_strides(code->stride, imm_abc_size(code->abc));
  calc_offsets(code->offset, imm_abc_size(code->abc));
}

int imm_code_encode(struct imm_code const *code, struct imm_seq const *seq)
{
  int size = imm_seq_size(seq);
  int c = code->offset[size];
  for (int i = 0; i < size; ++i)
  {
    int j = imm_seq_symbol_idx(seq, i);
    c += code->stride[IMM_STATE_MAX_SEQSIZE - size + i] * j;
  }
  return c;
}
