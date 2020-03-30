#include "dp2_emission.h"
#include "cartes.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/seq.h"
#include "imm/state.h"
#include "mstate.h"
#include "seq_code.h"
#include <stdlib.h>

struct dp2_emission const* dp2_emission_create(struct seq_code const*      seq_code,
                                               struct mstate const* const* mstates,
                                               unsigned                    nstates)
{
    struct dp2_emission* emiss_tbl = malloc(sizeof(struct dp2_emission));

    emiss_tbl->offset = malloc(sizeof(unsigned) * (nstates + 1));
    emiss_tbl->offset[0] = 0;

    unsigned size = seq_code_size(seq_code, imm_state_min_seq(mstate_get_state(mstates[0])));
    for (unsigned i = 1; i < nstates; ++i) {
        emiss_tbl->offset[i] = size;
        size += seq_code_size(seq_code, imm_state_min_seq(mstate_get_state(mstates[i])));
    }
    emiss_tbl->offset[nstates] = size;

    emiss_tbl->score = malloc(sizeof(double) * size);

    struct imm_abc const* abc = seq_code_abc(seq_code);
    char const*           set = imm_abc_symbols(abc);
    unsigned              set_size = imm_abc_length(abc);
    struct imm_cartes* cartes = imm_cartes_create(set, set_size, seq_code_max_seq(seq_code));

    for (unsigned i = 0; i < nstates; ++i) {

        unsigned min_seq = imm_state_min_seq(mstate_get_state(mstates[i]));
        for (unsigned len = min_seq; len <= imm_state_max_seq(mstate_get_state(mstates[i]));
             ++len) {

            imm_cartes_setup(cartes, len);
            char const* item = NULL;
            while ((item = imm_cartes_next(cartes)) != NULL) {

                struct imm_seq seq = IMM_SEQ(abc, item, len);
                unsigned       j = seq_code_encode(seq_code, &seq);
                j -= seq_code_offset(seq_code, min_seq);
                emiss_tbl->score[emiss_tbl->offset[i] + j] =
                    imm_state_lprob(mstate_get_state(mstates[i]), &seq);
            }
        }
    }

    imm_cartes_destroy(cartes);
    return emiss_tbl;
}

void dp2_emission_destroy(struct dp2_emission const* emission)
{
    imm_free(emission->score);
    imm_free(emission->offset);
    imm_free(emission);
}
