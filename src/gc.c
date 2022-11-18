#include "gc.h"
#include "imm/codon.h"
#include "imm/dna.h"
#include "imm/gc.h"
#include "imm/generics.h"
#include <assert.h>
#include <string.h>

extern struct gc const gencode[];
struct imm_dna const *const imm_gc_dna = &imm_dna_iupac;

char const *imm_gc_name1(unsigned id) { return gencode[id - 1].name1; }

char const *imm_gc_name2(unsigned id) { return gencode[id - 1].name2; }

unsigned imm_gc_size(void)
{
    struct gc const *gc = &gencode[0];
    return (unsigned)strlen(gc->ncbieaa);
}

struct imm_codon imm_gc_codon(unsigned id, unsigned idx)
{
    struct gc const *gc = &gencode[id - 1];
    struct imm_nuclt const *nuclt = &imm_gc_dna->super;
    return imm_codon_from_symbols(nuclt, gc->base1[idx], gc->base2[idx],
                                  gc->base3[idx]);
}

char imm_gc_aa(unsigned id, unsigned idx)
{
    struct gc const *gc = &gencode[id - 1];
    return gc->ncbieaa[idx];
}

static unsigned perfect_hash_id1(char key);

char imm_gc_decode(unsigned id, struct imm_codon codon)
{
    assert(id == 1);
    struct gc const *gc = &gencode[id - 1];
    char const *aa = gc->ncbieaa;
    char const a = imm_abc_symbols(&imm_gc_dna->super.super)[codon.a];
    char const b = imm_abc_symbols(&imm_gc_dna->super.super)[codon.b];
    char const c = imm_abc_symbols(&imm_gc_dna->super.super)[codon.c];
    unsigned const i[3] = {perfect_hash_id1(a), perfect_hash_id1(b),
                           perfect_hash_id1(c)};
    return aa[i[0] * 4 * 4 + i[1] * 4 + i[2]];
}

static inline unsigned hash_f(unsigned key, unsigned t)
{
    return (t * key) % 8;
}

static unsigned perfect_hash_id1(char key)
{
    /* Generated using perfect-hash Python package */
    unsigned k = (unsigned)key;
    static unsigned const G[] = {0, 1, 0, 2, 0, 3, 0, 0};
    return (G[hash_f(k, 'D')] + G[hash_f(k, 's')]) % 8;
}
