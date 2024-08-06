#include "dp.h"
#include "clock.h"
#include "dp_cfg.h"
#include "emis.h"
#include "fmt.h"
#include "likely.h"
#include "lite_pack.h"
#include "lite_pack_io.h"
#include "lprob.h"
#include "matrix.h"
#include "min.h"
#include "prod.h"
#include "reallocf.h"
#include "state.h"
#include "state_table.h"
#include "task.h"
#include "trans_table.h"
#include "viterbi.h"
#include "zspan.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

void imm_dp_init(struct imm_dp *dp, struct imm_code const *code)
{
  dp->code = code;
  imm_emis_init(&dp->emis);
  imm_trans_table_init(&dp->trans_table);
  imm_state_table_init(&dp->state_table);
}

void imm_dp_cleanup(struct imm_dp *dp)
{
  if (dp)
  {
    imm_emis_cleanup(&dp->emis);
    imm_trans_table_cleanup(&dp->trans_table);
    imm_state_table_cleanup(&dp->state_table);
  }
}

int imm_dp_reset(struct imm_dp *dp, struct imm_dp_cfg const *cfg)
{
  int rc = 0;

  if ((rc = imm_emis_reset(&dp->emis, dp->code, cfg->states, cfg->nstates)))
    return rc;

  if ((rc = imm_trans_table_reset(&dp->trans_table, cfg))) return rc;

  if ((rc = imm_state_table_reset(&dp->state_table, cfg))) return rc;

  return rc;
}

void imm_dp_set_state_name(struct imm_dp *x, imm_state_name *callb)
{
  imm_state_table_debug_setup(&x->state_table, callb);
}

static float read_result(struct imm_dp const *dp, struct imm_task *task,
                         int *last_state, int *last_seqsize)
{
  float score = imm_lprob_zero();
  int end = dp->state_table.end_state_idx;

  int state = IMM_STATE_NULL_IDX;
  int seqsize = IMM_STATE_NULL_SEQSIZE;

  int length = imm_eseq_size(task->seq);
  int max_seq = imm_zspan_max(imm_state_table_zspan(&dp->state_table, end));

  for (int len = imm_min(max_seq, length);; --len)
  {
    struct imm_cell cell =
        imm_cell(length - len, (int_fast16_t)end, (int_fast8_t)len);
    float v = imm_matrix_get_score(&task->matrix, cell);
    if (v > score)
    {
      score = v;
      state = end;
      seqsize = len;
    }

    if (imm_zspan_min(imm_state_table_zspan(&dp->state_table, end)) == len)
      break;
  }
  if (state != end) score = imm_lprob_nan();

  *last_state = state;
  *last_seqsize = seqsize;
  return score;
}

static void unzip_path(struct imm_trellis *x, struct imm_state_table const *st,
                       int seq_size, struct imm_path *path)
{
  imm_trellis_seek(x, seq_size, st->end_state_idx);
  if (imm_lprob_is_nan(imm_trellis_head(x)->score)) return;

  int size = 0;
  int start_state = st->start_state_idx;
  while (imm_trellis_state_idx(x) != start_state || imm_trellis_stage_idx(x))
  {
    int id = imm_state_table_id(st, imm_trellis_state_idx(x));
    float score = imm_trellis_head(x)->score;
    imm_path_add(path, imm_step(id, size, score));

    size = imm_trellis_head(x)->emission_size;
    imm_trellis_back(x);
  }
  int id = imm_state_table_id(st, imm_trellis_state_idx(x));
  imm_path_add(path, imm_step(id, size, 0));
  imm_path_reverse(path);
}

int imm_dp_viterbi(struct imm_dp const *x, struct imm_task *task,
                   struct imm_prod *prod)
{
  imm_prod_reset(prod);
  if (!task->seq) return IMM_ENOSEQ;

  if (x->code->abc != imm_eseq_abc(task->seq)) return IMM_EDIFFABC;

  int end_state = x->state_table.end_state_idx;
  int min = imm_zspan_min(imm_state_table_zspan(&x->state_table, end_state));
  if (imm_eseq_size(task->seq) < min) return IMM_ESHORTSEQ;

  long start = imm_clock();

  struct imm_viterbi viterbi = {0};
  imm_viterbi_init(&viterbi, x, task);

  imm_viterbi_run(&viterbi);

  int last_state = 0;
  int last_seqsize = 0;
  prod->loglik = read_result(x, task, &last_state, &last_seqsize);

  int rc = 0;

  imm_path_reset(&prod->path);
  int seqsize = imm_eseq_size(task->seq);
  unzip_path(&task->trellis, &x->state_table, seqsize, &prod->path);
  return rc;

  prod->mseconds = (uint64_t)(imm_clock() - start);

  return rc;
}

