#include "frame_cond.h"
#include "logsum.h"
#include "lprob.h"

struct imm_frame_cond imm_frame_cond(struct imm_frame_state const *s)
{
  return (struct imm_frame_cond){s->eps, s->nucltp, s->codonm};
}

static float lprob_frag_given_codon1(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon);

static float lprob_frag_given_codon2(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon);

static float lprob_frag_given_codon3(struct imm_frame_cond const *c,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon);

static float lprob_frag_given_codon4(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon);

static float lprob_frag_given_codon5(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon);

float imm_frame_cond_decode(struct imm_frame_cond const *cond,
                            struct imm_seq const *seq, struct imm_codon *codon)
{
  int const n = imm_nuclt_size(cond->nucltp->nuclt);

  float max_lprob = imm_lprob_zero();
  struct imm_codon tmp;
  tmp.nuclt = cond->nucltp->nuclt;

  for (int i0 = 0; i0 < n; ++i0)
  {
    for (int i1 = 0; i1 < n; ++i1)
    {
      for (int i2 = 0; i2 < n; ++i2)
      {

        tmp.a = i0;
        tmp.b = i1;
        tmp.c = i2;
        float lprob = imm_frame_cond_lprob(cond, &tmp, seq);

        if (lprob >= max_lprob)
        {
          max_lprob = lprob;
          codon->a = tmp.a;
          codon->b = tmp.b;
          codon->c = tmp.c;
        }
      }
    }
  }
  return max_lprob;
}

float imm_frame_cond_loglik(struct imm_frame_cond const *cond,
                            struct imm_codon const *codon,
                            struct imm_seq const *seq)
{
  float lprob = imm_lprob_zero();

  switch (imm_seq_size(seq))
  {
  case 1: lprob = lprob_frag_given_codon1(cond, seq, codon); break;
  case 2: lprob = lprob_frag_given_codon2(cond, seq, codon); break;
  case 3: lprob = lprob_frag_given_codon3(cond, seq, codon); break;
  case 4: lprob = lprob_frag_given_codon4(cond, seq, codon); break;
  case 5: lprob = lprob_frag_given_codon5(cond, seq, codon); break;
  default: return imm_lprob_nan();
  }

  return lprob;
}

float imm_frame_cond_lprob(struct imm_frame_cond const *cond,
                           struct imm_codon const *codon,
                           struct imm_seq const *seq)
{
  float loglik = imm_frame_cond_loglik(cond, codon, seq);
  if (!imm_lprob_is_nan(loglik))
    loglik += imm_codon_marg_lprob(cond->codonm, *codon);
  return loglik;
}

static inline float logaddexp3(float const a, float const b, float const c)
{
  return imm_lprob_add(imm_lprob_add(a, b), c);
}

static float lprob_frag_given_codon1(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  int const *x = codon->idx;

  int const z[1] = {imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[0])};

  float c = 2 * cond->epsilon.loge + 2 * cond->epsilon.log1e;

  return c + logf((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) - logf(3);
}

