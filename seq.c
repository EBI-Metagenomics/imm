#include "seq.h"
#include "abc.h"
#include "rc.h"
#include <assert.h>

struct imm_seq imm_seq_unsafe(unsigned size, char const *str,
                              struct imm_abc const *abc)
{
  return (struct imm_seq){size, str, abc};
}

struct imm_seq imm_seq(struct imm_str str, struct imm_abc const *abc)
{
  for (unsigned i = 0; i < str.len; ++i)
  {
    char const any = imm_abc_any_symbol(abc);
    assert(imm_abc_has_symbol(abc, str.data[i]) || str.data[i] == any);
    (void)any;
  }

  return (struct imm_seq){str.len, str.data, abc};
}

int imm_seq_init(struct imm_seq *x, struct imm_str str,
                 struct imm_abc const *abc)
{
  char const any = imm_abc_any_symbol(abc);
  for (unsigned i = 0; i < str.len; ++i)
  {
    if (!(imm_abc_has_symbol(abc, str.data[i]) || str.data[i] == any))
      return IMM_EINVAL;
  }
  x->size = str.len;
  x->str = str.data;
  x->abc = abc;
  return 0;
}

struct imm_abc const *imm_seq_abc(struct imm_seq const *seq)
{
  return seq->abc;
}

unsigned imm_seq_size(struct imm_seq const *seq) { return seq->size; }

char const *imm_seq_str(struct imm_seq const *seq) { return seq->str; }

unsigned imm_seq_symbol_idx(struct imm_seq const *seq, unsigned i)
{
  return imm_abc_symbol_idx(seq->abc, seq->str[i]);
}
