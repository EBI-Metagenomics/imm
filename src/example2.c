#include "imm/imm.h"

/* State IDs */
#define START ((imm_state_id_t)(0U << 11))
#define B ((imm_state_id_t)(1U << 11))
#define M ((imm_state_id_t)(2U << 11))
#define I ((imm_state_id_t)(3U << 11))
#define D ((imm_state_id_t)(4U << 11))
#define E ((imm_state_id_t)(5U << 11))
#define J ((imm_state_id_t)(6U << 11))
#define END ((imm_state_id_t)(7U << 11))
#define N ((imm_state_id_t)(8U << 11))

char const imm_example2_seq[] =
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC";

struct imm_example2 imm_example2 = {.dna = &imm_dna_iupac};

#define SET_TRANS(hmm, a, b, v)                                                \
    imm_hmm_set_trans(&hmm, imm_super(&a), imm_super(&b), v)

#define SET_CODONP(codonp, codon, v) imm_codon_lprob_set(codonp, codon, v)

static struct imm_codon_lprob create_codonp(struct imm_nuclt const *nuclt)
{
    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
    SET_CODONP(&codonp, IMM_CODON(nuclt, "AAA"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "AAC"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "AAG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "AAT"), imm_log(0.0023173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "ACA"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "ACG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "ACT"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "AGC"), imm_log(0.0029114));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "AGG"), imm_log(0.0029003));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "AGT"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "ATA"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "ATG"), imm_log(0.0049178));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "ATT"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CAA"), imm_log(0.0029478));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CAC"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CAG"), imm_log(0.0029123));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CAT"), imm_log(0.0009133));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CCA"), imm_log(0.0029179));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CCG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CCT"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CGC"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CGG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CGT"), imm_log(0.0041183));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CTA"), imm_log(0.0038173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CTG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "CTT"), imm_log(0.0029111));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GAA"), imm_log(0.0019173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GAC"), imm_log(0.0029103));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GAG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GAT"), imm_log(0.0029103));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GCA"), imm_log(0.0003138));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GCG"), imm_log(0.0039173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GCT"), imm_log(0.0029103));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GGC"), imm_log(0.0019173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GGG"), imm_log(0.0009173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GGT"), imm_log(0.0029143));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GTA"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GTG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "GTT"), imm_log(0.0029171));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TAA"), imm_log(0.0099113));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TAC"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TAG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TAT"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TCA"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TCG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TCT"), imm_log(0.0020173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TGC"), imm_log(0.0029193));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TGG"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TGT"), imm_log(0.0029173));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TTA"), imm_log(0.0089193));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TTG"), imm_log(0.0029138));
    SET_CODONP(&codonp, IMM_CODON(nuclt, "TTT"), imm_log(0.0089183));
    return codonp;
}

static struct imm_codon_marg codonm(struct imm_codon codon, imm_float lprob)
{
    struct imm_codon_lprob codonp = create_codonp(codon.nuclt);
    imm_codon_lprob_set(&codonp, codon, lprob);
    return imm_codon_marg(&codonp);
}

