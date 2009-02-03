// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef __ENDIANNESS_H_
#define __ENDIANNESS_H_

// ------------------------------------------------------
// Functions
#if defined(__BIG_ENDIAN__)

int __Swap_32(int Value);
short __Swap_16(short Value);

#define Swap_32(x) __Swap_32((int) x);
#define Swap_16(x) __Swap_16((int) x);

#else

#define Swap_32(x) (int) x
#define Swap_16(x) (short) x

#endif

#endif
