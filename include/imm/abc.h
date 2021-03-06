#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/export.h"
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define IMM_ABC_INVALID_IDX UINT8_MAX

/** @file abc.h
 * Alphabet module.
 *
 * An alphabet is represented by an (immutable) object of type @ref imm_abc.
 */

enum imm_symbol_type
{
    IMM_SYMBOL_UNKNOWN = 0,
    IMM_SYMBOL_NORMAL = 1,
    IMM_SYMBOL_ANY = 2,
};

#define IMM_FIRST_CHAR '!'
#define IMM_LAST_CHAR '~'

#define IMM_SYMBOL_IDX_SIZE ((size_t)((IMM_LAST_CHAR - IMM_FIRST_CHAR) + 1))

struct imm_abc;

struct imm_abc_vtable
{
    uint8_t (*type_id)(struct imm_abc const* abc);
    int (*write)(struct imm_abc const* abc, FILE* stream);
    void (*destroy)(struct imm_abc const* abc);
    struct imm_abc const* (*clone)(struct imm_abc const* abc);
};

/**
 * Alphabet.
 *
 * It represents a finite set of symbols and a special any-symbol
 * symbol. It should be used as an immutable object.
 */
struct imm_abc
{
    char const* symbols;
    uint8_t     length;
    uint8_t     symbol_idx[IMM_SYMBOL_IDX_SIZE];
    char        any_symbol;

    struct imm_abc_vtable vtable;
    void*                 derived;
};

static inline char            imm_abc_any_symbol(struct imm_abc const* abc);
IMM_API struct imm_abc const* imm_abc_clone(struct imm_abc const* abc);
IMM_API struct imm_abc const* imm_abc_create(char const* symbols, char any_symbol);
IMM_API void                  imm_abc_destroy(struct imm_abc const* abc);
static inline bool            imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id);
static inline uint8_t         imm_abc_length(struct imm_abc const* abc);
IMM_API struct imm_abc*       imm_abc_read(FILE* stream);
static inline char            imm_abc_symbol_id(struct imm_abc const* abc, uint8_t symbol_idx);
static inline uint8_t         imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id);
IMM_API enum imm_symbol_type  imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id);
static inline char const*     imm_abc_symbols(struct imm_abc const* abc);
static inline uint8_t         imm_abc_type_id(struct imm_abc const* abc);
IMM_API int                   imm_abc_write(struct imm_abc const* abc, FILE* stream);

IMM_API struct imm_abc const* __imm_abc_clone(struct imm_abc const* abc);
IMM_API struct imm_abc*       __imm_abc_create(char const* symbols, char any_symbol, void* derived);
static inline void const*     __imm_abc_derived(struct imm_abc const* abc);
IMM_API void                  __imm_abc_destroy(struct imm_abc const* abc);
static inline uint8_t         __imm_abc_index(char const c);
IMM_API uint8_t               __imm_abc_type_id(struct imm_abc const* abc);
IMM_API int                   __imm_abc_write(struct imm_abc const* abc, FILE* stream);

static inline char imm_abc_any_symbol(struct imm_abc const* abc) { return abc->any_symbol; }

static inline bool imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id < IMM_FIRST_CHAR || symbol_id > IMM_LAST_CHAR)
        return false;

    return abc->symbol_idx[__imm_abc_index(symbol_id)] != IMM_ABC_INVALID_IDX;
}

static inline uint8_t imm_abc_length(struct imm_abc const* abc) { return abc->length; }

static inline char imm_abc_symbol_id(struct imm_abc const* abc, uint8_t symbol_idx) { return abc->symbols[symbol_idx]; }

static inline uint8_t imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id)
{
    return abc->symbol_idx[__imm_abc_index(symbol_id)];
}

static inline char const* imm_abc_symbols(struct imm_abc const* abc) { return abc->symbols; }

static inline uint8_t imm_abc_type_id(struct imm_abc const* abc) { return abc->vtable.type_id(abc); }

static inline void const* __imm_abc_derived(struct imm_abc const* abc) { return abc->derived; }

static inline uint8_t __imm_abc_index(char const c) { return (uint8_t)(c - IMM_FIRST_CHAR); }

#endif
