#ifndef IMM_API_H_API
#define IMM_API_H_API

#ifdef _WIN32
  #ifdef IMM_API_EXPORTS
    #define IMM_API __declspec(dllexport)
  #else
    #define IMM_API __declspec(dllimport)
  #endif
#else
  #define IMM_API
#endif

#endif
