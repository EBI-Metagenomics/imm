#ifndef IMM_API_H
#define IMM_API_H

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

#endif
