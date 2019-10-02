#ifndef IMM_STATE_STATE_H
#define IMM_STATE_STATE_H

#include "imm.h"
#include "rapidstring.h"

typedef void (*state_destroy)(struct imm_state *state);
typedef double (*state_emiss_lprob)(const struct imm_state *state, const char *seq,
                                    size_t seq_len);
typedef int (*state_normalize)(struct imm_state *state);

struct imm_state
{
    rapidstring name;
    const struct imm_alphabet *alphabet;
    bool end_state;

    state_destroy destroy;
    state_emiss_lprob emiss_lprob;
    state_normalize normalize;
    void *impl;
};

#endif
