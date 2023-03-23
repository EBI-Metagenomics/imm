#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

#include "imm/cell.h"
#include "imm/float.h"
#include "imm/matrixf.h"
#include <stdio.h>

struct imm_state_table;

struct imm_matrix
{
    struct imm_matrixf score;
    int *state_col;
};

void imm_matrix_del(struct imm_matrix const *);
int imm_matrix_init(struct imm_matrix *, struct imm_state_table const *);
int imm_matrix_reset(struct imm_matrix *, struct imm_state_table const *);
imm_float imm_matrix_get_score(struct imm_matrix const *, struct imm_cell);
void imm_matrix_set_score(struct imm_matrix *, struct imm_cell, imm_float);
void imm_matrix_dump(struct imm_matrix *, FILE *);

#endif
