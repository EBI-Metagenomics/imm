#include "imm/emis.h"
#include "code.h"
#include "error.h"
#include "imm/abc.h"
#include "imm/cartes.h"
#include "imm/code.h"
#include "imm/state.h"
#include "reallocf.h"
#include <assert.h>
#include <stdlib.h>

static inline void cleanup(struct imm_emis *x)
{
    if (x->score)
    {
        free(x->score);
        x->score = NULL;
    }
    if (x->offset)
    {
        free(x->offset);
        x->offset = NULL;
    }
}

void imm_emis_init(struct imm_emis *emis)
{
    emis->score = NULL;
    emis->offset = NULL;
}

void imm_emis_del(struct imm_emis const *x)
{
    if (x)
    {
        free(x->score);
        free(x->offset);
    }
}

static enum imm_rc realloc_offset(struct imm_emis *emis, unsigned nstates)
{
    unsigned offsize = imm_emis_offset_size(nstates);
    emis->offset = imm_reallocf(emis->offset, sizeof(*emis->offset) * offsize);
    if (!emis->offset) return error(IMM_NOMEM);
    return 0;
}

static enum imm_rc realloc_score(struct imm_emis *emis, unsigned nstates)
{
    unsigned score_size = imm_emis_score_size(emis, nstates);
    emis->score = imm_reallocf(emis->score, sizeof(*emis->score) * score_size);
    if (!emis->score && score_size > 0) return error(IMM_NOMEM);
    return 0;
}

static void calc_offset(struct imm_emis *emis, struct imm_code const *code,
                        unsigned nstates, struct imm_state **states)
{
    emis->offset[0] = 0;
    unsigned min = imm_state_span(states[0]).min;
    unsigned max = imm_state_span(states[0]).max;
    unsigned size = code_size(code, min, max);
    for (unsigned i = 1; i < nstates; ++i)
    {
        assert(size <= UINT32_MAX);
        emis->offset[i] = (uint32_t)size;
        min = imm_state_span(states[i]).min;
        max = imm_state_span(states[i]).max;
        size += code_size(code, min, max);
    }
    assert(size <= UINT32_MAX);
    emis->offset[nstates] = (uint32_t)size;
}

static void calc_score(struct imm_emis *emis, struct imm_code const *code,
                       unsigned nstates, struct imm_state **states)
{
    struct imm_abc const *abc = code->abc;
    char const *set = abc->symbols;
    unsigned set_size = abc->size;
    struct imm_cartes cartes;
    /* TODO: consider to avoid initing this everytime */
    imm_cartes_init(&cartes, set, set_size, IMM_STATE_MAX_SEQLEN);

    for (unsigned i = 0; i < nstates; ++i)
    {
        unsigned min = imm_state_span(states[i]).min;
        unsigned max = imm_state_span(states[i]).max;
        for (unsigned len = min; len <= max; ++len)
        {
            imm_cartes_setup(&cartes, len);
            char const *item = NULL;
            while ((item = imm_cartes_next(&cartes)) != NULL)
            {
                struct imm_seq seq = IMM_SEQ_UNSAFE(len, item, abc);
                unsigned c =
                    code_translate(code, imm_code_encode(code, &seq), min);
                imm_float score = imm_state_lprob(states[i], &seq);
                assert(!imm_lprob_is_nan(score));
                emis->score[emis->offset[i] + c] = score;
            }
        }
    }
    imm_cartes_deinit(&cartes);
}

int imm_emis_reset(struct imm_emis *emis, struct imm_code const *code,
                   struct imm_state **states, unsigned nstates)
{
    int rc = 0;
    if ((rc = realloc_offset(emis, nstates))) goto cleanup;
    calc_offset(emis, code, nstates, states);

    if ((rc = realloc_score(emis, nstates))) goto cleanup;
    calc_score(emis, code, nstates, states);

    return rc;

cleanup:
    cleanup(emis);
    return rc;
}

imm_float imm_emis_score(struct imm_emis const *x, unsigned state,
                         unsigned seq_code)
{
    return x->score[x->offset[state] + seq_code];
}

unsigned imm_emis_score_size(struct imm_emis const *x, unsigned nstates)
{
    return x->offset[nstates];
}

unsigned imm_emis_offset_size(unsigned nstates) { return nstates + 1; }
