#include "imm/codon_state.h"
#include "common/common.h"
#include "imm/abc.h"
#include "imm/codon_lprob.h"
#include "imm/compiler.h"
#include "imm/generics.h"
#include "imm/seq.h"
#include "state.h"

static void del(struct imm_state const *state);

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

struct imm_codon_state *
imm_codon_state_new(unsigned id, struct imm_codon_lprob const *codonp)
{
    struct imm_codon_state *codon_state = xmalloc(sizeof(*codon_state));
    codon_state->codonp = codonp;
    struct imm_state_vtable vtable = {del, lprob, IMM_CODON_STATE, codon_state};
    struct imm_abc const *abc = imm_super(codonp->nuclt);
    codon_state->super = state_new(id, abc, vtable, IMM_SPAN(3, 3));
    return codon_state;
}

void imm_codon_state_del(struct imm_codon_state const *codon_state)
{
    codon_state->super->vtable.del(codon_state->super);
}

static void del(struct imm_state const *state)
{
    struct imm_codon_state const *codon_state = state->vtable.derived;
    free((void *)codon_state);
    state_del(state);
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    struct imm_codon_state const *codon_state = state->vtable.derived;
    if (imm_unlikely(imm_seq_len(seq) != 3))
        return imm_lprob_nan();

    struct imm_codon_lprob const *codonp = codon_state->codonp;
    struct imm_codon codon = {
        .nuclt = codonp->nuclt,
        .a = imm_abc_symbol_idx(state->abc, imm_seq_str(seq)[0]),
        .b = imm_abc_symbol_idx(state->abc, imm_seq_str(seq)[1]),
        .c = imm_abc_symbol_idx(state->abc, imm_seq_str(seq)[2])};

    return imm_codon_lprob_get(codonp, &codon);
}
