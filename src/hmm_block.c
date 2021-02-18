#include "hmm_block.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "imm/imm.h"
#include "profile.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "seq_code.h"
#include <stdlib.h>

static int                  write_dp(struct imm_profile const* model, struct imm_hmm_block const* block, FILE* stream);
static int                  write_hmm(struct imm_profile const* model, struct imm_hmm_block const* block, FILE* stream);
static int                  read_dp(struct imm_profile* model, struct imm_hmm_block* block, FILE* stream);
static int                  read_hmm(struct imm_profile* model, struct imm_hmm_block* block, FILE* stream);
static struct model_state** read_mstates(struct imm_profile* model, struct imm_hmm_block* block, FILE* stream);
static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct model_state* const* const mstates);

struct imm_dp const* imm_hmm_block_dp(struct imm_hmm_block const* block) { return block->dp; }

struct imm_hmm* imm_hmm_block_hmm(struct imm_hmm_block const* block) { return block->hmm; }

uint16_t imm_hmm_block_nstates(struct imm_hmm_block const* block) { return block->nstates; }

struct imm_state const* imm_hmm_block_state(struct imm_hmm_block const* block, uint16_t i) { return block->states[i]; }

struct imm_hmm_block* hmm_block_new(void)
{
    struct imm_hmm_block* block = malloc(sizeof(*block));
    block->hmm = NULL;
    block->mstates = NULL;
    block->nstates = 0;
    block->states = NULL;
    block->seq_code = NULL;
    block->emission = NULL;
    block->trans_table = NULL;
    block->state_table = NULL;
    block->dp = NULL;
    return block;
}

struct imm_hmm_block* imm_hmm_block_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    struct imm_hmm_block* block = hmm_block_new();
    block->hmm = hmm;
    block->mstates = (struct model_state**)hmm_get_mstates(hmm, dp);

    block->nstates = (uint16_t)dp_state_table_nstates(dp_get_state_table(dp));
    block->states = malloc(sizeof(*block->states) * block->nstates);
    for (uint16_t i = 0; i < block->nstates; ++i)
        block->states[i] = block->mstates[i]->state;

    block->seq_code = dp_get_seq_code(dp);
    block->emission = dp_get_emission(dp);
    block->trans_table = dp_get_trans_table(dp);
    block->state_table = dp_get_state_table(dp);
    block->dp = dp;
    return block;
}

void hmm_block_destroy(struct imm_hmm_block const* block)
{
    free_c(block->states);
    free_c(block);
}

void hmm_block_deep_destroy(struct imm_hmm_block const* block)
{
    if (block->hmm)
        imm_hmm_destroy(block->hmm);

    if (block->mstates)
        free_c(block->mstates);

    if (block->states)
        free_c(block->states);

    if (block->seq_code)
        seq_code_destroy(block->seq_code);

    if (block->emission)
        dp_emission_destroy(block->emission);

    if (block->trans_table)
        dp_trans_table_destroy(block->trans_table);

    if (block->state_table)
        dp_state_table_destroy(block->state_table);

    if (block->dp)
        imm_dp_destroy(block->dp);
}

struct imm_hmm_block const* hmm_block_read(struct imm_profile* model, FILE* stream)
{
    struct imm_hmm_block* block = hmm_block_new();

    if (read_hmm(model, block, stream)) {
        imm_error("could not read hmm");
        goto err;
    }

    if (read_dp(model, block, stream)) {
        imm_error("could not read dp");
        goto err;
    }
    return block;

err:
    hmm_block_deep_destroy(block);
    return NULL;
}

