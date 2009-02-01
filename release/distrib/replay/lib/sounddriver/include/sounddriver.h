// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------
#ifndef _SOUNDDRIVER_H_
#define _SOUNDDRIVER_H_

// ------------------------------------------------------
// Includes
#if defined(__LINUX__)
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <float.h>
#include <memory.h>
#include <math.h>
#endif

#if defined(__WIN32__)
#if !defined(MINGW)
#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>
#endif

#if defined(__WIN32__)
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#if !defined(__WIN32__)
#define TRUE 1
#define FALSE 0
#endif

// ------------------------------------------------------
// Constants
#define AUDIO_PCM_FREQ 44100

#if defined(__PSP__)
#define AUDIO_THREAD_STACKSIZE (1024 * 32)
#define AUDIO_THREAD_PRIORITY (SCE_KERNEL_USER_HIGHEST_PRIORITY + 2)
#endif

#define AUDIO_DBUF_CHANNELS 2
#define AUDIO_DBUF_RESOLUTION 16

// ------------------------------------------------------
// Types
typedef unsigned int Uint32;
typedef int int32;
typedef unsigned char Uint8;
typedef char int8;

// ------------------------------------------------------
// Functions
extern int AUDIO_Latency;
extern int AUDIO_Milliseconds;

#if defined(__WIN32__)
int AUDIO_Init_Driver(HWND hWnd, void (STDCALL *Mixer)(Uint8 *, Uint32));
#else
int AUDIO_Init_Driver(void (STDCALL *Mixer)(Uint8 *, Uint32));
#endif
int AUDIO_Create_Sound_Buffer(int milliseconds);
void AUDIO_Stop_Sound_Buffer(void);
void AUDIO_Stop_Driver(void);
void AUDIO_Play(void);
void AUDIO_Stop(void);
int AUDIO_IsPlaying(void);
float AUDIO_GetTime(void);
int AUDIO_GetSamples(void);
void AUDIO_ResetTimer(void);

#endif
