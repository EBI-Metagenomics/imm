#include "imm_ex1.h"
#include "imm_abc.h"
#include "imm_code.h"
#include "imm_hmm.h"
#include "imm_mute_state.h"
#include "imm_normal_state.h"
#include "imm_state.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Log-probabilities */
#define ZERO -INFINITY
#define ONE 0.0f

/* State IDs */
#define M (0 << 12)
#define I (1 << 12)
#define D (2 << 12)
#define START (3 << 12)
#define B (4 << 12)
#define E (5 << 12)
#define J (6 << 12)
#define END (7 << 12)
#define N (8 << 12)
#define NSTART (9 << 12)
#define NEND (10 << 12)

static float b_lprobs[] = {ONE, ZERO, ZERO, ZERO, ZERO};
static float m_lprobs[] = {ZERO, ONE, ZERO, ZERO, ZERO};
static float i_lprobs[] = {ZERO, ZERO, ONE, ZERO, ZERO};
static float e_lprobs[] = {ZERO, ZERO, ZERO, ONE, ZERO};
static float j_lprobs[] = {ZERO, ZERO, ZERO, ZERO, ONE};
static float n_lprobs[] = {-1e3f, -1e3f, -1e3f, -1e3f, -1e3f};

struct imm_ex1 imm_ex1;

#define SET_TRANS(hmm, a, b, v) imm_hmm_set_trans(hmm, &a.super, &b.super, v)

void imm_ex1_init(int core_size)
{
  assert(core_size > 0);
  assert(core_size <= IMM_EX1_SIZE);
  struct imm_ex1 *m = &imm_ex1;
  imm_abc_init(&m->abc, imm_str(IMM_EX1_NUCLT_SYMBOLS),
               IMM_EX1_NUCLT_ANY_SYMBOL);
  imm_code_init(&m->code, &m->abc);
  m->hmm = imm_hmm_new(&m->code);
  m->core_size = core_size;

  imm_mute_state_init(&m->start, START, &m->abc);
  imm_hmm_add_state(m->hmm, &m->start.super);
  imm_hmm_set_start(m->hmm, &m->start);

  imm_mute_state_init(&m->end, END, &m->abc);
  imm_hmm_add_state(m->hmm, &m->end.super);
  imm_hmm_set_end(m->hmm, &m->end);

  imm_normal_state_init(&m->b, B, &m->abc, b_lprobs);
  imm_hmm_add_state(m->hmm, &m->b.super);

  imm_normal_state_init(&m->e, E, &m->abc, e_lprobs);
  imm_hmm_add_state(m->hmm, &m->e.super);

  imm_normal_state_init(&m->j, J, &m->abc, j_lprobs);
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
    imm_normal_state_init(&m->m[k], M | k, &m->abc, m_lprobs);
    imm_normal_state_init(&m->i[k], I | k, &m->abc, i_lprobs);
    imm_mute_state_init(&m->d[k], D | k, &m->abc);

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

  imm_mute_state_init(&m->null.nstart, NSTART, &m->abc);
  imm_mute_state_init(&m->null.nend, NEND, &m->abc);
  imm_normal_state_init(&m->null.n, N, &m->abc, n_lprobs);

  imm_hmm_add_state(m->null.hmm, &m->null.nstart.super);
  imm_hmm_add_state(m->null.hmm, &m->null.n.super);
  imm_hmm_add_state(m->null.hmm, &m->null.nend.super);

  imm_hmm_set_trans(m->null.hmm, &m->null.nstart.super, &m->null.n.super, 0);
  imm_hmm_set_trans(m->null.hmm, &m->null.n.super, &m->null.nend.super, 0);
  SET_TRANS(m->null.hmm, m->null.n, m->null.n, logf(0.2f));

  imm_hmm_set_start(m->null.hmm, &m->null.nstart);
  imm_hmm_set_end(m->null.hmm, &m->null.nend);
}

void imm_ex1_cleanup(void)
{
  imm_hmm_del(imm_ex1.hmm);
  imm_hmm_del(imm_ex1.null.hmm);
}

void imm_ex1_remove_insertion_states(int core_size)
{
  struct imm_ex1 *m = &imm_ex1;
  for (int k = 0; k < core_size; ++k)
    imm_hmm_del_state(m->hmm, &m->i[k].super);
}

void imm_ex1_remove_deletion_states(int core_size)
{
  struct imm_ex1 *m = &imm_ex1;
  for (int k = 0; k < core_size; ++k)
    imm_hmm_del_state(m->hmm, &m->d[k].super);
}

char *imm_ex1_state_name(int id, char *name)
{
  int idx = id & (0xFFFF >> 4);
  if ((id & (15 << 12)) == START) strcpy(name, "START");
  if ((id & (15 << 12)) == B) strcpy(name, "B");
  if ((id & (15 << 12)) == M) snprintf(name, IMM_STATE_NAME_SIZE, "M%d", idx);
  if ((id & (15 << 12)) == I) snprintf(name, IMM_STATE_NAME_SIZE, "I%d", idx);
  if ((id & (15 << 12)) == D) snprintf(name, IMM_STATE_NAME_SIZE, "D%d", idx);
  if ((id & (15 << 12)) == E) strcpy(name, "E");
  if ((id & (15 << 12)) == J) strcpy(name, "J");
  if ((id & (15 << 12)) == END) strcpy(name, "END");
  if ((id & (15 << 12)) == NSTART) strcpy(name, "NSTART");
  if ((id & (15 << 12)) == N) strcpy(name, "N");
  if ((id & (15 << 12)) == NEND) strcpy(name, "NEND");
  return name;
}
