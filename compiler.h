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
#define CONST_ATTR __attribute__((const))
#else
#define CONST_ATTR
#endif

#if __has_attribute(pure)
#define PURE_ATTR __attribute__((pure))
#else
#define PURE_ATTR
#endif

#if __has_attribute(always_inline)
#define FORCE_INLINE_ATTR __attribute__((always_inline))
#else
#define FORCE_INLINE_ATTR
#endif

/*
 * FORCE_INLINE_TEMPLATE is used to define C "templates", which take constant
 * parameters. They must be inlined for the compiler to eliminate the constant
 * branches.
 *
 * Acknowledgement: ZSTD.
 */
#define TEMPLATE static inline FORCE_INLINE_ATTR

#define NOINLINE __attribute__((noinline))

#endif
