#ifndef IMM_EXAMPLE1_H
#define IMM_EXAMPLE1_H

#include "imm/abc.h"
#include "imm/code.h"
#include "imm/export.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"

#define IMM_EXAMPLE1_SIZE 1000U

IMM_API void imm_example1_init(void);

struct imm_example1
{
    struct imm_abc abc;
    struct imm_code code;
    struct imm_hmm hmm;
    struct imm_mute_state start;
    struct imm_normal_state b;
    struct imm_normal_state j;
    struct imm_normal_state m[IMM_EXAMPLE1_SIZE];
    struct imm_normal_state i[IMM_EXAMPLE1_SIZE];
    struct imm_mute_state d[IMM_EXAMPLE1_SIZE];
    struct imm_normal_state e;
    struct imm_mute_state end;
    struct
    {
        struct imm_hmm hmm;
        struct imm_normal_state n;
    } null;
};

IMM_API extern char const imm_example1_seq[];
IMM_API extern struct imm_example1 imm_example1;

#endif
