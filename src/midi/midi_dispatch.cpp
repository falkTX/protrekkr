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

#if !defined(__NO_MIDI__)

// ------------------------------------------------------
// Includes
#include "../include/variables.h"
#include "../include/ptk.h"

#include "include/midi_dispatch.h"

void Midi_Set_Master_Volume(int Data);

// ------------------------------------------------------
// Master volume (EDIT)
void Midi_Set_Master_Volume(int Data)
{
    mas_vol = ((float) Data / 127.0f);
    Display_Master_Volume();
}

// ------------------------------------------------------
// 303: set tune (LIVE)
void Midi_Set_303_Tune(int Unit, int Data)
{
    tb303[Unit].tune = Data;
    liveparam = LIVE_PARAM_303_1_TUNE + Unit;
    livevalue = Data << 1;
    if(Unit == sl3) Refresh_303_Unit(Unit, 3);
}

void Midi_Set_303_1_Tune(int Data) { Midi_Set_303_Tune(0, Data); }
void Midi_Set_303_2_Tune(int Data) { Midi_Set_303_Tune(1, Data); }
void Midi_Set_303_Cur_Tune(int Data) { Midi_Set_303_Tune(sl3, Data); }

// ------------------------------------------------------
// 303: set cutoff (LIVE)
void Midi_Set_303_Cutoff(int Unit, int Data)
{
    tb303[Unit].cutoff = Data;
    liveparam = LIVE_PARAM_303_1_CUTOFF + Unit;
    livevalue = Data << 1;
    if(Unit == sl3) Refresh_303_Unit(Unit, 4);
}

void Midi_Set_303_1_Cutoff(int Data) { Midi_Set_303_Cutoff(0, Data); }
void Midi_Set_303_2_Cutoff(int Data) { Midi_Set_303_Cutoff(1, Data); }
void Midi_Set_303_Cur_Cutoff(int Data) { Midi_Set_303_Cutoff(sl3, Data); }

// ------------------------------------------------------
// 303: set resonance (LIVE)
void Midi_Set_303_Resonance(int Unit, int Data)
{
    tb303[Unit].resonance = Data;
    liveparam = LIVE_PARAM_303_1_RESONANCE + Unit;
    livevalue = Data << 1;
    if(Unit == sl3) Refresh_303_Unit(Unit, 5);
}

void Midi_Set_303_1_Resonance(int Data) { Midi_Set_303_Resonance(0, Data); }
void Midi_Set_303_2_Resonance(int Data) { Midi_Set_303_Resonance(1, Data); }
void Midi_Set_303_Cur_Resonance(int Data) { Midi_Set_303_Resonance(sl3, Data); }

// ------------------------------------------------------
// 303: set env mod (LIVE)
void Midi_Set_303_Envmod(int Unit, int Data)
{
    tb303[Unit].envmod = Data;
    liveparam = LIVE_PARAM_303_1_ENVMOD + Unit;
    livevalue = Data << 1;
    if(Unit == sl3) Refresh_303_Unit(Unit, 6);
}

void Midi_Set_303_1_Envmod(int Data) { Midi_Set_303_Envmod(0, Data); }
void Midi_Set_303_2_Envmod(int Data) { Midi_Set_303_Envmod(1, Data); }
void Midi_Set_303_Cur_Envmod(int Data) { Midi_Set_303_Envmod(sl3, Data); }

// ------------------------------------------------------
// 303: set decay (LIVE)
void Midi_Set_303_Decay(int Unit, int Data)
{
    tb303[Unit].decay = Data;
    liveparam = LIVE_PARAM_303_1_DECAY + Unit;
    livevalue = Data << 1;
    if(Unit == sl3) Refresh_303_Unit(Unit, 7);
}

void Midi_Set_303_1_Decay(int Data) { Midi_Set_303_Decay(0, Data); }
void Midi_Set_303_2_Decay(int Data) { Midi_Set_303_Decay(1, Data); }
void Midi_Set_303_Cur_Decay(int Data) { Midi_Set_303_Decay(sl3, Data); }

// ------------------------------------------------------
// 303: set accent (LIVE)
void Midi_Set_303_Accent(int Unit, int Data)
{
    tb303[Unit].accent = Data;
    liveparam = LIVE_PARAM_303_1_ACCENT + Unit;
    livevalue = Data << 1;
    if(Unit == sl3) Refresh_303_Unit(Unit, 8);
}

