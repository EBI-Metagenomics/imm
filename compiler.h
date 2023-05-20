#ifndef IMM_COMPILER_H
#define IMM_COMPILER_H

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
#define IMM_CONST __attribute__((const))
#else
#define IMM_CONST
#endif

#if __has_attribute(pure)
#define IMM_PURE __attribute__((pure))
#else
#define IMM_PURE
#endif

#if __has_attribute(always_inline)
#define IMM_FORCE_INLINE __attribute__((always_inline))
#else
#define IMM_FORCE_INLINE
#endif

/*
 * FORCE_INLINE_IMM_TEMPLATE is used to define C "templates", which take
 * constant parameters. They must be inlined for the compiler to eliminate the
 * constant branches.
 *
 * Acknowledgement: ZSTD.
 */
#define IMM_TEMPLATE static inline IMM_FORCE_INLINE

#define IMM_NOINLINE __attribute__((noinline))

#endif
