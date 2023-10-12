#include "ex1.h"
#include "abc.h"
#include "code.h"
#include "hmm.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "state.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Log-probabilities */
#define ZERO -INFINITY
#define ONE 0.0f

/* State IDs */
#define M ((uint16_t)(0U << 12))
#define I ((uint16_t)(1U << 12))
#define D ((uint16_t)(2U << 12))
#define START ((uint16_t)(3U << 12))
#define B ((uint16_t)(4U << 12))
#define E ((uint16_t)(5U << 12))
#define J ((uint16_t)(6U << 12))
#define END ((uint16_t)(7U << 12))
#define N ((uint16_t)(8U << 12))
#define NSTART ((uint16_t)(9U << 12))
#define NEND ((uint16_t)(10U << 12))

static float b_lprobs[] = {ONE, ZERO, ZERO, ZERO, ZERO};
static float m_lprobs[] = {ZERO, ONE, ZERO, ZERO, ZERO};
static float i_lprobs[] = {ZERO, ZERO, ONE, ZERO, ZERO};
static float e_lprobs[] = {ZERO, ZERO, ZERO, ONE, ZERO};
static float j_lprobs[] = {ZERO, ZERO, ZERO, ZERO, ONE};
static float n_lprobs[] = {-1e3f, -1e3f, -1e3f, -1e3f, -1e3f};

struct imm_ex1 imm_ex1;

#define SET_TRANS(hmm, a, b, v) imm_hmm_set_trans(hmm, &a.super, &b.super, v)

void imm_ex1_init(unsigned core_size)
{
  assert(core_size > 0);
  assert(core_size <= IMM_EX1_SIZE);
  struct imm_ex1 *m = &imm_ex1;
  imm_abc_init(&m->abc, imm_str(IMM_EX1_NUCLT_SYMBOLS),
               IMM_EX1_NUCLT_ANY_SYMBOL);
  imm_code_init(&m->code, &m->abc);
  imm_hmm_init(&m->hmm, &m->code);
  m->core_size = core_size;

  imm_mute_state_init(&m->start, START, &m->abc);
  imm_hmm_add_state(&m->hmm, &m->start.super);
  imm_hmm_set_start(&m->hmm, &m->start);

  imm_mute_state_init(&m->end, END, &m->abc);
  imm_hmm_add_state(&m->hmm, &m->end.super);
  imm_hmm_set_end(&m->hmm, &m->end);

  imm_normal_state_init(&m->b, B, &m->abc, b_lprobs);
  imm_hmm_add_state(&m->hmm, &m->b.super);

  imm_normal_state_init(&m->e, E, &m->abc, e_lprobs);
  imm_hmm_add_state(&m->hmm, &m->e.super);

  imm_normal_state_init(&m->j, J, &m->abc, j_lprobs);
  imm_hmm_add_state(&m->hmm, &m->j.super);

  SET_TRANS(&m->hmm, m->start, m->b, log(0.2));
  SET_TRANS(&m->hmm, m->b, m->b, log(0.2));
  SET_TRANS(&m->hmm, m->e, m->e, log(0.2));
  SET_TRANS(&m->hmm, m->j, m->j, log(0.2));
  SET_TRANS(&m->hmm, m->e, m->j, log(0.2));
  SET_TRANS(&m->hmm, m->j, m->b, log(0.2));
  SET_TRANS(&m->hmm, m->e, m->end, log(0.2));

  for (unsigned k = 0; k < core_size; ++k)
  {
    imm_normal_state_init(&m->m[k], M | k, &m->abc, m_lprobs);
    imm_normal_state_init(&m->i[k], I | k, &m->abc, i_lprobs);
    imm_mute_state_init(&m->d[k], D | k, &m->abc);

    imm_hmm_add_state(&m->hmm, &m->m[k].super);
    imm_hmm_add_state(&m->hmm, &m->i[k].super);
    imm_hmm_add_state(&m->hmm, &m->d[k].super);

    if (k == 0) SET_TRANS(&m->hmm, m->b, m->m[0], log(0.2));

    SET_TRANS(&m->hmm, m->m[k], m->i[k], log(0.2));
    SET_TRANS(&m->hmm, m->i[k], m->i[k], log(0.2));

    if (k > 0)
    {
      SET_TRANS(&m->hmm, m->m[k - 1], m->m[k], log(0.2));
      SET_TRANS(&m->hmm, m->d[k - 1], m->m[k], log(0.2));
      SET_TRANS(&m->hmm, m->i[k - 1], m->m[k], log(0.2));

      SET_TRANS(&m->hmm, m->m[k - 1], m->d[k], log(0.2));
      SET_TRANS(&m->hmm, m->d[k - 1], m->d[k], log(0.2));
    }

    if (k == core_size - 1)
    {
      SET_TRANS(&m->hmm, m->m[k], m->e, log(0.2));
      SET_TRANS(&m->hmm, m->d[k], m->e, log(0.2));
      SET_TRANS(&m->hmm, m->i[k], m->e, log(0.2));
    }
  }

  // Null one
  imm_hmm_init(&m->null.hmm, &m->code);

  imm_mute_state_init(&m->null.nstart, NSTART, &m->abc);
  imm_mute_state_init(&m->null.nend, NEND, &m->abc);
  imm_normal_state_init(&m->null.n, N, &m->abc, n_lprobs);

  imm_hmm_add_state(&m->null.hmm, &m->null.nstart.super);
  imm_hmm_add_state(&m->null.hmm, &m->null.n.super);
  imm_hmm_add_state(&m->null.hmm, &m->null.nend.super);

  imm_hmm_set_trans(&m->null.hmm, &m->null.nstart.super, &m->null.n.super, 0);
  imm_hmm_set_trans(&m->null.hmm, &m->null.n.super, &m->null.nend.super, 0);
  SET_TRANS(&m->null.hmm, m->null.n, m->null.n, log(0.2));

  imm_hmm_set_start(&m->null.hmm, &m->null.nstart);
  imm_hmm_set_end(&m->null.hmm, &m->null.nend);
}

void imm_ex1_remove_insertion_states(unsigned core_size)
{
  struct imm_ex1 *m = &imm_ex1;
  for (unsigned k = 0; k < core_size; ++k)
    imm_hmm_del_state(&m->hmm, &m->i[k].super);
}

void imm_ex1_remove_deletion_states(unsigned core_size)
{
  struct imm_ex1 *m = &imm_ex1;
  for (unsigned k = 0; k < core_size; ++k)
    imm_hmm_del_state(&m->hmm, &m->d[k].super);
}

char *imm_ex1_state_name(unsigned id, char *name)
{
  unsigned idx = id & (0xFFFF >> 4);
  if ((id & (15U << 12)) == START) strcpy(name, "START");
  if ((id & (15U << 12)) == B) strcpy(name, "B");
  if ((id & (15U << 12)) == M) snprintf(name, IMM_STATE_NAME_SIZE, "M%u", idx);
  if ((id & (15U << 12)) == I) snprintf(name, IMM_STATE_NAME_SIZE, "I%u", idx);
  if ((id & (15U << 12)) == D) snprintf(name, IMM_STATE_NAME_SIZE, "D%u", idx);
  if ((id & (15U << 12)) == E) strcpy(name, "E");
  if ((id & (15U << 12)) == J) strcpy(name, "J");
  if ((id & (15U << 12)) == END) strcpy(name, "END");
  if ((id & (15U << 12)) == NSTART) strcpy(name, "NSTART");
  if ((id & (15U << 12)) == N) strcpy(name, "N");
  if ((id & (15U << 12)) == NEND) strcpy(name, "NEND");
  return name;
}
