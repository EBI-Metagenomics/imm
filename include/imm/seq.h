#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/api.h"

struct imm_abc;
struct imm_seq;

IMM_API struct imm_seq const* imm_seq_create(char const* seq, struct imm_abc const* abc);
IMM_API struct imm_abc const* imm_seq_get_abc(struct imm_seq const* seq);
IMM_API unsigned              imm_seq_length(struct imm_seq const* seq);
IMM_API char const*           imm_seq_string(struct imm_seq const* seq);
IMM_API struct imm_seq const* imm_seq_duplicate(struct imm_seq const* seq);
IMM_API void                  imm_seq_destroy(struct imm_seq const* seq);

#endif
