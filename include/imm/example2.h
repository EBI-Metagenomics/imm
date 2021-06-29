#ifndef IMM_EXAMPLE2_H
#define IMM_EXAMPLE2_H

#include "imm/abc.h"
#include "imm/codon_marg.h"
#include "imm/export.h"
#include "imm/nuclt_lprob.h"

#define IMM_EXAMPLE2_SIZE 1000U

IMM_API void imm_example2_init(void);
IMM_API void imm_example2_deinit(void);

struct imm_example2
{
    struct imm_dna const *dna;
    struct imm_hmm *hmm;
    struct imm_mute_state *start;
    struct imm_frame_state *b;
    struct imm_frame_state *j;
    struct imm_frame_state *m[IMM_EXAMPLE2_SIZE];
    struct imm_frame_state *i[IMM_EXAMPLE2_SIZE];
    struct imm_mute_state *d[IMM_EXAMPLE2_SIZE];
    struct imm_frame_state *e;
    struct imm_mute_state *end;
    struct imm_nuclt_lprob nucltp;
    struct imm_codon_marg m_marg;
    struct imm_codon_marg i_marg;
    struct imm_codon_marg b_marg;
    struct imm_codon_marg e_marg;
    struct imm_codon_marg j_marg;
    struct
    {
        struct imm_hmm *hmm;
        struct imm_frame_state *n;
        struct imm_codon_marg n_marg;
    } null;
};

IMM_API extern char const imm_example2_str[];
IMM_API extern struct imm_example2 imm_example2;

#endif