void Midi_Set_303_1_Accent(int Data) { Midi_Set_303_Accent(0, Data); }
void Midi_Set_303_2_Accent(int Data) { Midi_Set_303_Accent(1, Data); }
void Midi_Set_303_Cur_Accent(int Data) { Midi_Set_303_Accent(sl3, Data); }

// ------------------------------------------------------
// 303: set volume (LIVE)
void Midi_Set_303_Volume(int Unit, int Data)
{
    tb303engine[Unit].tbVolume = ((float) Data) / 127.0f;
    liveparam = LIVE_PARAM_303_1_VOLUME + Unit;
    livevalue = (int) tb303engine[Unit].tbVolume * 255.0f;
    if(Unit == sl3) Refresh_303_Unit(Unit, 15);
}

void Midi_Set_303_1_Volume(int Data) { Midi_Set_303_Volume(0, Data); }
void Midi_Set_303_2_Volume(int Data) { Midi_Set_303_Volume(1, Data); }
void Midi_Set_303_Cur_Volume(int Data) { Midi_Set_303_Volume(sl3, Data); }

// ------------------------------------------------------
// Track panning (LIVE)
void Midi_Set_Track_Panning(int Data)
{
    TPan[Track_Under_Caret] = ((float) Data) / 127.0f;
    liveparam = LIVE_PARAM_TRACK_PANNING;
    livevalue = Data,
    Actualize_Track_Ed(9);
}

// ------------------------------------------------------
// Track cutoff (LIVE)
void Midi_Set_Track_Cutoff(int Data)
{
    TCut[Track_Under_Caret] = ((float) Data);
    liveparam = LIVE_PARAM_TRACK_CUTOFF;
    livevalue = Data,
    Actualize_Track_Ed(1);
}

// ------------------------------------------------------
// Track resonance (LIVE)
void Midi_Set_Track_Resonance(int Data)
{
    FRez[Track_Under_Caret] = ((float) Data);
    liveparam = LIVE_PARAM_TRACK_RESONANCE;
    livevalue = Data,
    Actualize_Track_Ed(2);
}

// ------------------------------------------------------
// Track reverb amount (LIVE)
void Midi_Set_Track_Reverb(int Data)
{
    DSend[Track_Under_Caret] = ((float) Data) / 127.0f;
    liveparam = LIVE_PARAM_TRACK_REVERB_SEND;
    livevalue = Data,
    Actualize_Track_Ed(5);
}

// ------------------------------------------------------
// Track disto threshold (LIVE)
void Midi_Set_Track_Disto_Threshold(int Data)
{
    DThreshold[Track_Under_Caret] = (((float) Data) / 127.0f) * 32767.0f;
    liveparam = LIVE_PARAM_TRACK_THRESHOLD;
    livevalue = Data,
    Actualize_Track_Ed(7);
}

// ------------------------------------------------------
// Track disto clamp (LIVE)
void Midi_Set_Track_Disto_Clamp(int Data)
{
    DClamp[Track_Under_Caret] = (((float) Data) / 127.0f) * 32767.0f;
    liveparam = LIVE_PARAM_TRACK_CLAMP;
    livevalue = Data,
    Actualize_Track_Ed(8);
}

// ------------------------------------------------------
// Dispatch table
MIDI_DISPATCH Dispatch_Table[] =
{
    &Midi_Set_Master_Volume, 07,
    &Midi_Set_303_1_Tune, 71,
    &Midi_Set_303_1_Cutoff, 74,
    &Midi_Set_303_1_Resonance, 84,
    &Midi_Set_303_1_Envmod, 91,
    &Midi_Set_303_1_Decay, 93,
    &Midi_Set_303_1_Accent, 05,
    &Midi_Set_303_1_Volume, 10,
    //&Midi_Set_Track_Panning, 01,
    //&Midi_Set_Track_Cutoff, 01,
    //&Midi_Set_Track_Resonance, 01,
    //&Midi_Set_Track_Reverb, 01,
    //&Midi_Set_Track_Disto_Threshold, 01,
    &Midi_Set_Track_Disto_Clamp, 01,
};

// ------------------------------------------------------
// Run various command associated to midi messages
void Dispatch_Midi_Msg(int CC, int Data)
{
    int i;

    // Possible relations: 1 n
    //                     n 1

    for(i = 0; i < sizeof(Dispatch_Table) / sizeof(MIDI_DISPATCH); i++)
    {
        if(CC == Dispatch_Table[i].CC)
        {
            if(Dispatch_Table[i].routine)
            {
                Dispatch_Table[i].routine(Data);
            }
        }
    }
}

#endif
