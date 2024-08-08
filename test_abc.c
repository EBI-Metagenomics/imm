#include "aye.h"
#include "imm_abc.h"
#include "imm_abc_lprob.h"
#include "imm_amino.h"
#include "imm_amino_lprob.h"
#include "imm_dna.h"
#include "imm_lprob.h"
#include "imm_rc.h"
#include "imm_rna.h"
#include "lite_pack_io.h"
#include "near.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static void standard_usage(void)
{
  struct imm_abc abc = {0};
  aye(imm_abc_init(&abc, imm_str("ACGT"), '*') == 0);
  aye(imm_abc_symbol_idx(&abc, 'G') == 2);
  aye(imm_abc_size(&abc) == 4);
  aye(imm_abc_has_symbol(&abc, 'C'));
  aye(!imm_abc_has_symbol(&abc, 'L'));
  aye((int)imm_abc_symbol_type(&abc, 'A') == IMM_SYM_NORMAL);
  aye((int)imm_abc_symbol_type(&abc, '*') == IMM_SYM_ANY);
  aye((int)imm_abc_symbol_type(&abc, 'L') == IMM_SYM_NULL);
  aye((int)abc.typeid == IMM_ABC);
}

static void duplicated_alphabet(void)
{
  struct imm_abc abc = {0};
  aye(imm_abc_init(&abc, imm_str("ACTC"), '*') == IMM_EINVAL);
}

static void duplicated_any_symbol(void)
{
  struct imm_abc abc = {0};
  aye(imm_abc_init(&abc, imm_str("AC*T"), '*') == IMM_EINVAL);
}

static void symbol_outside_range(void)
{

  struct imm_abc abc = {0};
  char symbols[] = {3, '\0'};
  aye(imm_abc_init(&abc, (struct imm_str){2, symbols}, '*') == IMM_EINVAL);
}

static void any_symbol_outside_range(void)
{
  struct imm_abc abc = {0};
  aye(imm_abc_init(&abc, imm_str("ACGT"), 3) == IMM_EINVAL);
}

static void union_size(void)
{
  struct imm_abc abc = {0};
  aye(imm_abc_init(&abc, imm_str("ACT"), '*') == 0);
  char data[] = "ACAAAAAAAAC*AATT*G";
  aye(imm_abc_union_size(&abc, imm_str(data)) == 1);
}

static void get_lprob(void)
{
  struct imm_abc abc = {0};
  aye(imm_abc_init(&abc, imm_str("ACGT"), 'X') == 0);

  float const lprobs[4] = {logf(0.2f), logf(0.01f), logf(1.0f), logf(0.5f)};
  struct imm_abc_lprob lprob = imm_abc_lprob(&abc, lprobs);

  near(imm_abc_lprob_get(&lprob, 'A'), logf(0.2f));
  near(imm_abc_lprob_get(&lprob, 'C'), logf(0.01f));
  near(imm_abc_lprob_get(&lprob, 'T'), logf(0.5f));
}

static void amino_success(void)
{
  char const symbols[IMM_AMINO_SIZE + 1] = IMM_AMINO_SYMBOLS;

  struct imm_amino const *amino = &imm_amino_iupac;
  for (int i = 0; i < imm_abc_size(&amino->super); ++i)
  {
    aye(imm_abc_symbols(&amino->super)[i] == symbols[i]);
    aye(imm_abc_symbol_idx(&amino->super, symbols[i]) == i);
  }

  aye(imm_abc_any_symbol(&amino->super) == IMM_AMINO_ANY_SYMBOL);
  aye(imm_abc_has_symbol(&amino->super, 'A'));
  aye(!imm_abc_has_symbol(&amino->super, 'B'));

  aye((int)amino->super.typeid == IMM_AMINO);
}

static void amino_lprob(void)
{
  float const array[] = {imm_lprob_zero(), logf(1), [19] = logf(19)};
  struct imm_amino_lprob lprob = imm_amino_lprob(&imm_amino_iupac, array);

  aye(imm_lprob_is_zero(imm_amino_lprob_get(&lprob, 'A')));
  near(imm_amino_lprob_get(&lprob, 'C'), logf(1));
  near(imm_amino_lprob_get(&lprob, 'Y'), logf(19));
}

