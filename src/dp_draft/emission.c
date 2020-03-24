#include "dp_draft/emission.h"
#include "cartes.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/seq.h"
#include "imm/seq_code.h"
#include "imm/state.h"
#include "mstate.h"
#include <stdlib.h>

/**
 * States are unsigned integers: 0, 1, ..., nstates-1.
 * Sequence code is an unsigned integer assigned to a particular sequence.
 * This assignment differs from state to state according to the size of
 * smallest sequence it can emit. Let min_seq(j) be that size for state j,
 * and let SC() be the sequence code function.
 * cost[offset[i] + SC(seq, min_seq(j))] will give the emission cost for
 * sequence seq.
 *
 */
struct dp_emission
{
    double*   cost; /**< Sequence emission cost of a state. */
    unsigned* offset; /**< Maps state to cost array offset. */
};

struct dp_emission const* dp_emission_create(struct seq_code const*      seq_code,
                                             struct mstate const* const* mstates,
                                             unsigned                    nstates)
{
    struct dp_emission* emiss_tbl = malloc(sizeof(struct dp_emission));

    emiss_tbl->offset = malloc(sizeof(unsigned) * (nstates + 1));
    emiss_tbl->offset[0] = 0;

    unsigned size =
        imm_seq_code_size(seq_code, imm_state_min_seq(mstate_get_state(mstates[0])));
    for (unsigned i = 1; i < nstates; ++i) {
        emiss_tbl->offset[i] = size;
        size += imm_seq_code_size(seq_code, imm_state_min_seq(mstate_get_state(mstates[i])));
    }
    emiss_tbl->offset[nstates] = size;

    emiss_tbl->cost = malloc(sizeof(double) * size);

    for (unsigned i = 0; i < nstates; ++i) {

        struct imm_abc const* abc = imm_seq_code_abc(seq_code);
        unsigned              min_seq = imm_state_min_seq(mstate_get_state(mstates[i]));
        for (unsigned len = min_seq; len <= imm_state_max_seq(mstate_get_state(mstates[i]));
             ++len) {

            char const* set = imm_abc_symbols(abc);
            unsigned    set_size = imm_abc_length(abc);

            struct imm_cartes* cartes = imm_cartes_create(set, set_size, len);
            char const*        item = NULL;
            while ((item = imm_cartes_next(cartes)) != NULL) {

                struct imm_seq seq = IMM_SEQ(abc, item, len);
                unsigned       j = imm_seq_code_encode(seq_code, min_seq, &seq);
                emiss_tbl->cost[emiss_tbl->offset[i] + j] =
                    imm_state_lprob(mstate_get_state(mstates[i]), &seq);
            }
            imm_cartes_destroy(cartes);
        }
    }

    return emiss_tbl;
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
