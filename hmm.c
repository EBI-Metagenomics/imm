#include "hmm.h"
#include "array_size.h"
#include "dp.h"
#include "dp_cfg.h"
#include "htable.h"
#include "list.h"
#include "logaddexp.h"
#include "lprob.h"
#include "mute_state.h"
#include "path.h"
#include "stack.h"
#include "state.h"
#include "tsort.h"
#include <assert.h>
#include <stdlib.h>

static inline void start_init(struct imm_hmm *hmm)
{
  hmm->start_state_id = IMM_STATE_NULL_ID;
}

static inline void end_init(struct imm_hmm *hmm)
{
  hmm->end_state_id = IMM_STATE_NULL_ID;
}

static void init_states_table(struct imm_hmm *hmm)
{
  hmm->states.size = 0;
  cco_hash_init(hmm->states.tbl);
}

static void init_transitions_table(struct imm_hmm *hmm)
{
  hmm->transitions.size = 0;
  cco_hash_init(hmm->transitions.tbl);
  hmm->transitions.avail = &hmm->transitions.data[0];
}

static int add_transition(struct imm_hmm *hmm, struct imm_state *src,
                          struct imm_state *dst, float lprob)
{
  int max_size = (int)imm_array_size(hmm->transitions.data);
  if (hmm->transitions.size >= max_size) return IMM_EMANYTRANS;

  if (hmm->transitions.avail > &hmm->transitions.data[max_size - 1])
    return IMM_ENOMEM;

  struct imm_trans *trans = hmm->transitions.avail++;
  hmm->transitions.size++;
  imm_trans_init(trans, src->id, dst->id, lprob);
  cco_hash_add(hmm->transitions.tbl, &trans->hnode, trans->pair.id.key);
  imm_stack_put(&trans->outgoing, &src->trans.outgoing);
  imm_stack_put(&trans->incoming, &dst->trans.incoming);
  return 0;
}

static inline bool has_start_state(struct imm_hmm const *hmm)
{
  return hmm->start_state_id != IMM_STATE_NULL_ID;
}

static inline bool has_end_state(struct imm_hmm const *hmm)
{
  return hmm->end_state_id != IMM_STATE_NULL_ID;
}

static struct imm_state *hmm_state(struct imm_hmm const *hmm, int state_id);

static void set_state_indices(struct imm_hmm const *hmm,
                              struct imm_state **states)
{
  for (int i = 0; i < hmm->states.size; ++i)
    states[i]->idx = i;

  struct imm_trans *trans = NULL;
  unsigned bkt = 0;
  cco_hash_for_each(hmm->transitions.tbl, bkt, trans, hnode)
  {
    struct imm_state *src = hmm_state(hmm, trans->pair.id.src);
    struct imm_state *dst = hmm_state(hmm, trans->pair.id.dst);
    trans->pair.idx.src = (int16_t)src->idx;
    trans->pair.idx.dst = (int16_t)dst->idx;
  }
}

static char *id_state_name(int id, char *name)
{
  snprintf(name, IMM_STATE_NAME_SIZE, "%d", id);
  return name;
}

struct imm_hmm *imm_hmm_new(struct imm_code const *code)
{
  struct imm_hmm *x = malloc(sizeof(*x));
  x->code = code;
  imm_hmm_reset(x);
  return x;
}

void imm_hmm_del(struct imm_hmm const *x) { free((void *)x); }

void imm_hmm_dump(struct imm_hmm const *hmm, imm_state_name *callb,
                  FILE *restrict fp)
{
  if (!callb) callb = &id_state_name;
  fprintf(fp, "digraph hmm {\n");
  struct imm_trans *t = NULL;
  unsigned bkt = 0;
  cco_hash_for_each(hmm->transitions.tbl, bkt, t, hnode)
  {
    char src_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                          '\0', '\0', '\0', '\0'};
    char dst_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                          '\0', '\0', '\0', '\0'};
    struct imm_state *src = hmm_state(hmm, t->pair.id.src);
    struct imm_state *dst = hmm_state(hmm, t->pair.id.dst);
    (*callb)(src->id, src_name);
    (*callb)(dst->id, dst_name);
    fprintf(fp, "%s -> %s [label=%.4f];\n", src_name, dst_name, t->lprob);
  }
  fprintf(fp, "}\n");
}

int imm_hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
  if (cco_hash_hashed(&state->hnode)) return IMM_ESTATEPRESENT;
  assert(!hmm_state(hmm, state->id));
  cco_hash_add(hmm->states.tbl, &state->hnode, state->id);
  hmm->states.size++;
  return 0;
}

static int del_outgoing_transitions(struct imm_hmm *, struct imm_state *);
static int del_incoming_transitions(struct imm_hmm *, struct imm_state *);

