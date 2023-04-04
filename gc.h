#ifndef IMM_GC_H
#define IMM_GC_H

#include "codon.h"
#include "dna.h"
#include "export.h"

IMM_API extern struct imm_dna const *const imm_gc_dna;
IMM_API char const *imm_gc_name1(unsigned id);
IMM_API char const *imm_gc_name2(unsigned id);
IMM_API unsigned imm_gc_size(unsigned id);
IMM_API struct imm_codon imm_gc_codon(unsigned id, unsigned idx);
IMM_API char imm_gc_aa(unsigned id, unsigned idx);
IMM_API char imm_gc_decode(unsigned id, struct imm_codon codon);

#endif
