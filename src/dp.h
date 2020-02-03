#ifndef IMM_DP_H
#define IMM_DP_H

struct dp_matrix;
struct imm_path;
struct imm_seq;
struct imm_state;
struct mstate;

struct dp
{
    double                   lprob_zero;
    unsigned                 nstates;
    struct state_info*       states;
    struct state_info const* end_state;
};

struct dp* dp_create(struct mstate const* const* mm_states, unsigned nstates,
                     struct imm_seq const* seq, struct imm_state const* end_state);
double     dp_viterbi(struct dp const* dp, struct dp_matrix* matrix, struct imm_path* path);
void       dp_destroy(struct dp const* dp);

#endif
