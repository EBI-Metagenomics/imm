#ifndef IMM_API_H
#define IMM_API_H

#ifdef _WIN32
#ifdef IMM_EXPORTS
#define IMM_API __declspec(dllexport)
#else
#define IMM_API __declspec(dllimport)
#endif
#else
#define IMM_API
#endif

#endif
