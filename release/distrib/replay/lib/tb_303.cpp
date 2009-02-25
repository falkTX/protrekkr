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

// ------------------------------------------------------
// Includes
#include "include/tb_303.h"

#if defined(PTK_303)

// ------------------------------------------------------
// Constructor
gear303::gear303()
{
    tbVolume = 0.5f;
    reset();
}

// ------------------------------------------------------
// 303 initialization
void gear303::reset(void)
{
    tbPattern = 255;
    tbLine = 255;
    tbCurrentVolume = 0.5f;
    tbRealVolume = 0.5f;
    tbInnertime = 0.0f;
    tbTargetVolume = 0.5f;
    tbRealCutoff = 0.5f;

    tbCutoff = 0.5f;
    tbResonance = 0.5f;
    tbEnvmod = 0.5f;
    tbDecay = 0.5f;
    tbAccent = 0.5f;

    tbBuf0 = 0.0f;
    tbBuf1 = 0.0f;
    tbOscSpeedFreak = 0.0f;

    tbSample = 0.0f;
    tbOscPosition = 0.0f;
    tbOscSpeed = 0.0f;
    hpf = 0;
    tbWaveform = 0;
    RampVolume = 0.0f;
    Cur_RampVolume = 0.0f;
}

// ------------------------------------------------------
// Render 1 32bit-float sample
float gear303::tbGetSample(void)
{
    // Get Oscillator values
    switch(tbWaveform)
    {
        // SawTooth
        case TB303_WAVEFORM_SAW:
            tbSample = tbOscPosition;
            break;

        // Square
        case TB303_WAVEFORM_SQUARE:
            if(tbOscPosition < 0) tbSample = -16384.0f;
            else tbSample = 16384.0f;
            break;
    }

    if(tbCurrentVolume < tbTargetVolume) tbCurrentVolume += 0.0078125f;
    else tbCurrentVolume -= 0.0078125f;

    // Run Oscillator
    tbOscPosition += tbOscSpeed;

    if(tbInnertime > 0)
    {
        tbInnertime--;
        tbOscSpeed += tbOscSpeedFreak;
    }

    if(tbOscPosition >= 16384.0f) tbOscPosition -= 32768.0f;

    tbRealCutoff = tbCutoff + tbEnvmod;
    tbEnvmod -= tbDecay * tbRealCutoff * 0.015625f;

    if(tbRealCutoff < tbCutoff) tbRealCutoff = tbCutoff;
    tbSample *= tbCurrentVolume;
    if(tbSample < -32768.0f) tbSample = -32768.0f;
    if(tbSample > 32767.0f) tbSample = 32767.0f;
    float output = tbFilter();
    if(output < -32768.0f) output = -32768.0f;
    if(output > 32767.0f) output = 32767.0f;
    
    if(Cur_RampVolume >= RampVolume)
    {
        Cur_RampVolume -= 0.003f;
    }
    else
    {
        Cur_RampVolume += 0.003f;
    }
    if(Cur_RampVolume > 1.0f) Cur_RampVolume = 1.0f;
    if(Cur_RampVolume < 0.0f)
    {
        Cur_RampVolume = 0.0f;
    }
    
    return(output * Cur_RampVolume);
}

// ------------------------------------------------------
// Do Note On
void gear303::tbNoteOn(int tbNote, para303 *PARAT303)
{
    if(PARAT303->flag[tbPattern][tbLine].transposeup_flag) tbNote += 12;
    if(PARAT303->flag[tbPattern][tbLine].transposedown_flag) tbNote -= 12;

    tbWaveform = PARAT303->waveform; 
    tbOscSpeedFreak = 0;
    float frune = float(tbNote) - 17;

    frune += (float) PARAT303->tune * 0.1889763f;
    tbOscSpeed = powf(2.0f, frune / 12.0f) * 64.0f;

    if(PARAT303->flag[tbPattern][tbLine].pause)
    {
        tbCutoff = float(PARAT303->cutoff + 1) * 0.0026041f;
        tbEnvmod = (tbCutoff * 2) + (float) PARAT303->envmod * 0.0009531f;
        tbResonance = (float) PARAT303->resonance * 0.0078125f;
        tbDecay = (128.0f - (float) PARAT303->decay) * 0.000122f;
        tbRealVolume = tbVolume;
    }

    // Slide check...

    // Hay glide? no hay decay...
    if(PARAT303->flag[tbPattern][tbLine].slide_flag) tbDecay = 0.0f;

    // Aqui se mira el glide de atras...
    int tbLine2 = (tbLine - 1);
    if(tbLine2 < 0)
    {
        tbLine2 = PARAT303->patternlength[tbPattern] - 2;
    }
    if(tbLine2 < 0)
    {
        tbLine2 = 0;
    }
    if(tbLine < 0) tbLine = PARAT303->patternlength[tbPattern] - 1;
    char forcefault = 1;
    if(PARAT303->flag[tbPattern][tbLine2].slide_flag)
    {
        forcefault = 0;
        frune = float(PARAT303->tone[tbPattern][tbLine2]) - 17;
        if(PARAT303->flag[tbPattern][tbLine2].transposeup_flag) frune += 12;
        if(PARAT303->flag[tbPattern][tbLine2].transposedown_flag) frune -= 12;
        frune += (float) PARAT303->tune * 0.1889763f;
        tbInnertime = SamplesPerTick * 0.5f;
        float tbDestiny = tbOscSpeed;                            // Velocidad Destino
        float tbSource = powf(2.0f, frune / 12.0f) * 64.0f;       // Velocidad fuente
        tbOscSpeed = tbSource;                                   // Intercambioce....
        tbOscSpeedFreak = (tbDestiny - tbSource) / tbInnertime;  // Calculo del coeficiente del glide
    }

    if(PARAT303->flag[tbPattern][tbLine].accent_flag)
    {
        float accenta = (float) PARAT303->accent * 0.0001765f;
        tbResonance += accenta;
        tbCutoff += accenta;
        tbRealVolume *= ((accenta * 64.0f) + 1.0f);
    }

    if(!PARAT303->flag[tbPattern][tbLine].pause && forcefault)
    {
        tbRealVolume = 0;
    }
    tbTargetVolume = tbRealVolume;
    if(tbResonance > 0.95f) tbResonance = 0.95f;
    if(tbResonance < 0.01f) tbResonance = 0.01f;
}

// ------------------------------------------------------
// Filter routine.
// This is a 2pole filter [-12db/Octave] Lowpass filter.
// The original Roland Tb303 has a 3pole lowpass filter (-18dB/Octave)
float gear303::tbFilter(void)
{
    if(tbRealCutoff > 0.9f) tbRealCutoff = 0.9f;
    if(tbRealCutoff < 0.005f) tbRealCutoff = 0.005f;

    float fa = 1.0f - tbRealCutoff; 
    float fb = tbResonance * (1.0f + (1.0f / fa));
    tbBuf0 = fa * tbBuf0 + tbRealCutoff * (tbSample + fb * (tbBuf0 - tbBuf1)); 
    tbBuf1 = fa * tbBuf1 + tbRealCutoff * tbBuf0;
    return(hpf ? tbSample - tbBuf1 : tbBuf1);
}

#endif