static void abc_io(void)
{
  struct lio_writer writer = {0};
  struct lio_reader reader = {0};
  struct imm_abc abc_out = {0};

  imm_abc_init(&abc_out, imm_str("ACGT"), '*');

  int fd = open("abc_io.imm", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  aye(fd != 0);
  lio_wsetup(&writer, fd);
  aye(imm_abc_pack(&abc_out, &writer) == 0);
  aye(close(fd) == 0);

  struct imm_abc abc_in = {0};

  fd = open("abc_io.imm", O_RDONLY, 0644);
  aye(fd != 0);
  lio_rsetup(&reader, fd);
  aye(imm_abc_unpack(&abc_in, &reader) == 0);
  aye(close(fd) == 0);

  aye(abc_in.any_symbol_id == abc_out.any_symbol_id);
  aye(abc_in.size == abc_out.size);
  aye(abc_in.typeid == abc_out.typeid);
  aye(!strcmp(abc_in.symbols, abc_out.symbols));

  remove("abc_io.imm");
}

static void amino_io(void)
{
  struct lio_writer writer = {0};
  struct lio_reader reader = {0};
  struct imm_amino const *amino_out = &imm_amino_iupac;

  int fd = open("amino.imm", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  aye(fd != 0);
  lio_wsetup(&writer, fd);
  aye(imm_abc_pack((struct imm_abc const *)amino_out, &writer) == 0);
  aye(close(fd) == 0);

  struct imm_amino amino_in = {0};

  fd = open("amino.imm", O_RDONLY, 0644);
  aye(fd != 0);
  lio_rsetup(&reader, fd);
  aye(imm_abc_unpack((struct imm_abc *)&amino_in, &reader) == 0);
  aye(close(fd) == 0);

  struct imm_abc const *out = &amino_out->super;
  struct imm_abc const *in = &amino_in.super;

  aye(in->any_symbol_id == out->any_symbol_id);
  aye(in->size == out->size);
  aye(in->typeid == out->typeid);
  aye(!strcmp(in->symbols, out->symbols));

  remove("amino.imm");
}

static void dna_io(void)
{
  struct lio_writer writer = {0};
  struct lio_reader reader = {0};
  struct imm_dna const *dna_out = &imm_dna_iupac;

  int fd = open("dna.imm", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  aye(fd != 0);
  lio_wsetup(&writer, fd);
  aye(imm_abc_pack((struct imm_abc const *)dna_out, &writer) == 0);
  aye(close(fd) == 0);

  struct imm_dna dna_in = {0};

  fd = open("dna.imm", O_RDONLY, 0644);
  aye(fd != 0);
  lio_rsetup(&reader, fd);
  aye(imm_abc_unpack((struct imm_abc *)&dna_in, &reader) == 0);
  aye(close(fd) == 0);

  struct imm_abc const *out = &dna_out->super.super;
  struct imm_abc const *in = &dna_in.super.super;

  aye(in->any_symbol_id == out->any_symbol_id);
  aye(in->size == out->size);
  aye(in->typeid == out->typeid);
  aye(!strcmp(in->symbols, out->symbols));

  remove("dna.imm");
}

static void rna_io(void)
{
  struct lio_writer writer = {0};
  struct lio_reader reader = {0};
  struct imm_rna const *rna_out = &imm_rna_iupac;

  int fd = open("rna.imm", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  aye(fd != 0);
  lio_wsetup(&writer, fd);
  aye(imm_abc_pack((struct imm_abc const *)rna_out, &writer) == 0);
  aye(close(fd) == 0);

  struct imm_rna rna_in = {0};

  fd = open("rna.imm", O_RDONLY, 0644);
  aye(fd != 0);
  lio_rsetup(&reader, fd);
  aye(imm_abc_unpack((struct imm_abc *)&rna_in, &reader) == 0);
  aye(close(fd) == 0);

  struct imm_abc const *out = &rna_out->super.super;
  struct imm_abc const *in = &rna_in.super.super;

  aye(in->any_symbol_id == out->any_symbol_id);
  aye(in->size == out->size);
  aye(in->typeid == out->typeid);
  aye(!strcmp(in->symbols, out->symbols));

  remove("rna.imm");
}

int main(void)
{
  aye_begin();
  standard_usage();
  duplicated_alphabet();
  duplicated_any_symbol();
  symbol_outside_range();
  any_symbol_outside_range();
  union_size();
  get_lprob();
  amino_success();
  amino_lprob();
  abc_io();
  amino_io();
  dna_io();
  rna_io();
  return aye_end();
}
