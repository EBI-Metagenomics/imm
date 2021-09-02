#include "hope/hope.h"
#include "imm/imm.h"

void test_frame_state1(void);
void test_frame_state2(void);
void test_frame_state3(void);
void test_frame_state_lposterior(void);
void test_frame_state_decode(void);

int main(void)
{
    test_frame_state1();
    test_frame_state2();
    test_frame_state3();
    test_frame_state_lposterior();
    test_frame_state_decode();
    return hope_status();
}

void test_frame_state1(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp =
        imm_nuclt_lprob(nuclt, IMM_ARR(imm_log(0.25), imm_log(0.25),
                                       imm_log(0.25), imm_log(0.25)));

    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8 / 0.9));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1 / 0.9));
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);
    struct imm_state *s = imm_super(&state);

    struct imm_seq seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.9145034795);
    seq = imm_seq(IMM_STR("AT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.9158434138);
    seq = imm_seq(IMM_STR("ATA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.9055970891);
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -0.5347732947);
    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.5902373362);
    seq = imm_seq(IMM_STR("ATTA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.8810321984);
    seq = imm_seq(IMM_STR("ATTAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -12.0882895834);
    seq = imm_seq(IMM_STR("ATTAAT"), abc);
    COND(imm_lprob_is_zero(imm_state_lprob(s, &seq)));
}

void test_frame_state2(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        nuclt, IMM_ARR(imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)));

    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8 / 0.9));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1 / 0.9));
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);
    struct imm_state *s = imm_super(&state);

    struct imm_seq seq;
    seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.9145034795);
    seq = imm_seq(IMM_STR("C"), abc);
    COND(imm_lprob_is_zero(imm_state_lprob(s, &seq)));
    seq = imm_seq(IMM_STR("G"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.0322865151);
    seq = imm_seq(IMM_STR("T"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.8091429638);

    seq = imm_seq(IMM_STR("AT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.9159357400);
    seq = imm_seq(IMM_STR("ATA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.8215183173);
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -0.5344319144);
    seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.1294800576);
    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.5751452135);

    seq = imm_seq(IMM_STR("ATTA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.7896445735);
    seq = imm_seq(IMM_STR("ACTG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.0366370866);

    seq = imm_seq(IMM_STR("ATTAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -13.9208710471);

    seq = imm_seq(IMM_STR("ATTAAT"), abc);
    COND(imm_lprob_is_zero(imm_state_lprob(s, &seq)));
}

void test_frame_state3(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        nuclt, IMM_ARR(imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)));

    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8) - imm_log(1.3));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1) - imm_log(1.3));
    codon = imm_codon(nuclt, 'G', 'T', 'C');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.4) - imm_log(1.3));
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);
    struct imm_state *s = imm_super(&state);

    struct imm_seq seq;
    seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.2822282596);
    seq = imm_seq(IMM_STR("C"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.0931584758);
    seq = imm_seq(IMM_STR("G"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.9945461872);
    seq = imm_seq(IMM_STR("T"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.8403955073);
    seq = imm_seq(IMM_STR("AT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -3.2834143360);
    seq = imm_seq(IMM_STR("CG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -9.3957435523);
    seq = imm_seq(IMM_STR("ATA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -8.1891199599);
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -0.9021561046);
    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.9428648052);
    seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.3148113691);
    seq = imm_seq(IMM_STR("GTC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -1.5951613416);
    seq = imm_seq(IMM_STR("ATTA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -8.1573693536);
    seq = imm_seq(IMM_STR("GTTC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -4.7116424205);
    seq = imm_seq(IMM_STR("ACTG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.4043618667);
    seq = imm_seq(IMM_STR("ATTAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -14.2885958273);
    seq = imm_seq(IMM_STR("GTCAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -12.9023014661);
}

void test_frame_state_lposterior(void)
{
    struct imm_dna const *dna = &imm_dna_default;
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

    while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
    {
        struct imm_codon codon =
            imm_codon(nuclt, codon_item[0], codon_item[1], codon_item[2]);
        imm_codon_lprob_set(&codonp, codon, imm_log(0.001));
    }
    imm_cartes_deinit(&codon_iter);

    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1));
    codon = imm_codon(nuclt, 'G', 'T', 'C');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.4));

    imm_codon_lprob_normalize(&codonp);
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_cartes_init(&codon_iter, symbols, length, 3);

    while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
    {
        codon = imm_codon(nuclt, codon_item[0], codon_item[1], codon_item[2]);

        imm_float total = imm_lprob_zero();
        for (uint16_t times = 1; times < 6; ++times)
        {

            struct imm_cartes seq_iter;
            imm_cartes_init(&seq_iter, symbols, length, times);
            char const *seq = NULL;

            while ((seq = imm_cartes_next(&seq_iter)) != NULL)
            {
                struct imm_seq tmp = imm_seq((struct imm_str){times, seq}, abc);
                imm_float lprob =
                    imm_frame_state_lposterior(&state, &codon, &tmp);
                lprob -= imm_codon_marg_lprob(&codonm, codon);
                total = imm_lprob_add(total, lprob);
            }
            imm_cartes_deinit(&seq_iter);
        }
        CLOSE((imm_float)exp(total), 1.0);
    }
    imm_cartes_deinit(&codon_iter);
}

void test_frame_state_decode(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        nuclt, IMM_ARR(imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)));

    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8) - imm_log(1.3));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1) - imm_log(1.3));
    codon = imm_codon(nuclt, 'G', 'T', 'C');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.4) - imm_log(1.3));

    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    struct imm_seq seq;
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -0.9025667126);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATGT"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -4.7105990700);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATGA"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -6.0977143369);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATGGT"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -9.0311004552);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -2.9771014466);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'T'));

    seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -7.7202251148);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("TC"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -4.1990898725);
    COND(codon.a == imm_abc_symbol_idx(abc, 'G') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'C'));

    seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -6.4000112953);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("AG"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -3.5071734613);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("GC"), abc);
    CLOSE(imm_frame_state_decode(&state, &seq, &codon), -4.1997050678);
    COND(codon.a == imm_abc_symbol_idx(abc, 'G') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'C'));
}
