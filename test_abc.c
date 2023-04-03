#include "abc.h"
#include "abc_lprob.h"
#include "amino.h"
#include "amino_lprob.h"
#include "dna.h"
#include "lip/file/file.h"
#include "lprob.h"
#include "rc.h"
#include "rna.h"
#include "test_helper.h"

static void standard_usage(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("ACGT"), '*'), 0);
  eq(imm_abc_symbol_idx(&abc, 'G'), 2);
  eq(imm_abc_size(&abc), 4);
  ok(imm_abc_has_symbol(&abc, 'C'));
  ok(!imm_abc_has_symbol(&abc, 'L'));
  eq(imm_abc_symbol_type(&abc, 'A'), IMM_SYM_NORMAL);
  eq(imm_abc_symbol_type(&abc, '*'), IMM_SYM_ANY);
  eq(imm_abc_symbol_type(&abc, 'L'), IMM_SYM_NULL);
  eq(abc.typeid, IMM_ABC);
}

static void duplicated_alphabet(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("ACTC"), '*'), IMM_EINVAL);
}

static void duplicated_any_symbol(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("AC*T"), '*'), IMM_EINVAL);
}

static void symbol_outside_range(void)
{

  struct imm_abc abc = {0};
  char symbols[] = {3, '\0'};
  eq(imm_abc_init(&abc, (struct imm_str){2, symbols}, '*'), IMM_EINVAL);
}

static void any_symbol_outside_range(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("ACGT"), 3), IMM_EINVAL);
}

static void union_size(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("ACT"), '*'), 0);
  char data[] = "ACAAAAAAAAC*AATT*G";
  eq(imm_abc_union_size(&abc, imm_str(data)), 1);
}

static void get_lprob(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("ACGT"), 'X'), 0);

  float const lprobs[4] = {log(0.2), log(0.01), log(1.0), log(0.5)};
  struct imm_abc_lprob lprob = imm_abc_lprob(&abc, lprobs);

  close(imm_abc_lprob_get(&lprob, 'A'), log(0.2));
  close(imm_abc_lprob_get(&lprob, 'C'), log(0.01));
  close(imm_abc_lprob_get(&lprob, 'T'), log(0.5));
}

static void amino_success(void)
{
  char const symbols[IMM_AMINO_SIZE + 1] = IMM_AMINO_SYMBOLS;

  struct imm_amino const *amino = &imm_amino_iupac;
  for (unsigned i = 0; i < imm_abc_size(&amino->super); ++i)
  {
    eq(imm_abc_symbols(&amino->super)[i], symbols[i]);
    eq(imm_abc_symbol_idx(&amino->super, symbols[i]), i);
  }

  eq(imm_abc_any_symbol(&amino->super), IMM_AMINO_ANY_SYMBOL);
  ok(imm_abc_has_symbol(&amino->super, 'A'));
  ok(!imm_abc_has_symbol(&amino->super, 'B'));

  eq(amino->super.typeid, IMM_AMINO);
}

static void amino_lprob(void)
{
  float const array[] = {imm_lprob_zero(), log(1), [19] = log(19)};
  struct imm_amino_lprob lprob = imm_amino_lprob(&imm_amino_iupac, array);

  ok(imm_lprob_is_zero(imm_amino_lprob_get(&lprob, 'A')));
  close(imm_amino_lprob_get(&lprob, 'C'), log(1));
  close(imm_amino_lprob_get(&lprob, 'Y'), log(19));
}

static void abc_io(void)
{
  struct lip_file file = {0};
  struct imm_abc abc_out = {0};

  imm_abc_init(&abc_out, IMM_STR("ACGT"), '*');

  file.fp = fopen("abc_io.imm", "wb");
  eq(imm_abc_pack(&abc_out, &file), 0);
  fclose(file.fp);

  struct imm_abc abc_in = {0};

  file.fp = fopen("abc_io.imm", "rb");
  eq(imm_abc_unpack(&abc_in, &file), 0);
  fclose(file.fp);

  eq(abc_in.any_symbol_id, abc_out.any_symbol_id);
  eq(abc_in.size, abc_out.size);
  eq(abc_in.typeid, abc_out.typeid);
  lsequal(abc_in.symbols, abc_out.symbols);
}

static void amino_io(void)
{
  struct lip_file file = {0};
  struct imm_amino const *amino_out = &imm_amino_iupac;

  file.fp = fopen("amino.imm", "wb");
  eq(imm_abc_pack((struct imm_abc const *)amino_out, &file), 0);
  fclose(file.fp);

  struct imm_amino amino_in = {0};

  file.fp = fopen("amino.imm", "rb");
  eq(imm_abc_unpack((struct imm_abc *)&amino_in, &file), 0);
  fclose(file.fp);

  struct imm_abc const *out = &amino_out->super;
  struct imm_abc const *in = &amino_in.super;

  eq(in->any_symbol_id, out->any_symbol_id);
  eq(in->size, out->size);
  eq(in->typeid, out->typeid);
  lsequal(in->symbols, out->symbols);
}

static void dna_io(void)
{
  struct lip_file file = {0};
  struct imm_dna const *dna_out = &imm_dna_iupac;

  file.fp = fopen("dna.imm", "wb");
  eq(imm_abc_pack((struct imm_abc const *)dna_out, &file), 0);
  fclose(file.fp);

  struct imm_dna dna_in = {0};

  file.fp = fopen("dna.imm", "rb");
  eq(imm_abc_unpack((struct imm_abc *)&dna_in, &file), 0);
  fclose(file.fp);

  struct imm_abc const *out = &dna_out->super.super;
  struct imm_abc const *in = &dna_in.super.super;

  eq(in->any_symbol_id, out->any_symbol_id);
  eq(in->size, out->size);
  eq(in->typeid, out->typeid);
  lsequal(in->symbols, out->symbols);
}

static void rna_io(void)
{
  struct lip_file file = {0};
  struct imm_rna const *rna_out = &imm_rna_iupac;

  file.fp = fopen("rna.imm", "wb");
  eq(imm_abc_pack((struct imm_abc const *)rna_out, &file), 0);
  fclose(file.fp);

  struct imm_rna rna_in = {0};

  file.fp = fopen("rna.imm", "rb");
  eq(imm_abc_unpack((struct imm_abc *)&rna_in, &file), 0);
  fclose(file.fp);

  struct imm_abc const *out = &rna_out->super.super;
  struct imm_abc const *in = &rna_in.super.super;

  eq(in->any_symbol_id, out->any_symbol_id);
  eq(in->size, out->size);
  eq(in->typeid, out->typeid);
  lsequal(in->symbols, out->symbols);
}

int main(void)
{
  lrun("standard_usage", standard_usage);
  lrun("duplicated_alphabet", duplicated_alphabet);
  lrun("duplicated_any_symbol", duplicated_any_symbol);
  lrun("symbol_outside_range", symbol_outside_range);
  lrun("any_symbol_outside_range", any_symbol_outside_range);
  lrun("union_size", union_size);
  lrun("get_lprob", get_lprob);
  lrun("amino_success", amino_success);
  lrun("amino_lprob", amino_lprob);
  lrun("abc_io", abc_io);
  lrun("amino_io", amino_io);
  lrun("dna_io", dna_io);
  lrun("rna_io", rna_io);
  return lfails != 0;
}
