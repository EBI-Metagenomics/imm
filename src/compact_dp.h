#ifndef IMM_COMPACT_DP_H
#define IMM_COMPACT_DP_H

struct ctransition
{
    double*   cost;
    unsigned* offset;
};

struct cemission
{
    double*   cost;
    unsigned* offset;
    unsigned* min_seq;
    unsigned* max_seq;
};

struct cdp
{
    unsigned                nstates;
    double*                 start_lprob;
    struct ctransition*     transition;
    struct cemission*       emission;
    struct imm_state const* state;
};

#endif
