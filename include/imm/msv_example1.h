#ifndef IMM_MSV_EXAMPLE1_H
#define IMM_MSV_EXAMPLE1_H

#include "imm/abc.h"
#include "imm/code.h"
#include "imm/export.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"

#define IMM_MSV_EXAMPLE1_SIZE 3000U
#define IMM_MSV_EXAMPLE1_NUCLT_ANY_SYMBOL '*'
#define IMM_MSV_EXAMPLE1_NUCLT_SYMBOLS "BMEJ"
#define IMM_MSV_EXAMPLE1_NUCLT_SIZE 4

IMM_API void imm_msv_example1_init(unsigned core_size);

struct imm_msv_example1
{
    struct imm_abc abc;
    struct imm_code code;
    struct imm_hmm hmm;
    unsigned core_size;
    struct imm_mute_state start;
    struct imm_normal_state b;
    struct imm_normal_state j;
    struct imm_normal_state m[IMM_MSV_EXAMPLE1_SIZE];
    struct imm_normal_state e;
    struct imm_mute_state end;
    struct
    {
        struct imm_hmm hmm;
        struct imm_normal_state n;
    } null;
};

IMM_API extern char const imm_msv_example1_seq1[];
IMM_API extern struct imm_msv_example1 imm_msv_example1;

IMM_API unsigned imm_msv_example1_state_name(unsigned id, char *name);

#endif
