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
#include "include/editor_fx_setup.h"

// ------------------------------------------------------
// Variables
int Ticks_Synchro_Left = 1;
int Ticks_Synchro_Right = 1;
extern char Use_Cubic;

// ------------------------------------------------------
// Functions
void Draw_Fx_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_FX_SETUP_EDIT);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("FX Setup (Global)");
    Gui_Draw_Button_Box(8, 464, 224, 96, "Reverb Setup", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(18, 480, 56, 16, "Switch", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(18, 498, 56, 16, "Feedback", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(18, 516, 56, 16, "Room Size", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(18, 534, 56, 16, "Filter", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(240, 464, 288, 96, "Stereo Delay Settings", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, 480, 56, 16, "L.Delay", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, 498, 56, 16, "R.Delay", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, 516, 56, 16, "L.Decay", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, 534, 56, 16, "R.Decay", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(530, 464, 104, 56, "Ticks Delay Synchro", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(596, 480, 32, 16, "Set", BUTTON_NORMAL);
    Gui_Draw_Button_Box(596, 498, 32, 16, "Set", BUTTON_NORMAL);

    Gui_Draw_Button_Box(640, 464, 64, 16, "Interpolation", BUTTON_NORMAL | BUTTON_DISABLED);
}

void Actualize_Fx_Ed(char gode)
{
    if(userscreen == USER_SCREEN_FX_SETUP_EDIT)
    {
        if(gode == 0 || gode == 2)
        {
            Realslider(77, 498, (int) (Feedback * 127.0f), compressor);
        }

        if(gode == 0 || gode == 3)
        {
            if(lchorus_delay > 22100) lchorus_delay = 22100;
            if(lchorus_delay < 1) lchorus_delay = 1;
            if(gode == 3)
            {
                lchorus_counter = MIX_RATE;
                lchorus_counter2 = MIX_RATE - lchorus_delay;
            }
            Realslider(307, 480, lchorus_delay / 175, TRUE);
            outlong(458, 480, (lchorus_delay * 1000) / MIX_RATE, 2);
        }

        if(gode == 0 || gode == 4)
        {
            if(rchorus_delay > 22100) rchorus_delay = 22100;
            if(rchorus_delay < 1) rchorus_delay = 1;
            if(gode == 4)
            {
                rchorus_counter = MIX_RATE;
                rchorus_counter2 = MIX_RATE - rchorus_delay;
            }
            Realslider(307, 498, rchorus_delay / 175, TRUE);
            outlong(458, 498, (rchorus_delay * 1000) / MIX_RATE, 2);
        }

        if(gode == 0 || gode == 5)
        {
            if(lchorus_feedback > 0.95f) lchorus_feedback = 0.95f;
            if(lchorus_feedback < 0) lchorus_feedback = 0;
            Realslider(307, 516, (int) (lchorus_feedback * 127.0f), TRUE);
            outlong(458, 516, (int) (lchorus_feedback * 100.0f), 1);
        }

        if(gode == 0 || gode == 6)
        {
            if(rchorus_feedback > 0.95f) rchorus_feedback = 0.95f;
            if(rchorus_feedback < 0) rchorus_feedback = 0;
            Realslider(307, 534, (int) (rchorus_feedback * 127), TRUE);
            outlong(458, 534, (int) (rchorus_feedback * 100), 1);
        }

        if(gode == 0 || gode == 7)
        {
            if(c_threshold < 10) c_threshold = 10;
            if(c_threshold > 127) c_threshold = 127;
            Realslider(77, 516, c_threshold, compressor);
            
            // Re-generate
            if(gode == 7) InitRevervbFilter();
        }

        if(gode == 0 || gode == 8)
        {
            if(compressor == 0)
            {
                Gui_Draw_Button_Box(79, 480, 32, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(113, 480, 32, 16, "Off", BUTTON_PUSHED);
            }
            else
            {
                Gui_Draw_Button_Box(79, 480, 32, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(113, 480, 32, 16, "Off", BUTTON_NORMAL);
            }
        }

        if(gode == 0 || gode == 9)
        {
            if(REVERBFILTER < 0.05f) REVERBFILTER = 0.05f;
            if(REVERBFILTER > 0.99f) REVERBFILTER = 0.99f;
            Realslider(77, 534, (int) (REVERBFILTER * 128.0f), compressor);
        }

        if(gode == 0 || gode == 10)
        {
            if(Ticks_Synchro_Left < 1) Ticks_Synchro_Left = 1;
            if(Ticks_Synchro_Left > TicksPerBeat + 1) Ticks_Synchro_Left = TicksPerBeat + 1;
            Gui_Draw_Arrows_Number_Box2(534, 480, Ticks_Synchro_Left, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }
        if(gode == 0 || gode == 11)
        {
            if(Ticks_Synchro_Right < 1) Ticks_Synchro_Right = 1;
            if(Ticks_Synchro_Right > TicksPerBeat + 1) Ticks_Synchro_Right = TicksPerBeat + 1;
            Gui_Draw_Arrows_Number_Box2(534, 498, Ticks_Synchro_Right, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }

        if(gode == 0 || gode == 12)
        {
            Gui_Draw_Button_Box(706, 464, 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            if(Use_Cubic)
            {
                Gui_Draw_Button_Box(706 + 16 + 2, 464, 42, 16, "Cubic", BUTTON_NORMAL | BUTTON_DISABLED);
            }
            else
            {
                Gui_Draw_Button_Box(706 + 16 + 2, 464, 42, 16, "None", BUTTON_NORMAL | BUTTON_DISABLED);
            }
            Gui_Draw_Button_Box(706 + (18 + 42) + 2, 464, 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }
    }
}

void Mouse_Sliders_Fx_Ed(void)
{
    if(userscreen == USER_SCREEN_FX_SETUP_EDIT)
    {
        if(zcheckMouse(77, 498, 148, 16) && compressor)
        {
            Feedback = float(float(Mouse.x - 87) / 127.0f);
            if(Feedback < 0) Feedback = 0;
            if(Feedback > 0.99f) Feedback = 0.99f;

            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 2;
        }
        
        if(zcheckMouse(77, 516, 148, 16) && compressor)
        {
            c_threshold = Mouse.x - 87;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 7;
        }
        if(zcheckMouse(77, 534, 148, 16) && compressor)
        {
            REVERBFILTER = (float)(Mouse.x - 87) / 128.0f;
            if(REVERBFILTER < 0.0f) REVERBFILTER = 0.0f;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 9;
        }  
        if(zcheckMouse(307, 480, 148, 16))
        {
            lchorus_delay = (Mouse.x - 317) * 174;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 3;
        }
        if(zcheckMouse(307, 498, 148, 16))
        {
            rchorus_delay = (Mouse.x - 317) * 174;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 4;
        }
        if(zcheckMouse(307, 516, 148, 16))
        {
            lchorus_feedback = float(Mouse.x - 317) / 127;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 5;
        }
        if(zcheckMouse(307, 534, 148, 16))
        {
            rchorus_feedback = float(Mouse.x - 317) / 127;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 6;
        }
    } // userscreen
}

void Mouse_Right_Fx_Ed(void)
{
    if(userscreen == USER_SCREEN_FX_SETUP_EDIT)
    {
        // Ticks synchro left
        if(zcheckMouse(534, 480, 16, 16) == 1)
        {
            Ticks_Synchro_Left -= 10;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 10;
        }
        if(zcheckMouse(578, 480, 16, 16) == 1)
        {
            Ticks_Synchro_Left += 10;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 10;
        }

        // Ticks synchro right
        if(zcheckMouse(534, 498, 16, 16) == 1)
        {
            Ticks_Synchro_Right -= 10;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 11;
        }
        if(zcheckMouse(578, 498, 16, 16) == 1)
        {
            Ticks_Synchro_Right += 10;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 11;
        }
    }
}

void Mouse_Left_Fx_Ed(void)
{
    if(userscreen == USER_SCREEN_FX_SETUP_EDIT)
    {
        // Delay type
        if(zcheckMouse(79, 516, 16, 16) && compressor)
        {
            DelayType--;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 1;
        }
        // Delay type
        if(zcheckMouse(79 + 44, 516, 16, 16) && compressor)
        {
            DelayType++;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 1;
        }

        // Ticks synchro left
        if(zcheckMouse(534, 480, 16, 16))
        {
            Ticks_Synchro_Left--;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 10;
        }
        if(zcheckMouse(578, 480, 16, 16))
        {
            Ticks_Synchro_Left++;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 10;
        }

        // Ticks synchro right
        if(zcheckMouse(534, 498, 16, 16))
        {
            Ticks_Synchro_Right--;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 11;
        }
        if(zcheckMouse(578, 498, 16, 16))
        {
            Ticks_Synchro_Right++;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 11;
        }

        if(zcheckMouse(596, 480, 32, 16))
        {
            lchorus_delay = SamplesPerTick * Ticks_Synchro_Left;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 3;
        }
        if(zcheckMouse(596, 498, 32, 16))
        {
            rchorus_delay = SamplesPerTick * Ticks_Synchro_Right;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 4;
        }

        if(compressor == FALSE && zcheckMouse(79, 480, 32, 16))
        {
            compressor = TRUE;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 0;
        }
        if(compressor == TRUE && zcheckMouse(113, 480, 32, 16))
        {
            compressor = FALSE;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 0;
        }  

        if(zcheckMouse(706, 464, 16, 16))
        {
            Use_Cubic = FALSE;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 12;
        }
        if(zcheckMouse(706 + (18 + 42) + 2, 464, 16, 16))
        {
            Use_Cubic = TRUE;
            gui_action = GUI_CMD_UPDATE_FX_ED;
            teac = 12;
        }

    }
}
