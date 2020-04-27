#include "imm/io.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "imm/abc.h"
#include "imm/abc_types.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "io.h"
#include "mstate.h"
#include "mstate_table.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "mute_state.h"
#include "normal_state.h"
#include "seq_code.h"
#include "state.h"
#include <stdlib.h>

struct mstate_chunk
{
    double  start_lprob;
    uint8_t type_id;
};

struct mstates_chunk
{
    uint32_t            nstates;
    struct mstate_chunk mstate_chunk;
};

static struct imm_abc const* read_abc(FILE* stream, uint8_t type_id);
static int                   read_abc_chunk(FILE* stream, struct imm_io* io);
static int                   read_hmm(FILE* stream, struct imm_io* io);
static struct mstate** read_mstates(FILE* stream, uint32_t* nstates, struct imm_abc const* abc);
static struct imm_state const* read_state(FILE* stream, uint8_t type_id, struct imm_abc const* abc);
static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct mstate* const* const mstates);
static int write_abc(struct imm_io const* io, FILE* stream);
static int write_dp(struct imm_io const* io, FILE* stream);
static int write_hmm(struct imm_io const* io, FILE* stream);
static int write_mstate(struct mstate const* mstate, struct imm_io const* io, FILE* stream);
static int write_mstates(struct imm_io const* io, FILE* stream, struct mstate const* const* mstates,
                         uint32_t nstates);

static struct imm_io_vtable const __vtable = {read_abc, NULL};

struct imm_io const* imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    return __imm_io_create_parent(hmm, dp, __vtable, NULL);
}

struct imm_io const* imm_io_create_from_file(FILE* stream)
{
    struct imm_io* io = malloc(sizeof(*io));
    io->abc = NULL;
    io->dp = NULL;
    io->emission = NULL;
    io->hmm = NULL;
    io->mstates = NULL;
    io->nstates = 0;
    io->seq_code = NULL;
    io->state_table = NULL;
    io->states = NULL;
    io->trans_table = NULL;

    io->vtable = __vtable;
    io->child = NULL;

    if (read_hmm(stream, io)) {
        imm_error("could not read hmm");
        goto err;
    }

    if (dp_read(stream, io)) {
        imm_error("could not read dp");
        goto err;
    }

    dp_create_from_io(io);

    return io;

err:
    if (io->abc)
        imm_abc_destroy(io->abc);

    if (io->hmm)
        imm_hmm_destroy(io->hmm);

    if (io->mstates)
        free_c(io->mstates);

    if (io->seq_code)
        seq_code_destroy(io->seq_code);

    if (io->emission)
        dp_emission_destroy(io->emission);

    if (io->trans_table)
        dp_trans_table_destroy(io->trans_table);

    free_c(io);
    return NULL;
}

void imm_io_destroy_states(struct imm_io const* io)
{
    for (uint32_t i = 0; i < imm_io_nstates(io); ++i)
        imm_state_destroy(imm_io_state(io, i));
}

void imm_io_destroy(struct imm_io const* io)
{
    if (io->vtable.destroy)
        io->vtable.destroy(io);
    __imm_io_destroy_parent(io);
}

