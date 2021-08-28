#ifndef DP_EMIS_H
#define DP_EMIS_H

#include "imm/dp/emis.h"

struct imm_state;
struct imm_dp_code;

void emis_del(struct imm_dp_emis const *emis);

void emis_init(struct imm_dp_emis *emis);

enum imm_rc emis_reset(struct imm_dp_emis *emis, struct imm_dp_code const *code,
                       struct imm_state **states, unsigned nstates);

static inline imm_float emis_score(struct imm_dp_emis const *emis,
                                   unsigned state, unsigned seq_code)
{
    return emis->score[emis->offset[state] + seq_code];
}

static inline unsigned emis_score_size(struct imm_dp_emis const *emis,
                                       unsigned nstates)
{
    return emis->offset[nstates];
}

static inline unsigned emis_offset_size(unsigned nstates)
{
    return nstates + 1;
}

#endif
