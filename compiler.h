#ifndef IMM_COMPILER_H
#define IMM_COMPILER_H

/* clang-format off */
#ifdef IMM_STATIC_DEFINE
#  define IMM_API
#else
#  ifdef imm_EXPORTS /* We are building this library */
#    ifdef _WIN32
#      define IMM_API __declspec(dllexport)
#    else
#      define IMM_API __attribute__((visibility("default")))
#    endif
#  else /* We are using this library */
#    ifdef _WIN32
#      define IMM_API __declspec(dllimport)
#    else
#      define IMM_API __attribute__((visibility("default")))
#    endif
#  endif
#endif
/* clang-format on */

#ifdef __has_builtin
#define IMM_HAS_BUILTIN(x) __has_builtin(x)
#else
#define IMM_HAS_BUILTIN(x) (0)
#endif

#if IMM_HAS_BUILTIN(__builtin_unreachable)
#define IMM_UNREACHABLE() __builtin_unreachable()
#else
#define IMM_UNREACHABLE() (void)(0)
#endif

#ifdef __has_attribute
#define IMM_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define IMM_HAS_ATTRIBUTE(x) (0)
#endif

#if IMM_HAS_ATTRIBUTE(unused)
#define IMM_UNUSED __attribute__((unused))
#else
#define IMM_UNUSED
#endif

#if IMM_HAS_ATTRIBUTE(always_inline)
#define IMM_INLINE static inline __attribute__((always_inline))
#else
#define IMM_INLINE static inline
#endif

#if IMM_HAS_ATTRIBUTE(const)
#define IMM_CONST IMM_INLINE __attribute__((const))
#else
#define IMM_CONST IMM_INLINE
#endif

#if IMM_HAS_ATTRIBUTE(pure)
#define IMM_PURE IMM_INLINE __attribute__((pure))
#else
#define IMM_PURE IMM_INLINE
#endif

#endif
