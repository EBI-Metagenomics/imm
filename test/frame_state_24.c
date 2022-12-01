#include "hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_span span = IMM_SPAN(2, 4);
    struct imm_dna const *dna = &imm_dna_iupac;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        nuclt, IMM_ARR(imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)));

    char const *symbols = imm_abc_symbols(abc);
    unsigned length = imm_abc_size(abc);

    struct imm_cartes codon_iter;
    imm_cartes_init(&codon_iter, symbols, length, 3);
    char const *codon_item = NULL;

    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

    imm_cartes_setup(&codon_iter, 3);
    while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
    {
        struct imm_codon codon = IMM_CODON(nuclt, codon_item);
        imm_codon_lprob_set(&codonp, codon, imm_log(0.001));
    }
    imm_cartes_deinit(&codon_iter);

    struct imm_codon codon = IMM_CODON(nuclt, "ATG");
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8));
    codon = IMM_CODON(nuclt, "ATT");
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1));
    codon = IMM_CODON(nuclt, "GTC");
    imm_codon_lprob_set(&codonp, codon, imm_log(0.4));

    imm_codon_lprob_normalize(&codonp);
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, span);

    struct imm_cartes seq_iter;
    imm_cartes_init(&seq_iter, symbols, length, span.max);
    imm_float total = imm_lprob_zero();
    for (unsigned times = span.min; times <= span.max; ++times)
    {
        imm_cartes_setup(&seq_iter, times);

        char const *seq = NULL;
        while ((seq = imm_cartes_next(&seq_iter)) != NULL)
        {
            struct imm_seq tmp = imm_seq((struct imm_str){times, seq}, abc);
            struct imm_state const *s = imm_frame_state_super_c(&state);
            imm_float lprob = imm_state_lprob(s, &tmp);
            total = imm_lprob_add(total, lprob);
        }
    }
    close((imm_float)exp(total), 1.0);
    imm_cartes_deinit(&seq_iter);
    return hope_status();
}
