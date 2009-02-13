// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/endianness.h"

// ------------------------------------------------------
// Functions
#if defined(__BIG_ENDIAN__)

Uint64 __Swap_64(Uint64 Value)
{
#if defined(__GCC__)
    return(((Value & 0xffll) << 56) |
           ((Value & 0xff00ll) << 40) |
           ((Value & 0xff0000ll) << 24) |
           ((Value & 0xff000000ll) << 8) |
           ((Value & 0xff00000000ll) >> 8) |
           ((Value & 0xff0000000000ll) >> 24) |
           ((Value & 0xff000000000000ll) >> 40) |
           ((Value & 0xff00000000000000ll) >> 56)
          );
#else
    return(((Value & 0xff) << 56) |
           ((Value & 0xff00) << 40) |
           ((Value & 0xff0000) << 24) |
           ((Value & 0xff000000) << 8) |
           ((Value & 0xff00000000) >> 8) |
           ((Value & 0xff0000000000) >> 24) |
           ((Value & 0xff000000000000) >> 40) |
           ((Value & 0xff00000000000000) >> 56)
          );
#endif
}

int __Swap_32(int Value)
{
    return(((Value & 0xff) << 24) |
           ((Value & 0xff00) << 8) |
           ((Value & 0xff0000) >> 8) |
           ((Value & 0xff000000) >> 24)
          );
}

short __Swap_16(short Value)
{
    return(((Value & 0xff) << 8) |
           ((Value & 0xff00) >> 8)
          );
}

#endif
