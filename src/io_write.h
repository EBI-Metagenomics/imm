#ifndef IO_WRITE_H
#define IO_WRITE_H

#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct mstate;
struct seq_code;
struct dp_emission;
struct dp_trans;
struct dp_states;

int io_write_abc(FILE* stream, struct imm_abc const* abc);
int io_write_state(FILE* stream, struct mstate const* mstate);
int io_write_states(FILE* stream, struct mstate const* const* mstates, uint32_t nstates);
int io_write_seq_code(FILE* stream, struct seq_code const* seq_code);
int io_write_dp_emission(FILE* stream, struct dp_emission const* emission, uint32_t nstates);
int io_write_dp_trans(FILE* stream, struct dp_trans const* trans, uint32_t nstates,
                      unsigned ntrans);
int io_write_dp_states(FILE* stream, struct dp_states const* states);

#endif
