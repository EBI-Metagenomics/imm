#ifndef TEST_MODEL_FRAME_H
#define TEST_MODEL_FRAME_H

#include "imm/imm.h"

#define MODEL_FRAME_SIZE 1000U

void model_frame_init(void);
void model_frame_deinit(void);

struct model_frame
{
    struct imm_dna const *dna;
    struct imm_hmm *hmm;
    struct imm_mute_state *start;
    struct imm_frame_state *b;
    struct imm_frame_state *j;
    struct imm_frame_state *m[MODEL_FRAME_SIZE];
    struct imm_frame_state *i[MODEL_FRAME_SIZE];
    struct imm_mute_state *d[MODEL_FRAME_SIZE];
    struct imm_frame_state *e;
    struct imm_mute_state *end;
    struct imm_nuclt_lprob nucltp;
    struct imm_codon_marg m_marg;
    struct imm_codon_marg i_marg;
    struct imm_codon_marg b_marg;
    struct imm_codon_marg e_marg;
    struct imm_codon_marg j_marg;
};

extern char const model_frame_str[];
extern struct model_frame model_frame;

#endif
