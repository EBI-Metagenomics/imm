#include "dp.h"
#include "clock.h"
#include "dp_cfg.h"
#include "emis.h"
#include "ex1.h"
#include "expect.h"
#include "fmt.h"
#include "likely.h"
#include "lip/1darray/1darray.h"
#include "lip/lip.h"
#include "lprob.h"
#include "matrix.h"
#include "min.h"
#include "prod.h"
#include "reallocf.h"
#include "span.h"
#include "state.h"
#include "state_table.h"
#include "subseq.h"
#include "task.h"
#include "trans.h"
#include "trans_table.h"
#include "viterbi.h"
#include "zspan.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

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
                         unsigned *last_state, unsigned *last_seqsize)
{
  float score = imm_lprob_zero();
  unsigned end = dp->state_table.end_state_idx;

  unsigned state = IMM_STATE_NULL_IDX;
  unsigned seqsize = IMM_STATE_NULL_SEQLEN;

  unsigned length = imm_eseq_size(task->seq);
  unsigned max_seq =
      imm_zspan_max(imm_state_table_zspan(&dp->state_table, end));

  for (unsigned len = imm_min(max_seq, length);; --len)
  {
    struct imm_cell cell = imm_cell(length - len, end, len);
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
                       unsigned seq_size, struct imm_path *path)
{
  imm_trellis_seek(x, seq_size, st->end_state_idx);
  if (imm_lprob_is_nan(imm_trellis_head(x)->score)) return;

  unsigned size = 0;
  unsigned start_state = st->start_state_idx;
  while (imm_trellis_state_idx(x) != start_state || imm_trellis_stage_idx(x))
  {
    unsigned id = imm_state_table_id(st, imm_trellis_state_idx(x));
    float score = imm_trellis_head(x)->score;
    imm_path_add(path, imm_step(id, size, score));

    size = imm_trellis_head(x)->emission_size;
    imm_trellis_back(x);
  }
  unsigned id = imm_state_table_id(st, imm_trellis_state_idx(x));
  imm_path_add(path, imm_step(id, size, 0));
  imm_path_reverse(path);
}

int imm_dp_viterbi(struct imm_dp const *x, struct imm_task *task,
                   struct imm_prod *prod)
{
  imm_prod_reset(prod);
  if (!task->seq) return IMM_ENOSEQ;

  if (x->code->abc != imm_eseq_abc(task->seq)) return IMM_EDIFFABC;

  unsigned end_state = x->state_table.end_state_idx;
  unsigned min =
      imm_zspan_min(imm_state_table_zspan(&x->state_table, end_state));
  if (imm_eseq_size(task->seq) < min) return IMM_ESHORTSEQ;

  long start = imm_clock();

  struct imm_viterbi viterbi = {0};
  imm_viterbi_init(&viterbi, x, task);

  imm_viterbi_run(&viterbi);

  unsigned last_state = 0;
  unsigned last_seqsize = 0;
  prod->loglik = read_result(x, task, &last_state, &last_seqsize);

  int rc = 0;

  imm_path_reset(&prod->path);
  unsigned seqsize = imm_eseq_size(task->seq);
  unzip_path(&task->trellis, &x->state_table, seqsize, &prod->path);
  return rc;

  prod->mseconds = imm_clock() - start;

  return rc;
}

unsigned imm_dp_nstates(struct imm_dp const *dp)
{
  return dp->state_table.nstates;
}

unsigned imm_dp_trans_idx(struct imm_dp *dp, unsigned src_idx, unsigned dst_idx)
{
  return imm_trans_table_idx(&dp->trans_table, src_idx, dst_idx);
}

int imm_dp_change_trans(struct imm_dp *dp, unsigned trans_idx, float lprob)
{
  if (imm_unlikely(imm_lprob_is_nan(lprob))) return IMM_EINVAL;

  imm_trans_table_change(&dp->trans_table, trans_idx, lprob);
  return 0;
}

float imm_dp_emis_score(struct imm_dp const *dp, unsigned state_id,
                        struct imm_seq const *seq)
{
  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, dp->code);
  float score = imm_lprob_nan();
  if (imm_eseq_setup(&eseq, seq)) goto cleanup;

  unsigned state_idx = imm_state_table_idx(&dp->state_table, state_id);
  unsigned min =
      imm_zspan_min(imm_state_table_zspan(&dp->state_table, state_idx));

  unsigned seq_code = imm_eseq_get(&eseq, 0, imm_seq_size(seq), min);
  score = imm_emis_score(&dp->emis, state_idx, seq_code);

cleanup:
  imm_eseq_cleanup(&eseq);
  return score;
}

