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
extern int Songplaying_Pattern;
extern int patt_highlight;
extern char FullScreen;
extern int Cur_Width;
extern int Cur_Height;
extern char AutoSave;
extern int Beveled;
extern char Use_Shadows;
extern int Continuous_Scroll;
extern int wait_AutoSave;
extern char Global_Patterns_Font;

extern int Nbr_Keyboards;
extern int Keyboard_Idx;
extern char Jazz_Edit;

extern char Accidental;

int current_palette_idx;

char Paste_Across;

extern int Midi_Current_Notes[MAX_TRACKS][MAX_POLYPHONY];

char *Labels_PatSize[] =
{
    "Small",
    "Medium",
    "Large"
};

char *Labels_AutoSave[] =
{
    "Off",
    "1 min",
    "2 mins",
    "4 mins",
    "8 mins",
    "10 mins",
    "15 mins",
    "30 mins"
};

// ------------------------------------------------------
// Functions
char *Get_Keyboard_Label(void);
char *Get_Keyboard_FileName(void);
void Load_Keyboard_Def(char *FileName);

void Display_Milliseconds(int Milliseconds)
{
    char ms[64];
    sprintf(ms, "Milliseconds", Milliseconds);
    PrintXY(456, (Cur_Height - 143), USE_FONT, ms);
}