int hmm_block_write(struct imm_profile const* model, struct imm_hmm_block const* block, FILE* stream)
{
    if (write_hmm(model, block, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (write_dp(model, block, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

static int write_dp(struct imm_profile const* model, struct imm_hmm_block const* block, FILE* stream)
{
    if (seq_code_write(block->seq_code, stream)) {
        imm_error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(block->emission, block->nstates, stream)) {
        imm_error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(block->trans_table, block->nstates, stream)) {
        imm_error("could not write dp_trans_table");
        return 1;
    }

    if (dp_state_table_write(block->state_table, stream)) {
        imm_error("could not write dp_state_table");
        return 1;
    }

    return 0;
}

int write_hmm(struct imm_profile const* model, struct imm_hmm_block const* block, FILE* stream)
{
    if (write_mstates(model, stream, (struct model_state const* const*)block->mstates, block->nstates)) {
        imm_error("could not write states");
        return 1;
    }

    uint16_t ntrans = (uint16_t)dp_trans_table_total_ntrans(block->trans_table);
    if (fwrite(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not write ntrans");
        return 1;
    }

    for (uint16_t tgt_state = 0; tgt_state < block->nstates; ++tgt_state) {

        uint_fast16_t n = dp_trans_table_ntrans(block->trans_table, tgt_state);
        for (uint_fast16_t trans = 0; trans < n; ++trans) {

            uint16_t  src_state = (uint16_t)dp_trans_table_source_state(block->trans_table, tgt_state, trans);
            imm_float score = dp_trans_table_score(block->trans_table, tgt_state, trans);

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

static int read_dp(struct imm_profile* model, struct imm_hmm_block* block, FILE* stream)
{
    if (!(block->seq_code = seq_code_read(stream, model->abc))) {
        imm_error("could not read seq_code");
        return 1;
    }

    if (!(block->emission = dp_emission_read(stream))) {
        imm_error("could not read dp_emission");
        return 1;
    }

    if (!(block->trans_table = dp_trans_table_read(stream))) {
        imm_error("could not read dp_trans_table");
        return 1;
    }

    if (!(block->state_table = dp_state_table_read(stream))) {
        imm_error("could not read dp_state_table");
        return 1;
    }

    dp_create_from_model(model, block);

    return 0;
}

static int read_hmm(struct imm_profile* model, struct imm_hmm_block* block, FILE* stream)
{
    block->hmm = imm_hmm_create(model->abc);

    if (!(block->mstates = read_mstates(model, block, stream))) {
        imm_error("could not read states");
        goto err;
    }

    block->states = malloc(sizeof(*block->states) * block->nstates);

    for (uint16_t i = 0; i < block->nstates; ++i) {
        block->states[i] = block->mstates[i]->state;
        hmm_add_mstate(block->hmm, block->mstates[i]);
    }

    if (read_transitions(stream, block->hmm, block->mstates)) {
        imm_error("could not read transitions");
        goto err;
    }

    return 0;

err:
    if (block->hmm)
        imm_hmm_destroy(block->hmm);

    return 1;
}

static struct model_state** read_mstates(struct imm_profile* model, struct imm_hmm_block* block, FILE* stream)
{
    struct model_state** mstates = NULL;

    if (fread(&block->nstates, sizeof(block->nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        goto err;
    }

    mstates = malloc(sizeof(*mstates) * (block->nstates));
    for (uint16_t i = 0; i < block->nstates; ++i)
        mstates[i] = NULL;

    for (uint16_t i = 0; i < block->nstates; ++i) {

        imm_float start_lprob = 0.;
        if (fread(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
            imm_error("could not read start lprob");
            goto err;
        }

        struct imm_state const* state = model->vtable.read_state(model, stream);
        if (!state) {
            imm_error("could not read state");
            goto err;
        }

        mstates[i] = model_state_create(state, start_lprob);
    }

    return mstates;

err:
    if (mstates) {
        for (uint16_t i = 0; i < block->nstates; ++i) {
            if (mstates[i])
                model_state_destroy(mstates[i]);
        }

        free_c(mstates);
    }

    return NULL;
}

static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct model_state* const* const mstates)
{
    uint16_t ntrans = 0;

    if (fread(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not read ntransitions");
        return 1;
    }

    for (uint16_t i = 0; i < ntrans; ++i) {

        uint16_t  src_state = 0;
        uint16_t  tgt_state = 0;
        imm_float lprob = 0;

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

        struct model_trans_table* tbl = model_state_get_mtrans_table(mstates[src_state]);
        struct imm_state const*   tgt = model_state_get_state(mstates[tgt_state]);
        model_trans_table_add(tbl, model_trans_create(tgt, lprob));
    }

    return 0;
}
