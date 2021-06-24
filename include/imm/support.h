#ifndef IMM_SUPPORT_H
#define IMM_SUPPORT_H

#ifdef __GNUC__
#define imm_likely(x) __builtin_expect(!!(x), 1)
#define imm_unlikely(x) __builtin_expect(!!(x), 0)
#else
#define imm_likely(x) (x)
#define imm_unlikely(x) (x)
#endif

#define IMM_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define IMM_STRUCT_FIELD(s, f) (((struct s *)0)->f)

#endif
