#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

#include "assume.h"
#include "cell.h"
#include "compiler.h"
#include "matrixf.h"
#include "state.h"
#include <stdio.h>

#define IMM_MATRIX_NROWS (IMM_STATE_MAX_SEQLEN)

struct imm_state_table;

struct imm_matrix
{
  struct imm_matrixf score;
  int16_t *state_col;
};

void imm_matrix_cleanup(struct imm_matrix *);
int imm_matrix_init(struct imm_matrix *, struct imm_state_table const *);
int imm_matrix_reset(struct imm_matrix *, struct imm_state_table const *);
void imm_matrix_dump(struct imm_matrix *, FILE *restrict);

imm_pure_template float imm_matrix_get_score(struct imm_matrix const *x,
                                             struct imm_cell y)
{
  uint_fast8_t row = y.row % IMM_MATRIX_NROWS;
  uint_fast16_t col =
      (uint_fast16_t)(x->state_col[y.state_idx] + y.emission_length);
  return imm_matrixf_get(&x->score, row, col);
}

imm_template void imm_matrix_set_score(struct imm_matrix *x, struct imm_cell y,
                                       float score)
{
  uint_fast8_t row = y.row % IMM_MATRIX_NROWS;
  uint_fast16_t col =
      (uint_fast16_t)(x->state_col[y.state_idx] + y.emission_length);
  imm_matrixf_set(&x->score, row, (unsigned)col, score);
}

#endif
