#ifndef IMM_EMIS_H
#define IMM_EMIS_H

#include "imm/export.h"
#include "imm/float.h"
#include <stdint.h>

struct imm_emis
{
    imm_float *score; /**< Sequence emission score of a state. */
    uint32_t *offset; /**< Maps state to score array offset. */
};

struct imm_state;
struct imm_code;

void imm_emis_init(struct imm_emis *);
void imm_emis_del(struct imm_emis const *);
int imm_emis_reset(struct imm_emis *, struct imm_code const *,
                   struct imm_state **states, unsigned nstates);
imm_float imm_emis_score(struct imm_emis const *, unsigned state,
                         unsigned seq_code);
unsigned imm_emis_score_size(struct imm_emis const *, unsigned nstates);
unsigned imm_emis_offset_size(unsigned nstates);

#endif
