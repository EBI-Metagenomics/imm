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

static unsigned perfect_hash(char key);

char imm_gc_decode(unsigned id, char a, char b, char c)
{
    struct gc const *gc = &gencode[id - 1];
    char const *aa = gc->ncbieaa;
    unsigned const i[3] = {perfect_hash(a), perfect_hash(b), perfect_hash(c)};
    return aa[i[0] * 4 * 4 + i[1] * 4 + i[2]];
}

static inline unsigned hash_f(unsigned key, unsigned t)
{
    return (t * key) % 8;
}

static unsigned perfect_hash(char key)
{
    /* Generated using perfect-hash Python package */
    unsigned k = (unsigned)key;
    static unsigned const G[] = {0, 1, 0, 2, 0, 3, 0, 0};
    return (G[hash_f(k, 'D')] + G[hash_f(k, 's')]) % 8;
}
