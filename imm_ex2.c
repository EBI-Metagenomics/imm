#include "imm_ex2.h"
#include "imm_abc.h"
#include "imm_code.h"
#include "imm_codon_lprob.h"
#include "imm_codon_marg.h"
#include "imm_dna.h"
#include "imm_frame_state.h"
#include "imm_hmm.h"
#include "imm_mute_state.h"
#include "imm_normal_state.h"
#include <assert.h>

/* State IDs */
#define START (0 << 11)
#define B (1 << 11)
#define M (2 << 11)
#define I (3 << 11)
#define D (4 << 11)
#define E (5 << 11)
#define J (6 << 11)
#define END (7 << 11)
#define N (8 << 11)
#define NSTART (9 << 11)
#define NEND (10 << 11)

#define SET_TRANS(hmm, a, b, v) imm_hmm_set_trans(hmm, &a.super, &b.super, v)

#define SET_CODONP(codonp, codon, v) imm_codon_lprob_set(codonp, codon, v)

struct imm_ex2 imm_ex2 = {.dna = &imm_dna_iupac};

static struct imm_codon_lprob create_codonp(struct imm_nuclt const *nuclt)
{
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAA"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAC"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AAT"), logf(0.0023173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ACA"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ACG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ACT"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AGC"), logf(0.0029114f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AGG"), logf(0.0029003f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "AGT"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ATA"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ATG"), logf(0.0049178f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "ATT"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAA"), logf(0.0029478f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAC"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAG"), logf(0.0029123f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CAT"), logf(0.0009133f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CCA"), logf(0.0029179f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CCG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CCT"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CGC"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CGG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CGT"), logf(0.0041183f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CTA"), logf(0.0038173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CTG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "CTT"), logf(0.0029111f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAA"), logf(0.0019173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAC"), logf(0.0029103f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GAT"), logf(0.0029103f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GCA"), logf(0.0003138f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GCG"), logf(0.0039173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GCT"), logf(0.0029103f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GGC"), logf(0.0019173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GGG"), logf(0.0009173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GGT"), logf(0.0029143f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GTA"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GTG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "GTT"), logf(0.0029171f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAA"), logf(0.0099113f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAC"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TAT"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TCA"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TCG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TCT"), logf(0.0020173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TGC"), logf(0.0029193f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TGG"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TGT"), logf(0.0029173f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TTA"), logf(0.0089193f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TTG"), logf(0.0029138f));
  SET_CODONP(&codonp, IMM_CODON(nuclt, "TTT"), logf(0.0089183f));
  return codonp;
}

static struct imm_codon_marg codonm(struct imm_codon codon, float lprob)
{
  struct imm_codon_lprob codonp = create_codonp(codon.nuclt);
  imm_codon_lprob_set(&codonp, codon, lprob);
  return imm_codon_marg(&codonp);
}

