// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#if defined(__BIG_ENDIAN__)

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
