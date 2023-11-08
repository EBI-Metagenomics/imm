#include "score_table.h"
#include "abc.h"
#include "code.h"
#include "lprob.h"

void imm_score_table_init(struct imm_score_table *x,
                          struct imm_code const *code)
{
  x->code = code;
}

void imm_score_table_cleanup(struct imm_score_table *x) { x->code = NULL; }

int imm_score_table_size(struct imm_score_table const *x,
                         struct imm_state const *state)
{
  return imm_code_size(x->code, imm_state_span(state));
}

void imm_score_table_scores(struct imm_score_table const *x,
                            struct imm_state const *state, float *scores)
{
  struct imm_code const *code = x->code;
  struct imm_abc const *abc = code->abc;

  struct imm_cartes cartes = {0};
  char const *set = abc->symbols;
  imm_cartes_init(&cartes, set, abc->size);

  int min = imm_state_span(state).min;
  int max = imm_state_span(state).max;
  for (int size = min; size <= max; ++size)
  {
    imm_cartes_setup(&cartes, size);
    char const *item = NULL;
    while ((item = imm_cartes_next(&cartes)) != NULL)
    {
      struct imm_seq seq = imm_seq_unsafe((struct imm_str){size, item}, abc);
      int c = imm_code_translate(code, imm_code_encode(code, &seq), min);
      float score = imm_state_lprob(state, &seq);
      assert(!imm_lprob_is_nan(score));
      scores[c] = score;
    }
  }
}
