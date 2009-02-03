// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _SAMPLES_UNPACK_H_
#define _SAMPLES_UNPACK_H_

#if !defined(__NO_CODEC__)

// ------------------------------------------------------
// Includes
#if defined(__STAND_ALONE__)
    #if defined(__WINAMP__)
        #include "ptk_def_properties.h"
    #else
        #if defined(__GCC__)
        #include "../../ptk_properties.h"
        #else
        #include "../ptk_properties.h"
        #endif
    #endif
#else
    #include "ptk_def_properties.h"
#endif

// ------------------------------------------------------
// Types
typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;

// ------------------------------------------------------
// Constants
#define MP3_FRAMES_DELAG (1460 - 355)
#define BITRATE 64

// ------------------------------------------------------
// Functions
void UnpackGSM(Uint8 *Source, short *Dest, int Src_Size, int Dst_Size);
void UnpackTrueSpeech(Uint8 *Source, short *Dest, int Src_Size, int Dst_Size);
void UnpackMP3(Uint8 *Source, short *Dest, int Src_Size, int Dst_Size);

#endif

#endif
