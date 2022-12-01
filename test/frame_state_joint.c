#include "hope.h"
#include "imm/imm.h"

static void setup_codonp(struct imm_codon_lprob *codonp);
static struct imm_nuclt_lprob setup_nucltp(struct imm_nuclt const *);
static void check_joint(struct imm_state const *, struct imm_span);

static void test_frame_state_joint(struct imm_nuclt_lprob const *nucltp,
                                   struct imm_codon_marg const *codonm,
                                   struct imm_span span);

int main(void)
{
    struct imm_nuclt const *nuclt = imm_super(&imm_dna_iupac);
    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
    setup_codonp(&codonp);

    struct imm_nuclt_lprob nucltp = setup_nucltp(nuclt);
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    test_frame_state_joint(&nucltp, &codonm, imm_span(1, 5));
    test_frame_state_joint(&nucltp, &codonm, imm_span(2, 4));
    test_frame_state_joint(&nucltp, &codonm, imm_span(3, 3));

    return hope_status();
}

static void test_frame_state_joint(struct imm_nuclt_lprob const *nucltp,
                                   struct imm_codon_marg const *codonm,
                                   struct imm_span span)
{
    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, nucltp, codonm, 0.1f, span);
    check_joint(imm_frame_state_super_c(&state), span);
}

static void setup_codonp(struct imm_codon_lprob *codonp)
{
    struct imm_nuclt const *nuclt = codonp->nuclt;
    struct imm_abc const *abc = imm_super(nuclt);
    char const *symbols = imm_abc_symbols(abc);
    unsigned length = imm_abc_size(abc);

    struct imm_cartes iter;
    imm_cartes_init(&iter, symbols, length, 3);
    char const *item = NULL;

    imm_cartes_setup(&iter, 3);
    while ((item = imm_cartes_next(&iter)) != NULL)
    {
        struct imm_codon codon = IMM_CODON(nuclt, item);
        imm_codon_lprob_set(codonp, codon, imm_log(0.001));
    }
    imm_cartes_deinit(&iter);

    struct imm_codon codon = IMM_CODON(nuclt, "ATG");
    imm_codon_lprob_set(codonp, codon, imm_log(0.8));
    codon = IMM_CODON(nuclt, "ATT");
    imm_codon_lprob_set(codonp, codon, imm_log(0.1));
    codon = IMM_CODON(nuclt, "GTC");
    imm_codon_lprob_set(codonp, codon, imm_log(0.4));

    imm_codon_lprob_normalize(codonp);
}

static struct imm_nuclt_lprob setup_nucltp(struct imm_nuclt const *nuclt)
{
    imm_float arr[] = {imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)};
    return imm_nuclt_lprob(nuclt, arr);
}

static void check_joint(struct imm_state const *state, struct imm_span span)
{
    struct imm_abc const *abc = imm_state_abc(state);
    char const *symbols = imm_abc_symbols(abc);
    unsigned length = imm_abc_size(abc);

    struct imm_cartes iter;
    imm_cartes_init(&iter, symbols, length, span.max);
    imm_float total = imm_lprob_zero();
    for (unsigned times = span.min; times <= span.max; ++times)
    {
        imm_cartes_setup(&iter, times);

        char const *seq = NULL;
        while ((seq = imm_cartes_next(&iter)) != NULL)
        {
            struct imm_seq tmp = imm_seq((struct imm_str){times, seq}, abc);
            total = imm_lprob_add(total, imm_state_lprob(state, &tmp));
        }
    }
    close(imm_exp(total), 1.0);
    imm_cartes_deinit(&iter);
}
