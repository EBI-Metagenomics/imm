#include "dp_emission.h"
#include "imm/imm.h"
#include "log.h"
#include "model_state.h"
#include "seq_code.h"
#include <stdlib.h>

struct dp_emission_chunk
{
    uint32_t   score_size;
    imm_float* score;
    uint32_t   offset_size;
    uint32_t*  offset;
};

static inline uint_fast16_t offset_size(uint_fast16_t nstates) { return nstates + 1; }
static inline uint_fast32_t score_size(struct dp_emission const* emission, uint_fast16_t nstates);

struct dp_emission const* dp_emission_create(struct seq_code const* seq_code, struct model_state const* const* mstates,
                                             uint_fast16_t nstates)
{
    struct dp_emission* emiss_tbl = malloc(sizeof(*emiss_tbl));

    emiss_tbl->offset = malloc(sizeof(*emiss_tbl->offset) * offset_size(nstates));
    emiss_tbl->offset[0] = 0;

    uint8_t       min = imm_state_min_seq(model_state_get_state(mstates[0]));
    uint_fast32_t size = seq_code_size(seq_code, min);
    for (uint_fast16_t i = 1; i < nstates; ++i) {
        emiss_tbl->offset[i] = (uint32_t)size;
        size += seq_code_size(seq_code, imm_state_min_seq(model_state_get_state(mstates[i])));
    }
    emiss_tbl->offset[nstates] = (uint32_t)size;

    emiss_tbl->score = malloc(sizeof(*emiss_tbl->score) * score_size(emiss_tbl, nstates));

    struct imm_abc const* abc = seq_code_abc(seq_code);
    char const*           set = imm_abc_symbols(abc);
    uint_fast8_t          set_size = imm_abc_length(abc);
    struct imm_cartes*    cartes = imm_cartes_create(set, set_size, seq_code_max_seq(seq_code));

    for (uint_fast16_t i = 0; i < nstates; ++i) {

        uint8_t min_seq = imm_state_min_seq(model_state_get_state(mstates[i]));
        for (uint8_t len = min_seq; len <= imm_state_max_seq(model_state_get_state(mstates[i])); ++len) {

            imm_cartes_setup(cartes, len);
            char const* item = NULL;
            while ((item = imm_cartes_next(cartes)) != NULL) {

                struct imm_seq seq = IMM_SEQ(abc, item, len);
                uint_fast16_t  j = seq_code_encode(seq_code, &seq);
                j -= seq_code_offset(seq_code, min_seq);
                imm_float score = (imm_float)imm_state_lprob(model_state_get_state(mstates[i]), &seq);
                emiss_tbl->score[emiss_tbl->offset[i] + j] = score;
            }
        }
    }

    imm_cartes_destroy(cartes);
    return emiss_tbl;
}

void dp_emission_destroy(struct dp_emission const* emission)
{
    free(emission->score);
    free(emission->offset);
    free((void*)emission);
}

void dp_emission_offsets_dump(struct dp_emission const* emission, uint_fast16_t nstates)
{
    printf("state,offset\n");
    for (uint_fast16_t i = 0; i < nstates; ++i) {
        printf("%" PRIu16 ",%" PRIu16 "\n", (uint16_t)i, (uint16_t)emission->offset[i]);
    }
}

struct dp_emission const* dp_emission_read(FILE* stream)
{
    struct dp_emission_chunk chunk = {.score_size = 0, .score = NULL, .offset_size = 0, .offset = NULL};

    struct dp_emission* emission = malloc(sizeof(*emission));
    emission->offset = NULL;
    emission->score = NULL;

    if (fread(&chunk.score_size, sizeof(chunk.score_size), 1, stream) < 1) {
        error("could not read score_size");
        goto err;
    }

    chunk.score = malloc(sizeof(*chunk.score) * chunk.score_size);

    if (fread(chunk.score, sizeof(*chunk.score), chunk.score_size, stream) < chunk.score_size) {
        error("could not read score");
        goto err;
    }

    if (fread(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        error("could not read offset_size");
        goto err;
    }

    chunk.offset = malloc(sizeof(*chunk.offset) * chunk.offset_size);

    if (fread(chunk.offset, sizeof(*chunk.offset), chunk.offset_size, stream) < chunk.offset_size) {
        error("could not read offset");
        goto err;
    }

    emission->offset = chunk.offset;
    emission->score = chunk.score;

    return emission;

err:
    if (chunk.score)
        free(chunk.score);

    if (chunk.offset)
        free(chunk.offset);

    free(emission);

    return NULL;
}

void dp_emission_scores_dump(struct dp_emission const* emission, uint_fast16_t nstates)
{
    printf("state,score\n");
    for (uint_fast16_t i = 0; i < nstates; ++i) {
        printf("%" PRIu16 ",%f\n", (uint16_t)i, emission->score[i]);
    }
}

int dp_emission_write(struct dp_emission const* emission, uint_fast16_t nstates, FILE* stream)
{
    struct dp_emission_chunk chunk = {.score_size = (uint32_t)score_size(emission, nstates),
                                      .score = emission->score,
                                      .offset_size = (uint16_t)offset_size(nstates),
                                      .offset = emission->offset};

    if (fwrite(&chunk.score_size, sizeof(chunk.score_size), 1, stream) < 1) {
        error("could not write score_size");
        return 1;
    }

    if (fwrite(chunk.score, sizeof(*chunk.score), score_size(emission, nstates), stream) <
        score_size(emission, nstates)) {
        error("could not write score");
        return 1;
    }

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        error("could not write offset_size");
        return 1;
    }

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(nstates), stream) < offset_size(nstates)) {
        error("could not write offset");
        return 1;
    }

    return 0;
}

static inline uint_fast32_t score_size(struct dp_emission const* emission, uint_fast16_t nstates)
{
    return emission->offset[nstates];
}
