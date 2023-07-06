#ifndef IMM_SCORE_TABLE_H
#define IMM_SCORE_TABLE_H

#include "cartes.h"
#include "export.h"

struct imm_score_table
{
  struct imm_code const *code;
  struct imm_cartes cartes;
};

struct imm_state;

IMM_API void imm_score_table_init(struct imm_score_table *,
                                  struct imm_code const *);
IMM_API void imm_score_table_cleanup(struct imm_score_table *);

IMM_API unsigned imm_score_table_size(struct imm_score_table const *,
                                      struct imm_state const *);
IMM_API void imm_score_table_scores(struct imm_score_table *,
                                    struct imm_state const *, float *);

#endif
