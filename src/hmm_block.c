#include "imm/hmm_block.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "hmm_block.h"
#include "imm/dp.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "imm/table_state.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "seq_code.h"
#include <stdlib.h>

static struct mstate** read_mstates(struct imm_hmm_block* hmm_block, struct imm_abc const* abc,
                                    imm_hmm_block_read_state_cb read_state, void* read_state_args,
                                    FILE* stream);
static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct mstate* const* const mstates);
static int write_mstates(struct imm_hmm_block const* hmm_block, FILE* stream,
                         imm_hmm_block_write_state_cb write_state, void* write_state_args,
                         struct mstate const* const* mstates, uint32_t nstates);
static int write_mstate(struct imm_hmm_block const* hmm_block, FILE* stream,
                        imm_hmm_block_write_state_cb write_state, void* write_state_args,
                        struct mstate const* mstate);

struct imm_hmm_block* imm_hmm_block_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    return __imm_hmm_block_create(hmm, dp);
}
void imm_hmm_block_destroy(struct imm_hmm_block const* hmm_block)
{
    free_c(hmm_block->states);
    free_c(hmm_block);
}
struct imm_dp const* imm_hmm_block_dp(struct imm_hmm_block const* hmm_block)
{
    return hmm_block->dp;
}
struct imm_hmm* imm_hmm_block_hmm(struct imm_hmm_block const* hmm_block) { return hmm_block->hmm; }
uint32_t imm_hmm_block_nstates(struct imm_hmm_block const* hmm_block) { return hmm_block->nstates; }
struct imm_state const* imm_hmm_block_state(struct imm_hmm_block const* hmm_block, uint32_t i)
{
    return hmm_block->states[i];
}

struct imm_hmm_block* __imm_hmm_block_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    struct imm_hmm_block* hblk = malloc(sizeof(*hblk));

    hblk->hmm = hmm;
    hblk->mstates = (struct mstate**)hmm_get_mstates(hmm, dp);

    hblk->nstates = dp_state_table_nstates(dp_get_state_table(dp));
    hblk->states = malloc(sizeof(*hblk->states) * hblk->nstates);
    for (uint32_t i = 0; i < hblk->nstates; ++i)
        hblk->states[i] = hblk->mstates[i]->state;

    hblk->seq_code = dp_get_seq_code(dp);
    hblk->emission = dp_get_emission(dp);
    hblk->trans_table = dp_get_trans_table(dp);
    hblk->state_table = dp_get_state_table(dp);
    hblk->dp = dp;

    return hblk;
}

void __imm_hmm_block_deep_destroy(struct imm_hmm_block const* hmm_block)
{
    if (hmm_block->hmm)
        imm_hmm_destroy(hmm_block->hmm);

    if (hmm_block->mstates)
        free_c(hmm_block->mstates);

    if (hmm_block->states)
        free_c(hmm_block->states);

    if (hmm_block->seq_code)
        seq_code_destroy(hmm_block->seq_code);

    if (hmm_block->emission)
        dp_emission_destroy(hmm_block->emission);

    if (hmm_block->trans_table)
        dp_trans_table_destroy(hmm_block->trans_table);

    if (hmm_block->state_table)
        dp_state_table_destroy(hmm_block->state_table);

    if (hmm_block->dp)
        imm_dp_destroy(hmm_block->dp);

    free_c(hmm_block);
}

struct imm_hmm_block* __imm_hmm_block_new(void)
{
    struct imm_hmm_block* hmm_block = malloc(sizeof(*hmm_block));
    hmm_block->hmm = NULL;
    hmm_block->mstates = NULL;
    hmm_block->nstates = 0;
    hmm_block->states = NULL;
    hmm_block->seq_code = NULL;
    hmm_block->emission = NULL;
    hmm_block->trans_table = NULL;
    hmm_block->state_table = NULL;
    hmm_block->dp = NULL;
    return hmm_block;
}

int __imm_hmm_block_read_dp(struct imm_hmm_block* hmm_block, struct imm_abc const* abc,
                            FILE* stream)
{
    if (!(hmm_block->seq_code = seq_code_read(stream, abc))) {
        imm_error("could not read seq_code");
        return 1;
    }

    if (!(hmm_block->emission = dp_emission_read(stream))) {
        imm_error("could not read dp_emission");
        return 1;
    }

    if (!(hmm_block->trans_table = dp_trans_table_read(stream))) {
        imm_error("could not read dp_trans_table");
        return 1;
    }

    if (!(hmm_block->state_table = dp_state_table_read(stream))) {
        imm_error("could not read dp_state_table");
        return 1;
    }

    dp_create_from_hmm_block(hmm_block);

    return 0;
}

int __imm_hmm_block_read_hmm(struct imm_hmm_block* hmm_block, struct imm_abc const* abc,
                             imm_hmm_block_read_state_cb read_state, void* read_state_args,
                             FILE* stream)
{
    struct imm_hmm* hmm = NULL;
    hmm_block->mstates = NULL;

    hmm = imm_hmm_create(abc);

    if (!(hmm_block->mstates = read_mstates(hmm_block, abc, read_state, read_state_args, stream))) {
        imm_error("could not read states");
        goto err;
    }

    hmm_block->states = malloc(sizeof(*hmm_block->states) * hmm_block->nstates);

    for (uint32_t i = 0; i < hmm_block->nstates; ++i) {
        hmm_block->states[i] = hmm_block->mstates[i]->state;
        hmm_add_mstate(hmm, hmm_block->mstates[i]);
    }

    if (read_transitions(stream, hmm, hmm_block->mstates)) {
        imm_error("could not read transitions");
        goto err;
    }

    hmm_block->hmm = hmm;

    return 0;

err:
    if (hmm)
        imm_hmm_destroy(hmm);

    return 1;
}

