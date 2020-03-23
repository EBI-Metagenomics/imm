#include "dp/emission.h"
#include "cartes.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/seq.h"
#include "imm/seq_code.h"
#include "imm/state.h"
#include <stdlib.h>

struct dp_emission
{
    double*   cost;
    unsigned* offset;
};

struct dp_emission const* dp_emission_create(struct seq_code const*         seq_code,
                                             struct imm_state const* const* states,
                                             unsigned                       nstates)
{
    struct dp_emission* e = malloc(sizeof(struct dp_emission));

    e->offset = malloc(sizeof(unsigned) * nstates);

    e->offset[0] = 0;
    for (unsigned i = 1; i < nstates; ++i) {
        unsigned size = imm_seq_code_size(seq_code, imm_state_min_seq(states[i - 1]));
        e->offset[i] = e->offset[i - 1] + size;
    }

    unsigned size = imm_seq_code_size(seq_code, imm_state_min_seq(states[nstates - 1]));
    e->cost = malloc(sizeof(double) * (e->offset[nstates - 1] + size));

    for (unsigned i = 0; i < nstates; ++i) {

        struct imm_abc const* abc = imm_seq_code_abc(seq_code);
        unsigned              min_seq = imm_state_min_seq(states[i]);
        for (unsigned len = min_seq; len <= imm_state_max_seq(states[i]); ++len) {

            char const* set = imm_abc_symbols(abc);
            unsigned    set_size = imm_abc_length(abc);

            struct imm_cartes* cartes = imm_cartes_create(set, set_size, len);
            char const*        item = NULL;
            while ((item = imm_cartes_next(cartes)) != NULL) {

                struct imm_seq seq = IMM_SEQ(abc, item, len);
                unsigned       j = imm_seq_code_encode(seq_code, min_seq, &seq);
                e->cost[e->offset[i] + j] = imm_state_lprob(states[i], &seq);
            }
            imm_cartes_destroy(cartes);
        }
    }

    return e;
}

double dp_emission_cost(struct dp_emission const* emission, unsigned state, unsigned seq_code)
{
    return emission->cost[emission->offset[state] + seq_code];
}

void dp_emission_destroy(struct dp_emission const* emission)
{
    imm_free(emission->cost);
    imm_free(emission->offset);
    imm_free(emission);
}
