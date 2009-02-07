// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/editor_setup.h"
#include "include/editor_sequencer.h"
#include "../midi/include/midi.h"

// ------------------------------------------------------
// Variables
extern int Songplaying_Pattern;
extern int patt_highlight;
extern char FullScreen;
extern char AutoSave;
extern int Beveled;
extern int Continuous_Scroll;
extern int wait_AutoSave;

extern int Nbr_Keyboards;
extern int Keyboard_Idx;

int current_palette_idx;

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
    PrintXY(456, 457, USE_FONT, ms);
}

void Draw_Master_Ed(void)
{
    Get_Phony_Palette();

    Draw_Editors_Bar(USER_SCREEN_SETUP_EDIT);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Misc. Setup");

    char middev[80];

#if defined(__NO_MIDI__)
    sprintf(middev, "Midi Setup. Found: %d Midi-In devices and %d Midi-Out devices.", 0, 0);
#else
    sprintf(middev, "Midi Setup. Found: %d Midi-In devices and %d Midi-Out devices.", n_midiindevices, n_midioutdevices);
#endif
    Gui_Draw_Button_Box(8, 466, 310, 96, middev, BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(12, 482, 56, 16, "Midi IN", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(12, 500, 56, 16, "Midi OUT", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(12, 522, 124, 16, "All Notes Off (Track)", BUTTON_NORMAL
#if defined(__NO_MIDI__)
    | BUTTON_DISABLED
#endif
    );
    Gui_Draw_Button_Box(138, 522, 124, 16, "All Notes Off (Song)", BUTTON_NORMAL
#if defined(__NO_MIDI__)
    | BUTTON_DISABLED
#endif
    );
    PrintXY(12, 543, USE_FONT, "Go to 'Track' section to assign Midi Channels to the tracks");

    Gui_Draw_Button_Box(330, 455, 59, 16, "Latency", BUTTON_NORMAL | BUTTON_DISABLED);
    Display_Milliseconds(AUDIO_Milliseconds);
    Gui_Draw_Button_Box(330, 475, 114, 16, "Mousewheel Multiplier", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, 495, 114, 16, "Rows Highlight", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, 515, 114, 16, "Decimal Rows", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, 535, 114, 16, "Show Prev/Next Patt.", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(330, 555, 114, 16, "Continuous Scroll", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(520, 455, 60, 16, "Auto save", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(670, 455, 60, 16, "Full Screen", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(520, 475, 60, 16, "Keyboard", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20, 495, 40, 16, "Default", BUTTON_NORMAL);
    Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 44, 495, 14, 16, "B", BUTTON_NORMAL);
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
#if defined(__MACOSX__)
            Gui_Draw_Arrows_Number_Box(391, 455, AUDIO_Milliseconds, BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
#else
            Gui_Draw_Arrows_Number_Box(391, 455, AUDIO_Milliseconds, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif
        }

        // Create a new sound buffer with the new latency amount
        if(gode == 5)
        {
            AUDIO_Stop_Sound_Buffer();
            AUDIO_Create_Sound_Buffer(AUDIO_Milliseconds);
            AUDIO_Play();
            SongStop();
            Actupated(0);
        }

        // Mouse wheel
        if(gode == 0 || gode == 6)
        {
            if(MouseWheel_Multiplier < 1) MouseWheel_Multiplier = 1;
            if(MouseWheel_Multiplier > 16) MouseWheel_Multiplier = 16;
            value_box(446, 475, MouseWheel_Multiplier, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }

        // Pattern highlight
        if(gode == 0 || gode == 7)
        {
            if(patt_highlight < 1) patt_highlight = 1;
            if(patt_highlight > 16) patt_highlight = 16;
            if(patt_highlight == 1)
            {
                Gui_Draw_Button_Box(446, 495, 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 18, 495, 24, 16, "Off", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(446 + 44, 495, 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                value_box(446, 495, patt_highlight, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
        }
        if(gode == 7) Actupated(0);

        if(gode == 0 || gode == 8)
        {
            if(Rows_Decimal)
            {
                Gui_Draw_Button_Box(446, 515, 29, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(446 + 31, 515, 29, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(446, 515, 29, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(446 + 31, 515, 29, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 13)
        {
            if(See_Prev_Next_Pattern)
            {
                Gui_Draw_Button_Box(446, 535, 29, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(446 + 31, 535, 29, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(446, 535, 29, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(446 + 31, 535, 29, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 14)
        {
            if(Continuous_Scroll)
            {
                Gui_Draw_Button_Box(446, 555, 29, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(446 + 31, 555, 29, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(446, 555, 29, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(446 + 31, 555, 29, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 9)
        {
            if(FullScreen)
            {
                Gui_Draw_Button_Box(734, 455, 29, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(734 + 31, 455, 29, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(734, 455, 29, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(734 + 31, 455, 29, 16, "Off", BUTTON_PUSHED);
            }
        }

        // Refresh palette infos
        if(gode == 0 || gode == 10)
        {
            if(current_palette_idx < 0) current_palette_idx = 0;
            if(current_palette_idx > (NUMBER_COLORS - 1)) current_palette_idx = NUMBER_COLORS - 1;
            Gui_Draw_Button_Box(520, 495, 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520 + 16 + 2, 495, 108, 16, Labels_Palette[current_palette_idx], BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(520 + (18 + 108) + 2, 495, 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            if(Phony_Palette[Real_Palette_Idx].r < 0) Phony_Palette[Real_Palette_Idx].r = 0;
            if(Phony_Palette[Real_Palette_Idx].r > 255) Phony_Palette[Real_Palette_Idx].r = 255;
            if(Phony_Palette[Real_Palette_Idx].g < 0) Phony_Palette[Real_Palette_Idx].g = 0;
            if(Phony_Palette[Real_Palette_Idx].g > 255) Phony_Palette[Real_Palette_Idx].g = 255;
            if(Phony_Palette[Real_Palette_Idx].b < 0) Phony_Palette[Real_Palette_Idx].b = 0;
            if(Phony_Palette[Real_Palette_Idx].b > 255) Phony_Palette[Real_Palette_Idx].b = 255;
            Realslider(518, 515, Ptk_Palette[Real_Palette_Idx].r / 2, TRUE);
            outlong_small(668, 515, Ptk_Palette[Real_Palette_Idx].r, 0, 41, BUTTON_NORMAL | BUTTON_DISABLED);
            Realslider(518, 535, Ptk_Palette[Real_Palette_Idx].g / 2, TRUE);
            outlong_small(668, 535, Ptk_Palette[Real_Palette_Idx].g, 0, 41, BUTTON_NORMAL | BUTTON_DISABLED);
            Realslider(518, 555, Ptk_Palette[Real_Palette_Idx].b / 2, TRUE);
            outlong_small(668, 555, Ptk_Palette[Real_Palette_Idx].b, 0, 41, BUTTON_NORMAL | BUTTON_DISABLED);
            Set_Phony_Palette();
            Refresh_Palette();
        }

        // Bevel on/off
        if(gode == 0 || gode == 13)
        {
            if(Beveled)
            {
                Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 44, 495, 14, 16, "B", BUTTON_PUSHED);
            }
            else
            {
                Gui_Draw_Button_Box(520 + (18 + 108) + 2 + 20 + 44, 495, 14, 16, "B", BUTTON_NORMAL);
            }
            Set_Phony_Palette();
            Refresh_Palette();
        }

        // Set auto save interval
        if(gode == 0 || gode == 15)
        {
            if(AutoSave < 0) AutoSave = 0;
            if(AutoSave >= sizeof(Labels_AutoSave) / sizeof(char *)) AutoSave = sizeof(Labels_AutoSave) / sizeof(char *) - 1;
            Gui_Draw_Button_Box(520 + 62 + 2, 455, 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520 + 62 + 2 + 18, 455, 46, 16, Labels_AutoSave[AutoSave], BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(520 + 62 + 2 + 48 + 18, 455, 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
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
            Gui_Draw_Button_Box(520 + 62 + 2, 475, 16, 16, "\03", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
#else
            Gui_Draw_Button_Box(520 + 62 + 2, 475, 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif            

            Gui_Draw_Button_Box(520 + 62 + 2 + 18, 475, 106, 16, Get_Keyboard_Label(), BUTTON_NORMAL | BUTTON_DISABLED);

#if defined(__WIN32__)
            Gui_Draw_Button_Box(520 + 62 + 2 + 108 + 18, 475, 16, 16, "\04", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
#else
            Gui_Draw_Button_Box(520 + 62 + 2 + 108 + 18, 475, 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif

        }

        // -----------
#if !defined(__NO_MIDI__)
        Midi_InitIn();
        Midi_InitOut();
#endif

        if(gode == 0 || gode == 11)
        {
#if defined(__NO_MIDI__)
            value_box(70, 482, 0, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_DISABLED);
#else
            value_box(70, 482, c_midiin + 1, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif
#if !defined(__NO_MIDI__)
            if(c_midiin != -1)
            {
                Gui_Draw_Button_Box(132, 482, 182, 16, Midi_GetInName(), BUTTON_NORMAL | BUTTON_DISABLED);
            }
            else
            {
#endif
                Gui_Draw_Button_Box(132, 482, 182, 16, "None", BUTTON_NORMAL | BUTTON_DISABLED);
#if !defined(__NO_MIDI__)
            }
#endif
        }

        if(gode == 0 || gode == 12)
        {
#if defined(__NO_MIDI__)
            value_box(70, 500, 0, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_DISABLED);
#else
            value_box(70, 500, c_midiout + 1, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#endif
#if !defined(__NO_MIDI__)
            if(c_midiout != -1)
            {
                Gui_Draw_Button_Box(132, 500, 182, 16, Midi_GetOutName(), BUTTON_NORMAL | BUTTON_DISABLED);
            }
            else
            {
#endif
                Gui_Draw_Button_Box(132, 500, 182, 16, "None", BUTTON_NORMAL | BUTTON_DISABLED);
#if !defined(__NO_MIDI__)
            }
#endif
        }
    }
}

void Mouse_Right_Master_Ed(void)
{
    if(userscreen == USER_SCREEN_SETUP_EDIT)
    {
        // Previous color
        if(zcheckMouse(520, 501, 16, 16) == 1)
        {
            current_palette_idx -= 10;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Next color
        if(zcheckMouse(520 + (18 + 108) + 2, 501, 16, 16) == 1)
        {
            current_palette_idx += 10;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }
    }
}

void Mouse_Left_Master_Ed(void)
{
    if(userscreen == USER_SCREEN_SETUP_EDIT)
    {
        // Milliseconds
#if !defined(__MACOSX__)
        if(zcheckMouse(391, 455, 16, 16) == 1)
        {
            if(AUDIO_Milliseconds > 10)
            {
                AUDIO_Milliseconds -= 10;
                gui_action = GUI_CMD_UPDATE_SETUP_ED;
                teac = 5;
            }
        }

        // Milliseconds
        if(zcheckMouse(435, 455, 16, 16) == 1)
        {
            if(AUDIO_Milliseconds < 250)
            {
                AUDIO_Milliseconds += 10;
                gui_action = GUI_CMD_UPDATE_SETUP_ED;
                teac = 5;
            }
        }
#endif

        // Mousewheel
        if(zcheckMouse(446, 475, 16, 16) == 1)
        {
            MouseWheel_Multiplier--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 6;
        }

        // Mousewheel
        if(zcheckMouse(446 + 44, 475, 16, 16) == 1)
        {
            MouseWheel_Multiplier++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 6;
        }

        // Patterns sep.
        if(zcheckMouse(446, 495, 16, 16) == 1)
        {
            patt_highlight--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 7;
        }

        // Patterns sep.
        if(zcheckMouse(446 + 44, 495, 16, 16) == 1)
        {
            patt_highlight++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 7;
        }

        // Rows decimal on
        if(zcheckMouse(446, 515, 29, 16))
        {
            Rows_Decimal = TRUE;
            teac = 8;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actualize_Sequencer();
            Actupated(0);
        }

        // Rows decimal off
        if(zcheckMouse(446 + 31, 515, 29, 16))
        {
            Rows_Decimal = FALSE;
            teac = 8;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actualize_Sequencer();
            Actupated(0);
        }

        // See prev/next pattern
        if(zcheckMouse(446, 535, 29, 16))
        {
            See_Prev_Next_Pattern = TRUE;
            teac = 13;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actupated(0);
        }

        // See prev/next pattern
        if(zcheckMouse(446 + 31, 535, 29, 16))
        {
            See_Prev_Next_Pattern = FALSE;
            teac = 13;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            Actupated(0);
        }

        // Continuous scroll
        if(zcheckMouse(446, 555, 29, 16))
        {
            Continuous_Scroll = TRUE;
            teac = 14;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Continuous scroll
        if(zcheckMouse(446 + 31, 555, 29, 16))
        {
            Continuous_Scroll = FALSE;
            teac = 14;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
        }

        // Full screen on
        if(zcheckMouse(734, 455, 29, 16))
        {
            if(!FullScreen)
            {
                FullScreen = TRUE;
                teac = 9;
                Switch_FullScreen();
            }
        }

        // Full screen off
        if(zcheckMouse(734 + 31, 455, 29, 16))
        {
            if(FullScreen)
            {
                FullScreen = FALSE;
                teac = 9;
                Switch_FullScreen();
            }
        }

        // Previous color
        if(zcheckMouse(520, 495, 16, 16) == 1)
        {
            current_palette_idx--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Next color
        if(zcheckMouse(520 + (18 + 108) + 2, 495, 16, 16) == 1)
        {
            current_palette_idx++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // restore default palette
        if(zcheckMouse(520 + (18 + 108) + 2 + 20, 495, 40, 16))
        {
            Restore_Default_Palette();
            Get_Phony_Palette();
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }  

        // Turn beveling on/off
        if(zcheckMouse(520 + (18 + 108) + 2 + 20 + 44, 495, 14, 16))
        {
            Beveled ^= TRUE;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 13;
        }

        // Autosave
        if(zcheckMouse(520 + 62 + 2, 455, 16, 16) == 1)
        {
            AutoSave--;
            wait_AutoSave = 0;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 15;
        }

        // Autosave
        if(zcheckMouse(520 + 62 + 2 + 48 + 18, 455, 16, 16) == 1)
        {
            AutoSave++;
            wait_AutoSave = 0;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 15;
        }

        // ---

        // Midi track notes off
#if !defined(__NO_MIDI__)
        if(zcheckMouse(12, 522, 124, 16) == 1 && c_midiout != -1)
        {
            Midi_NoteOff(ped_track);
            gui_action = GUI_CMD_MIDI_NOTE_OFF_1_TRACK;
        }
#endif

        // All Midi notes off
#if !defined(__NO_MIDI__)
        if(zcheckMouse(138, 522, 124, 16) == 1 && c_midiout != -1)
        {
            Midi_AllNotesOff();
            gui_action = GUI_CMD_MIDI_NOTE_OFF_ALL_TRACKS;
        }
#endif

#if !defined(__NO_MIDI__)
        // Previous midi in device
        if(zcheckMouse(70, 482, 16, 16))
        {
            c_midiin--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiin_changed = 1;
            teac = 11;
        }
        // Next midi in device
        if(zcheckMouse(114, 482, 16, 16))
        {
            c_midiin++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiin_changed = 1;
            teac = 11;
        }

        // Previous midi out device
        if(zcheckMouse(70, 500, 16, 16) == 1)
        {
            c_midiout--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiout_changed = TRUE;
            teac = 12;
        }
        // Next midi out device
        if(zcheckMouse(114, 500, 16, 16) == 1)
        {
            c_midiout++;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            midiout_changed = TRUE;
            teac = 12;
        }
#endif

#if !defined(__WIN32__)
        // Keyboard
        if(zcheckMouse(520 + 62 + 2, 475, 16, 16) == 1)
        {
            Keyboard_Idx--;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 16;
        }

        // Keyboard
        if(zcheckMouse(520 + 62 + 2 + 108 + 18, 475, 16, 16) == 1)
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
        if(zcheckMouse(518, 519, 148, 16))
        {
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            Phony_Palette[Real_Palette_Idx].r = (int) ((Mouse.x - 10 - 518.0f)) * 2;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Green component
        if(zcheckMouse(518, 519 + 18, 148, 16))
        {
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            Phony_Palette[Real_Palette_Idx].g = (int) ((Mouse.x - 10 - 518.0f)) * 2;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }

        // Blue component
        if(zcheckMouse(518, 519 + 18 + 18, 148, 16))
        {
            Real_Palette_Idx = Idx_Palette[current_palette_idx];
            Phony_Palette[Real_Palette_Idx].b = (int) ((Mouse.x - 10 - 518.0f)) * 2;
            gui_action = GUI_CMD_UPDATE_SETUP_ED;
            teac = 10;
        }
    }
}