struct imm_state const* __imm_hmm_block_read_state(struct imm_hmm_block const* hmm_block,
                                                   struct imm_abc const* abc, FILE* stream,
                                                   void* args)
{
    struct imm_state const* state = NULL;
    uint8_t                 type_id = 0;

    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not read type id");
        return NULL;
    }

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = imm_mute_state_read(stream, abc)))
            imm_error("could not read mute state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = imm_normal_state_read(stream, abc)))
            imm_error("could not read normal state");
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        if (!(state = imm_table_state_read(stream, abc)))
            imm_error("could not read table state");
        break;
    default:
        imm_error("unknown state type id");
    }

    return state;
}

int __imm_hmm_block_write_dp(struct imm_hmm_block const* hmm_block, FILE* stream)
{
    if (seq_code_write(hmm_block->seq_code, stream)) {
        imm_error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(hmm_block->emission, hmm_block->nstates, stream)) {
        imm_error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(hmm_block->trans_table, hmm_block->nstates, stream)) {
        imm_error("could not write dp_trans_table");
        return 1;
    }

    if (dp_state_table_write(hmm_block->state_table, stream)) {
        imm_error("could not write dp_state_table");
        return 1;
    }

    return 0;
}

int __imm_hmm_block_write_hmm(struct imm_hmm_block const* hmm_block, FILE* stream,
                              imm_hmm_block_write_state_cb write_state, void* write_state_args)
{
    if (write_mstates(hmm_block, stream, write_state, write_state_args,
                      (struct mstate const* const*)hmm_block->mstates, hmm_block->nstates)) {
        imm_error("could not write states");
        return 1;
    }

    uint32_t ntrans = dp_trans_table_total_ntrans(hmm_block->trans_table);
    if (fwrite(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not write ntrans");
        return 1;
    }

    for (uint32_t tgt_state = 0; tgt_state < hmm_block->nstates; ++tgt_state) {

        ntrans = dp_trans_table_ntrans(hmm_block->trans_table, tgt_state);
        for (uint32_t trans = 0; trans < ntrans; ++trans) {

            uint32_t src_state =
                dp_trans_table_source_state(hmm_block->trans_table, tgt_state, trans);
            double score = dp_trans_table_score(hmm_block->trans_table, tgt_state, trans);

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

int __imm_hmm_block_write_state(struct imm_hmm_block const* hmm_block, FILE* stream,
                                struct imm_state const* state, void* args)
{
    uint8_t type_id = imm_state_type_id(state);
    if (fwrite(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not write state type id");
        return 1;
    }

    int errno = 0;
    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        errno = imm_mute_state_write(state, hmm_block, stream);
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        errno = imm_normal_state_write(state, hmm_block, stream);
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        errno = imm_table_state_write(state, hmm_block, stream);
        break;
    default:
        imm_error("unknown state type id");
        errno = 1;
    }
    return errno;
}

static struct mstate** read_mstates(struct imm_hmm_block* hmm_block, struct imm_abc const* abc,
                                    imm_hmm_block_read_state_cb read_state, void* read_state_args,
                                    FILE* stream)
{
    struct mstate** mstates = NULL;

    if (fread(&hmm_block->nstates, sizeof(hmm_block->nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        goto err;
    }

    mstates = malloc(sizeof(*mstates) * (hmm_block->nstates));
    for (uint32_t i = 0; i < hmm_block->nstates; ++i)
        mstates[i] = NULL;

    for (uint32_t i = 0; i < hmm_block->nstates; ++i) {

        double start_lprob = 0.;
        if (fread(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
            imm_error("could not read start lprob");
            goto err;
        }

        struct imm_state const* state = read_state(hmm_block, abc, stream, read_state_args);
        if (!state) {
            imm_error("could not read state");
            goto err;
        }

        mstates[i] = mstate_create(state, start_lprob);
    }

    return mstates;

err:
    if (mstates) {
        for (uint32_t i = 0; i < hmm_block->nstates; ++i) {
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

static int write_mstates(struct imm_hmm_block const* hmm_block, FILE* stream,
                         imm_hmm_block_write_state_cb write_state, void* write_state_args,
                         struct mstate const* const* mstates, uint32_t nstates)
{
    if (fwrite(&nstates, sizeof(nstates), 1, stream) < 1) {
        imm_error("could not write nstates");
        return 1;
    }

    for (uint32_t i = 0; i < nstates; ++i) {
        if (write_mstate(hmm_block, stream, write_state, write_state_args, mstates[i])) {
            imm_error("could not write mstate");
            return 1;
        }
    }

    return 0;
}

static int write_mstate(struct imm_hmm_block const* hmm_block, FILE* stream,
                        imm_hmm_block_write_state_cb write_state, void* write_state_args,
                        struct mstate const* mstate)
{
    struct imm_state const* state = mstate_get_state(mstate);

    double start_lprob = mstate_get_start(mstate);

    if (fwrite(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
        imm_error("could not write start_lprob");
        return 1;
    }

    if (write_state(hmm_block, stream, state, write_state_args)) {
        imm_error("could not write state");
        return 1;
    }

    return 0;
}
