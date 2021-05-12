#include "dp/emission.h"
#include "dp/code.h"
#include "imm/abc.h"
#include "imm/cartes.h"
#include "imm/log.h"
#include "imm/state.h"
#include "io.h"
#include "support.h"

static inline unsigned offset_size(unsigned nstates) { return nstates + 1; }

static inline unsigned score_size(struct emission const *emission,
                                  unsigned nstates)
{
    return emission->offset[nstates];
}

void emission_deinit(struct emission const *emission)
{
    free(emission->score);
    free(emission->offset);
}

void emission_init(struct emission *emiss, struct code const *code,
                   struct imm_state **states, unsigned nstates)
{
    emiss->score = NULL;
    emiss->offset = NULL;
    emission_reset(emiss, code, states, nstates);
}

void emission_reset(struct emission *emiss, struct code const *code,
                    struct imm_state **states, unsigned nstates)
{
    emiss->offset =
        xrealloc(emiss->offset, sizeof(*emiss->offset) * offset_size(nstates));
    emiss->offset[0] = 0;

    unsigned min = imm_state_span(states[0]).min;
    unsigned size = code_size(code, min);
    for (unsigned i = 1; i < nstates; ++i)
    {
        emiss->offset[i] = size;
        size += code_size(code, imm_state_span(states[i]).min);
    }
    emiss->offset[nstates] = size;

    emiss->score = xrealloc(emiss->score,
                            sizeof(*emiss->score) * score_size(emiss, nstates));

    struct imm_abc const *abc = code->abc;
    char const *set = abc->symbols;
    unsigned set_size = abc->size;
    struct imm_cartes cartes;
    imm_cartes_init(&cartes, set, set_size, code->seqlen.max);

    for (unsigned i = 0; i < nstates; ++i)
    {
        unsigned min_seq = imm_state_span(states[i]).min;
        for (unsigned len = min_seq; len <= imm_state_span(states[i]).max;
             ++len)
        {
            imm_cartes_setup(&cartes, len);
            char const *item = NULL;
            while ((item = imm_cartes_next(&cartes)) != NULL)
            {

                struct imm_seq seq = IMM_SEQ_UNSAFE(len, item, abc);
                unsigned j = code_encode(code, &seq);
                j -= code_offset(code, min_seq);
                imm_float score = imm_state_lprob(states[i], &seq);
                emiss->score[emiss->offset[i] + j] = score;
            }
        }
    }
    imm_cartes_deinit(&cartes);
}

struct chunk
{
    struct
    {
        uint32_t size;
        imm_float *data;
    } score;
    struct
    {
        uint32_t size;
        uint32_t *data;
    } offset;
};

void emission_write(struct emission const *e, unsigned nstates, FILE *stream)
{
    struct chunk c;
    c.score.size = score_size(e, nstates);
    c.offset.size = offset_size(nstates);
    write_array_u32_flt(&c.score.size, e->score, stream);
    write_array_u32_u32(&c.offset.size, e->offset, stream);
}

int emission_read(struct emission *e, FILE *stream)
{
    e->score = read_array_u32_flt(e->score, stream);
    e->offset = read_array_u32_u32(e->offset, stream);
    return IMM_SUCCESS;
}
