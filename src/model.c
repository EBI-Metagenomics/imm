#include "imm/model.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "hmm_block.h"
#include "imm/abc.h"
#include "imm/abc_types.h"
#include "imm/bug.h"
#include "imm/dp.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "imm/table_state.h"
#include "model.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "seq_code.h"
#include <stdlib.h>
#include <string.h>

static int                  read_abc(struct imm_model* model, FILE* stream);
static struct model_state** read_mstates(struct imm_model* model, struct imm_hmm_block* block,
                                         FILE* stream);
static int                  read_transitions(FILE* stream, struct imm_hmm* hmm,
                                             struct model_state* const* const mstates);
static int                  write_abc(struct imm_model const* model, FILE* stream);
static int                  write_mstate(struct imm_model const* model, FILE* stream,
                                         struct model_state const* mstate);
static int                  write_mstates(struct imm_model const* model, FILE* stream,
                                          struct model_state const* const* mstates, uint16_t nstates);

struct imm_abc const* imm_model_abc(struct imm_model const* model) { return model->abc; }

struct imm_model* imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    return __imm_model_create(hmm, dp, __imm_model_read_state, NULL, __imm_model_write_state, NULL);
}

void imm_model_destroy(struct imm_model const* model)
{
    for (size_t i = 0; i < imm_vecp_length(model->hmm_blocks); ++i) {
        struct imm_hmm_block* block = (void*)imm_vecp_get(model->hmm_blocks, i);
        hmm_block_destroy(block);
    }
    imm_vecp_destroy(model->hmm_blocks);
    free_c(model);
}

struct imm_model const* imm_model_read(FILE* stream)
{
    struct imm_model* model =
        __imm_model_new(__imm_model_read_state, NULL, __imm_model_write_state, NULL);

    if (read_abc(model, stream)) {
        imm_error("could not read abc");
        goto err;
    }