int imm_dp_nstates(struct imm_dp const *dp) { return dp->state_table.nstates; }

int imm_dp_trans_idx(struct imm_dp *dp, int src_idx, int dst_idx)
{
  return imm_trans_table_idx(&dp->trans_table, src_idx, dst_idx);
}

int imm_dp_change_trans(struct imm_dp *dp, int trans_idx, float lprob)
{
  if (imm_unlikely(imm_lprob_is_nan(lprob))) return IMM_EINVAL;

  imm_trans_table_change(&dp->trans_table, trans_idx, lprob);
  return 0;
}

float imm_dp_emis_score(struct imm_dp const *dp, int state_id,
                        struct imm_seq const *seq)
{
  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, dp->code);
  float score = imm_lprob_nan();
  if (imm_eseq_setup(&eseq, seq)) goto cleanup;

  int state_idx = imm_state_table_idx(&dp->state_table, state_id);
  int min = imm_zspan_min(imm_state_table_zspan(&dp->state_table, state_idx));

  int seq_code = imm_eseq_get(&eseq, 0, imm_seq_size(seq), min);
  score = imm_emis_score(&dp->emis, state_idx, seq_code);

cleanup:
  imm_eseq_cleanup(&eseq);
  return score;
}

float const *imm_dp_emis_table(struct imm_dp const *dp, int state_id, int *size)
{
  int state_idx = imm_state_table_idx(&dp->state_table, state_id);
  return imm_emis_table(&dp->emis, state_idx, size);
}

float imm_dp_trans_score(struct imm_dp const *dp, int src, int dst)
{
  int src_idx = imm_state_table_idx(&dp->state_table, src);
  int dst_idx = imm_state_table_idx(&dp->state_table, dst);

  if (src_idx == INT_MAX || dst_idx == INT_MAX) return imm_lprob_nan();

  for (int i = 0; i < dp->trans_table.ntrans; ++i)
  {
    if (imm_trans_table_source_state(&dp->trans_table, dst_idx, i) == src_idx)
      return imm_trans_table_score(&dp->trans_table, dst_idx, i);
  }
  return imm_lprob_nan();
}

void imm_dp_write_dot(struct imm_dp const *dp, FILE *restrict fp,
                      imm_state_name *name)
{
  fprintf(fp, "digraph dp {\n");
  for (int dst = 0; dst < dp->state_table.nstates; ++dst)
  {
    for (int t = 0; t < imm_trans_table_ntrans(&dp->trans_table, dst); ++t)
    {
      int src = imm_trans_table_source_state(&dp->trans_table, dst, t);

      char src_name[IMM_STATE_NAME_SIZE] = {0};
      char dst_name[IMM_STATE_NAME_SIZE] = {0};

      (*name)(imm_state_table_id(&dp->state_table, src), src_name);
      (*name)(imm_state_table_id(&dp->state_table, dst), dst_name);
      fprintf(fp, "%s -> %s [label=", src_name, dst_name);
      fprintf(fp, imm_fmt_get_f32(),
              imm_trans_table_score(&dp->trans_table, dst, t));
      fprintf(fp, "];\n");
    }
  }
  fprintf(fp, "}\n");
}

void imm_dp_dump(struct imm_dp const *x, FILE *restrict fp)
{
  fprintf(fp, "# emission\n\n");
  imm_emis_dump(&x->emis, &x->state_table, fp);
  fprintf(fp, "\n");

  fprintf(fp, "# trans_table\n\n");
  imm_trans_table_dump(&x->trans_table, &x->state_table, fp);
  fprintf(fp, "\n");

  fprintf(fp, "# state_table\n");
  imm_state_table_dump(&x->state_table, fp);
}

void imm_dp_dump_path(struct imm_dp const *x, struct imm_task const *t,
                      struct imm_prod const *prod, struct imm_seq const *seq,
                      FILE *restrict fp)
{
  int begin = 0;
  for (int i = 0; i < imm_path_nsteps(&prod->path); ++i)
  {
    struct imm_step const *step = imm_path_step(&prod->path, i);
    int idx = imm_state_table_idx(&x->state_table, step->state_id);

    int min = imm_zspan_min(imm_state_table_zspan(&x->state_table, idx));
    int seq_code = imm_eseq_get(t->seq, begin, step->seqsize, min);

    float score = imm_emis_score(&x->emis, idx, seq_code);
    struct imm_range range = imm_range(begin, begin + step->seqsize);
    struct imm_seq subseq = imm_seq_slice(seq, range);
    fprintf(fp, "<%s,%.*s,%.4f>\n", imm_state_table_name(&x->state_table, idx),
            subseq.str.size, subseq.str.data, score);
    begin += step->seqsize;
  }
}