void imm_example2_init(struct imm_span span)
{
    struct imm_example2 *m = &imm_example2;
    struct imm_nuclt const *nuclt = imm_super(m->dna);
    struct imm_abc const *abc = imm_super(nuclt);
    imm_code_init(&m->code, abc);
    imm_hmm_init(&m->hmm, &m->code);

    imm_float eps = (imm_float)0.01;
    m->nucltp = imm_nuclt_lprob(nuclt, IMM_ARR(imm_log(0.25), imm_log(0.25),
                                               imm_log(0.45), imm_log(0.05)));

    m->m_marg = codonm(IMM_CODON(nuclt, "ACG"), imm_log(100.0));
    m->i_marg = codonm(IMM_CODON(nuclt, "CGT"), imm_log(100.0));
    m->b_marg = codonm(IMM_CODON(nuclt, "AAA"), imm_log(100.0));
    m->e_marg = codonm(IMM_CODON(nuclt, "CCC"), imm_log(100.0));
    m->j_marg = codonm(IMM_CODON(nuclt, "GGG"), imm_log(100.0));
    m->null.n_marg = codonm(IMM_CODON(nuclt, "GTG"), imm_log(13.0));

    imm_mute_state_init(&m->start, START, abc);
    imm_hmm_add_state(&m->hmm, imm_super(&m->start));
    imm_hmm_set_start(&m->hmm, imm_super(&m->start), imm_log(1.0));

    imm_mute_state_init(&m->end, END, abc);
    imm_hmm_add_state(&m->hmm, imm_super(&m->end));

    imm_frame_state_init(&m->b, B, &m->nucltp, &m->b_marg, eps, span);
    imm_hmm_add_state(&m->hmm, imm_super(&m->b));

    imm_frame_state_init(&m->e, E, &m->nucltp, &m->e_marg, eps, span);
    imm_hmm_add_state(&m->hmm, imm_super(&m->e));

    imm_frame_state_init(&m->j, J, &m->nucltp, &m->j_marg, eps, span);
    imm_hmm_add_state(&m->hmm, imm_super(&m->j));

    SET_TRANS(m->hmm, m->start, m->b, imm_log(0.2));
    SET_TRANS(m->hmm, m->b, m->b, imm_log(0.2));
    SET_TRANS(m->hmm, m->e, m->e, imm_log(0.2));
    SET_TRANS(m->hmm, m->j, m->j, imm_log(0.2));
    SET_TRANS(m->hmm, m->e, m->j, imm_log(0.2));
    SET_TRANS(m->hmm, m->j, m->b, imm_log(0.2));
    SET_TRANS(m->hmm, m->e, m->end, imm_log(0.2));

    for (unsigned k = 0; k < IMM_EXAMPLE2_SIZE; ++k)
    {
        imm_frame_state_init(m->m + k, M | k, &m->nucltp, &m->m_marg, eps,
                             span);
        imm_frame_state_init(m->i + k, I | k, &m->nucltp, &m->i_marg, eps,
                             span);
        imm_mute_state_init(m->d + k, D | k, abc);

        imm_hmm_add_state(&m->hmm, imm_super(&m->m[k]));
        imm_hmm_add_state(&m->hmm, imm_super(&m->i[k]));
        imm_hmm_add_state(&m->hmm, imm_super(&m->d[k]));

        if (k == 0) SET_TRANS(m->hmm, m->b, m->m[0], imm_log(0.2));

        SET_TRANS(m->hmm, m->m[k], m->i[k], imm_log(0.2));
        SET_TRANS(m->hmm, m->i[k], m->i[k], imm_log(0.2));

        if (k > 0)
        {
            SET_TRANS(m->hmm, m->m[k - 1], m->m[k], imm_log(0.2));
            SET_TRANS(m->hmm, m->d[k - 1], m->m[k], imm_log(0.2));
            SET_TRANS(m->hmm, m->i[k - 1], m->m[k], imm_log(0.2));

            SET_TRANS(m->hmm, m->m[k - 1], m->d[k], imm_log(0.2));
            SET_TRANS(m->hmm, m->d[k - 1], m->d[k], imm_log(0.2));
        }

        if (k == IMM_EXAMPLE2_SIZE - 1)
        {
            SET_TRANS(m->hmm, m->m[k], m->e, imm_log(0.2));
            SET_TRANS(m->hmm, m->d[k], m->e, imm_log(0.2));
            SET_TRANS(m->hmm, m->i[k], m->e, imm_log(0.2));
        }
    }

    imm_hmm_init(&m->null.hmm, &m->code);
    imm_frame_state_init(&m->null.n, N, &m->nucltp, &m->null.n_marg, eps, span);
    imm_hmm_add_state(&m->null.hmm, imm_super(&m->null.n));
    imm_hmm_set_start(&m->null.hmm, imm_super(&m->null.n), imm_log(1.0));
    SET_TRANS(m->null.hmm, m->null.n, m->null.n, imm_log(0.2));
}
