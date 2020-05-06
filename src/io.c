#include "imm/io.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "imm/abc.h"
#include "imm/abc_types.h"
#include "imm/dp.h"
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

static struct mstate** read_mstates(struct imm_io* io, FILE* stream);
static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct mstate* const* const mstates);
static int write(struct imm_io const* io, FILE* stream);
static int write_mstate(struct imm_io const* io, FILE* stream, struct mstate const* mstate);
static int write_mstates(struct imm_io const* io, FILE* stream, struct mstate const* const* mstates,
                         uint32_t nstates);

static struct imm_io_vtable const __vtable = {
    __imm_io_destroy,
    __imm_io_destroy_on_read_failure,
    __imm_io_read_state,
    write,
};

struct imm_abc const* imm_io_abc(struct imm_io const* io) { return io->abc; }

struct imm_io const* imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    return __imm_io_create(hmm, dp, NULL);
}

struct imm_io const* imm_io_create_from_file(FILE* stream)
{
    struct imm_io* io = __imm_io_new(NULL);
    if (__imm_io_read(io, stream)) {
        imm_error("failed to io-create from file");
        return NULL;
    }
    return io;
}

void imm_io_destroy(struct imm_io const* io) { io->vtable.destroy(io); }

struct imm_dp const* imm_io_dp(struct imm_io const* io) { return io->dp; }

struct imm_hmm* imm_io_hmm(struct imm_io const* io) { return io->hmm; }

uint32_t imm_io_nstates(struct imm_io const* io) { return io->nstates; }

struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i) { return io->states[i]; }

int imm_io_write(struct imm_io const* io, FILE* stream) { return io->vtable.write(io, stream); }

struct imm_io* __imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp, void* derived)
{
    struct imm_io* io = malloc(sizeof(*io));

    io->abc = hmm_abc(hmm);
    io->hmm = hmm;
    io->mstates = (struct mstate**)hmm_get_mstates(hmm, dp);

    io->nstates = dp_state_table_nstates(dp_get_state_table(dp));
    io->states = malloc(sizeof(*io->states) * io->nstates);
    for (uint32_t i = 0; i < io->nstates; ++i)
        io->states[i] = io->mstates[i]->state;

    io->seq_code = dp_get_seq_code(dp);
    io->emission = dp_get_emission(dp);
    io->trans_table = dp_get_trans_table(dp);
    io->state_table = dp_get_state_table(dp);
    io->dp = dp;

    io->vtable = __vtable;
    io->derived = derived;

    return io;
}

void* __imm_io_derived(struct imm_io const* io) { return io->derived; }

void __imm_io_destroy(struct imm_io const* io)
{
    free_c(io->states);
    free_c(io);
}

void __imm_io_destroy_on_read_failure(struct imm_io const* io)
{
    if (io->abc)
        imm_abc_destroy(io->abc);

    if (io->hmm)
        imm_hmm_destroy(io->hmm);

    if (io->mstates)
        free_c(io->mstates);

    if (io->states)
        free_c(io->states);

    if (io->seq_code)
        seq_code_destroy(io->seq_code);

    if (io->emission)
        dp_emission_destroy(io->emission);

    if (io->trans_table)
        dp_trans_table_destroy(io->trans_table);

    if (io->state_table)
        dp_state_table_destroy(io->state_table);

    if (io->dp)
        imm_dp_destroy(io->dp);

    free_c(io);
}

struct imm_io* __imm_io_new(void* derived)
{
    struct imm_io* io = malloc(sizeof(*io));
    io->abc = NULL;
    io->hmm = NULL;
    io->mstates = NULL;
    io->nstates = 0;
    io->states = NULL;
    io->seq_code = NULL;
    io->emission = NULL;
    io->trans_table = NULL;
    io->state_table = NULL;
    io->dp = NULL;

    io->vtable = __vtable;
    io->derived = derived;

    return io;
}

int __imm_io_read(struct imm_io* io, FILE* stream)
{
    if (__imm_io_read_abc(io, stream)) {
        imm_error("could not read abc");
        goto err;
    }

    if (__imm_io_read_hmm(io, stream)) {
        imm_error("could not read hmm");
        goto err;
    }

    if (__imm_io_read_dp(io, stream)) {
        imm_error("could not read dp");
        goto err;
    }

    __imm_dp_create_from_io(io);
    return 0;

err:
    io->vtable.destroy_on_read_failure(io);
    return 1;
}

int __imm_io_read_abc(struct imm_io* io, FILE* stream)
{
    uint8_t type_id = 0;
    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not read type_id");
        return 1;
    }

    if (type_id != IMM_ABC_TYPE_ID) {
        imm_error("unknown abc type_id");
        return 1;
    }

    io->abc = imm_abc_read(stream);
    if (!io->abc) {
        imm_error("could not read abc");
        return 1;
    }

    return 0;
}

