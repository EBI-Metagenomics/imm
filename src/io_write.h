#ifndef IO_WRITE_H
#define IO_WRITE_H

#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct mstate;
struct dp_emission;
struct dp_trans;
struct dp_states;

int io_write_dp_emission(FILE* stream, struct dp_emission const* emission, uint32_t nstates);
int io_write_dp_trans(FILE* stream, struct dp_trans const* trans, uint32_t nstates,
                      unsigned ntrans);
int io_write_dp_states(FILE* stream, struct dp_states const* states);

#endif
