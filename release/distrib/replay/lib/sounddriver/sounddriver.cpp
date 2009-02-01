// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/sounddriver.h"

#if defined(__WIN32__)
    #define DIRECTSOUND_VERSION 0x0900
    #if defined(MINGW)
    #include "include/DSound.h"
    #else
    #include <DSound.h>
    #endif
#endif

// ------------------------------------------------------
// Variables
int AUDIO_Play_Flag;

unsigned int AUDIO_To_Fill;
int AUDIO_Samples;
int volatile AUDIO_Acknowledge;
float AUDIO_Timer;

#if defined(__LINUX__)
int AUDIO_Device;
short *AUDIO_SoundBuffer;
audio_buf_info AUDIO_Info_Buffer;
pthread_t hThread;
int volatile Thread_Running;
#endif

#if defined(__PSP__)
SceUID AUDIO_thid;
short *AUDIO_Buffer1;//[AUDIO_OUTPUT_SAMPLE * AUDIO_STEREO * sizeof(short)];
short *AUDIO_Buffer2;//[AUDIO_OUTPUT_SAMPLE * AUDIO_STEREO * sizeof(short)];
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
int AUDIO_Milliseconds = 20;

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

#if defined(__PSP__)
SceInt32 AUDIO_Thread(SceSize args, ScePVoid argp)
{
    short *ptrAudio_BufferCalc = (short *) args;        // Phony shit
    short *ptrAudio_BufferPlay = (short *) argp;
    int Sound_Buffer_FlipFlop = FALSE;
    for(;;)
    {
        if(AUDIO_Play_Flag)
        {
            AUDIO_Acknowledge = FALSE;

            Sound_Buffer_FlipFlop ^= TRUE;
            if(Sound_Buffer_FlipFlop)
            {
                ptrAudio_BufferCalc = AUDIO_Buffer2;
                ptrAudio_BufferPlay = AUDIO_Buffer1;
            }
            else
            {
                ptrAudio_BufferCalc = AUDIO_Buffer1;
                ptrAudio_BufferPlay = AUDIO_Buffer2;
            }
            sceWaveAudioWriteBlocking(0, SCE_WAVE_AUDIO_VOL_MAX, SCE_WAVE_AUDIO_VOL_MAX, ptrAudio_BufferPlay);
            AUDIO_Mixer(ptrAudio_BufferCalc, AUDIO_SoundBuffer_Size << 2);

            AUDIO_Samples += AUDIO_SoundBuffer_Size << 2;
            AUDIO_Timer = ((((float) AUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);  
        }
        else
        {
            AUDIO_Acknowledge = TRUE;
        }
        sceKernelDelayThread(10);
    }
    return(SCE_KERNEL_EXIT_SUCCESS);
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
            AUDIO_Samples += AUDIO_Audio_Bytes1 + AUDIO_Audio_Bytes2;
            AUDIO_Sound_Buffer->Unlock(AUDIO_Audio_Ptr1, AUDIO_Audio_Bytes1, AUDIO_Audio_Ptr2, AUDIO_Audio_Bytes2);

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

#if defined(__LINUX__)
    char *Mixer_Name;
    int8 Mixer_Volume[4];

    AUDIO_Device = open("/dev/dsp", O_WRONLY, 0);
    if(AUDIO_Device >= 0)
    {
        return(AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds));
    }
#endif

#if defined(__PSP__)
    sceWaveInit();
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
void *AUDIO_malloc_64(int size)
{
    int mod_64 = size & 0x3f;
    if(mod_64 != 0) size += 64 - mod_64;
    return((void *) memalign(64, size));
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
    //memset(AUDIO_SoundBuffer, 0, AUDIO_SoundBuffer_Size << 1);

    p.sched_priority = 1;
    Thread_Running = 1;
    pthread_setschedparam(pthread_self(), SCHED_FIFO , &p);
    if(pthread_create(&hThread, NULL, AUDIO_Thread, NULL) == 0)
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
    sceWaveAudioSetFormat(0, SCE_WAVE_AUDIO_FMT_S16_STEREO);

    AUDIO_SoundBuffer_Size = frag_size * ((AUDIO_DBUF_RESOLUTION * AUDIO_DBUF_CHANNELS) >> 3) * num_fragments;
    AUDIO_Latency = AUDIO_SoundBuffer_Size;
    AUDIO_Buffer1 = (short *) AUDIO_malloc_64(AUDIO_SoundBuffer_Size * AUDIO_STEREO * sizeof(short));
    AUDIO_Buffer2 = (short *) AUDIO_malloc_64(AUDIO_SoundBuffer_Size * AUDIO_STEREO * sizeof(short));
    sceWaveAudioSetSample(0, AUDIO_SoundBuffer_Size);
    AUDIO_thid = sceKernelCreateThread("PTK Audio thread", AUDIO_Thread, AUDIO_THREAD_PRIORITY, AUDIO_THREAD_STACKSIZE, 0, NULL);
    if(AUDIO_thid > 0)
    {
        sceKernelStartThread(AUDIO_thid, 0, NULL);
        return(TRUE);
    }
#endif

    return(FALSE);
}

// ------------------------------------------------------
// Name: AUDIO_Wait_For_Thread()
// Desc: Wait for a command acknowledgment from the thread
void AUDIO_Wait_For_Thread(void)
{
    if(AUDIO_Play_Flag)
    {
        while(AUDIO_Acknowledge)
        {

#if defined(__WIN32__)
            Sleep(10);
#endif
#if defined(__LINUX__)
            usleep(10);
#endif
#if defined(__PSP__)
            sceKernelDelayThread(10);
#endif

        };
    }
    else
    {
        while(!AUDIO_Acknowledge)
        {

#if defined(__WIN32__)
            Sleep(10);
#endif
#if defined(__LINUX__)
            usleep(10);
#endif
#if defined(__PSP__)
            sceKernelDelayThread(10);
#endif

        };
    }
}

// ------------------------------------------------------
// Name: AUDIO_Play()
// Desc: Play the sound buffer endlessly
void AUDIO_Play(void)
{
#if defined(__PSP__)
    sceWaveStart(SCE_KERNEL_USER_HIGHEST_PRIORITY);
#endif

#if defined(__WIN32__)
    AUDIO_Sound_Buffer->Play(0, 0, DSBPLAY_LOOPING);
#endif

    AUDIO_ResetTimer();
    AUDIO_Play_Flag = TRUE;
    AUDIO_Wait_For_Thread();
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
    AUDIO_Wait_For_Thread();

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
    Thread_Running = 0;
    while(!Thread_Running)
    {
        usleep(10);
    }
    if(AUDIO_SoundBuffer) free(AUDIO_SoundBuffer);
    AUDIO_SoundBuffer = NULL;
#endif

#if defined(__PSP__)
    if(AUDIO_thid > 0) sceKernelDeleteThread(AUDIO_thid);
    sceWaveEnd();
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
      AUDIO_Sound_Buffer->Stop();
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

#if defined(__LINUX__)
    if(AUDIO_Device) close(AUDIO_Device);
    AUDIO_Device = 0;
#endif

#if defined(__PSP__)
    sceWaveExit();
#endif

#if defined(__WIN32__)
   if(AUDIO_DSound_Context) AUDIO_DSound_Context->Release();
#endif
}
