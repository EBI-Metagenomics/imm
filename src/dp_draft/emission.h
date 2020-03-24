#ifndef IMM_DP_EMISSION_H
#define IMM_DP_EMISSION_H

struct dp_emission;
struct seq_code;
struct mstate;

struct dp_emission const* dp_emission_create(struct seq_code const*      seq_code,
                                             struct mstate const* const* mstates,
                                             unsigned                    nstates);
double dp_emission_cost(struct dp_emission const* emission, unsigned state, unsigned seq_code);
void   dp_emission_destroy(struct dp_emission const* emission);

#endif