void imm_ex2_init(int core_size, struct imm_span span)
{
  assert(core_size > 0);
  assert(core_size <= IMM_EX2_SIZE);
  struct imm_ex2 *m = &imm_ex2;
  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;
  imm_code_init(&m->code, abc);
  m->hmm = imm_hmm_new(&m->code);
  m->core_size = core_size;

  float eps = 0.01f;
  m->nucltp = imm_nuclt_lprob(
      nuclt, (float[]){logf(0.25f), logf(0.25f), logf(0.45f), logf(0.05f)});

  m->m_marg = codonm(IMM_CODON(nuclt, "ACG"), logf(100.0));
  m->i_marg = codonm(IMM_CODON(nuclt, "CGT"), logf(100.0));
  m->b_marg = codonm(IMM_CODON(nuclt, "AAA"), logf(100.0));
  m->e_marg = codonm(IMM_CODON(nuclt, "CCC"), logf(100.0));
  m->j_marg = codonm(IMM_CODON(nuclt, "GGG"), logf(100.0));
  m->null.n_marg = codonm(IMM_CODON(nuclt, "GTG"), logf(13.0));

  imm_mute_state_init(&m->start, START, abc);
  imm_hmm_add_state(m->hmm, &m->start.super);
  imm_hmm_set_start(m->hmm, &m->start);

  imm_mute_state_init(&m->end, END, abc);
  imm_hmm_add_state(m->hmm, &m->end.super);
  imm_hmm_set_end(m->hmm, &m->end);

  imm_frame_state_init(&m->b, B, &m->nucltp, &m->b_marg, eps, span);
  imm_hmm_add_state(m->hmm, &m->b.super);

  imm_frame_state_init(&m->e, E, &m->nucltp, &m->e_marg, eps, span);
  imm_hmm_add_state(m->hmm, &m->e.super);

  imm_frame_state_init(&m->j, J, &m->nucltp, &m->j_marg, eps, span);
  imm_hmm_add_state(m->hmm, &m->j.super);

  SET_TRANS(m->hmm, m->start, m->b, logf(0.2f));
  SET_TRANS(m->hmm, m->b, m->b, logf(0.2f));
  SET_TRANS(m->hmm, m->e, m->e, logf(0.2f));
  SET_TRANS(m->hmm, m->j, m->j, logf(0.2f));
  SET_TRANS(m->hmm, m->e, m->j, logf(0.2f));
  SET_TRANS(m->hmm, m->j, m->b, logf(0.2f));
  SET_TRANS(m->hmm, m->e, m->end, logf(0.2f));

  for (int k = 0; k < core_size; ++k)
  {
    imm_frame_state_init(m->m + k, M | k, &m->nucltp, &m->m_marg, eps, span);
    imm_frame_state_init(m->i + k, I | k, &m->nucltp, &m->i_marg, eps, span);
    imm_mute_state_init(m->d + k, D | k, abc);

    imm_hmm_add_state(m->hmm, &m->m[k].super);
    imm_hmm_add_state(m->hmm, &m->i[k].super);
    imm_hmm_add_state(m->hmm, &m->d[k].super);

    if (k == 0) SET_TRANS(m->hmm, m->b, m->m[0], logf(0.2f));

    SET_TRANS(m->hmm, m->m[k], m->i[k], logf(0.2f));
    SET_TRANS(m->hmm, m->i[k], m->i[k], logf(0.2f));

    if (k > 0)
    {
      SET_TRANS(m->hmm, m->m[k - 1], m->m[k], logf(0.2f));
      SET_TRANS(m->hmm, m->d[k - 1], m->m[k], logf(0.2f));
      SET_TRANS(m->hmm, m->i[k - 1], m->m[k], logf(0.2f));

      SET_TRANS(m->hmm, m->m[k - 1], m->d[k], logf(0.2f));
      SET_TRANS(m->hmm, m->d[k - 1], m->d[k], logf(0.2f));
    }

    if (k == core_size - 1)
    {
      SET_TRANS(m->hmm, m->m[k], m->e, logf(0.2f));
      SET_TRANS(m->hmm, m->d[k], m->e, logf(0.2f));
      SET_TRANS(m->hmm, m->i[k], m->e, logf(0.2f));
    }
  }

  // Null one
  m->null.hmm = imm_hmm_new(&m->code);

  imm_mute_state_init(&m->null.nstart, NSTART, abc);
  imm_mute_state_init(&m->null.nend, NEND, abc);
  imm_frame_state_init(&m->null.n, N, &m->nucltp, &m->null.n_marg, eps, span);

  imm_hmm_add_state(m->null.hmm, &m->null.nstart.super);
  imm_hmm_add_state(m->null.hmm, &m->null.n.super);
  imm_hmm_add_state(m->null.hmm, &m->null.nend.super);

  imm_hmm_set_trans(m->null.hmm, &m->null.nstart.super, &m->null.n.super, 0);
  imm_hmm_set_trans(m->null.hmm, &m->null.n.super, &m->null.nend.super, 0);
  SET_TRANS(m->null.hmm, m->null.n, m->null.n, logf(0.2f));

  imm_hmm_set_start(m->null.hmm, &m->null.nstart);
  imm_hmm_set_end(m->null.hmm, &m->null.nend);
}

void imm_ex2_cleanup(void)
{
  imm_hmm_del(imm_ex2.hmm);
  imm_hmm_del(imm_ex2.null.hmm);
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
