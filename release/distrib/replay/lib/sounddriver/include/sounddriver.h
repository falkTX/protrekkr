// ------------------------------------------------------
// Protrekkr
// Based on Juan Antonio Arguelles Rius's NoiseTrekker.
//
// Copyright (C) 2008-2009 Franck Charlet.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL FRANCK CHARLET OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
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

#if defined(__AMIGAOS4__) || defined(__AROS__)
#include <devices/ahi.h>
#include <proto/exec.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <float.h>
#include <memory.h>
#include <math.h>
#endif

#if defined(__MACOSX__)
#include <AudioUnit/AudioUnit.h>
#include <unistd.h>
#endif

#if defined(__PSP__)
#include <pspkernel.h>
#include <pspaudio.h>
#include <malloc.h>
#include <string.h>
#endif

#if defined(__WIN32__)
#if !defined(__GCC__)
#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>
#endif

#if defined(__WIN32__)
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#if !defined(__WIN32__) && !defined(__AROS__)
#define TRUE 1
#define FALSE 0
#endif

// ------------------------------------------------------
// Constants
#define AUDIO_PCM_FREQ 44100

#if defined(__PSP__)
#define AUDIO_THREAD_STACKSIZE (1024 * 4)
#define AUDIO_THREAD_PRIORITY 2
#endif

#define AUDIO_DBUF_CHANNELS 2
#define AUDIO_DBUF_RESOLUTION 16

// ------------------------------------------------------
// Types
typedef unsigned int Uint32;
typedef unsigned char Uint8;
#if !defined(__AMIGAOS4__)
typedef int int32;
typedef char int8;
#endif

#if defined(__AROS__)
#define int32 int32_t
#endif

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