void Draw_Master_Ed(void)
{
    Get_Phony_Palette();

    Draw_Editors_Bar(USER_SCREEN_SETUP_EDIT);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Misc. Setup");

    char middev[80];

    Gui_Draw_Button_Box(8, (Cur_Height - 133), 310, 64, "", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);

#if defined(__NO_MIDI__)
    sprintf(middev, "In (%d)", 0);
#else
    sprintf(middev, "In (%d)", n_midiindevices);
#endif
    Gui_Draw_Button_Box(12, (Cur_Height - 129), 56, 16, middev, BUTTON_NORMAL | BUTTON_DISABLED);

#if defined(__NO_MIDI__)
    sprintf(middev, "Out (%d)", 0);
#else
    sprintf(middev, "Out (%d)", n_midioutdevices);
#endif

    Gui_Draw_Button_Box(12, (Cur_Height - 110), 56, 16, middev, BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(12, (Cur_Height - 90), 124, 16, "All Notes Off (Track)", BUTTON_NORMAL | BUTTON_TEXT_CENTERED
#if defined(__NO_MIDI__)
    | BUTTON_DISABLED
#endif
    );
    Gui_Draw_Button_Box(138, (Cur_Height - 90), 124, 16, "All Notes Off (Song)", BUTTON_NORMAL | BUTTON_TEXT_CENTERED
#if defined(__NO_MIDI__)
    | BUTTON_DISABLED
#endif
    );

    Gui_Draw_Button_Box(330, (Cur_Height - 145), 59, 16, "Latency", BUTTON_NORMAL | BUTTON_DISABLED);
    Display_Milliseconds(AUDIO_Milliseconds);
    Gui_Draw_Button_Box(330, (Cur_Height - 125), 114, 16, "Mousewheel Multiplier", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, (Cur_Height - 105), 114, 16, "Rows Highlight", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, (Cur_Height - 85), 114, 16, "Decimal Rows", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, (Cur_Height - 65), 114, 16, "Show Prev/Next Patt.", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, (Cur_Height - 45), 114, 16, "Continuous Scroll", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(520, (Cur_Height - 145), 60, 16, "Auto Save", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(670, (Cur_Height - 145), 60, 16, "Full Screen", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(520, (Cur_Height - 125), 60, 16, "Keyboard", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66, (Cur_Height - 105), 60, 16, "Themes", BUTTON_NO_BORDER | BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66, (Cur_Height - 85), 18, 16, "1", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66 + 21, (Cur_Height - 85), 18, 16, "2", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66 + 42, (Cur_Height - 85), 18, 16, "3", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66, (Cur_Height - 65), 18, 16, "4", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66 + 21, (Cur_Height - 65), 18, 16, "5", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66 + 42, (Cur_Height - 65), 18, 16, "6", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66, (Cur_Height - 45), 18, 16, "7", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66 + 21, (Cur_Height - 45), 18, 16, "8", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 66 + 42, (Cur_Height - 45), 18, 16, "9", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(8, (Cur_Height - 65), 110, 16, "Default Pattern Font", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(8, (Cur_Height - 45), 110, 16, "Paste Across Pattern", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(194, (Cur_Height - 45), 62, 16, "Play In Edit", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(734, (Cur_Height - 125), 42, 16, "Accid.", BUTTON_NORMAL | BUTTON_DISABLED);
}

void Actualize_Master_Ed(char gode)
{
    int Real_Palette_Idx;

    if(userscreen == USER_SCREEN_SETUP_EDIT)
    {
        // Milliseconds
        if(gode == 0 || gode == 5)
        {
            if(AUDIO_Milliseconds < 10) AUDIO_Milliseconds = 10;
            if(AUDIO_Milliseconds > 250) AUDIO_Milliseconds = 250;
            Gui_Draw_Arrows_Number_Box(391, (Cur_Height - 145), AUDIO_Milliseconds, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }

        // Create a new sound buffer with the new latency amount
        if(gode == 5)
        {
            SongStop();
            AUDIO_Stop_Sound_Buffer();
            AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds);
            Init_Scopes_Buffers();
            AUDIO_Play();
        }

        // Mouse wheel
        if(gode == 0 || gode == 6)
        {
            if(MouseWheel_Multiplier < 1) MouseWheel_Multiplier = 1;
            if(MouseWheel_Multiplier > 16) MouseWheel_Multiplier = 16;
            Gui_Draw_Arrows_Number_Box2(446, (Cur_Height - 125), MouseWheel_Multiplier, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }

        // Pattern highlight
        if(gode == 0 || gode == 7)
        {
            if(patt_highlight < 1) patt_highlight = 1;
            if(patt_highlight > 16) patt_highlight = 16;
            if(patt_highlight == 1)
            {
                Gui_Draw_Button_Box(446, (Cur_Height - 105), 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 18, (Cur_Height - 105), 24, 16, "Off", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 44, (Cur_Height - 105), 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                if(Rows_Decimal)
                {
                    Gui_Draw_Arrows_Number_Box2(446, (Cur_Height - 105), patt_highlight, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                }
                else
                {
                    value_box(446, (Cur_Height - 105), patt_highlight, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                }
            }
        }
        if(gode == 7) Actupated(0);

        // Use decimal numbering for rows
        if(gode == 0 || gode == 8)
        {
            if(Rows_Decimal)
            {
                Gui_Draw_Button_Box(446, (Cur_Height - 85), 29, 16, "On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 31, (Cur_Height - 85), 29, 16, "Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(446, (Cur_Height - 85), 29, 16, "On", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 31, (Cur_Height - 85), 29, 16, "Off", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }

        // Show Prev. next pattern
        if(gode == 0 || gode == 13)
        {
            if(See_Prev_Next_Pattern)
            {
                Gui_Draw_Button_Box(446, (Cur_Height - 65), 29, 16, "On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 31, (Cur_Height - 65), 29, 16, "Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(446, (Cur_Height - 65), 29, 16, "On", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 31, (Cur_Height - 65), 29, 16, "Off", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }

        // Don't stop at the bottom of a pattern
        if(gode == 0 || gode == 14)
        {
            if(Continuous_Scroll)
            {
                Gui_Draw_Button_Box(446, (Cur_Height - 45), 29, 16, "On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 31, (Cur_Height - 45), 29, 16, "Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(446, (Cur_Height - 45), 29, 16, "On", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 31, (Cur_Height - 45), 29, 16, "Off", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            Draw_Pattern_Right_Stuff();
            Actupated(0);
        }

        if(gode == 0 || gode == 9)
        {
            if(FullScreen)
            {
                Gui_Draw_Button_Box(734, (Cur_Height - 145), 29, 16, "On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(734 + 31, (Cur_Height - 145), 29, 16, "Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(734, (Cur_Height - 145), 29, 16, "On", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(734 + 31, (Cur_Height - 145), 29, 16, "Off", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }

        // Refresh palette infos
        if(gode == 0 || gode == 10)
        {
            if(current_palette_idx < 0) current_palette_idx = 0;
            if(current_palette_idx > (NUMBER_COLORS - 1)) current_palette_idx = NUMBER_COLORS - 1;
            Gui_Draw_Button_Box(520, (Cur_Height - 105), 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Gui_Draw_Button_Box(520 + 16 + 2, (Cur_Height - 105), 108, 16, Labels_Palette[current_palette_idx], BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520 + (18 + 108) + 2, (Cur_Height - 105), 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            if(Phony_Palette[Real_Palette_Idx].r < 0) Phony_Palette[Real_Palette_Idx].r = 0;
            if(Phony_Palette[Real_Palette_Idx].r > 255) Phony_Palette[Real_Palette_Idx].r = 255;
            if(Phony_Palette[Real_Palette_Idx].g < 0) Phony_Palette[Real_Palette_Idx].g = 0;
            if(Phony_Palette[Real_Palette_Idx].g > 255) Phony_Palette[Real_Palette_Idx].g = 255;
            if(Phony_Palette[Real_Palette_Idx].b < 0) Phony_Palette[Real_Palette_Idx].b = 0;
            if(Phony_Palette[Real_Palette_Idx].b > 255) Phony_Palette[Real_Palette_Idx].b = 255;
            Realslider(518, (Cur_Height - 85), Ptk_Palette[Real_Palette_Idx].r / 2, TRUE);
            outlong_small(668, (Cur_Height - 85), Ptk_Palette[Real_Palette_Idx].r, 0, 41, BUTTON_NORMAL | BUTTON_DISABLED);
            Realslider(518, (Cur_Height - 65), Ptk_Palette[Real_Palette_Idx].g / 2, TRUE);
            outlong_small(668, (Cur_Height - 65), Ptk_Palette[Real_Palette_Idx].g, 0, 41, BUTTON_NORMAL | BUTTON_DISABLED);
            Realslider(518, (Cur_Height - 45), Ptk_Palette[Real_Palette_Idx].b / 2, TRUE);
            outlong_small(668, (Cur_Height - 45), Ptk_Palette[Real_Palette_Idx].b, 0, 41, BUTTON_NORMAL | BUTTON_DISABLED);
            Set_Phony_Palette();
            Refresh_Palette();
        }

        // Bevel on/off
        if(gode == 0 || gode == 10 || gode == 13)
        {
            switch(Beveled)
            {
                case 2:
                    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20, (Cur_Height - 105), 14, 16, "B", BUTTON_PUSHED | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
                    break;
                case 1:
                    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20, (Cur_Height - 105), 14, 16, "B", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    break;
                case 0:
                    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20, (Cur_Height - 105), 14, 16, "B", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
                    break;
            }
            Set_Phony_Palette();
            Refresh_Palette();
        }

        // Set auto save interval
        if(gode == 0 || gode == 15)
        {
            if(AutoSave < 0) AutoSave = 0;
            if(AutoSave >= sizeof(Labels_AutoSave) / sizeof(char *)) AutoSave = sizeof(Labels_AutoSave) / sizeof(char *) - 1;
            Gui_Draw_Button_Box(520 + 62 + 2, (Cur_Height - 145), 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520 + 62 + 2 + 18, (Cur_Height - 145), 46, 16, Labels_AutoSave[AutoSave], BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520 + 62 + 2 + 48 + 18, (Cur_Height - 145), 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }

        // Keyboard layout
        if(gode == 0 || gode == 16)
        {
            if(Keyboard_Idx < 0) Keyboard_Idx = 0;
            if(Keyboard_Idx >= (Nbr_Keyboards - 1)) Keyboard_Idx = Nbr_Keyboards - 1;
            if(gode == 16)
            {
                Load_Keyboard_Def(Get_Keyboard_FileName());
            }

#if defined(__WIN32__)
            Gui_Draw_Button_Box(520 + 62 + 2, (Cur_Height - 125), 16, 16, "\03", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
#else
            Gui_Draw_Button_Box(520 + 62 + 2, (Cur_Height - 125), 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif            

            Gui_Draw_Button_Box(520 + 62 + 2 + 18, (Cur_Height - 125), 106, 16, Get_Keyboard_Label(), BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);

#if defined(__WIN32__)
            Gui_Draw_Button_Box(520 + 62 + 2 + 108 + 18, (Cur_Height - 125), 16, 16, "\04", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
#else
            Gui_Draw_Button_Box(520 + 62 + 2 + 108 + 18, (Cur_Height - 125), 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif

        }

        // -----------
#if !defined(__NO_MIDI__)
        Midi_InitIn();
        Midi_InitOut();
#endif

        // Select midi in device
        if(gode == 0 || gode == 11)
        {
#if defined(__NO_MIDI__)
            value_box(70, (Cur_Height - 129), 0, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_DISABLED);
#else
            value_box(70, (Cur_Height - 129), c_midiin + 1, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif
#if !defined(__NO_MIDI__)
            if(c_midiin != -1)
            {
                Gui_Draw_Button_Box(132, (Cur_Height - 129), 182, 16, Midi_GetInName(), BUTTON_NORMAL | BUTTON_DISABLED);
            }
            else
            {
#endif
                Gui_Draw_Button_Box(132, (Cur_Height - 129), 182, 16, "None", BUTTON_NORMAL | BUTTON_DISABLED);
#if !defined(__NO_MIDI__)
            }
#endif
        }

        // Select midi out device
        if(gode == 0 || gode == 12)
        {
#if defined(__NO_MIDI__)
            value_box(70, (Cur_Height - 110), 0, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_DISABLED);
#else
            value_box(70, (Cur_Height - 110), c_midiout + 1, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif
#if !defined(__NO_MIDI__)
            if(c_midiout != -1)
            {
                Gui_Draw_Button_Box(132, (Cur_Height - 110), 182, 16, Midi_GetOutName(), BUTTON_NORMAL | BUTTON_DISABLED);
            }
            else
            {
#endif
                Gui_Draw_Button_Box(132, (Cur_Height - 110), 182, 16, "None", BUTTON_NORMAL | BUTTON_DISABLED);
#if !defined(__NO_MIDI__)
            }
#endif
        }

        // Paste across patterns
        if(gode == 0 || gode == 17)
        {
            if(Paste_Across)
            {
                Gui_Draw_Button_Box(120, (Cur_Height - 45), 29, 16, "On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(120 + 31, (Cur_Height - 45), 29, 16, "Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(120, (Cur_Height - 45), 29, 16, "On", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(120 + 31, (Cur_Height - 45), 29, 16, "Off", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }

        // Play instruments while editing
        if(gode == 0 || gode == 18)
        {
            if(Jazz_Edit)
            {
                Gui_Draw_Button_Box(258, (Cur_Height - 45), 29, 16, "On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(258 + 31, (Cur_Height - 45), 29, 16, "Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(258, (Cur_Height - 45), 29, 16, "On", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(258 + 31, (Cur_Height - 45), 29, 16, "Off", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }

        // Notes type
        if(gode == 0 || gode == 19)
        {
            if(Accidental)
            {
                Gui_Draw_Button_Box(780, (Cur_Height - 125), 14, 16, "b", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(780, (Cur_Height - 125), 14, 16, "#", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            if(gode == 19) Actupated(0);
        }

        // Bevel on/off
        if(gode == 0 || gode == 20)
        {
            switch(Use_Shadows)
            {
                case 0:
                    Gui_Draw_Button_Box(520 + 18 + (18 + 108) + 2 + 20, (Cur_Height - 105), 40, 16, "Shades", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    break;
                case 1:
                    Gui_Draw_Button_Box(520 + 18 + (18 + 108) + 2 + 20, (Cur_Height - 105), 40, 16, "Shades", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    break;
            }
            Actupated(0);
        }

        // Set default size of patterns
        if(gode == 0 || gode == 21)
        {
            if(Global_Patterns_Font < TRACK_SMALL) Global_Patterns_Font = TRACK_SMALL;
            if(Global_Patterns_Font >= TRACK_LARGE) Global_Patterns_Font = TRACK_LARGE;
            Gui_Draw_Button_Box(120, (Cur_Height - 65), 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(120 + 18, (Cur_Height - 65), 46, 16, Labels_PatSize[Global_Patterns_Font], BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(120 + 48 + 18, (Cur_Height - 65), 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }
    }
}

void Mouse_Right_Master_Ed(void)
{
    if(userscreen == USER_SCREEN_SETUP_EDIT)
    {
        // Previous color
        if(zcheckMouse(520, (Cur_Height - 105), 16, 16) == 1)
        {
            current_palette_idx -= 10;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Next color
        if(zcheckMouse(520 + (18 + 108) + 2, (Cur_Height - 105), 16, 16) == 1)
        {
            current_palette_idx += 10;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Turn beveling type 2 on/off
        if(zcheckMouse(520 + (18 + 108) + 2 + 20, (Cur_Height - 105), 14, 16))
        {
            if(Beveled == 2) Beveled = 0;
            else Beveled = 2;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 13;
        }
    }
}

void Mouse_Left_Master_Ed(void)
{
    if(userscreen == USER_SCREEN_SETUP_EDIT)
    {
        // Milliseconds
        if(zcheckMouse(391, (Cur_Height - 145), 16, 16) == 1)
        {
            if(AUDIO_Milliseconds > 10)
            {
                AUDIO_Milliseconds -= 10;
                gui_action = GUI_CMD_UPDATE_SETUP_ED;
                teac = 5;
            }
        }

        // Milliseconds
        if(zcheckMouse(435, (Cur_Height - 145), 16, 16) == 1)
        {
            if(AUDIO_Milliseconds < 250)
            {
                AUDIO_Milliseconds += 10;
                gui_action = GUI_CMD_UPDATE_SETUP_ED;
                teac = 5;
            }
        }

        // Mousewheel
        if(zcheckMouse(446, (Cur_Height - 125), 16, 16) == 1)
        {
            MouseWheel_Multiplier--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 6;
        }

        // Mousewheel
        if(zcheckMouse(446 + 44, (Cur_Height - 125), 16, 16) == 1)
        {
            MouseWheel_Multiplier++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 6;
        }

        // Patterns sep.
        if(zcheckMouse(446, (Cur_Height - 105), 16, 16) == 1)
        {
            patt_highlight--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 7;
        }

        // Patterns sep.
        if(zcheckMouse(446 + 44, (Cur_Height - 105), 16, 16) == 1)
        {
            patt_highlight++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 7;
        }

        // Rows decimal on
        if(zcheckMouse(446, (Cur_Height - 85), 29, 16))
        {
            Rows_Decimal = TRUE;
            teac = 0;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actualize_Sequencer();
            Actupated(0);
        }

        // Rows decimal off
        if(zcheckMouse(446 + 31, (Cur_Height - 85), 29, 16))
        {
            Rows_Decimal = FALSE;
            teac = 0;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actualize_Sequencer();
            Actupated(0);
        }

        // See prev/next pattern
        if(zcheckMouse(446, (Cur_Height - 65), 29, 16))
        {
            See_Prev_Next_Pattern = TRUE;
            teac = 13;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actupated(0);
        }

        // See prev/next pattern
        if(zcheckMouse(446 + 31, (Cur_Height - 65), 29, 16))
        {
            See_Prev_Next_Pattern = FALSE;
            teac = 13;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actupated(0);
        }

        // Continuous scroll
        if(zcheckMouse(446, (Cur_Height - 45), 29, 16))
        {
            Continuous_Scroll = 1;
            teac = 14;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Continuous scroll
        if(zcheckMouse(446 + 31, (Cur_Height - 45), 29, 16))
        {
            Continuous_Scroll = 0;
            teac = 14;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Paste across pattern
        if(zcheckMouse(120, (Cur_Height - 45), 29, 16))
        {
            Paste_Across = TRUE;
            teac = 17;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Paste across pattern
        if(zcheckMouse(120 + 31, (Cur_Height - 45), 29, 16))
        {
            Paste_Across = FALSE;
            teac = 17;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Play while editing
        if(zcheckMouse(258, (Cur_Height - 45), 29, 16))
        {
            Jazz_Edit = TRUE;
            teac = 18;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Play while editing
        if(zcheckMouse(258 + 31, (Cur_Height - 45), 29, 16))
        {
            Jazz_Edit = FALSE;
            teac = 18;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Play while editing
        if(zcheckMouse(780, (Cur_Height - 125), 14, 16))
        {
            Accidental ^= TRUE;
            teac = 19;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Full screen on
        if(zcheckMouse(734, (Cur_Height - 145), 29, 16))
        {
            if(!FullScreen)
            {
                FullScreen = TRUE;
                teac = 9;
                Switch_FullScreen(Cur_Width, Cur_Height);
            }
        }

        // Full screen off
        if(zcheckMouse(734 + 31, (Cur_Height - 145), 29, 16))
        {
            if(FullScreen)
            {
                FullScreen = FALSE;
                teac = 9;
                Switch_FullScreen(Cur_Width, Cur_Height);
            }
        }

        // Previous color
        if(zcheckMouse(520, (Cur_Height - 105), 16, 16) == 1)
        {
            current_palette_idx--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Next color
        if(zcheckMouse(520 + (18 + 108) + 2, (Cur_Height - 105), 16, 16) == 1)
        {
            current_palette_idx++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // set default palette 1
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66, (Cur_Height - 85), 18, 16))
        {
            Restore_Default_Palette(Default_Palette1, Default_Beveled1);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 2
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66 + 21, (Cur_Height - 85), 18, 16))
        {
            Restore_Default_Palette(Default_Palette2, Default_Beveled2);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 3
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66 + 42, (Cur_Height - 85), 18, 16))
        {
            Restore_Default_Palette(Default_Palette3, Default_Beveled3);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 4
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66, (Cur_Height - 65), 18, 16))
        {
            Restore_Default_Palette(Default_Palette4, Default_Beveled4);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 5
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66 + 21, (Cur_Height - 65), 18, 16))
        {
            Restore_Default_Palette(Default_Palette5, Default_Beveled5);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 6
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66 + 42, (Cur_Height - 65), 18, 16))
        {
            Restore_Default_Palette(Default_Palette6, Default_Beveled6);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 7
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66, (Cur_Height - 45), 18, 16))
        {
            Restore_Default_Palette(Default_Palette7, Default_Beveled7);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 8
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66 + 21, (Cur_Height - 45), 18, 16))
        {
            Restore_Default_Palette(Default_Palette8, Default_Beveled8);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Set default palette 9
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 66 + 42, (Cur_Height - 45), 18, 16))
        {
            Restore_Default_Palette(Default_Palette9, Default_Beveled9);
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Turn beveling type 1 on/off
        if(zcheckMouse(520 + (18 + 108) + 2 + 20, (Cur_Height - 105), 14, 16))
        {
            if(Beveled == 1) Beveled = 0;
            else Beveled = 1;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 13;
        }

        // Turn shadows on/off
        if(zcheckMouse(520 + 18 + (18 + 108) + 2 + 20, (Cur_Height - 105), 40, 16))
        {
            Use_Shadows ^= TRUE;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 20;
        }

        // Default patterns zoom
        if(zcheckMouse(120, (Cur_Height - 65), 16, 16) == 1)
        {
            Global_Patterns_Font--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 21;
        }

        // Default patterns zoom
        if(zcheckMouse(120 + 48 + 18, (Cur_Height - 65), 16, 16) == 1)
        {
            Global_Patterns_Font++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 21;
        }

        // Autosave
        if(zcheckMouse(520 + 62 + 2, (Cur_Height - 145), 16, 16) == 1)
        {
            AutoSave--;
            wait_AutoSave = 0;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 15;
        }

        // Autosave
        if(zcheckMouse(520 + 62 + 2 + 48 + 18, (Cur_Height - 145), 16, 16) == 1)
        {
            AutoSave++;
            wait_AutoSave = 0;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 15;
        }

        // ---

        // Midi track notes off
#if !defined(__NO_MIDI__)
        if(zcheckMouse(12, (Cur_Height - 90), 124, 16) == 1 && c_midiout != -1)
        {
            Midi_NoteOff(Track_Under_Caret, -1);
            int i;
            for(i = 0; i < MAX_POLYPHONY; i++)
            {
                Midi_Current_Notes[CHAN_MIDI_PRG[Track_Under_Caret]][i] = 0;
            }
            gui_action = GUI_CMD_MIDI_NOTE_OFF_1_TRACK;
        }
#endif

        // All Midi notes off
#if !defined(__NO_MIDI__)
        if(zcheckMouse(138, (Cur_Height - 90), 124, 16) == 1 && c_midiout != -1)
        {
            Midi_AllNotesOff();
            gui_action = GUI_CMD_MIDI_NOTE_OFF_ALL_TRACKS;
        }
#endif

#if !defined(__NO_MIDI__)
        // Previous midi in device
        if(zcheckMouse(70, (Cur_Height - 129), 16, 16))
        {
            c_midiin--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiin_changed = 1;
            teac = 11;
        }
        // Next midi in device
        if(zcheckMouse(114, (Cur_Height - 129), 16, 16))
        {
            c_midiin++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiin_changed = 1;
            teac = 11;
        }

        // Previous midi out device
        if(zcheckMouse(70, (Cur_Height - 110), 16, 16) == 1)
        {
            c_midiout--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiout_changed = TRUE;
            teac = 12;
        }
        // Next midi out device
        if(zcheckMouse(114, (Cur_Height - 110), 16, 16) == 1)
        {
            c_midiout++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiout_changed = TRUE;
            teac = 12;
        }
#endif

#if !defined(__WIN32__)
        // Keyboard
        if(zcheckMouse(520 + 62 + 2, (Cur_Height - 125), 16, 16) == 1)
        {
            Keyboard_Idx--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 16;
        }

        // Keyboard
        if(zcheckMouse(520 + 62 + 2 + 108 + 18, (Cur_Height - 125), 16, 16) == 1)
        {
            Keyboard_Idx++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 16;
        }
#endif

    }
}

void Mouse_Sliders_Master_Ed(void)
{
    int Real_Palette_Idx;

    if(userscreen == USER_SCREEN_SETUP_EDIT)
    {
        // Red component
        if(zcheckMouse(518, (Cur_Height - 81), 148, 16))
        {
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            Phony_Palette[Real_Palette_Idx].r = (int) ((Mouse.x - 10 - 518.0f)) * 2;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Green component
        if(zcheckMouse(518, (Cur_Height - 81) + 18, 148, 16))
        {
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            Phony_Palette[Real_Palette_Idx].g = (int) ((Mouse.x - 10 - 518.0f)) * 2;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Blue component
        if(zcheckMouse(518, (Cur_Height - 81) + 18 + 18, 148, 16))
        {
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            Phony_Palette[Real_Palette_Idx].b = (int) ((Mouse.x - 10 - 518.0f)) * 2;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }
    }
}