#define KEY_EMIS_SCORE "emis_score"
#define KEY_EMIS_OFFSET "emis_offset"
#define KEY_TRANS_SIZE "trans_size"
#define KEY_TRANS_SCORE "trans_score"
#define KEY_TRANS_SRC "trans_src"
#define KEY_TRANS_DST "trans_dst"
#define KEY_TRANS_OFFSET "trans_offset"
#define KEY_STATE_SIZE "state_size"
#define KEY_STATE_IDS "state_ids"
#define KEY_STATE_START "state_start"
#define KEY_STATE_LPROB "state_lprob"
#define KEY_STATE_END "state_end"
#define KEY_STATE_SPAN "state_span"

static int write_cstring(struct lio_writer *x, char const *string)
{
  uint32_t length = (uint32_t)strlen(string);
  if (lio_write(x, lip_pack_string(lio_alloc(x), length))) return 1;
  if (lio_writeb(x, length, string)) return 1;
  return 0;
}

int imm_dp_pack(struct imm_dp const *dp, struct lio_writer *f)
{
  unsigned size = 0;
  int nstates = dp->state_table.nstates;
  int ntrans = dp->trans_table.ntrans;

  if (lio_write(f, lip_pack_map(lio_alloc(f), 10))) return IMM_EIO;

  /* emission */
  if (write_cstring(f, KEY_EMIS_SCORE)) return IMM_EIO;
  size = (unsigned)imm_emis_score_size(&dp->emis, nstates);
  if (lio_write(f, lip_pack_array(lio_alloc(f), size))) return IMM_EIO;
  for (unsigned i = 0; i < size; ++i)
    if (lio_write(f, lip_pack_float(lio_alloc(f), dp->emis.score[i]))) return IMM_EIO;

  if (write_cstring(f, KEY_EMIS_OFFSET)) return IMM_EIO;
  unsigned offset_size = (unsigned)imm_emis_offset_size(nstates);
  if (lio_write(f, lip_pack_array(lio_alloc(f), offset_size))) return IMM_EIO;
  for (unsigned i = 0; i < offset_size; ++i)
    if (lio_write(f, lip_pack_int(lio_alloc(f), dp->emis.offset[i]))) return IMM_EIO;

  /* trans_table */
  size = (unsigned)imm_trans_table_transsize(ntrans);
  if (write_cstring(f, KEY_TRANS_SCORE)) return IMM_EIO;
  if (lio_write(f, lip_pack_array(lio_alloc(f), size))) return IMM_EIO;
  for (unsigned i = 0; i < size; ++i)
    if (lio_write(f, lip_pack_float(lio_alloc(f), dp->trans_table.trans[i].score))) return IMM_EIO;

  size = (unsigned)imm_trans_table_transsize(ntrans);
  if (write_cstring(f, KEY_TRANS_SRC)) return IMM_EIO;
  if (lio_write(f, lip_pack_array(lio_alloc(f), size))) return IMM_EIO;
  for (unsigned i = 0; i < size; ++i)
    if (lio_write(f, lip_pack_int(lio_alloc(f), dp->trans_table.trans[i].src))) return IMM_EIO;

  size = (unsigned)imm_trans_table_transsize(ntrans);
  if (write_cstring(f, KEY_TRANS_DST)) return IMM_EIO;
  if (lio_write(f, lip_pack_array(lio_alloc(f), size))) return IMM_EIO;
  for (unsigned i = 0; i < size; ++i)
    if (lio_write(f, lip_pack_int(lio_alloc(f), dp->trans_table.trans[i].dst))) return IMM_EIO;

  size = imm_trans_table_offsize(nstates);
  if (write_cstring(f, KEY_TRANS_OFFSET)) return IMM_EIO;
  if (lio_write(f, lip_pack_array(lio_alloc(f), size))) return IMM_EIO;
  for (unsigned i = 0; i < size; ++i)
    if (lio_write(f, lip_pack_int(lio_alloc(f), dp->trans_table.offset[i]))) return IMM_EIO;

  /* state_table */
  if (write_cstring(f, KEY_STATE_IDS)) return IMM_EIO;
  if (lio_write(f, lip_pack_array(lio_alloc(f), (uint32_t)nstates))) return IMM_EIO;
  for (int i = 0; i < nstates; ++i)
    if (lio_write(f, lip_pack_int(lio_alloc(f), dp->state_table.ids[i]))) return IMM_EIO;

  if (write_cstring(f, KEY_STATE_START)) return IMM_EIO;
  if (lio_write(f, lip_pack_int(lio_alloc(f), dp->state_table.start_state_idx))) return IMM_EIO;
  if (write_cstring(f, KEY_STATE_END)) return IMM_EIO;
  if (lio_write(f, lip_pack_int(lio_alloc(f), dp->state_table.end_state_idx))) return IMM_EIO;

  if (write_cstring(f, KEY_STATE_SPAN)) return IMM_EIO;
  if (lio_write(f, lip_pack_array(lio_alloc(f), (unsigned)nstates))) return IMM_EIO;
  for (int i = 0; i < nstates; ++i)
    if (lio_write(f, lip_pack_int(lio_alloc(f), dp->state_table.span[i]))) return IMM_EIO;

  if (lio_flush(f)) return IMM_EIO;

  return 0;
}