int imm_hmm_del_state(struct imm_hmm *hmm, struct imm_state *state)
{
  int rc = 0;

  if ((rc = del_outgoing_transitions(hmm, state))) return rc;
  if ((rc = del_incoming_transitions(hmm, state))) return rc;

  cco_hash_del(&state->hnode);
  hmm->states.size--;

  return rc;
}

int imm_hmm_init_dp(struct imm_hmm const *hmm, struct imm_dp *dp)
{
  imm_dp_init(dp, hmm->code);
  return imm_hmm_reset_dp(hmm, dp);
}

void imm_hmm_reset(struct imm_hmm *hmm)
{
  start_init(hmm);
  end_init(hmm);
  init_states_table(hmm);
  init_transitions_table(hmm);
}

static bool has_dst_start(struct imm_hmm const *x)
{
  struct imm_trans *t = NULL;
  unsigned bkt = 0;
  cco_hash_for_each(x->transitions.tbl, bkt, t, hnode)
  {
    if (t->pair.id.dst == x->start_state_id) return true;
  }
  return false;
}

static bool has_src_end(struct imm_hmm const *x)
{
  struct imm_trans *t = NULL;
  unsigned bkt = 0;
  cco_hash_for_each(x->transitions.tbl, bkt, t, hnode)
  {
    if (t->pair.id.src == x->end_state_id) return true;
  }
  return false;
}

int imm_hmm_reset_dp(struct imm_hmm const *hmm, struct imm_dp *dp)
{
  int rc = 0;
  assert(hmm->states.size >= 0);
  struct imm_state **states =
      malloc(sizeof(struct imm_state *) * (unsigned)hmm->states.size);
  if (!states) return IMM_ENOMEM;

  if (!has_start_state(hmm))
  {
    rc = IMM_ENOSTART;
    goto cleanup;
  }
  if (!has_end_state(hmm))
  {
    rc = IMM_ENOEND;
    goto cleanup;
  }
  if (has_dst_start(hmm))
  {
    rc = IMM_EDSTSTART;
    goto cleanup;
  }
  if (has_src_end(hmm))
  {
    rc = IMM_ESRCEND;
    goto cleanup;
  }

  unsigned bkt = 0;
  int i = 0;
  struct imm_state *state = NULL;
  cco_hash_for_each(hmm->states.tbl, bkt, state, hnode) { states[i++] = state; }
  set_state_indices(hmm, states);

  int start_idx = hmm_state(hmm, hmm->start_state_id)->idx;
  if ((rc = imm_tsort(hmm->states.size, states, start_idx))) goto cleanup;
  set_state_indices(hmm, states);

  struct imm_dp_cfg cfg = {.ntrans = hmm->transitions.size,
                           .nstates = hmm->states.size,
                           .states = states,
                           .start_state = hmm_state(hmm, hmm->start_state_id),
                           .end_state = hmm_state(hmm, hmm->end_state_id)};

  imm_dp_reset(dp, &cfg);

cleanup:
  free(states);
  return rc;
}

static struct imm_trans *hmm_trans(struct imm_hmm const *hmm,
                                   struct imm_state const *src,
                                   struct imm_state const *dst);

float imm_hmm_trans(struct imm_hmm const *hmm, struct imm_state const *src,
                    struct imm_state const *dst)
{
  if (!cco_hash_hashed(&src->hnode) || !cco_hash_hashed(&dst->hnode))
    return imm_lprob_nan();
  struct imm_trans const *trans = hmm_trans(hmm, src, dst);
  if (trans) return trans->lprob;
  return imm_lprob_nan();
}

float imm_hmm_loglik(struct imm_hmm const *hmm, struct imm_seq const *seq,
                     struct imm_path const *path)
{
  if (hmm->code->abc != seq->abc) return imm_lprob_nan();

  int nsteps = imm_path_nsteps(path);
  if (nsteps == 0) return imm_lprob_nan();

  struct imm_step const *step = imm_path_step(path, 0);
  struct imm_state const *state = hmm_state(hmm, step->state_id);
  if (state != hmm_state(hmm, hmm->start_state_id)) return imm_lprob_nan();

  if (step->seqsize > imm_seq_size(seq)) return imm_lprob_nan();

  struct imm_seq subseq = imm_seq_slice(seq, imm_range(0, step->seqsize));
  float lprob = imm_state_lprob(state, &subseq);

  int start = 0;
  for (int i = 1; i < nsteps; ++i)
  {
    start += step->seqsize;
    step = imm_path_step(path, i);
    if (start + step->seqsize > imm_seq_size(seq)) return imm_lprob_nan();

    struct imm_state const *prev_state = state;
    if (!(state = hmm_state(hmm, step->state_id))) return imm_lprob_nan();
    subseq = imm_seq_slice(seq, imm_range(start, start + step->seqsize));
    lprob += imm_hmm_trans(hmm, prev_state, state);
    lprob += imm_state_lprob(state, &subseq);
  }

