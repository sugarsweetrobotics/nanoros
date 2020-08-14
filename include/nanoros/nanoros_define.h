#pragma once



#ifdef NANOROS_API

#else

#ifdef WIN32
#ifdef nanoros_EXPORTS // for DLL build
#define NANOROS_API __declspec(dllexport)
#else // for user
#define NANOROS_API __declspec(dllimport)
#endif

#else
#define NANOROS_API
#endif


#endif