#ifndef IO_DP_H
#define IO_DP_H

#include <stdio.h>

struct dp_emission;
struct dp_trans;
struct dp_states;

int io_dp_emission_write(FILE* stream, struct dp_emission const* emission, unsigned nstates);
int io_dp_trans_write(FILE* stream, struct dp_trans const* trans, unsigned nstates,
                      unsigned ntrans);
int io_dp_states_write(FILE* stream, struct dp_states const* states);

#endif