static float lprob_frag_given_codon2(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  int const *x = codon->idx;

  int z[2] = {imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[0]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[1])};

  float lprob_z1 = imm__nuclt_lprob_get(cond->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(cond->nucltp, z[1]);

  float c1 = logf(2) + cond->epsilon.loge + cond->epsilon.log1e * 3 - logf(3);
  float v0 = c1 + logf((x[1] == z[0]) * (x[2] == z[1]) +
                       (x[0] == z[0]) * (x[2] == z[1]) +
                       (x[0] == z[0]) * (x[1] == z[1]));

  float c2 = 3 * cond->epsilon.loge + cond->epsilon.log1e - logf(3);

  float v1 =
      c2 + logf((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) + lprob_z2;
  float v2 =
      c2 + logf((x[0] == z[1]) + (x[1] == z[1]) + (x[2] == z[1])) + lprob_z1;

  return logaddexp3(v0, v1, v2);
}

static float lprob_frag_given_codon3(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  int const *x = codon->idx;

  int z[3] = {imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[0]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[1]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[2])};

  float lprob_z1 = imm__nuclt_lprob_get(cond->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(cond->nucltp, z[1]);
  float lprob_z3 = imm__nuclt_lprob_get(cond->nucltp, z[2]);

  float loge = cond->epsilon.loge;
  float log1e = cond->epsilon.log1e;

  float v0 = 4 * log1e + logf((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2]));

  float c1 = logf(4) + 2 * loge + 2 * log1e - logf(9);

  float v1 =
      c1 +
      logf((x[1] == z[1]) * (x[2] == z[2]) + (x[0] == z[1]) * (x[2] == z[2]) +
           (x[0] == z[1]) * (x[1] == z[2])) +
      lprob_z1;

  float v2 =
      c1 +
      logf((x[1] == z[0]) * (x[2] == z[2]) + (x[0] == z[0]) * (x[2] == z[2]) +
           (x[0] == z[0]) * (x[1] == z[2])) +
      lprob_z2;

  float v3 =
      c1 +
      logf((x[1] == z[0]) * (x[2] == z[1]) + (x[0] == z[0]) * (x[2] == z[1]) +
           (x[0] == z[0]) * (x[1] == z[1])) +
      lprob_z3;

  float c2 = 4 * loge - logf(9);

  float v4 = c2 + logf((x[0] == z[2]) + (x[1] == z[2]) + (x[2] == z[2])) +
             lprob_z1 + lprob_z2;
  float v5 = c2 + logf((x[0] == z[1]) + (x[1] == z[1]) + (x[2] == z[1])) +
             lprob_z1 + lprob_z3;
  float v6 = c2 + logf((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) +
             lprob_z2 + lprob_z3;

  return logsum(v0, v1, v2, v3, v4, v5, v6);
}

static float lprob_frag_given_codon4(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  int const *x = codon->idx;

  int z[4] = {imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[0]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[1]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[2]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[3])};

  float lprob_z1 = imm__nuclt_lprob_get(cond->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(cond->nucltp, z[1]);
  float lprob_z3 = imm__nuclt_lprob_get(cond->nucltp, z[2]);
  float lprob_z4 = imm__nuclt_lprob_get(cond->nucltp, z[3]);

  float v0 =
      logsum(logf((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[3])) + lprob_z1,
             logf((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[3])) + lprob_z2,
             logf((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[3])) + lprob_z3,
             logf((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2])) + lprob_z4);

  float v1 =
      logsum(logf((x[1] == z[2]) * (x[2] == z[3])) + lprob_z1 + lprob_z2,
             logf((x[1] == z[1]) * (x[2] == z[3])) + lprob_z1 + lprob_z3,
             logf((x[1] == z[1]) * (x[2] == z[2])) + lprob_z1 + lprob_z4,
             logf((x[1] == z[0]) * (x[2] == z[3])) + lprob_z2 + lprob_z3,
             logf((x[1] == z[0]) * (x[2] == z[2])) + lprob_z2 + lprob_z4,
             logf((x[1] == z[0]) * (x[2] == z[1])) + lprob_z3 + lprob_z4,
             logf((x[0] == z[2]) * (x[2] == z[3])) + lprob_z1 + lprob_z2,
             logf((x[0] == z[1]) * (x[2] == z[3])) + lprob_z1 + lprob_z3,
             logf((x[0] == z[1]) * (x[2] == z[2])) + lprob_z1 + lprob_z4,
             logf((x[0] == z[0]) * (x[2] == z[3])) + lprob_z2 + lprob_z3,
             logf((x[0] == z[0]) * (x[2] == z[2])) + lprob_z2 + lprob_z4,
             logf((x[0] == z[0]) * (x[2] == z[1])) + lprob_z3 + lprob_z4,
             logf((x[0] == z[2]) * (x[1] == z[3])) + lprob_z1 + lprob_z2,
             logf((x[0] == z[1]) * (x[1] == z[3])) + lprob_z1 + lprob_z3,
             logf((x[0] == z[1]) * (x[1] == z[2])) + lprob_z1 + lprob_z4,
             logf((x[0] == z[0]) * (x[1] == z[3])) + lprob_z2 + lprob_z3,
             logf((x[0] == z[0]) * (x[1] == z[2])) + lprob_z2 + lprob_z4,
             logf((x[0] == z[0]) * (x[1] == z[1])) + lprob_z3 + lprob_z4);

  return imm_lprob_add(
      cond->epsilon.loge + cond->epsilon.log1e * 3 - logf(2) + v0,
      3 * cond->epsilon.loge + cond->epsilon.log1e - logf(9) + v1);
}

static float lprob_frag_given_codon5(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  int const *x = codon->idx;

  int z[5] = {imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[0]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[1]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[2]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[3]),
              imm_abc_symbol_idx(seq->abc, imm_seq_data(seq)[4])};

  float lprob_z1 = imm__nuclt_lprob_get(cond->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(cond->nucltp, z[1]);
  float lprob_z3 = imm__nuclt_lprob_get(cond->nucltp, z[2]);
  float lprob_z4 = imm__nuclt_lprob_get(cond->nucltp, z[3]);
  float lprob_z5 = imm__nuclt_lprob_get(cond->nucltp, z[4]);

  float v =
      logsum(lprob_z1 + lprob_z2 +
                 logf((x[0] == z[2]) * (x[1] == z[3]) * (x[2] == z[4])),
             lprob_z1 + lprob_z3 +
                 logf((x[0] == z[1]) * (x[1] == z[3]) * (x[2] == z[4])),
             lprob_z1 + lprob_z4 +
                 logf((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[4])),
             lprob_z1 + lprob_z5 +
                 logf((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[3])),
             lprob_z2 + lprob_z3 +
                 logf((x[0] == z[0]) * (x[1] == z[3]) * (x[2] == z[4])),
             lprob_z2 + lprob_z4 +
                 logf((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[4])),
             lprob_z2 + lprob_z5 +
                 logf((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[3])),
             lprob_z3 + lprob_z4 +
                 logf((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[4])),
             lprob_z3 + lprob_z5 +
                 logf((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[3])),
             lprob_z4 + lprob_z5 +
                 logf((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2])), );

  return 2 * cond->epsilon.loge + 2 * cond->epsilon.log1e - logf(10) + v;
}
