#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "htable.h"
#include "list.h"
#include "seq.h"
#include "span.h"
#include "state_vtable.h"
#include <assert.h>

#define IMM_STATE_NULL_ID UINT16_MAX
#define IMM_STATE_NULL_IDX UINT16_MAX
#define IMM_STATE_MAX_SEQLEN 5
#define IMM_NSTATES_NULL UINT16_MAX
#define IMM_STATE_NULL_SEQLEN UINT8_MAX
#define IMM_STATE_NAME_SIZE 8

struct imm_seq;
struct imm_state;

struct imm_state
{
  unsigned id;
  unsigned idx;
  struct imm_abc const *abc;
  struct imm_span span;
  struct imm_state_vtable vtable;
  struct
  {
    struct imm_list outgoing;
    struct imm_list incoming;
  } trans;
  struct cco_hnode hnode;
  int mark;
};

typedef char *(imm_state_name)(unsigned id, char *name);

IMM_API struct imm_abc const *imm_state_abc(struct imm_state const *);
IMM_API unsigned imm_state_id(struct imm_state const *);
IMM_API unsigned imm_state_idx(struct imm_state const *);
IMM_API float imm_state_lprob(struct imm_state const *, struct imm_seq const *);
IMM_API struct imm_span imm_state_span(struct imm_state const *);
IMM_API enum imm_state_typeid imm_state_typeid(struct imm_state const *);
IMM_API void imm_state_detach(struct imm_state *);
IMM_API void imm_state_init(struct imm_state *, unsigned id,
                            struct imm_abc const *, struct imm_state_vtable,
                            struct imm_span);
IMM_API char *imm_state_default_name(unsigned id, char *name);

#endif
