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
  unsigned const n = imm_nuclt_size(cond->nucltp->nuclt);

  float max_lprob = imm_lprob_zero();
  struct imm_codon tmp;
  tmp.nuclt = cond->nucltp->nuclt;

  for (unsigned i0 = 0; i0 < n; ++i0)
  {
    for (unsigned i1 = 0; i1 < n; ++i1)
    {
      for (unsigned i2 = 0; i2 < n; ++i2)
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
  case 1:
    lprob = lprob_frag_given_codon1(cond, seq, codon);
    break;
  case 2:
    lprob = lprob_frag_given_codon2(cond, seq, codon);
    break;
  case 3:
    lprob = lprob_frag_given_codon3(cond, seq, codon);
    break;
  case 4:
    lprob = lprob_frag_given_codon4(cond, seq, codon);
    break;
  case 5:
    lprob = lprob_frag_given_codon5(cond, seq, codon);
    break;
  default:
    return imm_lprob_nan();
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
  unsigned const *x = codon->idx;

  unsigned const z[1] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0])};

  float c = 2 * cond->eps.loge + 2 * cond->eps.log1e;

  return c + log((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) - log(3);
}

static float lprob_frag_given_codon2(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  unsigned const *x = codon->idx;

  unsigned z[2] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1])};

  float lprob_z1 = imm__nuclt_lprob_get(cond->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(cond->nucltp, z[1]);

  float c1 = log(2) + cond->eps.loge + cond->eps.log1e * 3 - log(3);
  float v0 = c1 + log((x[1] == z[0]) * (x[2] == z[1]) +
                      (x[0] == z[0]) * (x[2] == z[1]) +
                      (x[0] == z[0]) * (x[1] == z[1]));

  float c2 = 3 * cond->eps.loge + cond->eps.log1e - log(3);

  float v1 =
      c2 + log((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) + lprob_z2;
  float v2 =
      c2 + log((x[0] == z[1]) + (x[1] == z[1]) + (x[2] == z[1])) + lprob_z1;

  return logaddexp3(v0, v1, v2);
}

static float lprob_frag_given_codon3(struct imm_frame_cond const *c,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  unsigned const *x = codon->idx;

  unsigned z[3] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[2])};

  float lprob_z1 = imm__nuclt_lprob_get(c->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(c->nucltp, z[1]);
  float lprob_z3 = imm__nuclt_lprob_get(c->nucltp, z[2]);

  float v0 =
      4 * c->eps.log1e + log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2]));

  float c1 = log(4) + 2 * c->eps.loge + 2 * c->eps.log1e - log(9);

  float v1 =
      c1 +
      log((x[1] == z[1]) * (x[2] == z[2]) + (x[0] == z[1]) * (x[2] == z[2]) +
          (x[0] == z[1]) * (x[1] == z[2])) +
      lprob_z1;

  float v2 =
      c1 +
      log((x[1] == z[0]) * (x[2] == z[2]) + (x[0] == z[0]) * (x[2] == z[2]) +
          (x[0] == z[0]) * (x[1] == z[2])) +
      lprob_z2;

  float v3 =
      c1 +
      log((x[1] == z[0]) * (x[2] == z[1]) + (x[0] == z[0]) * (x[2] == z[1]) +
          (x[0] == z[0]) * (x[1] == z[1])) +
      lprob_z3;

  float c2 = 4 * c->eps.loge - log(9);

  float v4 = c2 + log((x[0] == z[2]) + (x[1] == z[2]) + (x[2] == z[2])) +
             lprob_z1 + lprob_z2;
  float v5 = c2 + log((x[0] == z[1]) + (x[1] == z[1]) + (x[2] == z[1])) +
             lprob_z1 + lprob_z3;
  float v6 = c2 + log((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) +
             lprob_z2 + lprob_z3;

  return logsum(v0, v1, v2, v3, v4, v5, v6);
}

