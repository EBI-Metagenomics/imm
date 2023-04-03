#include "ex2.h"
#include "abc.h"
#include "code.h"
#include "codon_lprob.h"
#include "codon_marg.h"
#include "dna.h"
#include "frame_state.h"
#include "hmm.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "state.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* State IDs */
#define START ((uint16_t)(0U << 11))
#define B ((uint16_t)(1U << 11))
#define M ((uint16_t)(2U << 11))
#define I ((uint16_t)(3U << 11))
#define D ((uint16_t)(4U << 11))
#define E ((uint16_t)(5U << 11))
#define J ((uint16_t)(6U << 11))
#define END ((uint16_t)(7U << 11))
#define N ((uint16_t)(8U << 11))

#define SET_TRANS(hmm, a, b, v) imm_hmm_set_trans(&hmm, &a.super, &b.super, v)

#define SET_CODONP(codonp, codon, v) imm_codon_lprob_set(codonp, codon, v)

struct imm_ex2 imm_ex2 = {.dna = &imm_dna_iupac};

static struct imm_codon_lprob create_codonp(struct imm_nuclt const *nuclt)
{
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAA"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAC"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAT"), log(0.0023173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ACA"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ACG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ACT"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AGC"), log(0.0029114));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AGG"), log(0.0029003));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AGT"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ATA"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ATG"), log(0.0049178));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ATT"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAA"), log(0.0029478));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAC"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAG"), log(0.0029123));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAT"), log(0.0009133));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CCA"), log(0.0029179));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CCG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CCT"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CGC"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CGG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CGT"), log(0.0041183));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CTA"), log(0.0038173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CTG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CTT"), log(0.0029111));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAA"), log(0.0019173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAC"), log(0.0029103));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAT"), log(0.0029103));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GCA"), log(0.0003138));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GCG"), log(0.0039173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GCT"), log(0.0029103));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GGC"), log(0.0019173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GGG"), log(0.0009173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GGT"), log(0.0029143));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GTA"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GTG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GTT"), log(0.0029171));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAA"), log(0.0099113));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAC"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAT"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TCA"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TCG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TCT"), log(0.0020173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TGC"), log(0.0029193));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TGG"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TGT"), log(0.0029173));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TTA"), log(0.0089193));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TTG"), log(0.0029138));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TTT"), log(0.0089183));
  return codonp;
}

static struct imm_codon_marg codonm(struct imm_codon codon, float lprob)
{
  struct imm_codon_lprob codonp = create_codonp(codon.nuclt);
  imm_codon_lprob_set(&codonp, codon, lprob);
  return imm_codon_marg(&codonp);
}

void imm_ex2_init(unsigned core_size, struct imm_span span)
{
  assert(core_size > 0);
  assert(core_size <= IMM_EX2_SIZE);
  struct imm_ex2 *m = &imm_ex2;
  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;
  imm_code_init(&m->code, abc);
  imm_hmm_init(&m->hmm, &m->code);
  m->core_size = core_size;

  float eps = (float)0.01;
  m->nucltp = imm_nuclt_lprob(
      nuclt, (float[]){log(0.25), log(0.25), log(0.45), log(0.05)});

  m->m_marg = codonm(IMM_CODON(nuclt, "ACG"), log(100.0));
  m->i_marg = codonm(IMM_CODON(nuclt, "CGT"), log(100.0));
  m->b_marg = codonm(IMM_CODON(nuclt, "AAA"), log(100.0));
  m->e_marg = codonm(IMM_CODON(nuclt, "CCC"), log(100.0));
  m->j_marg = codonm(IMM_CODON(nuclt, "GGG"), log(100.0));
  m->null.n_marg = codonm(IMM_CODON(nuclt, "GTG"), log(13.0));

  imm_mute_state_init(&m->start, START, abc);
  imm_hmm_add_state(&m->hmm, &m->start.super);
  imm_hmm_set_start(&m->hmm, &m->start.super, log(1.0));

  imm_mute_state_init(&m->end, END, abc);
  imm_hmm_add_state(&m->hmm, &m->end.super);

  imm_frame_state_init(&m->b, B, &m->nucltp, &m->b_marg, eps, span);
  imm_hmm_add_state(&m->hmm, &m->b.super);

  imm_frame_state_init(&m->e, E, &m->nucltp, &m->e_marg, eps, span);
  imm_hmm_add_state(&m->hmm, &m->e.super);

  imm_frame_state_init(&m->j, J, &m->nucltp, &m->j_marg, eps, span);
  imm_hmm_add_state(&m->hmm, &m->j.super);

  SET_TRANS(m->hmm, m->start, m->b, log(0.2));
  SET_TRANS(m->hmm, m->b, m->b, log(0.2));
  SET_TRANS(m->hmm, m->e, m->e, log(0.2));
  SET_TRANS(m->hmm, m->j, m->j, log(0.2));
  SET_TRANS(m->hmm, m->e, m->j, log(0.2));
  SET_TRANS(m->hmm, m->j, m->b, log(0.2));
  SET_TRANS(m->hmm, m->e, m->end, log(0.2));

  for (unsigned k = 0; k < core_size; ++k)
  {
    imm_frame_state_init(m->m + k, M | k, &m->nucltp, &m->m_marg, eps, span);
    imm_frame_state_init(m->i + k, I | k, &m->nucltp, &m->i_marg, eps, span);
    imm_mute_state_init(m->d + k, D | k, abc);

    imm_hmm_add_state(&m->hmm, &m->m[k].super);
    imm_hmm_add_state(&m->hmm, &m->i[k].super);
    imm_hmm_add_state(&m->hmm, &m->d[k].super);

    if (k == 0) SET_TRANS(m->hmm, m->b, m->m[0], log(0.2));

    SET_TRANS(m->hmm, m->m[k], m->i[k], log(0.2));
    SET_TRANS(m->hmm, m->i[k], m->i[k], log(0.2));

    if (k > 0)
    {
      SET_TRANS(m->hmm, m->m[k - 1], m->m[k], log(0.2));
      SET_TRANS(m->hmm, m->d[k - 1], m->m[k], log(0.2));
      SET_TRANS(m->hmm, m->i[k - 1], m->m[k], log(0.2));

      SET_TRANS(m->hmm, m->m[k - 1], m->d[k], log(0.2));
      SET_TRANS(m->hmm, m->d[k - 1], m->d[k], log(0.2));
    }

    if (k == core_size - 1)
    {
      SET_TRANS(m->hmm, m->m[k], m->e, log(0.2));
      SET_TRANS(m->hmm, m->d[k], m->e, log(0.2));
      SET_TRANS(m->hmm, m->i[k], m->e, log(0.2));
    }
  }

  imm_hmm_init(&m->null.hmm, &m->code);
  imm_frame_state_init(&m->null.n, N, &m->nucltp, &m->null.n_marg, eps, span);
  imm_hmm_add_state(&m->null.hmm, &m->null.n.super);
  imm_hmm_set_start(&m->null.hmm, &m->null.n.super, log(1.0));
  SET_TRANS(m->null.hmm, m->null.n, m->null.n, log(0.2));
}

char const imm_ex2_seq[] = "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
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
