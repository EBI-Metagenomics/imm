#ifndef HMM_H
#define HMM_H

#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_io;

struct imm_abc const* hmm_abc(struct imm_hmm const* hmm);
int                   hmm_write(struct imm_dp const* dp, FILE* stream);
int                   hmm_read(FILE* stream, struct imm_io* io);

#endif
