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
#include "imm/hmm_block.h"
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

static int read_abc(struct imm_model* model, FILE* stream);
static int write_abc(struct imm_model const* model, FILE* stream);
static int write_mstate(struct imm_model const* model, FILE* stream,
                        struct model_state const* mstate);

struct imm_abc const* imm_model_abc(struct imm_model const* model) { return model->abc; }

struct imm_model* imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    return __imm_model_create(hmm, dp, __imm_model_read_state, NULL, __imm_model_write_state, NULL);
}

void imm_model_destroy(struct imm_model const* model)
{
    for (size_t i = 0; i < imm_model_nhmm_blocks(model); ++i) {
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

    if (__imm_model_read_hmm_blocks(model, stream))
        goto err;

    return model;

err:
    __imm_model_deep_destroy(model);
    return NULL;
}

int __imm_model_read_hmm_blocks(struct imm_model* model, FILE* stream)
{
    uint8_t nhmms = 0;
    if (fread(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not read nhmms");
        return 1;
    }

    for (uint8_t i = 0; i < nhmms; ++i) {

        struct imm_hmm_block const* block = hmm_block_read(model, stream);
        if (!block)
            return 1;
        imm_vecp_append(model->hmm_blocks, block);
    }
    return 0;
}

int imm_model_write(struct imm_model const* model, FILE* stream)
{
    if (write_abc(model, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    if (__imm_model_write_hmm_blocks(model, stream))
        return 1;

    return 0;
}

int __imm_model_write_hmm_blocks(struct imm_model const* model, FILE* stream)
{
    uint8_t nhmms = imm_model_nhmm_blocks(model);
    if (fwrite(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not write nhmms");
        return 1;
    }

    for (uint8_t i = 0; i < nhmms; ++i) {

        struct imm_hmm_block* block = (void*)imm_vecp_get(model->hmm_blocks, i);
        if (hmm_block_write(model, block, stream))
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

    for (uint8_t i = 0; i < imm_model_nhmm_blocks(model); ++i) {
        struct imm_hmm_block* block = (void*)imm_vecp_get(model->hmm_blocks, 0);
        hmm_block_deep_destroy(block);
    }
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

int write_mstates(struct imm_model const* model, FILE* stream,
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