static float lprob_frag_given_codon4(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  unsigned const *x = codon->idx;

  unsigned z[4] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[2]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[3])};

  float lprob_z1 = imm__nuclt_lprob_get(cond->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(cond->nucltp, z[1]);
  float lprob_z3 = imm__nuclt_lprob_get(cond->nucltp, z[2]);
  float lprob_z4 = imm__nuclt_lprob_get(cond->nucltp, z[3]);

  float v0 =
      logsum(log((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[3])) + lprob_z1,
             log((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[3])) + lprob_z2,
             log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[3])) + lprob_z3,
             log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2])) + lprob_z4);

  float v1 = logsum(log((x[1] == z[2]) * (x[2] == z[3])) + lprob_z1 + lprob_z2,
                    log((x[1] == z[1]) * (x[2] == z[3])) + lprob_z1 + lprob_z3,
                    log((x[1] == z[1]) * (x[2] == z[2])) + lprob_z1 + lprob_z4,
                    log((x[1] == z[0]) * (x[2] == z[3])) + lprob_z2 + lprob_z3,
                    log((x[1] == z[0]) * (x[2] == z[2])) + lprob_z2 + lprob_z4,
                    log((x[1] == z[0]) * (x[2] == z[1])) + lprob_z3 + lprob_z4,
                    log((x[0] == z[2]) * (x[2] == z[3])) + lprob_z1 + lprob_z2,
                    log((x[0] == z[1]) * (x[2] == z[3])) + lprob_z1 + lprob_z3,
                    log((x[0] == z[1]) * (x[2] == z[2])) + lprob_z1 + lprob_z4,
                    log((x[0] == z[0]) * (x[2] == z[3])) + lprob_z2 + lprob_z3,
                    log((x[0] == z[0]) * (x[2] == z[2])) + lprob_z2 + lprob_z4,
                    log((x[0] == z[0]) * (x[2] == z[1])) + lprob_z3 + lprob_z4,
                    log((x[0] == z[2]) * (x[1] == z[3])) + lprob_z1 + lprob_z2,
                    log((x[0] == z[1]) * (x[1] == z[3])) + lprob_z1 + lprob_z3,
                    log((x[0] == z[1]) * (x[1] == z[2])) + lprob_z1 + lprob_z4,
                    log((x[0] == z[0]) * (x[1] == z[3])) + lprob_z2 + lprob_z3,
                    log((x[0] == z[0]) * (x[1] == z[2])) + lprob_z2 + lprob_z4,
                    log((x[0] == z[0]) * (x[1] == z[1])) + lprob_z3 + lprob_z4);

  return imm_lprob_add(cond->eps.loge + cond->eps.log1e * 3 - log(2) + v0,
                       3 * cond->eps.loge + cond->eps.log1e - log(9) + v1);
}

static float lprob_frag_given_codon5(struct imm_frame_cond const *cond,
                                     struct imm_seq const *seq,
                                     struct imm_codon const *codon)
{
  unsigned const *x = codon->idx;

  unsigned z[5] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[2]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[3]),
                   imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[4])};

  float lprob_z1 = imm__nuclt_lprob_get(cond->nucltp, z[0]);
  float lprob_z2 = imm__nuclt_lprob_get(cond->nucltp, z[1]);
  float lprob_z3 = imm__nuclt_lprob_get(cond->nucltp, z[2]);
  float lprob_z4 = imm__nuclt_lprob_get(cond->nucltp, z[3]);
  float lprob_z5 = imm__nuclt_lprob_get(cond->nucltp, z[4]);

  float v = logsum(lprob_z1 + lprob_z2 +
                       log((x[0] == z[2]) * (x[1] == z[3]) * (x[2] == z[4])),
                   lprob_z1 + lprob_z3 +
                       log((x[0] == z[1]) * (x[1] == z[3]) * (x[2] == z[4])),
                   lprob_z1 + lprob_z4 +
                       log((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[4])),
                   lprob_z1 + lprob_z5 +
                       log((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[3])),
                   lprob_z2 + lprob_z3 +
                       log((x[0] == z[0]) * (x[1] == z[3]) * (x[2] == z[4])),
                   lprob_z2 + lprob_z4 +
                       log((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[4])),
                   lprob_z2 + lprob_z5 +
                       log((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[3])),
                   lprob_z3 + lprob_z4 +
                       log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[4])),
                   lprob_z3 + lprob_z5 +
                       log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[3])),
                   lprob_z4 + lprob_z5 +
                       log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2])), );

  return 2 * cond->eps.loge + 2 * cond->eps.log1e - log(10) + v;
}