static int expect_key(struct lio_reader *x, char const *key)
{
  uint32_t size = 0;
  unsigned char buf[16] = {0};

  if (lio_free(x, lip_unpack_string(lio_read(x), &size))) return 1;
  if ((size_t)size > sizeof(buf)) return 1;

  if (lio_readb(x, size, buf)) return 1;
  if (size != (uint32_t)strlen(key)) return 1;
  return memcmp(key, buf, size) != 0;
}

int imm_dp_unpack(struct imm_dp *dp, struct lio_reader *f)
{
  uint32_t u32 = 0;
  struct imm_emis *e = &dp->emis;
  struct imm_trans_table *tt = &dp->trans_table;
  struct imm_state_table *st = &dp->state_table;

  if (lio_free(f, lip_unpack_map(lio_read(f), &u32))) return IMM_EIO;
  if (u32 != 10) return IMM_EIO;

  /* emission */
  if (expect_key(f, KEY_EMIS_SCORE)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  e->score = imm_reallocf(e->score, sizeof(*e->score) * u32);
  if (!e->score && u32 > 0) goto cleanup;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_float(lio_read(f), e->score + i))) goto cleanup;

  if (expect_key(f, KEY_EMIS_OFFSET)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  e->offset = imm_reallocf(e->offset, sizeof(*e->offset) * u32);
  if (!e->offset && u32 > 0) goto cleanup;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_int(lio_read(f), e->offset + i))) goto cleanup;

  /* trans_table */
  if (expect_key(f, KEY_TRANS_SCORE)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  tt->trans = imm_reallocf(tt->trans, sizeof(*tt->trans) * u32);
  if (!tt->trans && u32 > 0) goto cleanup;
  tt->ntrans = u32 - 1;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_float(lio_read(f), &tt->trans[i].score))) goto cleanup;

  if (expect_key(f, KEY_TRANS_SRC)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  if ((uint32_t)imm_trans_table_transsize(tt->ntrans) != u32) goto cleanup;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_int(lio_read(f), &tt->trans[i].src))) goto cleanup;

  if (expect_key(f, KEY_TRANS_DST)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  if ((uint32_t)imm_trans_table_transsize(tt->ntrans) != u32) goto cleanup;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_int(lio_read(f), &tt->trans[i].dst))) goto cleanup;

  if (expect_key(f, KEY_TRANS_OFFSET)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  tt->offset = imm_reallocf(tt->offset, sizeof(*tt->offset) * u32);
  if (!tt->offset && u32 > 0) goto cleanup;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_int(lio_read(f), tt->offset + i))) goto cleanup;

  /* state_table */
  if (expect_key(f, KEY_STATE_IDS)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  st->nstates = (uint32_t)u32;
  st->ids = imm_reallocf(st->ids, sizeof(*st->ids) * (unsigned)st->nstates);
  if (!st->ids && st->nstates > 0) goto cleanup;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_int(lio_read(f), st->ids + i))) goto cleanup;

  if (expect_key(f, KEY_STATE_START)) goto cleanup;
  if (lio_free(f, lip_unpack_int(lio_read(f), &dp->state_table.start_state_idx))) goto cleanup;
  if (expect_key(f, KEY_STATE_END)) goto cleanup;
  if (lio_free(f, lip_unpack_int(lio_read(f), &dp->state_table.end_state_idx))) goto cleanup;

  if (expect_key(f, KEY_STATE_SPAN)) goto cleanup;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) goto cleanup;
  if ((uint32_t)st->nstates != u32) goto cleanup;
  st->span = imm_reallocf(st->span, sizeof(*st->span) * u32);
  if (!st->span && u32 > 0) goto cleanup;
  for (uint32_t i = 0; i < u32; ++i)
    if (lio_free(f, lip_unpack_int(lio_read(f), st->span + i))) goto cleanup;

  return 0;

cleanup:
  if (e)
  {
    free(e->score);
    e->score = NULL;
    free(e->offset);
    e->offset = NULL;
  }
  if (tt)
  {
    free(tt->trans);
    tt->trans = NULL;
    free(tt->offset);
    tt->offset = NULL;
  }
  if (st)
  {
    free(st->ids);
    st->ids = NULL;
    free(st->span);
    st->span = NULL;
  }
  return IMM_EIO;
}
