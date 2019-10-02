#ifndef IMM_ALPHABET_H
#define IMM_ALPHABET_H

struct imm_alphabet;

int alphabet_has_symbol(const struct imm_alphabet *alphabet, char symbol_id);
int alphabet_symbol_idx(const struct imm_alphabet *alphabet, char symbol_id);
char alphabet_symbol_id(const struct imm_alphabet *alphabet, int symbol_idx);

#endif