int __imm_io_read_dp(struct imm_io* io, FILE* stream)
{
    if (!(io->seq_code = seq_code_read(stream, io->abc))) {
        imm_error("could not read seq_code");
        return 1;
    }

    if (!(io->emission = dp_emission_read(stream))) {
        imm_error("could not read dp_emission");
        return 1;
    }

    if (!(io->trans_table = dp_trans_table_read(stream))) {
        imm_error("could not read dp_trans_table");
        return 1;
    }

    if (!(io->state_table = dp_state_table_read(stream))) {
        imm_error("could not read dp_state_table");
        return 1;
    }

    return 0;
}

int __imm_io_read_hmm(struct imm_io* io, FILE* stream)
{
    struct imm_hmm* hmm = NULL;
    io->mstates = NULL;

    hmm = imm_hmm_create(io->abc);

    if (!(io->mstates = read_mstates(io, stream))) {
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
    if (hmm)
        imm_hmm_destroy(hmm);

    return 1;
}

struct imm_state const* __imm_io_read_state(struct imm_io const* io, FILE* stream, uint8_t type_id)
{
    struct imm_state const* state = NULL;

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = mute_state_read(stream, io->abc)))
            imm_error("could not read mute_state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = normal_state_read(stream, io->abc)))
            imm_error("could not read normal_state");
        break;
    default:
        imm_error("unknown state type_id");
    }

    return state;
}

void __imm_io_set_abc(struct imm_io* io, struct imm_abc const* abc) { io->abc = abc; }

struct imm_io_vtable* __imm_io_vtable(struct imm_io* io) { return &io->vtable; }

int __imm_io_write_abc(struct imm_io const* io, FILE* stream)
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

int __imm_io_write_dp(struct imm_io const* io, FILE* stream)
{
    if (seq_code_write(io->seq_code, stream)) {
        imm_error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(io->emission, io->nstates, stream)) {
        imm_error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(io->trans_table, io->nstates, stream)) {
        imm_error("could not write dp_trans_table");
        return 1;
    }

    if (dp_state_table_write(io->state_table, stream)) {
        imm_error("could not write dp_state_table");
        return 1;
    }

    return 0;
}

int __imm_io_write_hmm(struct imm_io const* io, FILE* stream)
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
            double   score = dp_trans_table_score(io->trans_table, tgt_state, trans);

            if (fwrite(&src_state, sizeof(src_state), 1, stream) < 1) {
                imm_error("could not write source_state");
                return 1;
            }

            if (fwrite(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
                imm_error("could not write target_state");
                return 1;
            }

            if (fwrite(&score, sizeof(score), 1, stream) < 1) {
                imm_error("could not write score");
                return 1;
            }
        }
    }

    return 0;
}

static struct mstate** read_mstates(struct imm_io* io, FILE* stream)
{
    struct mstate** mstates = NULL;

    if (fread(&io->nstates, sizeof(io->nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        goto err;
    }

    mstates = malloc(sizeof(*mstates) * (io->nstates));
    for (uint32_t i = 0; i < io->nstates; ++i)
        mstates[i] = NULL;

    for (uint32_t i = 0; i < io->nstates; ++i) {

        struct mstate_chunk chunk = {0., 0};

        if (fread(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1) {
            imm_error("could not read start_lprob");
            goto err;
        }

        if (fread(&chunk.type_id, sizeof(chunk.type_id), 1, stream) < 1) {
            imm_error("could not read type_id");
            goto err;
        }

        struct imm_state const* state = io->vtable.read_state(io, stream, chunk.type_id);
        if (!state) {
            imm_error("could not read state");
            goto err;
        }

        mstates[i] = mstate_create(state, chunk.start_lprob);
    }

    return mstates;

err:
    if (mstates) {
        for (uint32_t i = 0; i < io->nstates; ++i) {
            if (mstates[i])
                mstate_destroy(mstates[i]);
        }

        free_c(mstates);
    }

    return NULL;
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

static int write(struct imm_io const* io, FILE* stream)
{
    if (__imm_io_write_abc(io, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    if (__imm_io_write_hmm(io, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (__imm_io_write_dp(io, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

static int write_mstate(struct imm_io const* io, FILE* stream, struct mstate const* mstate)
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

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1) {
        imm_error("could not write nstates");
        return 1;
    }

    for (uint32_t i = 0; i < nstates; ++i) {
        if (write_mstate(io, stream, mstates[i])) {
            imm_error("could not write mstate");
            return 1;
        }
    }

    return 0;
}
