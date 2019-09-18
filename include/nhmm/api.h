#ifndef NHMM_API_H
#define NHMM_API_H

#ifdef _WIN32
#ifdef NHMM_API_EXPORTS
#define NHMM_API __declspec(dllexport)
#else
#define NHMM_API __declspec(dllimport)
#endif
#else
#define NHMM_API
#endif

/* Borrowed from GLIB. */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#define NHMM_DEPR __attribute__((__deprecated__))
#else
#define NHMM_DEPR
#endif

#define NHMM_DEPRECATED NHMM_DEPR
#define NHMM_DEPRECATED_API NHMM_DEPR NHMM_API

#endif