int imm_io_write(struct imm_io const* io, FILE* stream)
{
    if (write_abc(io, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    if (write_hmm(io, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (write_dp(io, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i) { return io->states[i]; }

uint32_t imm_io_nstates(struct imm_io const* io) { return io->nstates; }

struct imm_abc const* imm_io_abc(struct imm_io const* io) { return io->abc; }

struct imm_hmm* imm_io_hmm(struct imm_io const* io) { return io->hmm; }

struct imm_dp const* imm_io_dp(struct imm_io const* io) { return io->dp; }

void* __imm_io_child(struct imm_io const* io) { return io->child; }

struct imm_io const* __imm_io_create_parent(struct imm_hmm* hmm, struct imm_dp const* dp,
                                            struct imm_io_vtable vtable, void* child)
{
    struct imm_io* io = malloc(sizeof(*io));

    io->abc = hmm_abc(hmm);
    io->hmm = hmm;
    io->mstates = hmm_get_mstates(hmm, dp);

    io->nstates = dp_state_table_nstates(dp_get_state_table(dp));
    io->states = malloc(sizeof(*io->states) * io->nstates);
    for (uint32_t i = 0; i < io->nstates; ++i)
        io->states[i] = io->mstates[i]->state;

    io->seq_code = dp_get_seq_code(dp);
    io->emission = dp_get_emission(dp);
    io->trans_table = dp_get_trans_table(dp);
    io->state_table = dp_get_state_table(dp);
    io->dp = dp;

    io->vtable = vtable;
    io->child = child;

    return io;
}

void __imm_io_destroy_parent(struct imm_io const* io)
{
    free_c(io->states);
    free_c(io);
}

static struct imm_abc const* read_abc(FILE* stream, uint8_t type_id)
{
    if (type_id != IMM_ABC_TYPE_ID) {
        imm_error("unknown abc type_id");
        return NULL;
    }

    struct imm_abc const* abc = imm_abc_read(stream);
    if (!abc) {
        imm_error("could not read abc");
        return abc;
    }
    return abc;
}

static int read_abc_chunk(FILE* stream, struct imm_io* io)
{
    uint8_t type_id = 0;
    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not read type_id");
        return 1;
    }

    io->abc = io->vtable.read_abc(stream, type_id);

    return !io->abc;
}

static int read_hmm(FILE* stream, struct imm_io* io)
{
    struct imm_hmm* hmm = NULL;
    io->mstates = NULL;

    if (read_abc_chunk(stream, io)) {
        imm_error("could not read abc");
        goto err;
    }

    hmm = imm_hmm_create(io->abc);

    if (!(io->mstates = read_mstates(stream, &io->nstates, io->abc))) {
        imm_error("could not read states");
        goto err;
    }

    io->states = malloc(sizeof(*io->states) * io->nstates);

    for (uint32_t i = 0; i < io->nstates; ++i) {
        io->states[i] = io->mstates[i]->state;
        hmm_add_mstate(hmm, io->mstates[i]);
    }

    if (read_transitions(stream, hmm, io->mstates)) {
        imm_error("could not read transitions");
        goto err;
    }

    io->hmm = hmm;

    return 0;

err:
    if (io->abc)
        imm_abc_destroy(io->abc);

    if (hmm)
        imm_hmm_destroy(hmm);

    return 1;
}

static struct mstate** read_mstates(FILE* stream, uint32_t* nstates, struct imm_abc const* abc)
{
    struct mstate** mstates = NULL;

    if (fread(nstates, sizeof(*nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        goto err;
    }

    mstates = malloc(sizeof(*mstates) * (*nstates));
    for (uint32_t i = 0; i < *nstates; ++i)
        mstates[i] = NULL;

    for (uint32_t i = 0; i < *nstates; ++i) {

        struct mstate_chunk chunk;

        if (fread(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1) {
            imm_error("could not read start_lprob");
            goto err;
        }

        if (fread(&chunk.type_id, sizeof(chunk.type_id), 1, stream) < 1) {
            imm_error("could not read type_id");
            goto err;
        }

        struct imm_state const* state = read_state(stream, chunk.type_id, abc);
        if (!state) {
            imm_error("could not read state");
            goto err;
        }

        mstates[i] = mstate_create(state, chunk.start_lprob);
    }

    return mstates;

err:
    if (mstates) {
        for (uint32_t i = 0; i < *nstates; ++i) {
            if (mstates[i])
                mstate_destroy(mstates[i]);
        }

        free_c(mstates);
    }

    return NULL;
}

static struct imm_state const* read_state(FILE* stream, uint8_t type_id, struct imm_abc const* abc)
{
    struct imm_state const* state = NULL;

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = mute_state_read(stream, abc)))
            imm_error("could not read mute_state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = normal_state_read(stream, abc)))
            imm_error("could not read normal_state");
        break;
    default:
        imm_error("unknown state type_id");
    }
    return state;
}

static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct mstate* const* const mstates)
{
    uint32_t ntrans = 0;

    if (fread(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not read ntransitions");
        return 1;
    }

    for (uint32_t i = 0; i < ntrans; ++i) {

        uint32_t src_state = 0;
        uint32_t tgt_state = 0;
        double   lprob = 0;

        if (fread(&src_state, sizeof(src_state), 1, stream) < 1) {
            imm_error("could not read source_state");
            return 1;
        }

        if (fread(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
            imm_error("could not read target_state");
            return 1;
        }

        if (fread(&lprob, sizeof(lprob), 1, stream) < 1) {
            imm_error("could not read lprob");
            return 1;
        }

        struct mtrans_table*    tbl = mstate_get_mtrans_table(mstates[src_state]);
        struct imm_state const* tgt = mstate_get_state(mstates[tgt_state]);
        mtrans_table_add(tbl, mtrans_create(tgt, lprob));
    }

    return 0;
}

static int write_abc(struct imm_io const* io, FILE* stream)
{
    uint8_t type_id = imm_abc_type_id(io->abc);
    if (fwrite(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not write type_id");
        return 1;
    }

    if (imm_abc_write(io->abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    return 0;
}

static int write_dp(struct imm_io const* io, FILE* stream)
{
    if (seq_code_write(io->seq_code, stream)) {
        imm_error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(io->emission, dp_state_table_nstates(io->state_table), stream)) {
        imm_error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(io->trans_table, dp_state_table_nstates(io->state_table), stream)) {
        imm_error("could not write dp_trans_table");
        return 1;
    }

    if (dp_state_table_write(io->state_table, stream)) {
        imm_error("could not write dp_state_table");
        return 1;
    }

    return 0;
}

static int write_hmm(struct imm_io const* io, FILE* stream)
{
    if (write_mstates(io, stream, (struct mstate const* const*)io->mstates, io->nstates)) {
        imm_error("could not write states");
        return 1;
    }

    uint32_t ntrans = dp_trans_table_total_ntrans(io->trans_table);
    if (fwrite(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not write ntrans");
        return 1;
    }

    for (uint32_t tgt_state = 0; tgt_state < io->nstates; ++tgt_state) {

        ntrans = dp_trans_table_ntrans(io->trans_table, tgt_state);
        for (uint32_t trans = 0; trans < ntrans; ++trans) {

            uint32_t src_state = dp_trans_table_source_state(io->trans_table, tgt_state, trans);
            double   lprob = dp_trans_table_score(io->trans_table, tgt_state, trans);

            if (fwrite(&src_state, sizeof(src_state), 1, stream) < 1) {
                imm_error("could not write source_state");
                return 1;
            }

            if (fwrite(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
                imm_error("could not write target_state");
                return 1;
            }

            if (fwrite(&lprob, sizeof(lprob), 1, stream) < 1) {
                imm_error("could not write lprob");
                return 1;
            }
        }
    }

    return 0;
}

static int write_mstate(struct mstate const* mstate, struct imm_io const* io, FILE* stream)
{
    struct imm_state const* state = mstate_get_state(mstate);

    struct mstate_chunk chunk = {mstate_get_start(mstate), imm_state_type_id(state)};

    if (fwrite(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1) {
        imm_error("could not write start_lprob");
        return 1;
    }

    if (fwrite(&chunk.type_id, sizeof(chunk.type_id), 1, stream) < 1) {
        imm_error("could not write type_id");
        return 1;
    }

    if (state_write(state, io, stream)) {
        imm_error("could not write state");
        return 1;
    }

    return 0;
}

static int write_mstates(struct imm_io const* io, FILE* stream, struct mstate const* const* mstates,
                         uint32_t nstates)
{
    struct mstates_chunk chunk = {.nstates = nstates};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
        return 1;

    for (uint32_t i = 0; i < nstates; ++i) {
        if (write_mstate(mstates[i], io, stream))
            return 1;
    }

    return 0;
}
