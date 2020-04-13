#include "io_write.h"
#include "cast.h"
#include "dp_emission.h"
#include "dp_states.h"
#include "dp_trans.h"
#include "imm/abc.h"
#include "imm/state.h"
#include "mstate.h"
#include <inttypes.h>
#include <string.h>

struct abc_chunk
{
    uint16_t    symbols_size;
    char const* symbols;
    char        any_symbol;
};

struct dp_emission_chunk
{
    uint32_t  score_size;
    double*   score;
    uint32_t  offset_size;
    uint32_t* offset;
};

struct dp_trans_chunk
{
    uint32_t  score_size;
    double*   score;
    uint32_t  source_state_size;
    uint32_t* source_state;
    uint32_t  offset_size;
    uint32_t* offset;
};

struct dp_states_chunk
{
    uint32_t nstates;
    uint8_t* min_seq;
    uint8_t* max_seq;
    double*  start_lprob;
    uint32_t end_state;
};

int io_write_abc(FILE* stream, struct imm_abc const* abc)
{
    struct abc_chunk chunk = {.symbols_size = cast_u16_zu(strlen(abc->symbols) + 1),
                              .symbols = abc->symbols,
                              .any_symbol = abc->any_symbol};

    if (fwrite(&chunk.symbols_size, sizeof(chunk.symbols_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.symbols, sizeof(*chunk.symbols) * chunk.symbols_size, 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1)
        return 1;

    return 0;
}

int io_write_dp_emission(FILE* stream, struct dp_emission const* emission, uint32_t nstates)
{
    struct dp_emission_chunk chunk = {.score_size = dp_emission_score_size(emission, nstates),
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

int io_write_dp_trans(FILE* stream, struct dp_trans const* trans, uint32_t nstates,
                      unsigned ntrans)
{
    struct dp_trans_chunk chunk = {.score_size = ntrans,
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

int io_write_dp_states(FILE* stream, struct dp_states const* states)
{
    struct dp_states_chunk chunk = {.nstates = states->nstates,
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
