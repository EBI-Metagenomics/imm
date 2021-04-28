#include "dp/emission.h"
#include "cartes.h"
#include "common/common.h"
#include "dp/seq_code.h"
#include "imm/abc.h"
#include "imm/seq.h"
#include "imm/state.h"

static inline unsigned offset_size(unsigned nstates) { return nstates + 1; }

static inline unsigned score_size(struct emission const *emission,
                                  unsigned nstates)
{
    return emission->offset[nstates];
}

void emission_del(struct emission const *emission)
{
    free(emission->score);
    free(emission->offset);
    free((void *)emission);
}

struct emission const *emission_new(struct seq_code const *seq_code,
                                    struct imm_state **states, unsigned nstates)
{
    struct emission *tbl = xmalloc(sizeof(*tbl));

    tbl->offset = xmalloc(sizeof(*tbl->offset) * offset_size(nstates));
    tbl->offset[0] = 0;

    unsigned min = imm_state_min_seqlen(states[0]);
    unsigned size = seq_code_size(seq_code, min);
    for (unsigned i = 1; i < nstates; ++i)
    {
        tbl->offset[i] = (uint32_t)size;
        size += seq_code_size(seq_code, imm_state_min_seqlen(states[i]));
    }
    tbl->offset[nstates] = (uint32_t)size;

    tbl->score = xmalloc(sizeof(*tbl->score) * score_size(tbl, nstates));

    struct imm_abc const *abc = seq_code->abc;
    char const *set = abc->symbols;
    unsigned set_size = abc->nsymbols;
    struct cartes *cartes = cartes_new(set, set_size, seq_code->seqlen.max);

    for (unsigned i = 0; i < nstates; ++i)
    {
        unsigned min_seq = imm_state_min_seqlen(states[i]);
        for (unsigned len = min_seq; len <= imm_state_max_seqlen(states[i]);
             ++len)
        {
            cartes_setup(cartes, len);
            char const *item = NULL;
            while ((item = cartes_next(cartes)) != NULL)
            {

                struct imm_seq seq = IMM_SEQ(len, item, abc);
                unsigned j = seq_code_encode(seq_code, &seq);
                j -= seq_code_offset(seq_code, min_seq);
                imm_float score = imm_state_lprob(states[i], &seq);
                tbl->score[tbl->offset[i] + j] = score;
            }
        }
    }

    cartes_del(cartes);
    return tbl;
}
