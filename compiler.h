#ifndef IMM_COMPILER_H
#define IMM_COMPILER_H

/*
 * __has_builtin is supported on gcc >= 10, clang >= 3 and icc >= 21.
 * In the meantime, to support gcc < 10, we implement __has_builtin
 * by hand.
 */
#ifndef __has_builtin
#define __has_builtin(x) (0)
#endif

#if !__has_builtin(__builtin_unreachable)
#define __builtin_unreachable() (void)(0)
#endif

#ifndef __has_attribute
#define __has_attribute(x) (0)
#endif

#if __has_attribute(const)
#define imm_const __attribute__((const))
#else
#define imm_const
#endif

#if __has_attribute(pure)
#define imm_pure __attribute__((pure))
#else
#define imm_pure
#endif

#if __has_attribute(always_inline)
#define imm_force_inline __attribute__((always_inline))
#else
#define imm_force_inline
#endif

/*
 * imm_template is used to define C "templates", which take constant
 * parameters. They must be inlined for the compiler to eliminate the constant
 * branches.
 *
 * Acknowledgement: ZSTD.
 */
#define imm_template static inline imm_force_inline

#define imm_const_template imm_const imm_template
#define imm_pure_template imm_pure imm_template

#endif
