#ifndef HMM_H
#define HMM_H

#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_io;

int hmm_write(struct imm_hmm const* hmm, struct imm_dp const* dp, FILE* stream);
int hmm_read(FILE* stream, struct imm_io *io);

#endif
