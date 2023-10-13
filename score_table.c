#include "score_table.h"
#include "abc.h"
#include "code.h"
#include "lprob.h"

void imm_score_table_init(struct imm_score_table *x,
                          struct imm_code const *code)
{
  x->code = code;
  struct imm_abc const *abc = code->abc;
  char const *set = abc->symbols;
  imm_cartes_init(&x->cartes, set, abc->size, IMM_STATE_MAX_SEQLEN);
}

void imm_score_table_cleanup(struct imm_score_table *x)
{
  x->code = NULL;
  imm_cartes_cleanup(&x->cartes);
}

int imm_score_table_size(struct imm_score_table const *x,
                         struct imm_state const *state)
{
  return imm_code_size(x->code, imm_state_span(state));
}

void imm_score_table_scores(struct imm_score_table *x,
                            struct imm_state const *state, float *scores)
{
  struct imm_code const *code = x->code;
  struct imm_abc const *abc = code->abc;

  int min = imm_state_span(state).min;
  int max = imm_state_span(state).max;
  for (int size = min; size <= max; ++size)
  {
    imm_cartes_setup(&x->cartes, size);
    char const *item = NULL;
    while ((item = imm_cartes_next(&x->cartes)) != NULL)
    {
      struct imm_seq seq = imm_seq_unsafe((struct imm_str){size, item}, abc);
      int c = imm_code_translate(code, imm_code_encode(code, &seq), min);
      float score = imm_state_lprob(state, &seq);
      assert(!imm_lprob_is_nan(score));
      scores[c] = score;
    }
  }
}
