#include "node.h"
#include "array_size.h"
#include "state_table.h"
#include <string.h>

void imm_node_dump(struct imm_node const *x, uint16_t *ids,
                   imm_state_name *callb, FILE *restrict fp)
{
  char emis_size[16] = {0};
  if (x->emission_size == IMM_STATE_NULL_SEQLEN) strcpy(emis_size, "?");
  else snprintf(emis_size, imm_array_size(emis_size), "%u", x->emission_size);

  char name[IMM_STATE_NAME_SIZE] = {0};
  if (x->state_source == IMM_STATE_NULL_IDX) strcpy(name, "?");
  else (*callb)(ids[x->state_source], name);

  fprintf(fp, "(%s;%s;%.9g)", name, emis_size, x->score);
}
