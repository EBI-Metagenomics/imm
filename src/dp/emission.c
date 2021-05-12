#include "dp/emission.h"
#include "dp/code.h"
#include "imm/abc.h"
#include "imm/cartes.h"
#include "imm/state.h"
#include "support.h"

static inline unsigned offset_size(unsigned nstates) { return nstates + 1; }

static inline unsigned score_size(struct emission const *emission,
                                  unsigned nstates)
{
    return emission->offset[nstates];
}

void emission_deinit(struct emission const *emission)
{
    free(emission->score);
    free(emission->offset);
    imm_cartes_deinit(&emission->cartes);
}

void emission_init(struct emission *emiss, struct code const *code,
                   struct imm_state **states, unsigned nstates)
{
    emiss->capacity.score = 0;
    emiss->capacity.offset = 0;
    emiss->score = NULL;
    emiss->offset = NULL;
    emission_reset(emiss, code, states, nstates);
}

void emission_reset(struct emission *emiss, struct code const *code,
                    struct imm_state **states, unsigned nstates)
{
    size_t offset_capacity = sizeof(*emiss->offset) * offset_size(nstates);
    if (offset_capacity > emiss->capacity.offset)
    {
        emiss->offset = xrealloc(emiss->offset, offset_capacity);
        emiss->capacity.offset = offset_capacity;
    }
    emiss->offset[0] = 0;

    unsigned min = imm_state_span(states[0]).min;
    unsigned size = code_size(code, min);
    for (unsigned i = 1; i < nstates; ++i)
    {
        emiss->offset[i] = size;
        size += code_size(code, imm_state_span(states[i]).min);
    }
    emiss->offset[nstates] = size;

    size_t score_capacity = sizeof(*emiss->score) * score_size(emiss, nstates);
    if (score_capacity > emiss->capacity.score)
    {
        emiss->score = xrealloc(emiss->score, score_capacity);
        emiss->capacity.score = score_capacity;
    }

    struct imm_abc const *abc = code->abc;
    char const *set = abc->symbols;
    unsigned set_size = abc->size;
    imm_cartes_init(&emiss->cartes, set, set_size, code->seqlen.max);

    for (unsigned i = 0; i < nstates; ++i)
    {
        unsigned min_seq = imm_state_span(states[i]).min;
        for (unsigned len = min_seq; len <= imm_state_span(states[i]).max;
             ++len)
        {
            imm_cartes_setup(&emiss->cartes, len);
            char const *item = NULL;
            while ((item = imm_cartes_next(&emiss->cartes)) != NULL)
            {

                struct imm_seq seq = IMM_SEQ_UNSAFE(len, item, abc);
                unsigned j = code_encode(code, &seq);
                j -= code_offset(code, min_seq);
                imm_float score = imm_state_lprob(states[i], &seq);
                emiss->score[emiss->offset[i] + j] = score;
            }
        }
    }
}
