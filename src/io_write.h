#ifndef IO_WRITE_H
#define IO_WRITE_H

#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct dp_emission;
struct dp_trans;
struct dp_states;
struct seq_code;
struct mstate;

int io_abc_write(FILE* stream, struct imm_abc const* abc);
int io_states_write(FILE* stream, struct mstate const* const* mstates, uint32_t nstates);
int io_state_write(FILE* stream, struct mstate const* mstate);
int io_seq_code_write(FILE* stream, struct seq_code const* seq_code);
int io_dp_emission_write(FILE* stream, struct dp_emission const* emission, unsigned nstates);
int io_dp_trans_write(FILE* stream, struct dp_trans const* trans, unsigned nstates,
                      unsigned ntrans);
int io_dp_states_write(FILE* stream, struct dp_states const* states);
int io_seq_code_write(FILE* stream, struct seq_code const* seq_code);

#endif
