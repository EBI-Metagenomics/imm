#include "io_write.h"
#include "cast.h"
#include "dp_emission.h"
#include "dp_states.h"
#include "dp_trans.h"
#include "imm/abc.h"
#include "imm/state.h"
#include "mstate.h"
#include "seq_code.h"
#include <inttypes.h>
#include <string.h>

struct abc_chunk
{
    uint16_t    symbols_size;
    char const* symbols;
    char        any_symbol;
};

struct state_chunk
{
    uint8_t     state_type;
    uint16_t    name_size;
    char const* name;
    double      start_lprob;
    uint32_t    impl_chunk_size;
    char*       impl_chunk;
};

struct states_chunk
{
    uint32_t           nstates;
    struct state_chunk state_chunk;
};

struct seq_code_chunk
{
    uint32_t  chunk_size;
    uint8_t   min_seq;
    uint8_t   max_seq;
    uint32_t* offset;
    uint32_t* stride;
    uint32_t  size;
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

int io_write_state(FILE* stream, struct mstate const* mstate)
{
    struct imm_state const* state = mstate_get_state(mstate);

    struct state_chunk chunk = {.state_type = 0,
                                .name_size =
                                    (cast_u16_zu(strlen(imm_state_get_name(state))) + 1),
                                .name = imm_state_get_name(state),
                                .start_lprob = mstate_get_start(mstate),
                                .impl_chunk_size = 0,
                                .impl_chunk = NULL};

    if (fwrite(&chunk.state_type, sizeof(chunk.state_type), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.name_size, sizeof(chunk.name_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.name, sizeof(*chunk.name) * chunk.name_size, 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.impl_chunk_size, sizeof(chunk.impl_chunk_size), 1, stream) < 1)
        return 1;

    if (chunk.impl_chunk_size > 0) {
        if (fwrite(chunk.impl_chunk, sizeof(*chunk.impl_chunk) * chunk.impl_chunk_size, 1,
                   stream) < 1)
            return 1;
    }

    return 0;
}

int io_write_states(FILE* stream, struct mstate const* const* mstates, uint32_t nstates)
{
    struct states_chunk chunk = {.nstates = nstates};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
        return 1;

    for (uint32_t i = 0; i < nstates; ++i) {
        if (io_write_state(stream, mstates[i]))
            return 1;
    }

    return 0;
}

int io_write_seq_code(FILE* stream, struct seq_code const* seq_code)
{
    struct seq_code_chunk chunk = {.chunk_size = 0,
                                   .min_seq = seq_code->min_seq,
                                   .max_seq = seq_code->max_seq,
                                   .offset = seq_code->offset,
                                   .stride = seq_code->stride,
                                   .size = seq_code->size};

    chunk.chunk_size =
        (sizeof(chunk.chunk_size) + sizeof(chunk.min_seq) + sizeof(chunk.max_seq) +
         sizeof(*chunk.offset) * seq_code_offset_size(seq_code) +
         sizeof(*chunk.stride) * seq_code_stride_size(seq_code) + sizeof(chunk.size));

    if (fwrite(&chunk.chunk_size, sizeof(chunk.chunk_size), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.min_seq, sizeof(chunk.min_seq), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.max_seq, sizeof(chunk.max_seq), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.offset, sizeof(*chunk.offset) * seq_code_offset_size(seq_code), 1,
               stream) < 1)
        return 1;

    if (fwrite(chunk.stride, sizeof(*chunk.stride) * seq_code_stride_size(seq_code), 1,
               stream) < 1)
        return 1;

    if (fwrite(&chunk.size, sizeof(chunk.size), 1, stream) < 1)
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
