#ifndef IMM_EX1_H
#define IMM_EX1_H

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

#define IMM_EX1_SIZE 3000U
#define IMM_EX1_NUCLT_ANY_SYMBOL '*'
#define IMM_EX1_NUCLT_SYMBOLS "BMIEJ"

struct imm_ex1
{
  struct imm_abc abc;
  struct imm_code code;
  struct imm_hmm hmm;
  unsigned core_size;
  struct imm_mute_state start;
  struct imm_normal_state b;
  struct imm_normal_state j;
  struct imm_normal_state m[IMM_EX1_SIZE];
  struct imm_normal_state i[IMM_EX1_SIZE];
  struct imm_mute_state d[IMM_EX1_SIZE];
  struct imm_normal_state e;
  struct imm_mute_state end;
  struct
  {
    struct imm_hmm hmm;
    struct imm_normal_state n;
  } null;
};

void imm_ex1_init(unsigned core_size);
void imm_ex1_remove_insertion_states(unsigned core_size);
void imm_ex1_remove_deletion_states(unsigned core_size);
unsigned imm_ex1_state_name(unsigned id, char *name);

extern struct imm_ex1 imm_ex1;

#endif