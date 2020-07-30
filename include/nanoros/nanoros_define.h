#pragma once



#if defined NANOROS_API

#else

#ifdef nanoros_EXPORTS // for DLL build
#define NANOROS_API __declspec(dllexport)
#else // for user
#define NANOROS_API __declspec(dllimport)
#endif 

#endif