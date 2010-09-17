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
#include "include/editor_setup.h"
#include "include/editor_sequencer.h"
#include "include/editor_pattern.h"
#include "../midi/include/midi.h"

// ------------------------------------------------------
// Variables

// ------------------------------------------------------
// Functions

void Draw_Midi_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_SETUP_MIDI);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Midi Setup");

}

void Actualize_Midi_Ed(char gode)
{
    if(userscreen == USER_SCREEN_SETUP_MIDI)
    {

    }
}

void Mouse_Right_Midi_Ed(void)
{
    if(userscreen == USER_SCREEN_SETUP_MIDI)
    {

    }
}

void Mouse_Left_Midi_Ed(void)
{
    if(userscreen == USER_SCREEN_SETUP_MIDI)
    {

    }
}

void Mouse_Sliders_Midi_Ed(void)
{
    if(userscreen == USER_SCREEN_SETUP_MIDI)
    {

    }
}
