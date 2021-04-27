#include "dp/emission.h"
#include "common/common.h"
#include "imm/seq.h"
#include "seq_code.h"

struct chunk
{
    uint32_t score_size;
    imm_float *score;
    uint32_t offset_size;
    uint32_t *offset;
};

static inline unsigned offset_size(unsigned nstates) { return nstates + 1; }

static inline uint_fast32_t score_size(struct emission const *emission,
                                       unsigned nstates)
{
    return emission->offset[nstates];
}

struct emission const *emission_create(struct seq_code const *seq_code,
                                       struct imm_state **states,
                                       unsigned nstates)
{
    struct emission *tbl = xmalloc(sizeof(*tbl));

    tbl->offset = xmalloc(sizeof(*tbl->offset) * offset_size(nstates));
    tbl->offset[0] = 0;

    uint8_t min = imm_state_min_seq(states[0]);
    uint_fast32_t size = seq_code_size(seq_code, min);
    for (unsigned i = 1; i < nstates; ++i)
    {
        tbl->offset[i] = (uint32_t)size;
        size += seq_code_size(seq_code, imm_state_min_seq(states[i]));
    }
    tbl->offset[nstates] = (uint32_t)size;

    tbl->score = xmalloc(sizeof(*tbl->score) * score_size(tbl, nstates));

    struct imm_abc const *abc = seq_code->abc;
    char const *set = abc->symbols;
    uint_fast8_t set_size = abc->len;
    struct imm_cartes *cartes =
        imm_cartes_create(set, set_size, seq_code->max_seq);

    for (unsigned i = 0; i < nstates; ++i)
    {
        uint8_t min_seq = imm_state_min_seq(states[i]);
        for (uint8_t len = min_seq; len <= imm_state_max_seq(states[i]); ++len)
        {
            imm_cartes_setup(cartes, len);
            char const *item = NULL;
            while ((item = imm_cartes_next(cartes)) != NULL)
            {

                struct imm_seq seq = IMM_SEQ(len, item, abc);
                unsigned j = seq_code_encode(seq_code, &seq);
                j -= seq_code_offset(seq_code, min_seq);
                imm_float score = imm_state_lprob(states[i], &seq);
                tbl->score[tbl->offset[i] + j] = score;
            }
        }
    }

    imm_cartes_destroy(cartes);
    return tbl;
}

void emission_destroy(struct emission const *emission)
{
    free(emission->score);
    free(emission->offset);
    free((void *)emission);
}

#if 0
struct emission const* dp_emission_read(FILE* stream)
{
    struct chunk chunk = {.score_size = 0, .score = NULL, .offset_size = 0, .offset = NULL};

    struct emission* emission = xmalloc(sizeof(*emission));
    emission->offset = NULL;
    emission->score = NULL;

    if (fread(&chunk.score_size, sizeof(chunk.score_size), 1, stream) < 1) {
        error("could not read score_size");
        goto err;
    }

    chunk.score = xmalloc(sizeof(*chunk.score) * chunk.score_size);

    if (fread(chunk.score, sizeof(*chunk.score), chunk.score_size, stream) < chunk.score_size) {
        error("could not read score");
        goto err;
    }

    if (fread(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        error("could not read offset_size");
        goto err;
    }

    chunk.offset = xmalloc(sizeof(*chunk.offset) * chunk.offset_size);

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

int dp_emission_write(struct emission const* emission, unsigned nstates, FILE* stream)
{
    struct chunk chunk = {.score_size = (uint32_t)score_size(emission, nstates),
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

#endif
