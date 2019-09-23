#ifndef NHMM_ALPHABET_H
#define NHMM_ALPHABET_H

struct nhmm_alphabet;

int alphabet_has_symbol(const struct nhmm_alphabet *alphabet, char symbol_id);
int alphabet_symbol_idx(const struct nhmm_alphabet *alphabet, char symbol_id);
char alphabet_symbol_id(const struct nhmm_alphabet *alphabet, int symbol_idx);

#endif
