#ifndef NHMM_API_H_API
#define NHMM_API_H_API

#ifdef _WIN32
#ifdef NHMM_API_EXPORTS
#define NHMM_API __declspec(dllexport)
#else
#define NHMM_API __declspec(dllimport)
#endif
#else
#define NHMM_API
#endif

#endif