  if (start + step->seqsize < imm_seq_size(seq)) return imm_lprob_nan();
  return lprob;
}

int imm_hmm_normalize_trans(struct imm_hmm const *hmm)
{
  struct imm_state *state = NULL;
  unsigned bkt = 0;
  int rc = 0;
  cco_hash_for_each(hmm->states.tbl, bkt, state, hnode)
  {
    if ((rc = imm_hmm_normalize_state_trans(state))) break;
  }
  return rc;
}

int imm_hmm_normalize_state_trans(struct imm_state *src)
{
  if (!cco_hash_hashed(&src->hnode)) return IMM_ENOTFOUND;

  if (imm_list_empty(&src->trans.outgoing)) return 0;

  struct imm_trans *trans = NULL;
  float lnorm = imm_lprob_zero();
  imm_list_for_each_entry(trans, &src->trans.outgoing, outgoing)
  {
    lnorm = imm_logaddexp(lnorm, trans->lprob);
  }

  if (!imm_lprob_is_finite(lnorm)) return IMM_EINVAL;

  imm_list_for_each_entry(trans, &src->trans.outgoing, outgoing)
  {
    trans->lprob -= lnorm;
  }
  return 0;
}

int imm_hmm_set_start(struct imm_hmm *x, struct imm_mute_state const *state)
{
  if (!cco_hash_hashed(&state->super.hnode)) return IMM_ENOTFOUND;
  x->start_state_id = state->super.id;
  return 0;
}

int imm_hmm_set_end(struct imm_hmm *x, struct imm_mute_state const *state)
{
  if (!cco_hash_hashed(&state->super.hnode)) return IMM_ENOTFOUND;
  x->end_state_id = state->super.id;
  return 0;
}

int imm_hmm_set_trans(struct imm_hmm *hmm, struct imm_state *src,
                      struct imm_state *dst, float lprob)
{
  if (!imm_lprob_is_finite(lprob)) return IMM_EINVAL;

  if (!cco_hash_hashed(&src->hnode)) return IMM_ENOTFOUND;

  if (!cco_hash_hashed(&dst->hnode)) return IMM_ENOTFOUND;

  struct imm_trans *trans = hmm_trans(hmm, src, dst);

  if (trans) trans->lprob = lprob;
  else return add_transition(hmm, src, dst, lprob);

  return 0;
}

static struct imm_state *hmm_state(struct imm_hmm const *hmm, int state_id)
{
  struct imm_state *state = NULL;
  cco_hash_for_each_possible(hmm->states.tbl, state, hnode, state_id)
  {
    if (state->id == state_id) return state;
  }
  return NULL;
}

static struct imm_trans *hmm_trans(struct imm_hmm const *hmm,
                                   struct imm_state const *src,
                                   struct imm_state const *dst)
{
  struct imm_trans *trans = NULL;
  struct imm_pair pair = imm_pair(src->id, dst->id);
  cco_hash_for_each_possible(hmm->transitions.tbl, trans, hnode, pair.id.key)
  {
    if (trans->pair.id.src == src->id && trans->pair.id.dst == dst->id)
      return trans;
  }
  return NULL;
}

static int del_outgoing_transitions(struct imm_hmm *hmm,
                                    struct imm_state *state)
{
  if (!cco_hash_hashed(&state->hnode)) return IMM_ENOTFOUND;
  assert(hmm_state(hmm, state->id));

  struct imm_trans *t = NULL;
  imm_list_for_each_entry(t, &state->trans.outgoing, outgoing)
  {
    struct imm_state *dst = hmm_state(hmm, t->pair.id.dst);
    struct imm_list *pos = NULL;
    struct imm_list *n = NULL;
    imm_list_for_each_safe(pos, n, &dst->trans.incoming)
    {
      struct imm_trans *x = imm_container(pos, struct imm_trans, incoming);
      if (x == t)
      {
        imm_list_del(pos);
        break;
      }
    }
    cco_hash_del(&t->hnode);
    hmm->transitions.size--;
  }

  return 0;
}

static int del_incoming_transitions(struct imm_hmm *hmm,
                                    struct imm_state *state)
{
  if (!cco_hash_hashed(&state->hnode)) return IMM_ENOTFOUND;
  assert(hmm_state(hmm, state->id));

  struct imm_trans *t = NULL;
  imm_list_for_each_entry(t, &state->trans.incoming, incoming)
  {
    struct imm_state *src = hmm_state(hmm, t->pair.id.src);
    struct imm_list *pos = NULL;
    struct imm_list *n = NULL;
    imm_list_for_each_safe(pos, n, &src->trans.outgoing)
    {
      struct imm_trans *x = imm_container(pos, struct imm_trans, outgoing);
      if (x == t)
      {
        imm_list_del(pos);
        break;
      }
    }
    cco_hash_del(&t->hnode);
    hmm->transitions.size--;
  }

  return 0;
}
