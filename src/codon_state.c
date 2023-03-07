#include "imm/codon_state.h"
#include "imm/abc.h"
#include "imm/codon_lprob.h"
#include "imm/generics.h"
#include "imm/seq.h"
#include "state.h"

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

void imm_codon_state_init(struct imm_codon_state *state, unsigned id,
                          struct imm_codon_lprob const *codonp)
{
    state->codonp = codonp;
    struct imm_state_vtable vtable = {lprob, IMM_CODON_STATE, state};
    struct imm_abc const *abc = imm_super(codonp->nuclt);
    __imm_state_init(&state->super, id, abc, vtable, imm_span(3, 3));
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    struct imm_codon_state const *codon_state = state->vtable.derived;
    if (imm_unlikely(imm_seq_size(seq) != 3)) return imm_lprob_nan();

    struct imm_codon_lprob const *codonp = codon_state->codonp;
    struct imm_codon codon = {
        .nuclt = codonp->nuclt,
        .a = imm_abc_symbol_idx(state->abc, imm_seq_str(seq)[0]),
        .b = imm_abc_symbol_idx(state->abc, imm_seq_str(seq)[1]),
        .c = imm_abc_symbol_idx(state->abc, imm_seq_str(seq)[2])};

    return imm_codon_lprob_get(codonp, codon);
}
