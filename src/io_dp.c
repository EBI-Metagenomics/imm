#include "io_dp.h"
#include "dp_emission.h"
#include "dp_states.h"
#include "dp_trans.h"
#include <inttypes.h>

struct io_dp_emission_chunk
{
    uint32_t  score_size;
    double*   score;
    uint32_t  offset_size;
    uint32_t* offset;
};

struct io_dp_trans_chunk
{
    uint32_t  score_size;
    double*   score;
    uint32_t  source_state_size;
    uint32_t* source_state;
    uint32_t  offset_size;
    uint32_t* offset;
};

struct io_dp_states_chunk
{
    uint32_t nstates;
    uint8_t* min_seq;
    uint8_t* max_seq;
    double*  start_lprob;
    uint32_t end_state;
};

struct io_dp_matrix_chunk
{
    struct dp_states const* states;
    struct matrixd*         score;
    struct step_matrix*     prev_step;
    int*                    state_col;
    unsigned                nstates;
};

int io_dp_emission_write(FILE* stream, struct dp_emission const* emission, unsigned nstates)
{
    struct io_dp_emission_chunk chunk = {.score_size =
                                             dp_emission_score_size(emission, nstates),
                                         .score = emission->score,
                                         .offset_size = dp_emission_offset_size(nstates),
                                         .offset = emission->offset};

    if (fwrite(&chunk.score_size, sizeof(chunk.score_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.score, sizeof(*chunk.score) * dp_emission_score_size(emission, nstates),
               1, stream) < 1)
        return 1;

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.offset, sizeof(*chunk.offset) * dp_emission_offset_size(nstates), 1,
               stream) < 1)
        return 1;

    return 0;
}

int io_dp_trans_write(FILE* stream, struct dp_trans const* trans, unsigned nstates,
                      unsigned ntrans)
{
    struct io_dp_trans_chunk chunk = {.score_size = ntrans,
                                      .score = trans->score,
                                      .source_state_size = ntrans,
                                      .source_state = trans->source_state,
                                      .offset_size = nstates + 1,
                                      .offset = trans->offset};

    if (fwrite(&chunk.score_size, sizeof(chunk.score_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.score, sizeof(*chunk.score) * dp_trans_score_size(ntrans), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.source_state_size, sizeof(chunk.source_state_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.source_state,
               sizeof(*chunk.source_state) * dp_trans_source_state_size(ntrans), 1,
               stream) < 1)
        return 1;

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.offset, sizeof(*chunk.offset) * dp_trans_offset_size(nstates), 1,
               stream) < 1)
        return 1;

    return 0;
}

int io_dp_states_write(FILE* stream, struct dp_states const* states)
{
    struct io_dp_states_chunk chunk = {.nstates = states->nstates,
                                       .min_seq = states->min_seq,
                                       .max_seq = states->max_seq,
                                       .start_lprob = states->start_lprob,
                                       .end_state = states->end_state};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.min_seq, sizeof(*chunk.min_seq) * states->nstates, 1, stream) < 1)
        return 1;

    if (fwrite(chunk.max_seq, sizeof(*chunk.max_seq) * states->nstates, 1, stream) < 1)
        return 1;

    if (fwrite(chunk.start_lprob, sizeof(*chunk.start_lprob) * states->nstates, 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.end_state, sizeof(chunk.end_state), 1, stream) < 1)
        return 1;

    return 0;
}
