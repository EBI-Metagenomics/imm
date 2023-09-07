#include "node.h"
#include "state_table.h"
#include <string.h>

void imm_node_dump(struct imm_node const *x, struct imm_state_table const *st,
                   FILE *restrict fp)
{
  char emis_size[16] = {0};
  if (x->emission_size == IMM_STATE_NULL_SEQLEN) strcpy(emis_size, "?");
  else sprintf(emis_size, "%u", x->emission_size);

  fprintf(fp, "(%s;%s;%.9g)", imm_state_table_name(st, x->state_source),
          emis_size, x->score);
}
