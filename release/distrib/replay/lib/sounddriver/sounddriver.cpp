// ------------------------------------------------------
// Protrekkr
// Based on Juan Antonio Arguelles Rius's NoiseTrekker.
//
// Copyright (C) 2008-2010 Franck Charlet.
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

// TODO: split all this, it's starting to become a bit messy

// ------------------------------------------------------
// Includes
#include "include/sounddriver.h"

#if defined(__WIN32__)
    #define DIRECTSOUND_VERSION 0x0900
    #if defined(__GCC__)
    #include "include/DSound.h"
    #else
    #include <DSound.h>
    #endif
#endif

// ------------------------------------------------------
// Variables
unsigned int AUDIO_To_Fill;
#if defined(__PSP__)
int AUDIO_Samples __attribute__((aligned(64)));
int AUDIO_Play_Flag __attribute__((aligned(64)));
float AUDIO_Timer __attribute__((aligned(64)));
#else
int AUDIO_Samples;
int AUDIO_Play_Flag;
float AUDIO_Timer;
#endif

int volatile AUDIO_Acknowledge;

#if defined(__MACOSX__)
AudioDeviceID AUDIO_Device;
UInt32 Amount;
AudioStreamBasicDescription	Desc;
#endif

#if defined(__LINUX__)
int AUDIO_Device;
short *AUDIO_SoundBuffer;
audio_buf_info AUDIO_Info_Buffer;
pthread_t hThread;
int volatile Thread_Running;
#endif

#if defined(__AMIGAOS4__) || defined(__AROS__)
struct MsgPort *AHImp;
struct AHIRequest *AHIio;
struct AHIRequest *AHIio2;
struct AHIRequest *join;
short *AHIbuf;
short *AHIbuf2;
int volatile Thread_Running;
#endif

#if defined(__AMIGAOS4__)
pthread_t hThread;
#endif

#if defined(__AROS__)
#define USE_SDL_THREADS
#include <string.h>
#if defined(USE_SDL_THREADS)
#include <SDL/SDL_thread.h>
SDL_Thread *hThread;
#else
#include <pthread.h>
pthread_t hThread;
#endif
#endif

#if defined(__PSP__)
SceUID AUDIO_thid;
int AUDIO_HWChannel;
extern int Songplaying;
extern "C"
{
    void me_stub(void);
    void me_stub_end(void);
    void me_disable_int(void);
    void me_sceKernelDcacheWritebackInvalidateAll(void);
}
void Me_Handler(void) __attribute__((aligned(64)));
#endif

int AUDIO_SoundBuffer_Size;

#if defined(__WIN32__)
LPDIRECTSOUND AUDIO_DSound_Context;
DSBUFFERDESC AUDIO_Sound_Buffer_Desc;
LPDIRECTSOUNDBUFFER AUDIO_Sound_Buffer;
WAVEFORMATEX AUDIO_Wave_Format;

DWORD AUDIO_Buffer_Pos;
DWORD AUDIO_Old_Buffer_Pos;
LPVOID AUDIO_Audio_Ptr1;
DWORD AUDIO_Audio_Bytes1;
LPVOID AUDIO_Audio_Ptr2;
DWORD AUDIO_Audio_Bytes2;
DWORD AUDIO_ThreadId;
HANDLE AUDIO_hReplayThread;
#endif

int AUDIO_Latency;

#if defined(__AMIGAOS4__) || defined(__AROS__)
int AUDIO_Milliseconds = 100;
#else
int AUDIO_Milliseconds = 20;
#endif

// ------------------------------------------------------
// Functions
int AUDIO_Create_Sound_Buffer(int milliseconds);
void AUDIO_Stop_Sound_Buffer(void);
void (STDCALL *AUDIO_Mixer)(Uint8 *, Uint32);
void AUDIO_Mixer_Fill_Buffer(void *, Uint32);
void AUDIO_Synth_Play(void);

