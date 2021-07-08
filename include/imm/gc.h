#ifndef IMM_GC_H
#define IMM_GC_H

#include "imm/codon.h"
#include "imm/export.h"

IMM_API char const *imm_gc_name1(unsigned id);

IMM_API char const *imm_gc_name2(unsigned id);

IMM_API unsigned imm_gc_ncodons(unsigned id, char aa);

IMM_API struct imm_codon imm_gc_codon(unsigned id, char aa, unsigned idx);

IMM_API char imm_gc_decode(unsigned id, char a, char b, char c);

#endif
