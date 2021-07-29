#include "hope/hope.h"
#include "imm/imm.h"

void test_codon_state(void);

int main(void)
{
    test_codon_state();
    return hope_status();
}

void test_codon_state(void)
{
    struct imm_nuclt nuclt;
    imm_nuclt_init(&nuclt, "ACGT", 'X');
    struct imm_abc const *abc = imm_super(&nuclt);

    struct imm_codon_lprob codonp = imm_codon_lprob(&nuclt);
    struct imm_codon codon = imm_codon(&nuclt, 'A', 'T', 'G');

    imm_codon_lprob_set(&codonp, codon, imm_log(0.8 / 0.9));
    codon = imm_codon(&nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1 / 0.9));

    struct imm_codon_state state;
    imm_codon_state_init(&state, 0, &codonp);
    const struct imm_state *s = imm_super(&state);

    struct imm_seq seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_state_lprob(s, &seq), imm_log(0.8 / 0.9));

    seq = imm_seq(IMM_STR("AG"), abc);
    COND(imm_lprob_is_nan(imm_state_lprob(s, &seq)));
}
