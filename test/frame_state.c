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
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        imm_super(dna), (imm_float[]){imm_log(0.25), imm_log(0.25),
                                      imm_log(0.25), imm_log(0.25)});

    struct imm_codon_lprob codonp = imm_codon_lprob(imm_super(dna));
    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8 / 0.9));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1 / 0.9));
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state *state =
        imm_frame_state_new(0, &nucltp, &codonm, (imm_float)0.1);
    struct imm_state *s = imm_frame_state_super(state);

    struct imm_seq seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.914503505971854);
    seq = imm_seq(IMM_STR("AT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.915843423869834);
    seq = imm_seq(IMM_STR("ATA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.905597115665666);
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -0.534773288204706);
    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.590237330499946);
    seq = imm_seq(IMM_STR("ATTA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.881032208841384);
    seq = imm_seq(IMM_STR("ATTAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -12.08828960987379);
    seq = imm_seq(IMM_STR("ATTAAT"), abc);
    COND(imm_lprob_is_zero(imm_state_lprob(s, &seq)));

    imm_frame_state_del(state);
}

void test_frame_state2(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        imm_super(dna),
        (imm_float[]){imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)});

    struct imm_codon_lprob codonp = imm_codon_lprob(imm_super(dna));
    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8 / 0.9));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1 / 0.9));
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state *state =
        imm_frame_state_new(0, &nucltp, &codonm, (imm_float)0.1);
    struct imm_state *s = imm_frame_state_super(state);

    struct imm_seq seq;
    seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.914503505971854);
    seq = imm_seq(IMM_STR("C"), abc);
    COND(imm_lprob_is_zero(imm_state_lprob(s, &seq)));
    seq = imm_seq(IMM_STR("G"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.032286541628237);
    seq = imm_seq(IMM_STR("T"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.809142990314027);

    seq = imm_seq(IMM_STR("AT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.9159357500274385);
    seq = imm_seq(IMM_STR("ATA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.821518343902165);
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -0.5344319079005616);
    seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.129480084106424);
    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.57514520832882);

    seq = imm_seq(IMM_STR("ATTA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.789644584138959);
    seq = imm_seq(IMM_STR("ACTG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.036637096635257);

    seq = imm_seq(IMM_STR("ATTAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -13.920871073622099);

    seq = imm_seq(IMM_STR("ATTAAT"), abc);
    COND(imm_lprob_is_zero(imm_state_lprob(s, &seq)));

    imm_frame_state_del(state);
}

void test_frame_state3(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(imm_super(dna));
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        imm_super(dna),
        (imm_float[]){imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)});

    struct imm_codon_lprob codonp = imm_codon_lprob(imm_super(dna));

    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8) - imm_log(1.3));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1) - imm_log(1.3));
    codon = imm_codon(nuclt, 'G', 'T', 'C');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.4) - imm_log(1.3));
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state *state =
        imm_frame_state_new(0, &nucltp, &codonm, (imm_float)0.1);
    struct imm_state *s = imm_frame_state_super(state);

    struct imm_seq seq;
    seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_state_lprob(s, &seq), -6.282228286097171);
    seq = imm_seq(IMM_STR("C"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.0931585023135);
    seq = imm_seq(IMM_STR("G"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.99454621364539);
    seq = imm_seq(IMM_STR("T"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.840395533818132);
    seq = imm_seq(IMM_STR("AT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -3.283414346005771);
    seq = imm_seq(IMM_STR("CG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -9.395743595307545);
    seq = imm_seq(IMM_STR("ATA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -8.18911998648269);
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -0.9021560981322401);
    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_state_lprob(s, &seq), -2.9428648000333952);
    seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -7.314811395663229);
    seq = imm_seq(IMM_STR("GTC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -1.5951613351178675);
    seq = imm_seq(IMM_STR("ATTA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -8.157369364264277);
    seq = imm_seq(IMM_STR("GTTC"), abc);
    CLOSE(imm_state_lprob(s, &seq), -4.711642430498609);
    seq = imm_seq(IMM_STR("ACTG"), abc);
    CLOSE(imm_state_lprob(s, &seq), -5.404361876760574);
    seq = imm_seq(IMM_STR("ATTAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -14.288595853747417);
    seq = imm_seq(IMM_STR("GTCAA"), abc);
    CLOSE(imm_state_lprob(s, &seq), -12.902301492627526);

    imm_frame_state_del(state);
}

void test_frame_state_lposterior(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        imm_super(dna),
        (imm_float[]){imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)});

    char const *symbols = imm_abc_symbols(abc);
    unsigned length = imm_abc_size(abc);

    struct imm_cartes codon_iter;
    imm_cartes_init(&codon_iter, symbols, length, 3);
    char const *codon_item = NULL;

    struct imm_codon_lprob codonp = imm_codon_lprob(imm_super(dna));

    while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
    {
        struct imm_codon codon = imm_codon(imm_super(dna), codon_item[0],
                                           codon_item[1], codon_item[2]);
        imm_codon_lprob_set(&codonp, codon, imm_log(0.001));
    }
    imm_cartes_deinit(&codon_iter);

    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1));
    codon = imm_codon(nuclt, 'G', 'T', 'C');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.4));

    COND(imm_codon_lprob_normalize(&codonp) == 0);
    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state *state =
        imm_frame_state_new(0, &nucltp, &codonm, (imm_float)0.1);

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
                    imm_frame_state_lposterior(state, &codon, &tmp);
                lprob -= imm_codon_marg_lprob(&codonm, codon);
                total = imm_lprob_add(total, lprob);
            }
            imm_cartes_deinit(&seq_iter);
        }
        CLOSE((imm_float)exp(total), 1.0);
    }
    imm_cartes_deinit(&codon_iter);

    imm_frame_state_del(state);
}

void test_frame_state_decode(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(imm_super(dna));
    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
        imm_super(dna),
        (imm_float[]){imm_log(0.1), imm_log(0.2), imm_log(0.3), imm_log(0.4)});

    struct imm_codon_lprob codonp = imm_codon_lprob(imm_super(dna));

    struct imm_codon codon = imm_codon(nuclt, 'A', 'T', 'G');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.8) - imm_log(1.3));
    codon = imm_codon(nuclt, 'A', 'T', 'T');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.1) - imm_log(1.3));
    codon = imm_codon(nuclt, 'G', 'T', 'C');
    imm_codon_lprob_set(&codonp, codon, imm_log(0.4) - imm_log(1.3));

    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    struct imm_frame_state const *state =
        imm_frame_state_new(0, &nucltp, &codonm, (imm_float)0.1);

    struct imm_seq seq;
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -0.902566706136);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATGT"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -4.710599080052);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATGA"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -6.097714346951);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATGGT"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -9.031100481720);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -2.977101440300);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'T'));

    seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -7.720225141384);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("TC"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -4.199089882536);
    COND(codon.a == imm_abc_symbol_idx(abc, 'G') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'C'));

    seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -6.400011321754);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("AG"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -3.507173471362);
    COND(codon.a == imm_abc_symbol_idx(abc, 'A') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'G'));

    seq = imm_seq(IMM_STR("GC"), abc);
    CLOSE(imm_frame_state_decode(state, &seq, &codon), -4.199705077880);
    COND(codon.a == imm_abc_symbol_idx(abc, 'G') &&
         codon.b == imm_abc_symbol_idx(abc, 'T') &&
         codon.c == imm_abc_symbol_idx(abc, 'C'));

    imm_frame_state_del(state);
}
