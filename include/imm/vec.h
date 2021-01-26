#ifndef IMM_VEC_H
#define IMM_VEC_H

#include "imm/export.h"
#include <stddef.h>

struct imm_state;
struct imm_veci;
struct imm_vecp;

IMM_API void             imm_veci_append(struct imm_veci* vec, int item);
IMM_API struct imm_veci* imm_veci_create(void);
IMM_API void             imm_veci_destroy(struct imm_veci const* vec);
IMM_API int              imm_veci_get(struct imm_veci const* vec, size_t i);
IMM_API size_t           imm_veci_length(struct imm_veci const* vec);

IMM_API void             imm_vecp_append(struct imm_vecp* vec, void const* item);
IMM_API struct imm_vecp* imm_vecp_create(void);
IMM_API void             imm_vecp_destroy(struct imm_vecp const* vec);
IMM_API void const*      imm_vecp_get(struct imm_vecp const* vec, size_t i);
IMM_API size_t           imm_vecp_length(struct imm_vecp const* vec);

#endif