// ------------------------------------------------------
// Name: AUDIO_Thread()
// Desc: Audio rendering
#if defined(__MACOSX__)
static OSStatus AUDIO_Callback(AudioDeviceID device,
                               const AudioTimeStamp* current_time,
	                           const AudioBufferList* data_in,
                               const AudioTimeStamp* time_in,
	                           AudioBufferList *data_out,
                               const AudioTimeStamp *time_out,
                               void *client_data)
{

    if(AUDIO_Play_Flag)
    {
        AUDIO_Mixer((Uint8 *) data_out->mBuffers[0].mData, data_out->mBuffers[0].mDataByteSize);

        AUDIO_Samples += data_out->mBuffers[0].mDataByteSize;
        AUDIO_Timer = ((((float) AUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);
    }
    return(kAudioHardwareNoError);
}
#endif

#if defined(__LINUX__)
void *AUDIO_Thread(void *arg)
{
    while(Thread_Running)
    {
        if(AUDIO_Play_Flag && AUDIO_SoundBuffer)
        {
            AUDIO_Acknowledge = FALSE;
            AUDIO_Mixer((Uint8 *) AUDIO_SoundBuffer, AUDIO_SoundBuffer_Size);
            write(AUDIO_Device, AUDIO_SoundBuffer, AUDIO_SoundBuffer_Size);

            AUDIO_Samples += AUDIO_SoundBuffer_Size;
            AUDIO_Timer = ((((float) AUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);
        }
        else
        {
            AUDIO_Acknowledge = TRUE;
        }
        usleep(10);
    }
    Thread_Running = 1; 
    pthread_exit(0);
    return(0);
}
#endif

#if defined(__AMIGAOS4__)
void *AUDIO_Thread(void *arg)
{
    int32 old_sigbit = AHImp->mp_SigBit;
    void *old_sigtask = AHImp->mp_SigTask;
    AHImp->mp_SigBit = IExec->AllocSignal(-1);
    AHImp->mp_SigTask = IExec->FindTask(NULL);
    while(Thread_Running)
    {
        if(AUDIO_Play_Flag && AHIbuf)
        {
            struct AHIRequest *io = AHIio;
            short *buf = AHIbuf;
    
            AUDIO_Acknowledge = FALSE;
            AUDIO_Mixer((Uint8 *)buf, AUDIO_SoundBuffer_Size);
    
            io->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
            io->ahir_Std.io_Command = CMD_WRITE;
            io->ahir_Std.io_Data = buf;
            io->ahir_Std.io_Length = AUDIO_SoundBuffer_Size;
            io->ahir_Std.io_Offset = 0;
            io->ahir_Frequency = AUDIO_PCM_FREQ;
            io->ahir_Type = AHIST_S16S;
            io->ahir_Volume = 0x10000;
            io->ahir_Position = 0x8000;
            io->ahir_Link = join;
            IExec->SendIO((struct IORequest *)io);
            if (join) IExec->WaitIO((struct IORequest *)join);
            join = io;
            AHIio = AHIio2; AHIio2 = io;
            AHIbuf = AHIbuf2; AHIbuf2 = buf;
    
            AUDIO_Samples += AUDIO_SoundBuffer_Size;
            AUDIO_Timer = ((((float) AUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);
        }
        else
        {
            AUDIO_Acknowledge = TRUE;
            usleep(10);
        }
    }
    if (join)
    {
        IExec->AbortIO((struct IORequest *) join);
        IExec->WaitIO((struct IORequest *) join);
    }
    IExec->FreeSignal(AHImp->mp_SigBit);
    AHImp->mp_SigBit = old_sigbit;
    AHImp->mp_SigTask = old_sigtask;
    Thread_Running = 1;
    pthread_exit(0);
    return(0);
}
#endif

#if defined(__AROS__)
#if defined(USE_SDL_THREADS)
int AUDIO_Thread(void *arg)
#else
void *AUDIO_Thread(void *arg)
#endif
{
    int32 old_sigbit = AHImp->mp_SigBit;
    void *old_sigtask = AHImp->mp_SigTask;
    AHImp->mp_SigBit = AllocSignal(-1);
    AHImp->mp_SigTask = FindTask(NULL);

    while(Thread_Running)
    {
        if(AUDIO_Play_Flag && AHIbuf)
        {
            struct AHIRequest *io = AHIio;
            short *buf = AHIbuf;
        
            AUDIO_Acknowledge = FALSE;
            AUDIO_Mixer((Uint8 *) buf, AUDIO_SoundBuffer_Size);
        
            io->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
            io->ahir_Std.io_Command = CMD_WRITE;
            io->ahir_Std.io_Data = buf;
            io->ahir_Std.io_Length = AUDIO_SoundBuffer_Size;
            io->ahir_Std.io_Offset = 0;
            io->ahir_Frequency = AUDIO_PCM_FREQ;
            io->ahir_Type = AHIST_S16S;
            io->ahir_Volume = 0x10000;
            io->ahir_Position = 0x8000;
            io->ahir_Link = join;
            SendIO((struct IORequest *) io);
            if (join) WaitIO((struct IORequest *) join);
            join = io;
            AHIio = AHIio2; AHIio2 = io;
            AHIbuf = AHIbuf2; AHIbuf2 = buf;
        
            AUDIO_Samples += AUDIO_SoundBuffer_Size;
            AUDIO_Timer = ((((float) AUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);
        }
        else
        {
            AUDIO_Acknowledge = TRUE;
            usleep(10);
        }
        
    }
    if (join)
    {
        AbortIO((struct IORequest *) join);
        WaitIO((struct IORequest *) join);
    }
    FreeSignal(AHImp->mp_SigBit);
    AHImp->mp_SigBit = old_sigbit;
    AHImp->mp_SigTask = old_sigtask;
    Thread_Running = 1;
    
    #if !defined(USE_SDL_THREADS)
        //pthread_exit(0);
    #endif
    return(0);
}
#endif

#if defined(__PSP__)
short *ptrAudio_BufferPlay1;
short *ptrAudio_BufferPlay2;
int AUDIO_FlipFlop = FALSE;
int Mutex = FALSE;

void Me_Handler(void)
{
    volatile int *ptrAUDIO_Samples = (int *) (((int) &AUDIO_Samples) | 0x40000000);
    me_sceKernelDcacheWritebackInvalidateAll();
    me_disable_int();
    for(;;)
    {
        volatile int *ptrAUDIO_Samples = (int *) (((int) &AUDIO_Samples) | 0x40000000);
        volatile int *ptrAUDIO_Timer = (int *) (((int) &AUDIO_Timer) | 0x40000000);
        volatile int *ptrAUDIO_SoundBuffer_Size = (int *) (((int) &AUDIO_SoundBuffer_Size) | 0x40000000);
        volatile int *ptrAUDIO_FlipFlop = (int *) (((int) &AUDIO_FlipFlop) | 0x40000000);
        volatile int *ptrAUDIO_Play_Flag = (int *) (((int) &AUDIO_Play_Flag) | 0x40000000);
        volatile int *ptrSongplaying = (int *) (((int) &Songplaying) | 0x40000000);
        volatile short *ptrBuffer1 = (short *) (((int) ptrAudio_BufferPlay1) | 0x40000000);
        volatile short *ptrBuffer2 = (short *) (((int) ptrAudio_BufferPlay2) | 0x40000000);
        volatile int *ptrMutex = (int *) (((int) &Mutex) | 0x40000000);

        if(*ptrAUDIO_Play_Flag && *ptrSongplaying)
        {
            if(*ptrMutex == FALSE)
            {
                if(*ptrAUDIO_FlipFlop)
                {
                    AUDIO_Mixer((Uint8 *) ptrBuffer1, *ptrAUDIO_SoundBuffer_Size);
                }
                else
                {
                    AUDIO_Mixer((Uint8 *) ptrBuffer2, *ptrAUDIO_SoundBuffer_Size);
                }

                *ptrAUDIO_Samples += *ptrAUDIO_SoundBuffer_Size;
                *ptrAUDIO_Timer = ((((float) *ptrAUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);
                *ptrMutex = TRUE;
                asm volatile ( "sync\n" );
            }
        }    
    }
}

SceInt32 AUDIO_Thread(SceSize args, ScePVoid argp)
{
    for(;;)
    {
        volatile int *ptrAUDIO_FlipFlop = (int *) (((int) &AUDIO_FlipFlop) | 0x40000000);
        volatile short *ptrBuffer1 = (short *) (((int) ptrAudio_BufferPlay1) | 0x40000000);
        volatile short *ptrBuffer2 = (short *) (((int) ptrAudio_BufferPlay2) | 0x40000000);
        volatile int *ptrMutex = (int *) (((int) &Mutex) | 0x40000000);

        volatile int *ptrAUDIO_Samples = (int *) (((int) &AUDIO_Samples) | 0x40000000);
        if(AUDIO_Play_Flag && Songplaying)
        {
            if(sceAudioGetChannelRestLen(AUDIO_HWChannel) <= 0)
            {
                if(*ptrMutex)
                {
                    if(*ptrAUDIO_FlipFlop)
                    {
                        sceAudioOutput(AUDIO_HWChannel, PSP_AUDIO_VOLUME_MAX, (void *) ptrBuffer1);
                    }
                    else
                    {
                        sceAudioOutput(AUDIO_HWChannel, PSP_AUDIO_VOLUME_MAX, (void *) ptrBuffer2);
                    }
                    *ptrAUDIO_FlipFlop ^= TRUE;
                    *ptrMutex = FALSE;
                    asm volatile ( "sync\n" );
                }
            }
        }
        sceKernelDelayThread(10);
    }
    return(0);
}
#endif

#if defined(__WIN32__)
DWORD WINAPI AUDIO_Thread(LPVOID lpParameter)
{
    int Bytes_To_Lock;

    for(;;)
    {
        if(AUDIO_Play_Flag && AUDIO_Sound_Buffer)
        {
            AUDIO_Acknowledge = FALSE;

            AUDIO_Sound_Buffer->GetCurrentPosition(&AUDIO_Buffer_Pos, NULL);
            Bytes_To_Lock = AUDIO_Buffer_Pos - AUDIO_Old_Buffer_Pos;
            if(Bytes_To_Lock < 0) Bytes_To_Lock += AUDIO_Latency;
            AUDIO_Sound_Buffer->Lock(AUDIO_Old_Buffer_Pos, Bytes_To_Lock, &AUDIO_Audio_Ptr1, &AUDIO_Audio_Bytes1, &AUDIO_Audio_Ptr2, &AUDIO_Audio_Bytes2, 0);
            AUDIO_Old_Buffer_Pos = AUDIO_Buffer_Pos;

            AUDIO_Mixer((Uint8 *) AUDIO_Audio_Ptr1, AUDIO_Audio_Bytes1);
            AUDIO_Mixer((Uint8 *) AUDIO_Audio_Ptr2, AUDIO_Audio_Bytes2);

            AUDIO_Sound_Buffer->Unlock(AUDIO_Audio_Ptr1, AUDIO_Audio_Bytes1, AUDIO_Audio_Ptr2, AUDIO_Audio_Bytes2);

            AUDIO_Samples += (AUDIO_Audio_Bytes1 + AUDIO_Audio_Bytes2);
            AUDIO_Timer = ((((float) AUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);
        }
        else
        {
            AUDIO_Acknowledge = TRUE;
        }
        Sleep(10);
    }
    return(FALSE);
}
#endif

// ------------------------------------------------------
// Name: AUDIO_Init_Driver()
// Desc: Init the audio driver
#if defined(__WIN32__)
int AUDIO_Init_Driver(HWND hWnd, void (STDCALL *Mixer)(Uint8 *, Uint32))
{
#else
int AUDIO_Init_Driver(void (*Mixer)(Uint8 *, Uint32))
{
#endif

    AUDIO_Mixer = Mixer;

#if defined(__MACOSX__)
    AUDIO_Device = kAudioDeviceUnknown;
    Amount = sizeof(AudioDeviceID);
    if(AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
                                &Amount,
                                (void *) &AUDIO_Device) == noErr)
    {
        if(AudioDeviceAddIOProc(AUDIO_Device,
                                AUDIO_Callback,
                                NULL) == noErr)
        {
            return(AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds));
        }
    }
#endif

#if defined(__LINUX__)
    char *Mixer_Name;
    int8 Mixer_Volume[4];

    AUDIO_Device = open("/dev/dsp", O_WRONLY, 0);
    if(AUDIO_Device >= 0)
    {
        return(AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds));
    }
#endif

#if defined(__AMIGAOS4__)
    AHImp = (struct MsgPort *) IExec->AllocSysObject(ASOT_PORT, NULL);
    AHIio = (struct AHIRequest *) IExec->AllocSysObjectTags(ASOT_IOREQUEST,
                                                            ASOIOR_ReplyPort, AHImp,
                                                            ASOIOR_Size, sizeof(struct AHIRequest),
                                                            TAG_END);
    AHIio2 = (struct AHIRequest *) IExec->AllocSysObjectTags(ASOT_IOREQUEST,
                                                             ASOIOR_ReplyPort, AHImp,
                                                             ASOIOR_Size, sizeof(struct AHIRequest),
                                                             TAG_END);
    join = NULL;
    if (!AHIio || !AHIio2)
    {
        return(FALSE);
    }
    AHIio->ahir_Version = 4;
    if (IExec->OpenDevice("ahi.device", 0, (struct IORequest *) AHIio, 0))
    {
        AHIio->ahir_Std.io_Device = NULL;
        return(FALSE);
    }
    IExec->CopyMem(AHIio, AHIio2, sizeof(struct AHIRequest));
    return(AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds));
#endif

#if defined(__AROS__)
    AHImp = CreateMsgPort();
    AHIio = (struct AHIRequest *) CreateIORequest(AHImp, sizeof(struct AHIRequest));
    AHIio2 = (struct AHIRequest *) CreateIORequest(AHImp, sizeof(struct AHIRequest));
    join = NULL;
    
    //check ahiios are allocated
    if (!AHIio || !AHIio2)
    {
        return(FALSE);
    }
    
    AHIio->ahir_Version = 4;
    
    //open ahi
    if (OpenDevice("ahi.device", 0, (struct IORequest *) AHIio, 0))
    {
        AHIio->ahir_Std.io_Device = NULL;
        return(FALSE);
    }
    
    //copy for double buffering
    CopyMem(AHIio, AHIio2, sizeof(struct AHIRequest));
    
    //create audio buffer
    return(AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds));
#endif

#if defined(__PSP__)
    return(AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds));
#endif

#if defined(__WIN32__)
    if(DirectSoundCreate(NULL, &AUDIO_DSound_Context, NULL) == DS_OK)
    {
        if(AUDIO_DSound_Context->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE) == DS_OK)
        {
            return(AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds));
        }
    }
#endif
    return(FALSE);
}

#if defined(__PSP__)
// ------------------------------------------------------
// Name: AUDIO_malloc_64()
// Desc: Create an audio buffer of given milliseconds
void *AUDIO_malloc_64(int *size)
{
    int mod_64 = *size & 0x3f;
    if(mod_64 != 0) *size += 64 - mod_64;
    return((void *) memalign(64, *size));
}
#endif

// ------------------------------------------------------
// Name: AUDIO_Create_Sound_Buffer()
// Desc: Create an audio buffer of given milliseconds
int AUDIO_Create_Sound_Buffer(int milliseconds)
{
    int num_fragments;
    int frag_size;

    if(milliseconds < 10) milliseconds = 10;
    if(milliseconds > 250) milliseconds = 250;

    num_fragments = 6;
    frag_size = (int) (AUDIO_PCM_FREQ * (milliseconds / 1000.0f));

#if defined(__MACOSX__)
    AudioValueRange Frame_Range;

    Amount = sizeof(AudioStreamBasicDescription);
    if(AudioDeviceGetProperty(AUDIO_Device,
                              0,
                              FALSE,
                              kAudioDevicePropertyStreamFormat,
                              &Amount,
                              &Desc) == noErr)
    {
        Desc.mSampleRate = AUDIO_PCM_FREQ;
        Desc.mChannelsPerFrame = AUDIO_DBUF_CHANNELS;
        Desc.mBitsPerChannel = 32;
        Desc.mFormatID = kAudioFormatLinearPCM;

        Desc.mFramesPerPacket = 1;
        Desc.mBytesPerFrame = (Desc.mBitsPerChannel * Desc.mChannelsPerFrame) >> 3;
        Desc.mBytesPerPacket = Desc.mBytesPerFrame * Desc.mFramesPerPacket;

        Desc.mFormatFlags = kLinearPCMFormatFlagIsPacked | kAudioFormatFlagIsFloat;

#if defined(__BIG_ENDIAN__)
        Desc.mFormatFlags |= kLinearPCMFormatFlagIsBigEndian;
#endif

        if(AudioDeviceSetProperty(AUDIO_Device,
                                  NULL,
                                  0,
                                  FALSE,
                                  kAudioDevicePropertyStreamFormat,
                                  sizeof(AudioStreamBasicDescription),
                                  &Desc) == noErr)
        {
	        Amount = sizeof(AudioValueRange);

            AUDIO_SoundBuffer_Size = frag_size << 2;

            if(AudioDeviceGetProperty(AUDIO_Device,
                                      0,
                                      FALSE,
                                      kAudioDevicePropertyBufferFrameSizeRange,
                                      &Amount,
                                      &Frame_Range) == noErr)
            {
                if(AUDIO_SoundBuffer_Size > (int) Frame_Range.mMaximum)
                {
                    AUDIO_SoundBuffer_Size = Frame_Range.mMaximum;
                }
            }

            AUDIO_Latency = AUDIO_SoundBuffer_Size;

            if(AudioDeviceSetProperty(AUDIO_Device,
                                      0,
                                      0,
                                      0,
                                      kAudioDevicePropertyBufferFrameSize,
                                      sizeof(AUDIO_SoundBuffer_Size),
                                      &AUDIO_SoundBuffer_Size) == noErr)
            {
                return(TRUE);
            }
        }
    }
#endif

#if defined(__LINUX__)
    int Dsp_Val;
    struct sched_param p;

	frag_size = 10 + (int) (logf((float) (frag_size >> 9)) / logf(2.0f));

    Dsp_Val = (num_fragments << 16) | frag_size;
    ioctl(AUDIO_Device, SNDCTL_DSP_SETFRAGMENT, &Dsp_Val);

    Dsp_Val = AUDIO_DBUF_CHANNELS;
    ioctl(AUDIO_Device, SNDCTL_DSP_CHANNELS, &Dsp_Val);
    Dsp_Val = AFMT_S16_NE;
    ioctl(AUDIO_Device, SNDCTL_DSP_SETFMT, &Dsp_Val);
    Dsp_Val = AUDIO_PCM_FREQ;
    ioctl(AUDIO_Device, SNDCTL_DSP_SPEED, &Dsp_Val);

    if((ioctl(AUDIO_Device, SNDCTL_DSP_GETOSPACE, &AUDIO_Info_Buffer) < 0))
    {
        ioctl(AUDIO_Device, SNDCTL_DSP_GETBLKSIZE, &AUDIO_Info_Buffer.fragsize);
    }

    AUDIO_SoundBuffer_Size = AUDIO_Info_Buffer.fragsize;
    AUDIO_Latency = AUDIO_SoundBuffer_Size;

    AUDIO_SoundBuffer = (short *) malloc(AUDIO_SoundBuffer_Size << 1);

    p.sched_priority = 1;
    Thread_Running = 1;
    pthread_setschedparam(pthread_self(), SCHED_FIFO , &p);
    if(pthread_create(&hThread, NULL, AUDIO_Thread, NULL) == 0)
    {
        return(TRUE);
    }
    Thread_Running = 0;
#endif

#if defined(__AMIGAOS4__)
    struct sched_param p;
    
    AUDIO_SoundBuffer_Size = frag_size << 2;
    AUDIO_Latency = AUDIO_SoundBuffer_Size;
    
    AHIbuf = (short *) IExec->AllocVec(AUDIO_SoundBuffer_Size << 1, MEMF_SHARED);
    AHIbuf2 = (short *) IExec->AllocVec(AUDIO_SoundBuffer_Size << 1, MEMF_SHARED);
    if (!AHIbuf || !AHIbuf2)
    {
        return(FALSE);
    }
    
    memset(&p, 0, sizeof(p));
    p.sched_priority = 1;
    Thread_Running = 1;
    pthread_setschedparam(pthread_self(), SCHED_FIFO , &p);
    if(pthread_create(&hThread, NULL, AUDIO_Thread, NULL) == 0)
    {
        return(TRUE);
    }
    Thread_Running = 0;
#endif

#if defined(__AROS__)
	AUDIO_SoundBuffer_Size = frag_size << 2;
    AUDIO_Latency = AUDIO_SoundBuffer_Size;

	AHIbuf = (short *) AllocVec(AUDIO_SoundBuffer_Size << 1, MEMF_ANY);
	AHIbuf2 = (short *) AllocVec(AUDIO_SoundBuffer_Size << 1, MEMF_ANY);
	if (!AHIbuf || !AHIbuf2)
    {
		return(FALSE);
	}

    Thread_Running = 1;
    
#if defined(USE_SDL_THREADS)
    if(hThread = SDL_CreateThread(AUDIO_Thread, NULL))
#else
    //struct sched_param p;
    //memset(&p, 0, sizeof(p));
    //p.sched_priority = 1;
    //pthread_setschedparam(pthread_self(), SCHED_FIFO , &p);
    if(pthread_create(&hThread, NULL, AUDIO_Thread, NULL) == 0)
#endif
    {
    return(TRUE);
    }
    Thread_Running = 0;
#endif

#if defined(__WIN32__)
    AUDIO_Sound_Buffer_Desc.dwSize = sizeof(AUDIO_Sound_Buffer_Desc);
    AUDIO_Sound_Buffer_Desc.dwFlags = DSBCAPS_GLOBALFOCUS;

    AUDIO_Wave_Format.wFormatTag = WAVE_FORMAT_PCM;
    // Stereo
    AUDIO_Wave_Format.nChannels = AUDIO_DBUF_CHANNELS;
    AUDIO_Wave_Format.nSamplesPerSec = AUDIO_PCM_FREQ;
    AUDIO_Wave_Format.nAvgBytesPerSec = (AUDIO_PCM_FREQ * ((AUDIO_DBUF_RESOLUTION * AUDIO_DBUF_CHANNELS) >> 3));
    AUDIO_Wave_Format.nBlockAlign = ((AUDIO_DBUF_RESOLUTION * AUDIO_DBUF_CHANNELS) >> 3);
    AUDIO_Sound_Buffer_Desc.dwBufferBytes = frag_size * AUDIO_Wave_Format.nBlockAlign * num_fragments;
    AUDIO_Latency = AUDIO_Sound_Buffer_Desc.dwBufferBytes;
    // 16 bits
    AUDIO_Wave_Format.wBitsPerSample = AUDIO_DBUF_RESOLUTION;
    AUDIO_Sound_Buffer_Desc.lpwfxFormat = &AUDIO_Wave_Format;
    if(AUDIO_DSound_Context->CreateSoundBuffer(&AUDIO_Sound_Buffer_Desc, &AUDIO_Sound_Buffer, NULL) == DS_OK)
    {
        AUDIO_hReplayThread = CreateThread(NULL, 0, &AUDIO_Thread, 0, 0, &AUDIO_ThreadId);
        SetThreadPriority(AUDIO_hReplayThread, THREAD_PRIORITY_TIME_CRITICAL);
        return(TRUE);
    }
#endif

#if defined(__PSP__)
    AUDIO_SoundBuffer_Size = frag_size * ((AUDIO_DBUF_RESOLUTION * AUDIO_DBUF_CHANNELS) >> 3);
    AUDIO_SoundBuffer_Size = PSP_AUDIO_SAMPLE_ALIGN(AUDIO_SoundBuffer_Size);

    AUDIO_HWChannel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, AUDIO_SoundBuffer_Size, PSP_AUDIO_FORMAT_STEREO);
    AUDIO_SoundBuffer_Size <<= 2;

    AUDIO_Latency = AUDIO_SoundBuffer_Size;
    int buf_size = AUDIO_SoundBuffer_Size;
    ptrAudio_BufferPlay1 = (short *) (((int) AUDIO_malloc_64(&buf_size)) | 0x40000000);
    sceKernelDcacheWritebackInvalidateAll();
    memset((void *) ptrAudio_BufferPlay1, 0, buf_size);

    if((int) ptrAudio_BufferPlay1 != 0x40000000)
    {
        buf_size = AUDIO_SoundBuffer_Size;
        ptrAudio_BufferPlay2 = (short *) (((int) AUDIO_malloc_64(&buf_size)) | 0x40000000);
        sceKernelDcacheWritebackInvalidateAll();
        memset((void *) ptrAudio_BufferPlay2, 0, buf_size);

        if((int) ptrAudio_BufferPlay2 != 0x40000000)
        {
            // Install the code for the media engine handler
            me_sceKernelDcacheWritebackInvalidateAll();
            sceKernelDcacheWritebackInvalidateAll();
            int i;
            u8 *Src = (u8 *) &me_stub;
            u8 *Dest = (u8 *) 0xbfc00040;
            for(i = 0; i < (int) ((int) &me_stub_end - (int) &me_stub); i++)
            {
                Dest[i] = Src[i];
            }
            _sw(((u32) Me_Handler) | 0x40000000, 0xbfc00600);
            me_sceKernelDcacheWritebackInvalidateAll();
            sceKernelDcacheWritebackAll();
            sceSysregMeResetEnable();
            sceSysregMeBusClockEnable();
            sceSysregMeResetDisable();

            AUDIO_thid = sceKernelCreateThread("Ptk Audio Thread", AUDIO_Thread, AUDIO_THREAD_PRIORITY, AUDIO_THREAD_STACKSIZE, 0, NULL);
            if(AUDIO_thid > 0)
            {
                sceKernelStartThread(AUDIO_thid, 0, NULL);
                return(TRUE);
            }
        }
    }
#endif

    return(FALSE);
}

// ------------------------------------------------------
// Name: AUDIO_Wait_For_Thread()
// Desc: Wait for a command acknowledgment from the thread
void AUDIO_Wait_For_Thread(void)
{

#if defined(__MACOSX__)
    usleep(10);

#else

    if(AUDIO_Play_Flag)
    {
        while(AUDIO_Acknowledge)
        {

#if defined(__WIN32__)
            Sleep(10);
#endif
#if defined(__LINUX__) || defined(__AMIGAOS4__) || defined(__AROS__) 
            usleep(10);
#endif

        };
    }
    else
    {
#if defined(__LINUX__) || defined(__AMIGAOS4__)
        if(hThread)
#elif defined(__AROS__)
    #if defined(USE_SDL_THREADS)
        if(hThread)     // sdl thread
    #else
        if(hThread.p)   // pthreads
    #endif
#endif
        {
            while(!AUDIO_Acknowledge)
            {

#if defined(__WIN32__)
                Sleep(10);
#endif
#if defined(__LINUX__) || defined(__MACOSX__) || defined(__AMIGAOS4__) || defined(__AROS__)
                usleep(10);
#endif

            };
        }
    }
#endif // __MACOSX__

}

// ------------------------------------------------------
// Name: AUDIO_Play()
// Desc: Play the sound buffer endlessly
void AUDIO_Play(void)
{

#if defined(__MACOSX__)
    AudioDeviceStart(AUDIO_Device, AUDIO_Callback);
#endif

#if defined(__WIN32__)
    AUDIO_Sound_Buffer->Play(0, 0, DSBPLAY_LOOPING);
#endif

    AUDIO_ResetTimer();

    AUDIO_Play_Flag = TRUE;

#if !defined(__PSP__)
    AUDIO_Wait_For_Thread();
#else
    sceKernelDcacheWritebackInvalidateAll();
#endif

}

// ------------------------------------------------------
// Name: AUDIO_IsPlaying()
// Desc: Return the playing state of the sound buffer
int AUDIO_IsPlaying(void)
{
    return(AUDIO_Play_Flag);
}

// ------------------------------------------------------
// Name: AUDIO_ResetTimer()
// Desc: Reset the samples counter
void AUDIO_ResetTimer(void)
{
    AUDIO_Samples = 0;
    AUDIO_Timer = 0.0f;

#if defined(__PSP__)
    sceKernelDcacheWritebackInvalidateAll();
#endif

}

// ------------------------------------------------------
// Name: AUDIO_GetTime()
// Desc: Return the played time in milliseconds
float AUDIO_GetTime(void)
{
    return(AUDIO_Timer);
}

// ------------------------------------------------------
// Name: AUDIO_GetSamples()
// Desc: Return the played time in milliseconds
int AUDIO_GetSamples(void)
{
    return(AUDIO_Samples);
}

// ------------------------------------------------------
// Name: AUDIO_Stop()
// Desc: Stop the sound buffer
void AUDIO_Stop(void)
{
    AUDIO_Play_Flag = FALSE;

#if !defined(__PSP__)
    AUDIO_Wait_For_Thread();
#endif

#if defined(__MACOSX__)
    AudioDeviceStop(AUDIO_Device, AUDIO_Callback);
#endif

#if defined(__WIN32__)
    if(AUDIO_Sound_Buffer) AUDIO_Sound_Buffer->Stop();
#endif
}

// ------------------------------------------------------
// Name: AUDIO_Stop_Sound_Buffer()
// Desc: Release the audio buffer
void AUDIO_Stop_Sound_Buffer(void)
{
    AUDIO_Stop();

#if defined(__LINUX__)
    if(hThread)
    {
        Thread_Running = 0;
        while(!Thread_Running)
        {
            usleep(10);
        }
        if(AUDIO_SoundBuffer) free(AUDIO_SoundBuffer);
        AUDIO_SoundBuffer = NULL;
    }
#endif

#if defined(__AMIGAOS4__)
    if(hThread)
    {
        Thread_Running = 0;
        while(!Thread_Running)
        {
            usleep(10);
        }
    }
    IExec->FreeVec(AHIbuf);
    IExec->FreeVec(AHIbuf2);
    AHIbuf = NULL;
    AHIbuf2 = NULL;
#endif

#if defined(__AROS__)
    #if defined(USE_SDL_THREADS)
        if(hThread) //sdl thread
    #else
        if(hThread.p) //pthreads
    #endif
    {
        Thread_Running = 0;
        while(!Thread_Running)
        {
            usleep(10);
        }
    }
    FreeVec(AHIbuf);
    FreeVec(AHIbuf2);
    AHIbuf = NULL;
    AHIbuf2 = NULL;
#endif

#if defined(__PSP__)
    me_sceKernelDcacheWritebackInvalidateAll();	
    sceSysregMeResetEnable();
    sceSysregMeBusClockDisable();
    if(AUDIO_thid > 0) sceKernelDeleteThread(AUDIO_thid);
    if(AUDIO_HWChannel) sceAudioChRelease(AUDIO_HWChannel);
    if(ptrAudio_BufferPlay1) free((void *) ptrAudio_BufferPlay1);
    if(ptrAudio_BufferPlay2) free((void *) ptrAudio_BufferPlay2);
#endif

#if defined(__WIN32__)
    if(AUDIO_hReplayThread)
    {
        TerminateThread(AUDIO_hReplayThread, 0);
        CloseHandle(AUDIO_hReplayThread);
        AUDIO_hReplayThread = 0;
    }
    if(AUDIO_Sound_Buffer)
    {
        AUDIO_Sound_Buffer->Release();
        AUDIO_Sound_Buffer = NULL;
    }
#endif
}

// ------------------------------------------------------
// Name: AUDIO_Stop_Driver()
// Desc: Stop everything
void AUDIO_Stop_Driver(void)
{
    AUDIO_Stop_Sound_Buffer();

#if defined(__MACOSX__)
    AudioDeviceRemoveIOProc(AUDIO_Device, AUDIO_Callback);
    AUDIO_Device = 0;
#endif

#if defined(__LINUX__)
    if(AUDIO_Device) close(AUDIO_Device);
    AUDIO_Device = 0;
#endif

#if defined(__AMIGAOS4__)
    if (AHIio && AHIio->ahir_Std.io_Device)
    {
        IExec->CloseDevice((struct IORequest *) AHIio);
    }
    IExec->FreeSysObject(ASOT_IOREQUEST, AHIio);
    IExec->FreeSysObject(ASOT_IOREQUEST, AHIio2);
    IExec->FreeSysObject(ASOT_PORT, AHImp);
    AHIio = NULL;
    AHIio2 = NULL;
    AHImp = NULL;
#endif

#if defined(__AROS__)
    if (AHIio && AHIio->ahir_Std.io_Device)
    {
        CloseDevice((struct IORequest *) AHIio);
    }
    DeleteIORequest((struct IORequest *) AHIio);
    DeleteIORequest((struct IORequest *) AHIio2);
    DeleteMsgPort(AHImp);
    AHIio = NULL;
    AHIio2 = NULL;
    AHImp = NULL;
#endif

#if defined(__WIN32__)
   if(AUDIO_DSound_Context) AUDIO_DSound_Context->Release();
#endif
}
