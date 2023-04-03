#ifndef IMM_CSPAN_H
#define IMM_CSPAN_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

struct imm_cspan
{
  uint8_t min;
  uint8_t max;
} __attribute__((__packed__));

static_assert(sizeof(struct imm_cspan) == 2, "struct imm_cspan must be packed");

struct imm_cspan imm_cspan_init(unsigned min, unsigned max);
uint16_t imm_cspan_zip(struct imm_cspan const *);
bool imm_cspan_unzip(struct imm_cspan *, uint16_t v);

#endif
