#include "ex3.h"
#include "abc.h"
#include "code.h"
#include "hmm.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "state.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Log-probabilities */
#define ZERO IMM_LPROB_ZERO
#define ONE 0.0f

/* State IDs */
#define M_ID (0 << 12)
#define S_ID (1 << 12)
#define N_ID (2 << 12)
#define B_ID (3 << 12)
#define E_ID (4 << 12)
#define C_ID (5 << 12)
#define T_ID (6 << 12)
#define J_ID (7 << 12)

static float N_LPROBS[] = {ONE, ZERO, ZERO, ZERO};
static float M_LPROBS[] = {ZERO, ONE, ZERO, ZERO};
static float C_LPROBS[] = {ZERO, ZERO, ONE, ZERO};
static float J_LPROBS[] = {ZERO, ZERO, ZERO, ONE};

#define SET_TRANS(hmm, a, b, v)                                                \
  do                                                                           \
  {                                                                            \
    imm_hmm_set_trans(hmm, &a.super, &b.super, v);                             \
  } while (0);

static void initialise_states(struct imm_ex3 *x)
{
  imm_mute_state_init(&x->S, S_ID, &x->abc);
  imm_normal_state_init(&x->N, N_ID, &x->abc, N_LPROBS);
  imm_mute_state_init(&x->B, B_ID, &x->abc);

  for (int k = 0; k < x->core_size; ++k)
    imm_normal_state_init(&x->M[k], M_ID | k, &x->abc, M_LPROBS);

  imm_mute_state_init(&x->E, E_ID, &x->abc);
  imm_normal_state_init(&x->C, C_ID, &x->abc, C_LPROBS);
  imm_mute_state_init(&x->T, T_ID, &x->abc);

  imm_normal_state_init(&x->J, J_ID, &x->abc, J_LPROBS);
}

static void add_states(struct imm_ex3 *x)
{
  imm_hmm_add_state(x->hmm, &x->S.super);
  imm_hmm_add_state(x->hmm, &x->N.super);
  imm_hmm_add_state(x->hmm, &x->B.super);

  for (int k = 0; k < x->core_size; ++k)
    imm_hmm_add_state(x->hmm, &x->M[k].super);

  imm_hmm_add_state(x->hmm, &x->E.super);
  imm_hmm_add_state(x->hmm, &x->C.super);
  imm_hmm_add_state(x->hmm, &x->T.super);

  imm_hmm_add_state(x->hmm, &x->J.super);
}

static void set_transitions(struct imm_ex3 *x)
{
  imm_hmm_set_start(x->hmm, &x->S);
  imm_hmm_set_end(x->hmm, &x->T);

  SET_TRANS(x->hmm, x->S, x->N, logf(0.1f));
  SET_TRANS(x->hmm, x->N, x->N, logf(0.2f));
  SET_TRANS(x->hmm, x->N, x->B, logf(0.3f));
  SET_TRANS(x->hmm, x->S, x->B, logf(0.4f));

  for (int k = 0; k < x->core_size; ++k)
    SET_TRANS(x->hmm, x->B, x->M[k], logf(0.11f));

  for (int k = 1; k < x->core_size; ++k)
    SET_TRANS(x->hmm, x->M[k - 1], x->M[k], logf(0.12f));

  SET_TRANS(x->hmm, x->M[x->core_size - 1], x->E, logf(0.13f));

  SET_TRANS(x->hmm, x->E, x->C, logf(0.1f));
  SET_TRANS(x->hmm, x->C, x->C, logf(0.2f));
  SET_TRANS(x->hmm, x->C, x->T, logf(0.3f));
  SET_TRANS(x->hmm, x->E, x->T, logf(0.4f));

  SET_TRANS(x->hmm, x->E, x->J, logf(0.5f));
  SET_TRANS(x->hmm, x->J, x->J, logf(0.6f));
  SET_TRANS(x->hmm, x->J, x->B, logf(0.7f));
  SET_TRANS(x->hmm, x->E, x->B, logf(0.8f));
}

void imm_ex3_init(int core_size)
{
  assert(core_size > 0);
  assert(core_size <= IMM_EX3_SIZE);
  struct imm_ex3 *x = &imm_ex3;
  imm_abc_init(&x->abc, imm_str(IMM_EX3_NUCLT_SYMBOLS),
               IMM_EX3_NUCLT_ANY_SYMBOL);
  imm_code_init(&x->code, &x->abc);
  x->hmm = imm_hmm_new(&x->code);
  x->core_size = core_size;

  initialise_states(&imm_ex3);
  add_states(&imm_ex3);
  set_transitions(&imm_ex3);
}

void imm_ex3_cleanup(void) { imm_hmm_del(imm_ex3.hmm); }

char *imm_ex3_state_name(int id, char *name)
{
  if ((id & (15 << 12)) == M_ID)
    snprintf(name, IMM_STATE_NAME_SIZE, "M%d", (id & (0xFFFF >> 4)));

  if ((id & (15 << 12)) == S_ID) strcpy(name, "S");
  if ((id & (15 << 12)) == N_ID) strcpy(name, "N");
  if ((id & (15 << 12)) == B_ID) strcpy(name, "B");

  if ((id & (15 << 12)) == E_ID) strcpy(name, "E");
  if ((id & (15 << 12)) == C_ID) strcpy(name, "C");
  if ((id & (15 << 12)) == T_ID) strcpy(name, "T");

  if ((id & (15 << 12)) == J_ID) strcpy(name, "J");

  return name;
}

struct imm_ex3 imm_ex3 = {0};

char const imm_ex3_seq1[] =
    "NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMC";
