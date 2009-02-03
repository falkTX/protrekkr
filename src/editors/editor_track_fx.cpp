// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/editor_track_fx.h"

// ------------------------------------------------------
// Variables
bool fld_chan = false;

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

    Gui_Draw_Button_Box(230, 462, 288, 110, "Flanger Settings", BUTTON_NORMAL | BUTTON_DISABLED);
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
            if(FLANGER_AMOUNT[ped_track] > 1.0f) FLANGER_AMOUNT[ped_track] = 1.0f;
            if(FLANGER_AMOUNT[ped_track] < -1.0f) FLANGER_AMOUNT[ped_track] = -1.0f;
            Realslider(298, 479, 64 + f2i(FLANGER_AMOUNT[ped_track] * 64.0f), TRUE);
            outlong(448, 479, long(FLANGER_AMOUNT[ped_track] * 100.0f), 1);
        }

        if(gode == 0 || gode == 7)
        {
            if(FLANGER_FEEDBACK[ped_track] > 1.0f) FLANGER_FEEDBACK[ped_track] = 1.0f;
            if(FLANGER_FEEDBACK[ped_track] < -1.0f) FLANGER_FEEDBACK[ped_track] = -1.0f;
            Realslider(298, 533, 64 + f2i(FLANGER_FEEDBACK[ped_track] * 64.0f), TRUE);
            outlong(448, 533, long(FLANGER_FEEDBACK[ped_track] * 100.0f), 1);
        }

        if(gode == 0 || gode == 4)
        {
            if(FLANGER_DEPHASE[ped_track] > 3.1415927f) FLANGER_DEPHASE[ped_track] = 3.1415927f;
            if(FLANGER_DEPHASE[ped_track] < 0.0f) FLANGER_DEPHASE[ped_track] = 0.0f;
            Realslider2(74, 544, f2i(FLANGER_DEPHASE[ped_track] * 20.371833f), TRUE);
            outlong(159, 544, f2i(FLANGER_DEPHASE[ped_track] * 57.29578f), 6);
        }

        if(gode == 0 || gode == 5)
        {
            if(FLANGER_RATE[ped_track] < 0.000001f) FLANGER_RATE[ped_track] = 0.000001f;
            if(FLANGER_RATE[ped_track] > 0.0001363f) FLANGER_RATE[ped_track] = 0.0001363f;
            Realslider(298, 497, f2i(FLANGER_RATE[ped_track] * 939104.92f), TRUE);
            outlong(448, 497, long(0.1424758f / FLANGER_RATE[ped_track]), 2);
        }

        if(gode == 0 || gode == 6)
        {
            if(FLANGER_AMPL[ped_track] > 0.01f) FLANGER_AMPL[ped_track] = 0.01f;
            if(FLANGER_AMPL[ped_track] < 0.0f) FLANGER_AMPL[ped_track] = 0.0f;
            Realslider(298, 515, f2i(FLANGER_AMPL[ped_track] * 12800.0f), TRUE);
            outlong(448, 515, f2i(FLANGER_AMPL[ped_track] * 10000.0f), 1);
        }

        if(gode == 0 || gode == 2)
        {
            if(LFO_RATE[ped_track] < 0.0001f) LFO_RATE[ped_track] = 0.0001f;
            if(LFO_RATE[ped_track] > 0.0078125f) LFO_RATE[ped_track] = 0.0078125f;
            Realslider(74, 490, f2i(LFO_RATE[ped_track] * 16384.0f), TRUE);
            float tmprate = (8.1632653f / LFO_RATE[ped_track]);
            outlong(76, 526, (long) tmprate, 2);
            tmprate = 1000.0f / tmprate;
            outfloat(138, 526, tmprate, 3);
        }

        if(gode == 0 || gode == 3)
        {
            if(LFO_AMPL[ped_track] < 0) LFO_AMPL[ped_track] = 0;
            if(LFO_AMPL[ped_track] > 128) LFO_AMPL[ped_track] = 128;
            Realslider(74, 508, f2i(LFO_AMPL[ped_track]), TRUE);
        }

        if(gode == 0 || gode == 9)
        {
            if(LFO_ON[ped_track] == 1)
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
            if(FLANGER_ON[ped_track] == 1)
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
            if(FLANGER_DELAY[ped_track] > 4096) FLANGER_DELAY[ped_track] = 4096;
            if(FLANGER_DELAY[ped_track] < 0) FLANGER_DELAY[ped_track] = 0;
            if(fld_chan == true)
            {
                FLANGER_OFFSET[ped_track] = 8192;
                foff2[ped_track] = float(FLANGER_OFFSET[ped_track] - FLANGER_DELAY[ped_track]);
                foff1[ped_track] = float(FLANGER_OFFSET[ped_track] - FLANGER_DELAY[ped_track]);  
                fld_chan = false;
            }
            Realslider(298, 551, FLANGER_DELAY[ped_track] / 32, TRUE);
            outlong(448, 551, long(FLANGER_DELAY[ped_track] / 44.1f), 2);
        }
    }//User gui screen match
}

void Mouse_Sliders_Track_Fx_Ed(void)
{
    if(userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(zcheckMouse(74, 490, 148, 16))
        {
            LFO_RATE[ped_track] = (Mouse.x - 84) / 16384.0f;
            teac = 2;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(74, 508, 148, 16))
        {
            LFO_AMPL[ped_track] = float(Mouse.x - 84);
            teac = 3;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(74, 544, 148, 16))
        {
            FLANGER_DEPHASE[ped_track] = (Mouse.x - 84) * 0.0490873f;
            teac = 4;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 4;
        }

        if(zcheckMouse(298, 479, 148, 16))
        {
            teac = 1;
            FLANGER_AMOUNT[ped_track] = ((Mouse.x - 308.0f) / 64.0f) - 1.0f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(298, 497, 148, 16))
        {
            FLANGER_RATE[ped_track] = (Mouse.x - 308.0f) / 939104.92f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 5;
        }
        if(zcheckMouse(298, 515, 148, 16))
        {
            FLANGER_AMPL[ped_track] = (Mouse.x - 308.0f) / 12800.0f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 6;
        }
        if(zcheckMouse(298, 533, 148, 16))
        {
            FLANGER_FEEDBACK[ped_track] = ((Mouse.x - 308) / 64.0f) - 1.0f;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 7;
        }
        if(zcheckMouse(298, 551, 148, 16))
        {
            FLANGER_DELAY[ped_track] = (Mouse.x - 308) * 32;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            fld_chan = true;
            teac = 8;
        }
    }
}

void Mouse_Left_Track_Fx_Ed(void)
{
    if(userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(zcheckMouse(74, 472, 20, 16) && LFO_ON[ped_track] == 0)
        {
            LFO_ON[ped_track] = 1;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 9;
        }
        if(zcheckMouse(96, 472, 20, 16) && LFO_ON[ped_track] == 1)
        {
            LFO_ON[ped_track] = 0;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 9;
        }
        if(zcheckMouse(184, 472, 20, 16) && FLANGER_ON[ped_track] == 0)
        {
            FLANGER_ON[ped_track] = 1;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 10;
        }
        if(zcheckMouse(206, 472, 20, 16) && FLANGER_ON[ped_track] == 1)
        {
            FLANGER_ON[ped_track] = 0;
            gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            teac = 10;
        }
    }
}
