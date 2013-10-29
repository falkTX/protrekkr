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

// ------------------------------------------------------
// Includes
#include "include/sounddriver_linux.h"

#include "jack/jack.h"

// ------------------------------------------------------
// Variables (internal)
int AUDIO_Samples = 0;
int AUDIO_Play_Flag = FALSE;
float AUDIO_Timer = 0.0f;

// ------------------------------------------------------
// Variables (jack)
jack_client_t* jaudio_client = NULL;
jack_port_t* jaudio_port1 = NULL;
jack_port_t* jaudio_port2 = NULL;

// ------------------------------------------------------
// Variables (external helpers, not really used here)
int volatile AUDIO_Acknowledge = 0;
int AUDIO_Latency = 0;
int AUDIO_Milliseconds = 20;

int AUDIO_Create_Sound_Buffer(int) {}
void AUDIO_Stop_Sound_Buffer(void) {}

// ------------------------------------------------------
// Functions
void (STDCALL *AUDIO_Mixer)(Uint8 *, Uint32) = NULL;
void (STDCALL *AUDIO_MixerFloat)(float*, float*, Uint32) = NULL;

static int jaudio_bufsize_callback(jack_nframes_t newBufferSize, void*)
{
    AUDIO_Latency = newBufferSize;
    return 0;
}

static int jaudio_process_callback(jack_nframes_t nframes, void*)
{
    float* audioBuf1 = (float*)jack_port_get_buffer(jaudio_port1, nframes);
    float* audioBuf2 = (float*)jack_port_get_buffer(jaudio_port2, nframes);

    memset(audioBuf1, 0, sizeof(float)*nframes);
    memset(audioBuf2, 0, sizeof(float)*nframes);

    AUDIO_Acknowledge = FALSE;

    if (AUDIO_Play_Flag && (AUDIO_MixerFloat || AUDIO_Mixer))
    {
        if (AUDIO_MixerFloat)
        {
            AUDIO_MixerFloat(audioBuf1, audioBuf2, nframes);
        }
        else
        {
            Uint8 mixerBuf[nframes*2];
            AUDIO_Mixer(mixerBuf, nframes*2);

            for (jack_nframes_t i=0; i < nframes; ++i)
                audioBuf1[i] = audioBuf2[i] = double((signed short)((mixerBuf[2*i+1]<<8) | mixerBuf[2*i]))*2.0/65535.0;
        }

        AUDIO_Samples += nframes;
        AUDIO_Timer    = ((((float) AUDIO_Samples) * (1.0f / (float) AUDIO_Latency)) * 1000.0f);
    }
    else
    {
        //for (jack_nframes_t i=0; i < nframes; ++i)
        //    *audioBuf1++ = *audioBuf2++ = 0.0f;

        AUDIO_Acknowledge = TRUE;
    }

    return 0;
}

static void jaudio_shutdown_callback(void* arg)
{
    jaudio_client = NULL;
    jaudio_port1 = NULL;
    jaudio_port2 = NULL;
    AUDIO_Play_Flag = FALSE;
}

// ------------------------------------------------------
// Name: AUDIO_Init_Driver()
// Desc: Init the audio driver
int _init_JACK()
{
    jaudio_client = jack_client_open("protrekkr", JackNullOption, NULL);

    if (jaudio_client == NULL)
        return FALSE;

    jaudio_port1 = jack_port_register(jaudio_client, "out1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    jaudio_port2 = jack_port_register(jaudio_client, "out2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    jack_set_buffer_size_callback(jaudio_client, jaudio_bufsize_callback, NULL);
    jack_set_process_callback(jaudio_client, jaudio_process_callback, NULL);
    jack_on_shutdown(jaudio_client, jaudio_shutdown_callback, NULL);

    AUDIO_Latency = jack_get_buffer_size(jaudio_client);

    jack_activate(jaudio_client);

    return TRUE;
}

int AUDIO_Init_Driver(void (*Mixer)(Uint8 *, Uint32))
{
    AUDIO_Mixer = Mixer;

    return _init_JACK();
}

int AUDIO_Init_DriverFloat(void (*MixerFloat)(float*, float*, Uint32))
{
    AUDIO_MixerFloat = MixerFloat;

    return _init_JACK();
}

// ------------------------------------------------------
// Name: AUDIO_Play()
// Desc: Play the sound buffer endlessly
void AUDIO_Play(void)
{
    AUDIO_ResetTimer();
    AUDIO_Play_Flag = TRUE;
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
}

// ------------------------------------------------------
// Name: AUDIO_Stop_Driver()
// Desc: Stop everything
void AUDIO_Stop_Driver(void)
{
    if (jaudio_client == NULL)
        return;

    jack_port_unregister(jaudio_client, jaudio_port2);
    jaudio_port2 = NULL;

    jack_port_unregister(jaudio_client, jaudio_port1);
    jaudio_port1 = NULL;

    jack_client_close(jaudio_client);
    jaudio_client = NULL;
}
