#include "seq.h"
#include "abc.h"
#include "rc.h"
#include <assert.h>

struct imm_seq imm_seq_unsafe(struct imm_str str, struct imm_abc const *abc)
{
  for (int i = 0; i < str.size; ++i)
  {
    char const any = imm_abc_any_symbol(abc);
    assert(imm_abc_has_symbol(abc, str.data[i]) || str.data[i] == any);
    (void)any;
  }

  return (struct imm_seq){str, abc};
}

int imm_seq_init(struct imm_seq *x, struct imm_str str,
                 struct imm_abc const *abc)
{
  char const any = imm_abc_any_symbol(abc);
  for (int i = 0; i < str.size; ++i)
  {
    if (!(imm_abc_has_symbol(abc, str.data[i]) || str.data[i] == any))
      return IMM_EINVAL;
  }
  x->str = str;
  x->abc = abc;
  return 0;
}

struct imm_abc const *imm_seq_abc(struct imm_seq const *seq)
{
  return seq->abc;
}

int imm_seq_size(struct imm_seq const *seq) { return seq->str.size; }

char const *imm_seq_data(struct imm_seq const *seq) { return seq->str.data; }

int imm_seq_symbol_idx(struct imm_seq const *seq, int i)
{
  return imm_abc_symbol_idx(seq->abc, seq->str.data[i]);
}

struct imm_seq imm_seq_slice(struct imm_seq const *x, struct imm_range range)
{
  return imm_seq_unsafe(imm_str_slice(x->str, range), x->abc);
}
