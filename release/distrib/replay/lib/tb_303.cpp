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
#include "include/tb_303.h"

#if defined(PTK_303)

// ------------------------------------------------------
// Constructor
gear303::gear303()
{
    tbVolume = 0.5f;
    hpf = TB303_LOPASS;
    reset();
}

// ------------------------------------------------------
// 303 initialization
void gear303::reset(void)
{
    int i;
    tbPattern = 255;
    tbLine = 255;
    tbCurrentVolume = 0.0f;
    tbRealVolume = 0.0f;
    tbInnertime = 0.0f;
    tbTargetVolume = 0.0f;
    tbRealCutoff = 0.0f;
    tbRealEnvmod = 0.0f;
    tbAutoSlide = 1;
    tbAutoMod = 1;
    tbFirstRow = 1;

    tbCutoff = 0.5f;
    tbResonance = 0.5f;
    tbEnvmod = 0.5f;
    tbDecay = 0.5f;
    tbAccent = 0.0f;

    for(i = 0 ; i < 5; i++)
    { 
        tbBuf[i] = 0.0f;
    }
    tbOscSpeedFreak = 0.0f;

    tbSample = 0.0f;
    tbOscPosition = 0.0f;
    tbOscSpeed = 0.0f;
    tbWaveform = 0;
    RampVolume = 0.0f;
    tbRampVolume = 0.0f;
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

    // Run Oscillator
    tbOscPosition += tbOscSpeed;

    if(tbInnertime > 0)
    {
        tbInnertime--;
        tbOscSpeed += tbOscSpeedFreak;
    }

    if(tbOscPosition >= 16384.0f) tbOscPosition -= 32768.0f;

    // The sliding operation reduces the cutoff freq of 1/4
    // by the speed tbDecay.
    TickPos = tbDecay / (float) SamplesPerTick;

    if(tbAutoSlide)
    {
        // Save it
        tbLastCutoff = tbRealCutoff;

        tbRealCutoff -= TickPos * 0.8f;
        if(tbRealCutoff < tbCutoff - 0.35f)
        {
            tbRealCutoff = tbCutoff - 0.35f;
            tbAutoSlide = 0;
        }

    }

    if(tbAutoMod)
    {
        tbLastEnvmod = tbRealEnvmod;
//        tbRealEnvmod -= TickPos;
        if(tbRealEnvmod < 0.0f)
        {
            tbRealEnvmod = 0.0f;
            tbAutoMod = 0;
        }
    }

    tbRealResonance = tbResonance;
    tbRealCutoff2 = tbRealCutoff + tbAccent;

    tbTargetVolume = tbRealVolume + tbAccent;
    if(tbRealCutoff2 > 1.0f) tbRealCutoff2 = 1.0f;
    if(tbTargetVolume > 1.0f) tbTargetVolume = 1.0f;

    if(tbAccent > 0.0f)
    {
        tbAccent -= 0.002f;
        if(tbAccent < 0.0f) tbAccent = 0.0f;
    }

    if(tbCurrentVolume < tbTargetVolume)
    {
        tbCurrentVolume += 0.003f;
        if(tbCurrentVolume > tbTargetVolume) tbCurrentVolume = tbTargetVolume;
    }
    else
    {
        tbCurrentVolume -= 0.003f;
        if(tbCurrentVolume < tbTargetVolume) tbCurrentVolume = tbTargetVolume;
    }


    if(tbSample < -32768.0f) tbSample = -32768.0f;
    if(tbSample > 32767.0f) tbSample = 32767.0f;
    float output = tbFilter();
    if(output < -32768.0f) output = -32768.0f;
    if(output > 32767.0f) output = 32767.0f;
    if(tbRampVolume > RampVolume)
    {
        tbRampVolume -= 0.003f;
        if(tbRampVolume < RampVolume) tbRampVolume = RampVolume;
    }
    else
    {
        tbRampVolume += 0.003f;
        if(tbRampVolume > RampVolume) tbRampVolume = RampVolume;
    }
    return(output * tbRampVolume * tbCurrentVolume);
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

    // 0.1889763f seems to be about right
    frune += (float) PARAT303->tune * 0.1889763f;
    float tbOscSpeedTmp = powf(2.0f, frune / 12.0f) * 64.0f;

    int tbLine2 = (tbLine - 1);
    if(tbLine2 < 0) tbLine2 = PARAT303->patternlength[tbPattern] - 1;
    if(tbLine2 < 0) tbLine2 = 0;
    if(tbLine == 255) tbLine = PARAT303->patternlength[tbPattern] - 1;

    if(PARAT303->flag[tbPattern][tbLine].slide_flag)
    {
        // coming from a slide ?
        if(PARAT303->flag[tbPattern][tbLine2].slide_flag && !tbFirstRow)
        {
            tbRealCutoff = tbLastCutoff;
            tbRealEnvmod = tbLastEnvmod;
        }
        else
        {
            // Re-init & prepare
            tbFirstRow = 0;
            tbAutoSlide = 1;
            tbAutoMod = 1;
            tbRealCutoff = float(PARAT303->cutoff) / 127.0f;
            tbRealEnvmod = (((float) PARAT303->envmod) / 127.0f);
            
        }
    }
    else
    {
        // coming from a slide ?
        if(PARAT303->flag[tbPattern][tbLine2].slide_flag)
        {
            // First time played ?
            if(tbFirstRow)
            {
                // Re-init the sliding
                tbAutoSlide = 1;
                tbAutoMod = 1;
                tbRealCutoff = float(PARAT303->cutoff) / 127.0f;;
                tbRealEnvmod = (((float) PARAT303->envmod) / 127.0f);
            }
            else
            {
                // Continue...
                tbRealCutoff = tbLastCutoff;
                tbRealEnvmod = tbLastEnvmod;
            }
        }
        else
        {
            // Re-init
            tbAutoSlide = 1;
            tbAutoMod = 1;
            tbRealCutoff = float(PARAT303->cutoff) / 127.0f;;
            tbRealEnvmod = (((float) PARAT303->envmod) / 127.0f);
        }
    }

    char forcefault = 1;
    // Don't use slide when the pattern is starting
    if(PARAT303->flag[tbPattern][tbLine2].slide_flag && !tbFirstRow)
    {
        tbOscSpeed = tbOscSpeedTmp;
        forcefault = 0;
        frune = float(PARAT303->tone[tbPattern][tbLine2]) - 17;
        if(PARAT303->flag[tbPattern][tbLine2].transposeup_flag) frune += 12;
        if(PARAT303->flag[tbPattern][tbLine2].transposedown_flag) frune -= 12;
        frune += (float) PARAT303->tune * 0.1889763f;
        tbInnertime = SamplesPerTick * 0.5f;
        float tbDestiny = tbOscSpeed;                            // Dest
        float tbSource = powf(2.0f, frune / 12.0f) * 64.0f;
        tbOscSpeed = tbSource;
        tbOscSpeedFreak = (tbDestiny - tbSource) / tbInnertime;  // Get glide coefficient
    }

    // Start volume rmaping if there's a pause and we're not sliding
    if(!PARAT303->flag[tbPattern][tbLine].pause && forcefault)
    {
        tbRealVolume = 0;
        tbAutoSlide = 1;
        tbAutoMod = 1;
        tbRealCutoff = tbLastCutoff;
        tbRealEnvmod = tbLastEnvmod;
    }
    else
    {
        tbOscSpeed = tbOscSpeedTmp;
        tbCutoff = float(PARAT303->cutoff) / 127.0f;
        tbEnvmod = (((float) PARAT303->envmod) / 127.0f);
        tbEnvmodInv = 1.0f - tbEnvmod;
        tbResonance = ((float) PARAT303->resonance) / 127.0f;
        tbDecay = 1.0f - (((float) PARAT303->decay) / 127.0f);
        if(PARAT303->flag[tbPattern][tbLine].slide_flag) tbDecay += 0.05f;
        tbRealVolume = tbVolume;

        // Accent is only used if there's a note to play
        if(PARAT303->flag[tbPattern][tbLine].accent_flag)
        {
            tbAccent = ((float) PARAT303->accent / 127.0f) * 8.0f;
            // Decay is automatically turned on
            tbDecay = 1.0f;
        }
    }
}

