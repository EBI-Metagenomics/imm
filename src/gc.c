#include "gc.h"
#include "imm/codon.h"
#include "imm/dna.h"
#include "imm/gc.h"
#include "imm/generics.h"

extern struct gc const gencode[];
struct imm_nuclt const *nuclt = &imm_dna_default.super;

char const *imm_gc_name1(unsigned id) { return gencode[id - 1].name1; }

char const *imm_gc_name2(unsigned id) { return gencode[id - 1].name2; }

unsigned imm_gc_ncodons(unsigned id, char aa)
{
    unsigned ncodons = 0;
    for (char const *c = gencode[id - 1].ncbieaa; *c; ++c)
    {
        if (*c == aa)
            ncodons++;
    }
    return ncodons;
}

struct imm_codon imm_gc_codon(unsigned id, char aa, unsigned idx)
{
    struct gc const *gc = &gencode[id - 1];
    unsigned i = 0;
    for (char const *c = gc->ncbieaa; *c; ++c)
    {
        if (*c == aa)
        {
            if (idx == 0)
                break;
            idx--;
        }
        i++;
    }
    return imm_codon(nuclt, gc->base1[i], gc->base2[i], gc->base3[i]);
}

char imm_gc_decode(unsigned id, char a, char b, char c)
{
    struct gc const *gc = &gencode[id - 1];

    char const *aa = gc->ncbieaa;
    char const *b1 = gc->base1;
    char const *b2 = gc->base2;
    char const *b3 = gc->base3;

    while (*aa)
    {
        if (*b1 == a && *b2 == b && *b3 == c)
            break;
        ++aa;
        ++b1;
        ++b2;
        ++b3;
    }
    return *aa;
}
