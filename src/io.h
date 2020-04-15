#ifndef IO_H
#define IO_H

struct imm_abc;
struct imm_hmm;

struct imm_io
{
    struct imm_abc const* abc;
    struct imm_hmm*       hmm;
};

#endif
