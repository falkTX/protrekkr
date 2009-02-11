// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _PTK_REPLAY_H_
#define _PTK_REPLAY_H_

// ------------------------------------------------------
// Includes
#if defined(__WIN32__)
#include <windows.h>
#define PTKEXPORT __stdcall
#endif

#if defined(__LINUX__) || defined(__MACOSX__) || defined(__PSP__)
#define PTKEXPORT
#endif

// ------------------------------------------------------
// Functions
#if !defined(__WINAMP__)
#if defined(__WIN32__)
int PTKEXPORT Ptk_InitDriver(HWND hWnd, int Latency);
#else
int PTKEXPORT Ptk_InitDriver(int Latency);
#endif // __WIN32__
#else
int PTKEXPORT Ptk_InitDriver(void);
#endif // __WINAMP__
int PTKEXPORT Ptk_InitModule(unsigned char *Module, int start_position);
int PTKEXPORT Ptk_GetRow(void);
int PTKEXPORT Ptk_GetPosition(void);
void PTKEXPORT Ptk_SetPosition(int position);
void PTKEXPORT Ptk_Play(void);
void PTKEXPORT Ptk_Stop(void);
void PTKEXPORT Ptk_ReleaseDriver(void);

#endif