    uint8_t nhmms = 0;
    if (fread(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not read nhmms");
        goto err;
    }

    struct imm_hmm_block* block = hmm_block_new();
    imm_vecp_append(model->hmm_blocks, block);

    if (__imm_model_read_hmm(model, block, stream)) {
        imm_error("could not read hmm");
        goto err;
    }

    if (__imm_model_read_dp(model, block, stream)) {
        imm_error("could not read dp");
        goto err;
    }

    return model;

err:
    __imm_model_deep_destroy(model);
    return NULL;
}

int imm_model_write(struct imm_model const* model, FILE* stream)
{
    if (write_abc(model, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    uint8_t nhmms = 1;
    if (fwrite(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not write nhmms");
        return 1;
    }

    if (__imm_model_write_hmm(model, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (__imm_model_write_dp(model, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

struct imm_model* __imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp,
                                     imm_model_read_state_cb read_state, void* read_state_args,
                                     imm_model_write_state_cb write_state, void* write_state_args)
{
    struct imm_model* model =
        __imm_model_new(read_state, read_state_args, write_state, write_state_args);
    model->abc = hmm_abc(hmm);
    imm_model_append_hmm_block(model, hmm, dp);
    return model;
}

void imm_model_append_hmm_block(struct imm_model* model, struct imm_hmm* hmm,
                                struct imm_dp const* dp)
{
    IMM_BUG(model->abc != hmm_abc(hmm));
    struct imm_hmm_block* block = hmm_block_create(hmm, dp);
    imm_vecp_append(model->hmm_blocks, block);
}

struct imm_hmm_block* imm_model_get_hmm_block(struct imm_model const* model, uint8_t i)
{
    return (void*)imm_vecp_get(model->hmm_blocks, i);
}

uint8_t imm_model_nhmm_blocks(struct imm_model const* model)
{
    return (uint8_t)imm_vecp_length(model->hmm_blocks);
}

struct imm_model* __imm_model_new(imm_model_read_state_cb read_state, void* read_state_args,
                                  imm_model_write_state_cb write_state, void* write_state_args)
{
    struct imm_model* model = malloc(sizeof(*model));
    model->abc = NULL;
    model->hmm_blocks = imm_vecp_create();
    model->read_state = read_state;
    model->read_state_args = read_state_args;
    model->write_state = write_state;
    model->write_state_args = write_state_args;
    return model;
}

int __imm_model_read_dp(struct imm_model* model, struct imm_hmm_block* block, FILE* stream)
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

int __imm_model_read_hmm(struct imm_model* model, struct imm_hmm_block* block, FILE* stream)
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

struct imm_state const* __imm_model_read_state(struct imm_model const* model, FILE* stream,
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
        if (!(state = imm_mute_state_read(stream, model->abc)))
            imm_error("could not read mute state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = imm_normal_state_read(stream, model->abc)))
            imm_error("could not read normal state");
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        if (!(state = imm_table_state_read(stream, model->abc)))
            imm_error("could not read table state");
        break;
    default:
        imm_error("unknown state type id");
    }

    return state;
}

void __imm_model_set_abc(struct imm_model* model, struct imm_abc const* abc) { model->abc = abc; }

int __imm_model_write_dp(struct imm_model const* model, FILE* stream)
{
    struct imm_hmm_block* block = (void*)imm_vecp_get(model->hmm_blocks, 0);

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

int __imm_model_write_hmm(struct imm_model const* model, FILE* stream)
{
    struct imm_hmm_block* block = (void*)imm_vecp_get(model->hmm_blocks, 0);

    if (write_mstates(model, stream, (struct model_state const* const*)block->mstates,
                      block->nstates)) {
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

            uint16_t src_state =
                (uint16_t)dp_trans_table_source_state(block->trans_table, tgt_state, trans);
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

int __imm_model_write_state(struct imm_model const* model, FILE* stream,
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
        errno = imm_mute_state_write(state, model, stream);
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        errno = imm_normal_state_write(state, model, stream);
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        errno = imm_table_state_write(state, model, stream);
        break;
    default:
        imm_error("unknown state type id");
        errno = 1;
    }
    return errno;
}

void __imm_model_deep_destroy(struct imm_model const* model)
{
    if (model->abc)
        imm_abc_destroy(model->abc);

    struct imm_hmm_block* block = (void*)imm_vecp_get(model->hmm_blocks, 0);
    hmm_block_deep_destroy(block);
    imm_vecp_destroy(model->hmm_blocks);

    free_c(model);
}

static int read_abc(struct imm_model* model, FILE* stream)
{
    if (!(model->abc = imm_abc_read(stream))) {
        imm_error("could not read abc");
        return 1;
    }
    return 0;
}

static struct model_state** read_mstates(struct imm_model* model, struct imm_hmm_block* block,
                                         FILE* stream)
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

        struct imm_state const* state = model->read_state(model, stream, model->read_state_args);
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

static int read_transitions(FILE* stream, struct imm_hmm* hmm,
                            struct model_state* const* const mstates)
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

static int write_abc(struct imm_model const* model, FILE* stream)
{
    if (imm_abc_write(model->abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    return 0;
}

static int write_mstate(struct imm_model const* model, FILE* stream,
                        struct model_state const* mstate)
{
    struct imm_state const* state = model_state_get_state(mstate);

    imm_float start_lprob = (imm_float)model_state_get_start(mstate);

    if (fwrite(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
        imm_error("could not write start_lprob");
        return 1;
    }

    if (model->write_state(model, stream, state, model->write_state_args)) {
        imm_error("could not write state");
        return 1;
    }

    return 0;
}

static int write_mstates(struct imm_model const* model, FILE* stream,
                         struct model_state const* const* mstates, uint16_t nstates)
{
    if (fwrite(&nstates, sizeof(nstates), 1, stream) < 1) {
        imm_error("could not write nstates");
        return 1;
    }

    for (uint16_t i = 0; i < nstates; ++i) {
        if (write_mstate(model, stream, mstates[i])) {
            imm_error("could not write mstate");
            return 1;
        }
    }

    return 0;
}
