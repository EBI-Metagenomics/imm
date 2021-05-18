#include "dp/emission.h"
#include "dp/code.h"
#include "imm/abc.h"
#include "imm/cartes.h"
#include "imm/state.h"
#include "support.h"

void emission_del(struct emission const *emission)
{
    free(emission->score);
    free(emission->offset);
}

void emission_init(struct emission *emiss)
{
    emiss->score = NULL;
    emiss->offset = NULL;
}

void emission_reset(struct emission *emiss, struct code const *code,
                    struct imm_state **states, unsigned nstates)
{
    emiss->offset = xrealloc(emiss->offset, sizeof(*emiss->offset) *
                                                emission_offset_size(nstates));
    emiss->offset[0] = 0;

    unsigned min = imm_state_span(states[0]).min;
    unsigned size = code_size(code, min);
    for (unsigned i = 1; i < nstates; ++i)
    {
        emiss->offset[i] = size;
        size += code_size(code, imm_state_span(states[i]).min);
    }
    emiss->offset[nstates] = size;

    emiss->score =
        xrealloc(emiss->score,
                 sizeof(*emiss->score) * emission_score_size(emiss, nstates));

    struct imm_abc const *abc = code->abc;
    char const *set = abc->symbols;
    unsigned set_size = abc->size;
    struct imm_cartes cartes;
    /* TODO: consider to avoid initing this everytime */
    imm_cartes_init(&cartes, set, set_size, code->seqlen.max);

    for (unsigned i = 0; i < nstates; ++i)
    {
        unsigned min_seq = imm_state_span(states[i]).min;
        for (unsigned len = min_seq; len <= imm_state_span(states[i]).max;
             ++len)
        {
            imm_cartes_setup(&cartes, len);
            char const *item = NULL;
            while ((item = imm_cartes_next(&cartes)) != NULL)
            {

                struct imm_seq seq = IMM_SEQ_UNSAFE(len, item, abc);
                unsigned j = code_encode(code, &seq);
                j -= code_offset(code, min_seq);
                imm_float score = imm_state_lprob(states[i], &seq);
                emiss->score[emiss->offset[i] + j] = score;
            }
        }
    }
    imm_cartes_deinit(&cartes);
}