float const *imm_dp_emis_table(struct imm_dp const *dp, unsigned state_id,
                               unsigned *size)
{
  unsigned state_idx = imm_state_table_idx(&dp->state_table, state_id);
  return imm_emis_table(&dp->emis, state_idx, size);
}

float imm_dp_trans_score(struct imm_dp const *dp, unsigned src, unsigned dst)
{
  unsigned src_idx = imm_state_table_idx(&dp->state_table, src);
  unsigned dst_idx = imm_state_table_idx(&dp->state_table, dst);

  if (src_idx == UINT_MAX || dst_idx == UINT_MAX) return imm_lprob_nan();

  for (unsigned i = 0; i < dp->trans_table.ntrans; ++i)
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
  for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
  {
    for (unsigned t = 0; t < imm_trans_table_ntrans(&dp->trans_table, dst); ++t)
    {
      unsigned src = imm_trans_table_source_state(&dp->trans_table, dst, t);

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
  unsigned begin = 0;
  for (unsigned i = 0; i < imm_path_nsteps(&prod->path); ++i)
  {
    struct imm_step const *step = imm_path_step(&prod->path, i);
    unsigned idx = imm_state_table_idx(&x->state_table, step->state_id);

    unsigned min = imm_zspan_min(imm_state_table_zspan(&x->state_table, idx));
    unsigned seq_code = imm_eseq_get(t->seq, begin, step->seqlen, min);

    float score = imm_emis_score(&x->emis, idx, seq_code);
    struct imm_seq subseq = imm_subseq(seq, begin, step->seqlen);
    fprintf(fp, "<%s,%.*s,%.4f>\n", imm_state_table_name(&x->state_table, idx),
            subseq.size, subseq.str, score);
    begin += step->seqlen;
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

int imm_dp_pack(struct imm_dp const *dp, struct lip_file *f)
{
  unsigned size = 0;
  unsigned nstates = dp->state_table.nstates;
  unsigned ntrans = dp->trans_table.ntrans;

  lip_write_map_size(f, 10);

  /* emission */
  lip_write_cstr(f, KEY_EMIS_SCORE);
  size = imm_emis_score_size(&dp->emis, nstates);
  lip_write_1darray_float(f, size, dp->emis.score);

  lip_write_cstr(f, KEY_EMIS_OFFSET);
  lip_write_1darray_int(f, imm_emis_offset_size(nstates), dp->emis.offset);

  /* trans_table */
  size = imm_trans_table_transsize(ntrans);
  lip_write_cstr(f, KEY_TRANS_SCORE);
  lip_write_1darray_size_type(f, size, LIP_1DARRAY_F32);
  for (unsigned i = 0; i < size; ++i)
    lip_write_1darray_float_item(f, dp->trans_table.trans[i].score);

  size = imm_trans_table_transsize(ntrans);
  lip_write_cstr(f, KEY_TRANS_SRC);
  lip_write_1darray_size_type(f, size, LIP_1DARRAY_UINT16);
  for (unsigned i = 0; i < size; ++i)
    lip_write_1darray_int_item(f, dp->trans_table.trans[i].src);

  size = imm_trans_table_transsize(ntrans);
  lip_write_cstr(f, KEY_TRANS_DST);
  lip_write_1darray_size_type(f, size, LIP_1DARRAY_UINT16);
  for (unsigned i = 0; i < size; ++i)
    lip_write_1darray_int_item(f, dp->trans_table.trans[i].dst);

  size = imm_trans_table_offsize(nstates);
  lip_write_cstr(f, KEY_TRANS_OFFSET);
  lip_write_1darray_int(f, size, dp->trans_table.offset);

  /* state_table */
  lip_write_cstr(f, KEY_STATE_IDS);
  lip_write_1darray_int(f, nstates, dp->state_table.ids);

  lip_write_cstr(f, KEY_STATE_START);
  lip_write_int(f, dp->state_table.start_state_idx);
  lip_write_cstr(f, KEY_STATE_END);
  lip_write_int(f, dp->state_table.end_state_idx);

  lip_write_cstr(f, KEY_STATE_SPAN);
  lip_write_1darray_size_type(f, nstates, LIP_1DARRAY_UINT8);
  for (unsigned i = 0; i < nstates; ++i)
    lip_write_1darray_int_item(f, dp->state_table.span[i]);

  return f->error ? IMM_EIO : 0;
}

int imm_dp_unpack(struct imm_dp *dp, struct lip_file *f)
{
  unsigned size = 0;
  enum lip_1darray_type type = 0;
  struct imm_emis *e = &dp->emis;
  struct imm_trans_table *tt = &dp->trans_table;
  struct imm_state_table *st = &dp->state_table;

  if (!imm_expect_map_size(f, 10)) return IMM_EIO;

  /* emission */
  if (!imm_expect_map_key(f, KEY_EMIS_SCORE)) goto cleanup;
  lip_read_1darray_size_type(f, &size, &type);
  if (type != LIP_1DARRAY_F32) goto cleanup;
  e->score = imm_reallocf(e->score, sizeof(*e->score) * size);
  if (!e->score && size > 0) goto cleanup;
  lip_read_1darray_float_data(f, size, e->score);

  if (!imm_expect_map_key(f, KEY_EMIS_OFFSET)) goto cleanup;
  lip_read_1darray_size_type(f, &size, &type);
  if (type != LIP_1DARRAY_UINT32) goto cleanup;
  e->offset = imm_reallocf(e->offset, sizeof(*e->offset) * size);
  if (!e->offset && size > 0) goto cleanup;
  lip_read_1darray_int_data(f, size, e->offset);

  /* trans_table */
  if (!imm_expect_map_key(f, KEY_TRANS_SCORE)) goto cleanup;
  lip_read_1darray_size_type(f, &size, &type);
  if (type != LIP_1DARRAY_F32) goto cleanup;
  tt->trans = imm_reallocf(tt->trans, sizeof(*tt->trans) * size);
  if (!tt->trans && size > 0) goto cleanup;
  tt->ntrans = size - 1;
  for (unsigned i = 0; i < size; ++i)
    lip_read_1darray_float_item(f, &tt->trans[i].score);

  if (!imm_expect_map_key(f, KEY_TRANS_SRC)) goto cleanup;
  lip_read_1darray_size_type(f, &size, &type);
  if (type != LIP_1DARRAY_UINT16) goto cleanup;
  if (imm_trans_table_transsize(tt->ntrans) != size) goto cleanup;
  for (unsigned i = 0; i < size; ++i)
    lip_read_1darray_int_item(f, &tt->trans[i].src);

  if (!imm_expect_map_key(f, KEY_TRANS_DST)) goto cleanup;
  lip_read_1darray_size_type(f, &size, &type);
  if (type != LIP_1DARRAY_UINT16) goto cleanup;
  if (imm_trans_table_transsize(tt->ntrans) != size) goto cleanup;
  for (unsigned i = 0; i < size; ++i)
    lip_read_1darray_int_item(f, &tt->trans[i].dst);

  if (!imm_expect_map_key(f, KEY_TRANS_OFFSET)) goto cleanup;
  lip_read_1darray_size_type(f, &size, &type);
  if (type != LIP_1DARRAY_UINT16) goto cleanup;
  tt->offset = imm_reallocf(tt->offset, sizeof(*tt->offset) * size);
  if (!tt->offset && size > 0) goto cleanup;
  lip_read_1darray_int_data(f, size, tt->offset);

  /* state_table */
  if (!imm_expect_map_key(f, KEY_STATE_IDS)) goto cleanup;
  lip_read_1darray_size_type(f, &st->nstates, &type);
  if (type != LIP_1DARRAY_UINT16) goto cleanup;
  st->ids = imm_reallocf(st->ids, sizeof(*st->ids) * st->nstates);
  if (!st->ids && st->nstates > 0) goto cleanup;
  lip_read_1darray_int_data(f, st->nstates, st->ids);

  if (!imm_expect_map_key(f, KEY_STATE_START)) goto cleanup;
  lip_read_int(f, &dp->state_table.start_state_idx);
  if (!imm_expect_map_key(f, KEY_STATE_END)) goto cleanup;
  lip_read_int(f, &dp->state_table.end_state_idx);

  if (!imm_expect_map_key(f, KEY_STATE_SPAN)) goto cleanup;
  lip_read_1darray_size_type(f, &size, &type);
  if (st->nstates != size) goto cleanup;
  if (type != LIP_1DARRAY_UINT8) goto cleanup;
  st->span = imm_reallocf(st->span, sizeof(*st->span) * size);
  if (!st->span && size > 0) goto cleanup;
  for (unsigned i = 0; i < size; ++i)
    lip_read_1darray_int_item(f, (st->span + i));

  if (!f->error) return 0;

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