// ------------------------------------------------------
// Filter routine.
// That's a custom moog like filter
// (very empirical implementation but sounds similar to the one used in rebirth)
float gear303::tbFilter(void)
{
    float f;
    float p;
    float q;
    float t[4];
    float cut = tbRealCutoff2;
    float res = tbRealResonance;
    float env = (1.0f + (tbRealEnvmod * 0.75f));
    float envinv = (1.0f - (tbRealEnvmod * 0.75f));
    float envres = (1.0f + (tbRealEnvmod * 1.25f));
    cut += 0.55f;
    // Lorsaue le cut est < 1.0f le signal decroit
    if(cut < 0.28f) cut = 0.28f;
    cut *= 0.5f;
    q = (0.85f - cut);
    p = ((cut * fabsf(cut)) * 0.45f);
    f = (p + p) - 1.0f * envinv;
    p *= env;
    res *= 5.0f;
    q = res * ((1.0f + (0.5f * q) * (1.0f - q + (5.6f * q * q))));
    if(q > 2.4f) q = 2.4f;
    if(envres > 1.9f) envres *= 1.0f - (envres - 1.9f);
    float in = (tbSample / 16384.0f) - (q * tbBuf[4]) * (envres * tbRealResonance);
    t[1] = tbBuf[1];
    t[2] = tbBuf[2];
    t[3] = tbBuf[3];
    tbBuf[1] = ((in + tbBuf[0]) * p) - (tbBuf[1] * f);
    tbBuf[2] = ((tbBuf[1] + t[1]) * p) - (tbBuf[2] * f);
    tbBuf[3] = ((tbBuf[2] + t[2]) * p) - (tbBuf[3] * f);
    tbBuf[4] = ((tbBuf[3] + t[3]) * p) - (tbBuf[4] * f);
    tbBuf[0] = in;
    // Selection of the output filter
    switch(hpf)
    {
        case TB303_HIPASS:
            return(tbSample - (tbBuf[4] * env * 32767.0f));
        case TB303_BANDPASS:
            return((3.0f * (tbBuf[3] - tbBuf[4])) * env * 32767.0f);
        case TB303_LOPASS:
        default:
            return(tbBuf[4] * env * 32767.0f);
    }
}

#endif
