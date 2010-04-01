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
#include "include/editor_track_fx.h"

// ------------------------------------------------------
// Variables
char fld_chan = FALSE;

// ------------------------------------------------------
// Functions
void Draw_Track_Fx_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_TRACK_FX_EDIT);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Track FX");

    Gui_Draw_Button_Box(8, 472, 64, 16, "Filter LFO", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(118, 472, 64, 16, "Flanger", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(8, 490, 64, 16, "Frequency", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, 508, 64, 16, "Amplitude", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, 526, 64, 16, "LFO Status", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, 544, 64, 16, "Flanger 3D", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(230, 462, 288, 110, "Flanger Settings", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(238, 479, 56, 16, "Amount", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(238, 497, 56, 16, "Period", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(238, 515, 56, 16, "Amplitude", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(238, 533, 56, 16, "Feedback", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(238, 551, 56, 16, "Delay", BUTTON_NORMAL | BUTTON_DISABLED);
}

void Actualize_Track_Fx_Ed(char gode)
{
    if(userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(gode == 0 || gode == 1)
        {
            if(FLANGER_AMOUNT[Track_Under_Caret] > 1.0f) FLANGER_AMOUNT[Track_Under_Caret] = 1.0f;
            if(FLANGER_AMOUNT[Track_Under_Caret] < -1.0f) FLANGER_AMOUNT[Track_Under_Caret] = -1.0f;
            Realslider(298, 479, 64 + (int) (FLANGER_AMOUNT[Track_Under_Caret] * 64.0f), FLANGER_ON[Track_Under_Caret]);
            outlong(448, 479, long(FLANGER_AMOUNT[Track_Under_Caret] * 100.0f), 1);
        }

        if(gode == 0 || gode == 7)
        {
            if(FLANGER_FEEDBACK[Track_Under_Caret] > 1.0f) FLANGER_FEEDBACK[Track_Under_Caret] = 1.0f;
            if(FLANGER_FEEDBACK[Track_Under_Caret] < -1.0f) FLANGER_FEEDBACK[Track_Under_Caret] = -1.0f;
            Realslider(298, 533, 64 + (int) (FLANGER_FEEDBACK[Track_Under_Caret] * 64.0f), FLANGER_ON[Track_Under_Caret]);
            outlong(448, 533, long(FLANGER_FEEDBACK[Track_Under_Caret] * 100.0f), 1);
        }

        if(gode == 0 || gode == 4)
        {
            if(FLANGER_DEPHASE[Track_Under_Caret] > 3.1415927f) FLANGER_DEPHASE[Track_Under_Caret] = 3.1415927f;
            if(FLANGER_DEPHASE[Track_Under_Caret] < 0.0f) FLANGER_DEPHASE[Track_Under_Caret] = 0.0f;
            Realslider2(74, 544, (int) (FLANGER_DEPHASE[Track_Under_Caret] * 20.371833f), FLANGER_ON[Track_Under_Caret]);
            outlong(159, 544, (int) (FLANGER_DEPHASE[Track_Under_Caret] * 57.29578f), 6);
        }

        if(gode == 0 || gode == 5)
        {
            if(FLANGER_RATE[Track_Under_Caret] < 0.000001f) FLANGER_RATE[Track_Under_Caret] = 0.000001f;
            if(FLANGER_RATE[Track_Under_Caret] > 0.0001363f) FLANGER_RATE[Track_Under_Caret] = 0.0001363f;
            Realslider(298, 497, (int) (FLANGER_RATE[Track_Under_Caret] * 939104.92f), FLANGER_ON[Track_Under_Caret]);
            outlong(448, 497, long(0.1424758f / FLANGER_RATE[Track_Under_Caret]), 2);
        }

        if(gode == 0 || gode == 6)
        {
            if(FLANGER_AMPL[Track_Under_Caret] > 0.01f) FLANGER_AMPL[Track_Under_Caret] = 0.01f;
            if(FLANGER_AMPL[Track_Under_Caret] < 0.0f) FLANGER_AMPL[Track_Under_Caret] = 0.0f;
            Realslider(298, 515, (int) (FLANGER_AMPL[Track_Under_Caret] * 12800.0f), FLANGER_ON[Track_Under_Caret]);
            outlong(448, 515, (int) (FLANGER_AMPL[Track_Under_Caret] * 10000.0f), 1);
        }

        if(gode == 0 || gode == 2)
        {
            if(LFO_RATE[Track_Under_Caret] < 0.0001f) LFO_RATE[Track_Under_Caret] = 0.0001f;
            if(LFO_RATE[Track_Under_Caret] > 0.0078125f) LFO_RATE[Track_Under_Caret] = 0.0078125f;
            Realslider(74, 490, (int) (LFO_RATE[Track_Under_Caret] * 16384.0f), LFO_ON[Track_Under_Caret]);
            float tmprate = (8.1632653f / LFO_RATE[Track_Under_Caret]);
            outlong(76, 526, (long) tmprate, 2);
            tmprate = 1000.0f / tmprate;
            outfloat(138, 526, tmprate, 3);
        }

        if(gode == 0 || gode == 3)
        {
            if(LFO_AMPL[Track_Under_Caret] < 0) LFO_AMPL[Track_Under_Caret] = 0;
            if(LFO_AMPL[Track_Under_Caret] > 128) LFO_AMPL[Track_Under_Caret] = 128;
            Realslider(74, 508, (int) (LFO_AMPL[Track_Under_Caret]), LFO_ON[Track_Under_Caret]);
        }

        if(gode == 0 || gode == 9)
        {
            if(LFO_ON[Track_Under_Caret] == 1)
            {
                Gui_Draw_Button_Box(74, 472, 20, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(96, 472, 20, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(74, 472, 20, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(96, 472, 20, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 10)
        {
            if(FLANGER_ON[Track_Under_Caret])
            {
                Gui_Draw_Button_Box(184, 472, 20, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(206, 472, 20, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(184, 472, 20, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(206, 472, 20, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 8)
        {
            if(FLANGER_DELAY[Track_Under_Caret] > 4096) FLANGER_DELAY[Track_Under_Caret] = 4096;
            if(FLANGER_DELAY[Track_Under_Caret] < 0) FLANGER_DELAY[Track_Under_Caret] = 0;
            if(fld_chan == TRUE)
            {
                FLANGER_OFFSET[Track_Under_Caret] = 8192;
                foff2[Track_Under_Caret] = float(FLANGER_OFFSET[Track_Under_Caret] - FLANGER_DELAY[Track_Under_Caret]);
                foff1[Track_Under_Caret] = float(FLANGER_OFFSET[Track_Under_Caret] - FLANGER_DELAY[Track_Under_Caret]);  
                fld_chan = FALSE;
            }
            Realslider(298, 551, FLANGER_DELAY[Track_Under_Caret] / 32, FLANGER_ON[Track_Under_Caret]);
            outlong(448, 551, long(FLANGER_DELAY[Track_Under_Caret] / 44.1f), 2);
        }
    }//User gui screen match
}

void Mouse_Sliders_Track_Fx_Ed(void)
{
    if(userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(zcheckMouse(74, 490, 148, 16) && LFO_ON[Track_Under_Caret])
        {
            LFO_RATE[Track_Under_Caret] = (Mouse.x - 84) / 16384.0f;
            teac = 2;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(74, 508, 148, 16) && LFO_ON[Track_Under_Caret])
        {
            LFO_AMPL[Track_Under_Caret] = float(Mouse.x - 84);
            teac = 3;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(74, 544, 148, 16) && FLANGER_ON[Track_Under_Caret])
        {
            FLANGER_DEPHASE[Track_Under_Caret] = (Mouse.x - 84) * 0.0490873f;
            teac = 4;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 4;
        }

        if(zcheckMouse(298, 479, 148, 16) && FLANGER_ON[Track_Under_Caret])
        {
            teac = 1;
            FLANGER_AMOUNT[Track_Under_Caret] = ((Mouse.x - 308.0f) / 64.0f) - 1.0f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(298, 497, 148, 16) && FLANGER_ON[Track_Under_Caret])
        {
            FLANGER_RATE[Track_Under_Caret] = (Mouse.x - 308.0f) / 939104.92f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 5;
        }
        if(zcheckMouse(298, 515, 148, 16) && FLANGER_ON[Track_Under_Caret])
        {
            FLANGER_AMPL[Track_Under_Caret] = (Mouse.x - 308.0f) / 12800.0f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 6;
        }
        if(zcheckMouse(298, 533, 148, 16) && FLANGER_ON[Track_Under_Caret])
        {
            FLANGER_FEEDBACK[Track_Under_Caret] = ((Mouse.x - 308) / 64.0f) - 1.0f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 7;
        }
        if(zcheckMouse(298, 551, 148, 16) && FLANGER_ON[Track_Under_Caret])
        {
            FLANGER_DELAY[Track_Under_Caret] = (Mouse.x - 308) * 32;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            fld_chan = TRUE;
            teac = 8;
        }
    }
}

void Mouse_Left_Track_Fx_Ed(void)
{
    if(userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(zcheckMouse(74, 472, 20, 16) && LFO_ON[Track_Under_Caret] == FALSE)
        {
            LFO_ON[Track_Under_Caret] = TRUE;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 0;
        }
        if(zcheckMouse(96, 472, 20, 16) && LFO_ON[Track_Under_Caret] == TRUE)
        {
            LFO_ON[Track_Under_Caret] = FALSE;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 0;
        }
        if(zcheckMouse(184, 472, 20, 16) && FLANGER_ON[Track_Under_Caret] == FALSE)
        {
            FLANGER_ON[Track_Under_Caret] = TRUE;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 0;
        }
        if(zcheckMouse(206, 472, 20, 16) && FLANGER_ON[Track_Under_Caret])
        {
            FLANGER_ON[Track_Under_Caret] = FALSE;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 0;
        }
    }
}
