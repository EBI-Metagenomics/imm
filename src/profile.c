#include "profile.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "hmm_block.h"
#include "imm/imm.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "seq_code.h"
#include <stdlib.h>
#include <string.h>

static int                     write_abc(struct imm_profile const* prof, FILE* stream);
static struct imm_state const* read_state(struct imm_profile* prof, FILE* stream);
static int                     write_state(struct imm_profile const* prof, FILE* stream, struct imm_state const* state);
static int write_mstate(struct imm_profile const* prof, FILE* stream, struct model_state const* mstate);

struct imm_abc const* imm_profile_abc(struct imm_profile const* prof) { return prof->abc; }

struct imm_profile* imm_profile_create(struct imm_abc const* abc)
{
    return __imm_profile_create(abc, (struct imm_profile_vtable){read_state, write_state}, NULL);
}

void* __imm_profile_derived(struct imm_profile* prof) { return prof->derived; }

void const* __imm_profile_derived_c(struct imm_profile const* prof) { return prof->derived; }

void imm_profile_destroy(struct imm_profile const* prof)
{
    for (size_t i = 0; i < imm_profile_nhmm_blocks(prof); ++i) {
        struct imm_hmm_block* block = (void*)imm_vecp_get(prof->hmm_blocks, i);
        hmm_block_destroy(block);
    }
    imm_vecp_destroy(prof->hmm_blocks);
    free_c(prof);
}

void imm_profile_free(struct imm_profile const* prof)
{
    imm_vecp_destroy(prof->hmm_blocks);
    free_c(prof);
}

struct imm_profile const* imm_profile_read(FILE* stream)
{
    struct imm_abc const* abc = imm_abc_read(stream);
    if (!abc) {
        imm_error("could not read abc");
        goto err;
    }
    struct imm_profile* prof = __imm_profile_create(abc, (struct imm_profile_vtable){read_state, write_state}, NULL);

    if (__imm_profile_read_hmm_blocks(prof, stream))
        goto err;

    return prof;

err:
    __imm_profile_deep_destroy(prof);
    return NULL;
}

int __imm_profile_read_hmm_blocks(struct imm_profile* prof, FILE* stream)
{
    uint8_t nhmms = 0;
    if (fread(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not read nhmms");
        return 1;
    }

    for (uint8_t i = 0; i < nhmms; ++i) {

        struct imm_hmm_block const* block = hmm_block_read(prof, stream);
        if (!block)
            return 1;
        imm_vecp_append(prof->hmm_blocks, block);
    }
    return 0;
}

int imm_profile_write(struct imm_profile const* prof, FILE* stream)
{
    if (write_abc(prof, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    if (__imm_profile_write_hmm_blocks(prof, stream))
        return 1;

    return 0;
}

int __imm_profile_write_hmm_blocks(struct imm_profile const* prof, FILE* stream)
{
    uint8_t nhmms = imm_profile_nhmm_blocks(prof);
    if (fwrite(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not write nhmms");
        return 1;
    }

    for (uint8_t i = 0; i < nhmms; ++i) {

        struct imm_hmm_block* block = (void*)imm_vecp_get(prof->hmm_blocks, i);
        if (hmm_block_write(prof, block, stream))
            return 1;
    }

    return 0;
}

struct imm_profile* __imm_profile_create(struct imm_abc const* abc, struct imm_profile_vtable vtable, void* derived)
{
    struct imm_profile* prof = malloc(sizeof(*prof));
    prof->abc = abc;
    prof->hmm_blocks = imm_vecp_create();
    prof->vtable = vtable;
    prof->derived = derived;
    return prof;
}

void imm_profile_append_hmm_block(struct imm_profile* prof, struct imm_hmm_block* block)
{
    IMM_BUG(prof->abc != hmm_abc(block->hmm));
    imm_vecp_append(prof->hmm_blocks, block);
}

struct imm_hmm_block* imm_profile_get_hmm_block(struct imm_profile const* prof, uint8_t i)
{
    return (void*)imm_vecp_get(prof->hmm_blocks, i);
}

uint8_t imm_profile_nhmm_blocks(struct imm_profile const* prof) { return (uint8_t)imm_vecp_length(prof->hmm_blocks); }

static struct imm_state const* read_state(struct imm_profile* prof, FILE* stream)
{
    struct imm_state const* state = NULL;
    uint8_t                 type_id = 0;

    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not read type id");
        return NULL;
    }

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = imm_mute_state_read(stream, prof->abc)))
            imm_error("could not read mute state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = imm_normal_state_read(stream, prof->abc)))
            imm_error("could not read normal state");
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        if (!(state = imm_table_state_read(stream, prof->abc)))
            imm_error("could not read table state");
        break;
    default:
        imm_error("unknown state type id");
    }

    return state;
}

void __imm_profile_set_abc(struct imm_profile* prof, struct imm_abc const* abc) { prof->abc = abc; }

static int write_state(struct imm_profile const* prof, FILE* stream, struct imm_state const* state)
{
    uint8_t type_id = imm_state_type_id(state);
    if (fwrite(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not write state type id");
        return 1;
    }

    int errno = 0;
    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        errno = imm_mute_state_write(state, prof, stream);
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        errno = imm_normal_state_write(state, prof, stream);
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        errno = imm_table_state_write(state, prof, stream);
        break;
    default:
        imm_error("unknown state type id");
        errno = 1;
    }
    return errno;
}

void __imm_profile_deep_destroy(struct imm_profile const* prof)
{
    if (prof->abc)
        imm_abc_destroy(prof->abc);

    for (uint8_t i = 0; i < imm_profile_nhmm_blocks(prof); ++i) {
        struct imm_hmm_block* block = (void*)imm_vecp_get(prof->hmm_blocks, 0);
        hmm_block_deep_destroy(block);
    }
    imm_vecp_destroy(prof->hmm_blocks);
    free_c(prof);
}

static int write_abc(struct imm_profile const* prof, FILE* stream)
{
    if (imm_abc_write(prof->abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    return 0;
}

static int write_mstate(struct imm_profile const* prof, FILE* stream, struct model_state const* mstate)
{
    struct imm_state const* state = model_state_get_state(mstate);

    imm_float start_lprob = (imm_float)model_state_get_start(mstate);

    if (fwrite(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
        imm_error("could not write start_lprob");
        return 1;
    }

    if (prof->vtable.write_state(prof, stream, state)) {
        imm_error("could not write state");
        return 1;
    }

    return 0;
}

int write_mstates(struct imm_profile const* prof, FILE* stream, struct model_state const* const* mstates,
                  uint16_t nstates)
{
    if (fwrite(&nstates, sizeof(nstates), 1, stream) < 1) {
        imm_error("could not write nstates");
        return 1;
    }

    for (uint16_t i = 0; i < nstates; ++i) {
        if (write_mstate(prof, stream, mstates[i])) {
            imm_error("could not write mstate");
            return 1;
        }
    }

    return 0;
}
