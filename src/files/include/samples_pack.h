// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _SAMPLES_PACK_H_
#define _SAMPLES_PACK_H_

#ifndef NOCODEC

// ------------------------------------------------------
// Constants
#define MP3_FRAMES_LAG 65536

// ------------------------------------------------------
// Functions
int ToGSM(short *Source, short *Dest, int Size);
int ToMP3(short *Source, short *Dest, int Size);
int ToTrueSpeech(short *Source, short *Dest, int Size);

#endif

#endif
