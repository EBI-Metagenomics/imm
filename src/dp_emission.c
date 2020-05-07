#include "dp_emission.h"
#include "cartes.h"
#include "cast.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/seq.h"
#include "imm/state.h"
#include "io.h"
#include "mstate.h"
#include "seq_code.h"
#include <stdlib.h>

struct dp_emission_chunk
{
    uint32_t  score_size;
    double*   score;
    uint32_t  offset_size;
    uint32_t* offset;
};

static inline uint32_t offset_size(uint32_t nstates) { return nstates + 1; }
static inline unsigned score_size(struct dp_emission const* emission, uint32_t nstates);

struct dp_emission const* dp_emission_create(struct seq_code const*      seq_code,
                                             struct mstate const* const* mstates, uint32_t nstates)
{
    struct dp_emission* emiss_tbl = malloc(sizeof(*emiss_tbl));

    emiss_tbl->offset = malloc(sizeof(*emiss_tbl->offset) * offset_size(nstates));
    emiss_tbl->offset[0] = 0;

    unsigned size = seq_code_size(seq_code, imm_state_min_seq(mstate_get_state(mstates[0])));
    for (uint32_t i = 1; i < nstates; ++i) {
        emiss_tbl->offset[i] = size;
        size += seq_code_size(seq_code, imm_state_min_seq(mstate_get_state(mstates[i])));
    }
    emiss_tbl->offset[nstates] = cast_u_u32(size);

    emiss_tbl->score = malloc(sizeof(*emiss_tbl->score) * score_size(emiss_tbl, nstates));

    struct imm_abc const* abc = seq_code_abc(seq_code);
    char const*           set = imm_abc_symbols(abc);
    unsigned              set_size = imm_abc_length(abc);
    struct imm_cartes*    cartes = imm_cartes_create(set, set_size, seq_code_max_seq(seq_code));

    for (uint32_t i = 0; i < nstates; ++i) {

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

void dp_emission_destroy(struct dp_emission const* emission)
{
    free_c(emission->score);
    free_c(emission->offset);
    free_c(emission);
}

struct dp_emission const* dp_emission_read(FILE* stream)
{
    struct dp_emission_chunk chunk = {
        .score_size = 0, .score = NULL, .offset_size = 0, .offset = NULL};

    struct dp_emission* emission = malloc(sizeof(*emission));
    emission->offset = NULL;
    emission->score = NULL;

    if (fread(&chunk.score_size, sizeof(chunk.score_size), 1, stream) < 1) {
        imm_error("could not read score_size");
        goto err;
    }

    chunk.score = malloc(sizeof(*chunk.score) * chunk.score_size);

    if (fread(chunk.score, sizeof(*chunk.score), chunk.score_size, stream) < chunk.score_size) {
        imm_error("could not read score");
        goto err;
    }

    if (fread(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        imm_error("could not read offset_size");
        goto err;
    }

    chunk.offset = malloc(sizeof(*chunk.offset) * chunk.offset_size);

    if (fread(chunk.offset, sizeof(*chunk.offset), chunk.offset_size, stream) < chunk.offset_size) {
        imm_error("could not read offset");
        goto err;
    }

    emission->offset = chunk.offset;
    emission->score = chunk.score;

    return emission;

err:
    if (chunk.score)
        free_c(chunk.score);

    if (chunk.offset)
        free_c(chunk.offset);

    free_c(emission);

    return NULL;
}

int dp_emission_write(struct dp_emission const* emission, uint32_t nstates, FILE* stream)
{
    struct dp_emission_chunk chunk = {.score_size = score_size(emission, nstates),
                                      .score = emission->score,
                                      .offset_size = offset_size(nstates),
                                      .offset = emission->offset};

    if (fwrite(&chunk.score_size, sizeof(chunk.score_size), 1, stream) < 1) {
        imm_error("could not write score_size");
        return 1;
    }

    if (fwrite(chunk.score, sizeof(*chunk.score), score_size(emission, nstates), stream) <
        score_size(emission, nstates)) {
        imm_error("could not write score");
        return 1;
    }

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        imm_error("could not write offset_size");
        return 1;
    }

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(nstates), stream) <
        offset_size(nstates)) {
        imm_error("could not write offset");
        return 1;
    }

    return 0;
}

static inline unsigned score_size(struct dp_emission const* emission, uint32_t nstates)
{
    return emission->offset[nstates];
}
