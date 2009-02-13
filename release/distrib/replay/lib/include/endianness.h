// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef __ENDIANNESS_H_
#define __ENDIANNESS_H_

#if defined(__WIN32__) && !defined(__GCC__)
typedef unsigned __int64 Uint64;
#else
typedef unsigned long long Uint64;
#endif

// ------------------------------------------------------
// Functions
#if defined(__BIG_ENDIAN__)

Uint64 __Swap_64(Uint64 Value);
int __Swap_32(int Value);
short __Swap_16(short Value);

#define Swap_64(x) __Swap_64((Uint64) x)
#define Swap_32(x) __Swap_32((int) x)
#define Swap_16(x) __Swap_16((int) x)

#else

#define Swap_64(x) (Uint64) x
#define Swap_32(x) (int) x
#define Swap_16(x) (int) x

#endif

#endif
