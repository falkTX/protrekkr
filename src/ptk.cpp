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
#include "include/ptk.h"

// ------------------------------------------------------
// Variables
unsigned char sl3 = 0;

int CONSOLE_WIDTH = 800;
int CHANNELS_WIDTH = 800 - 21;
int CONSOLE_HEIGHT = 600;
int CONSOLE_HEIGHT2 = 600;
int fluzy = -1;
int Scopish = SCOPE_ZONE_MOD_DIR;
char Scopish_LeftRight = FALSE;
int rev_counter2 = 0;
char Visible_Columns = 0;
int rs_coef = 32768;

int gui_lx = -1;
int gui_ly = -1;
int gui_lxs = -1;
int gui_lys = -1;
int gui_pushed = 0;
char teac = 0;
int liveparam = 0;
int livevalue = 0;
char trkchan = TRUE;
int pos_space = 0;
int multifactor = 4;
char seditor = 0;
char tipoftheday[256];
int ctipoftheday = 0;
char ped_split = 0;

int player_pos = -1;
int xew = 0;
char sas = FALSE;
int flagger = 0;
int ltretnote = 0;
int ltretnote_raw = 0;
int ltretvalue = 0;
int retletter[256];
int tretletter = 0;
int posletter = 0;
int last_index = -1;
int gui_action = GUI_CMD_NOP;
int ped_col = 0;
int ped_track = 0;
int gui_track = 0;
int xoffseted;
float gr = 0;
float synthsignal = 0;

char userscreen = USER_SCREEN_DISKIO_EDIT;
int c_r_release = 0;
int c_l_release = 0;

char name[MAX_PATH];
char synthname[MAX_PATH];
char instrname[MAX_PATH];
char name303[MAX_PATH];
char artist[20];
char style[20];
char autosavename[MAX_PATH];

int ped_patsam = 0;
int restx = 0;
int resty = 0;
int fsize = 0;
char draw_sampled_wave = FALSE;
char draw_sampled_wave2 = FALSE;
char draw_sampled_wave3 = FALSE;

int redraw_everything = 0;
char boing = FALSE;
int LastPedRow = -1;
char po_ctrl2 = TRUE;
char po_alt = TRUE;
char po_alt2 = TRUE;
int CHAN_MIDI_PRG[MAX_TRACKS];

int sp_TickCounter = 0;
int LVColor = 0;
int RVColor = 0;
int player_line = 0;
char actuloop = 0;
int namesize = 8;

int c_l_tvol = 32768;
int c_r_tvol = 32768;
int c_l_cvol = 32768;
int c_r_cvol = 32768;
int mlimit = 0;

int hd_isrecording = 0;

int snamesel = 0;

SDL_Surface *PFONT;
SDL_Surface *FONT;
SDL_Surface *FONT_LOW;

SDL_TimerID Timer;
Uint32 Timer_CallBack(Uint32 interval, void *param);
Uint32 (*Timer_Ptr)(Uint32 interval, void *param) = &Timer_CallBack;
char CpuStr[80];

int Asking_Exit;

int MouseWheel_Multiplier = 1;
char Rows_Decimal = FALSE;
char See_Prev_Next_Pattern = FALSE;

char cur_input_name[1024];

char *test_text = TITLE" was written by hitchhikr / Neural.";
SDL_Surface *LOGOPIC;
int wait_title;
int display_title = 0;

int key_record_first_time;
double key_ticks;
int old_key_ped_line;
Uint32 Alloc_midi_Channels[MAX_TRACKS][MAX_POLYPHONY];
int Record_Keys[37];
int Record_Keys_State[37];

void Mouse_Sliders_Master_Shuffle(void);
void Display_Beat_Time(void);
int Calc_Length(void);

extern int Ticks_Synchro_Left;
extern int Ticks_Synchro_Right;
extern int Continuous_Scroll;

void Draw_Scope(void);
void Draw_Scope_Files_Button(void);
void Display_Tracks_To_Render(void);
void Solo_Track(int track_to_solo);

JAZZ_KEY Sub_Channels_Jazz[MAX_TRACKS][MAX_POLYPHONY];

int wait_AutoSave;
extern char AutoSave;
int Values_AutoSave[] =
{
    0,
    1 * 60,
    2 * 60,
    4 * 60,
    8 * 60,
    10 * 60,
    15 * 60,
    30 * 60
};

char Prog_Path[MAX_PATH];

int LOGO_WIDTH;
int LOGO_HEIGHT;
int LOGO_BKCOL;

int Table_Right_Tab_Notes[] =
{
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
};

int Table_Left_Tab_Notes[] =
{
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
};

int Keyboard_Events_Channels[256];
JAZZ_KEY Sub_Channels_NoteOff[MAX_TRACKS][MAX_POLYPHONY];
int Nbr_Sub_NoteOff;

// ------------------------------------------------------
// Load a skin picture according to the xml script
SDL_Surface *Load_Skin_Picture(char *name)
{
    char filepath[MAX_PATH];
    const char *pic_filename;
    char error[256];
    SDL_Surface *surface;

    pic_filename = XML_get_string("files", "file", name, "value");
    if(!pic_filename)
    {
        Message_Error("XML error in files tag.");
        return(NULL);
    }

#if defined(__WIN32__)
    sprintf(filepath, "skins\\%s", pic_filename);
#else
    sprintf(filepath, "skins/%s", pic_filename);
#endif

    surface = Load_Picture(filepath);
    if(surface == NULL)
    {
        sprintf(error, "Can't load %s file.", pic_filename);
        Message_Error(error);
        return(NULL);
    }
    return(surface);
}

// ------------------------------------------------------
// Load the font letters sizes according to the xml script
int Load_Font_Datas(char *name)
{
    char filepath[MAX_PATH];
    const char *pic_filename;
    char error[256];

    pic_filename = XML_get_string("files", "file", name, "value");
    
    if(!pic_filename)
    {
        Message_Error("XML error in files tag.");
        return(FALSE);
    }
    
#if defined(__WIN32__)
    sprintf(filepath, "skins\\%s", pic_filename);
#else
    sprintf(filepath, "skins/%s", pic_filename);
#endif
    if(!Create_Font_Datas(filepath))
    {
        sprintf(error, "Can't load %s file.", pic_filename);
        Message_Error(error);
        return(FALSE);
    }
    return(TRUE);
}

// ------------------------------------------------------
// Load the necessary datas and initialize the interface
int Init_Context(void)
{
    restx = CONSOLE_WIDTH - 640;
    resty = CONSOLE_HEIGHT - 492;
    CONSOLE_HEIGHT2 = CONSOLE_HEIGHT - 42;
    mlimit = 619 + restx;
    fsize = 638 + restx;
    Visible_Columns = CONSOLE_WIDTH / 128;

#if defined(__WIN32__)
    srand(GetTickCount());
#else
    srand(rand());
#endif

    ctipoftheday = rand() % 12;

    switch(ctipoftheday)
    {
        case 0: sprintf(tipoftheday, "Tip Of The Hour: Pressing CTRL+I will interpolate effect value on a marked block."); break;
        case 1: sprintf(tipoftheday, "Tip Of The Hour: The right mouse button will have a secondary action on most buttons."); break;
        case 2: sprintf(tipoftheday, "Tip Of The Hour: Don't set excessive track reverb send values, to get better quality."); break;
        case 3: sprintf(tipoftheday, "Tip Of The Hour: When recording, number of positions will grow automatically when necessary."); break;
        case 4: sprintf(tipoftheday, "Remember: MIDI is not audio, realtime fx will not affect midi sound."); break;
        case 5: sprintf(tipoftheday, "Tip Of The Hour: On lower CPUs, you can renderize patterns to wav, and use them as samples without any loss of quality."); break;
        case 6: sprintf(tipoftheday, "Tip Of The Hour: Volume note-cut command 'Fx' is very useful to avoid sample-clicking."); break;
        case 7: sprintf(tipoftheday, "Tip Of The Hour: Left-Clicking on pattern editor channels numbers will mute/unmute any track (right clicking will solo it)."); break;
        case 8: sprintf(tipoftheday, "Tip Of The Hour: Pattern command '16xx' will reset the Filter LFO of the track. No parameter required."); break;
        case 9: sprintf(tipoftheday, "Tip Of The Hour: With a '90' value on the panning column you can change midi controllers values."); break;
        case 10: sprintf(tipoftheday, "Tip Of The Hour: Increase latency time if sound is distorted."); break;
        case 11: sprintf(tipoftheday, "Tip Of The Hour: Pressing right mouse button on most arrows buttons (\03\04) will speed operation up.");

        default: sprintf(tipoftheday, "Tip Of The Hour: See readme.txt for more infos about help and pattern commands."); break;
    }

    L_MaxLevel = 0;
    R_MaxLevel = 0;

    sprintf(name, "Untitled");
    sprintf(artist, "Somebody");
    sprintf(style, "Goa Trance");

    namesize = 8;
    IniCsParNames();

    Clear_Files_List();

#if !defined(__NO_MIDI__)
    midiin_changed = 2;
    midiout_changed = 2;

    Midi_InitIn();
    Midi_InitOut();
#endif

    GETCWD(Prog_Path, MAX_PATH);

    Read_SMPT();

    CHDIR(Prog_Path);

    if(!Init_Block_Work()) return(FALSE);

#if !defined(__NO_MIDI__)
    Midi_Reset();
#endif

    if(!Alloc_Patterns_Pool())
    {
        Message_Error("Can't allocate room for patterns.");
        return(FALSE);
    }

    // Player initialization
#if defined(__WIN32__)
    if(!Ptk_InitDriver(Main_Window, AUDIO_Milliseconds))
#else
    if(!Ptk_InitDriver(AUDIO_Milliseconds))
#endif
    {
        Ptk_ReleaseDriver();
        Message_Error("Can't initialize audio driver.");
        return(FALSE);
    }

    Set_Default_Channels_Polyphony();
    init_sample_bank();
    Pre_Song_Init();
    Post_Song_Init();

    char *toto = test_text;

    LOGOPIC = Load_Skin_Picture("logo");
    if(!LOGOPIC) return(FALSE);

    if(!XML_get_integer("files", "file", "logo", "width", &LOGO_WIDTH))
    {
        Message_Error("logo width key missing.");
        return(FALSE);
    }
    if(!XML_get_integer("files", "file", "logo", "height", &LOGO_HEIGHT))
    {
        Message_Error("logo height key missing.");
        return(FALSE);
    }
    if(!XML_get_integer("files", "file", "logo", "bkcol", &LOGO_BKCOL))
    {
        Message_Error("logo bkcol key missing.");
        return(FALSE);
    }

    POINTER = Load_Skin_Picture("pointer");
    if(!POINTER) return(FALSE);
    SKIN303 = Load_Skin_Picture("303");
    if(!SKIN303) return(FALSE);
    PFONT = Load_Skin_Picture("pattern_font");
    if(!PFONT) return(FALSE);
    FONT = Load_Skin_Picture("font");
    if(!FONT) return(FALSE);
    FONT_LOW = Load_Skin_Picture("font");
    if(!FONT_LOW) return(FALSE);

    if(!Set_Pictures_Colors()) return(FALSE);

    if(!Load_Font_Datas("font_datas")) return(FALSE);

    SDL_SetColorKey(FONT, SDL_SRCCOLORKEY, 0);
    SDL_SetColorKey(FONT_LOW, SDL_SRCCOLORKEY, 0);
    SDL_SetColorKey(POINTER, SDL_SRCCOLORKEY, 0);

    Timer = SDL_AddTimer(1000, Timer_Ptr, NULL);

    return(TRUE);
}
   
// ------------------------------------------------------
// Release the allocated resources
extern int volatile AUDIO_Acknowledge;
void Destroy_Context(void)
{
    if(Timer) SDL_RemoveTimer(Timer);
    AUDIO_Acknowledge = TRUE;
    Ptk_ReleaseDriver();

    Free_Samples();

    // Freeing Allocated Patterns
    if(RawPatterns) free(RawPatterns);

    Destroy_UI();
    SDL_Quit();
}

// ------------------------------------------------------
// Update the world
int Screen_Update(void)
{
    int FineTune_Value;
    int i;

    redraw_everything = 0;
    if(Env_Change)
    {
        redraw_everything = 1;
        Env_Change = FALSE;
    }

    for(i = 0; i < Channels_Polyphony[ped_track]; i++)
    {
        if(sp_Stage[ped_track][i] == PLAYING_SAMPLE && ped_patsam == sp_channelsample[ped_track][i] && ped_split == sp_split[ped_track][i])
        {
            draw_sampled_wave2 = TRUE;
            boing = TRUE;
            break;
        }
        else if(boing)
        {
            boing = FALSE;
            draw_sampled_wave3 = TRUE;
            break;
        }
    }

    if(display_title)
    {
        if(Scopish == SCOPE_ZONE_SCOPE) Draw_Scope();

        // Sample ed.
        Draw_Sampled_Wave();

        int Lt_vu = MIN_VUMETER + L_MaxLevel / LARG_VUMETER;
        int Rt_vu = MIN_VUMETER + R_MaxLevel / LARG_VUMETER;
        int Lt_vu_Peak = Lt_vu;
        int Rt_vu_Peak = Rt_vu;
        if(Lt_vu_Peak > MAX_VUMETER - 1) Lt_vu_Peak = MAX_VUMETER - 1;
        if(Rt_vu_Peak > MAX_VUMETER - 1) Rt_vu_Peak = MAX_VUMETER - 1;
        if(Lt_vu > MIN_PEAK) Lt_vu = MIN_PEAK;
        if(Rt_vu > MIN_PEAK) Rt_vu = MIN_PEAK;

        // Draw the vu meters
        DrawHLine(10, MIN_VUMETER, Lt_vu, COL_VUMETER);
        DrawHLine(11, MIN_VUMETER, Lt_vu, COL_VUMETER);
        DrawHLine(12, MIN_VUMETER, Lt_vu, COL_VUMETER);
        DrawHLine(13, MIN_VUMETER, Lt_vu, COL_VUMETER);

        DrawHLine(15, MIN_VUMETER, Rt_vu, COL_VUMETER);
        DrawHLine(16, MIN_VUMETER, Rt_vu, COL_VUMETER);
        DrawHLine(17, MIN_VUMETER, Rt_vu, COL_VUMETER);
        DrawHLine(18, MIN_VUMETER, Rt_vu, COL_VUMETER);

        DrawHLine(10, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(11, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(12, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(13, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);

        DrawHLine(15, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(16, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(17, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(18, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);

        DrawHLine(10, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(11, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(12, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(13, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);

        DrawHLine(15, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(16, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(17, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(18, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);

        if(actuloop) Afloop();
    }

    if(gui_action != 0)
    { // There are some for me today.....:)

        // Files list slider
        if(gui_action == GUI_CMD_SET_FILES_LIST_SLIDER)
        {
            lt_ykar = Mouse.y - 72;
            Actualize_Files_List(0);
        }

        // File selection
        if(gui_action == GUI_CMD_SET_FILES_LIST_SELECT_FILE)
        {
            int broadcast = lt_index + (Mouse.y - 43) / 12;
            last_index = -1;
            if(broadcast != lt_curr)
            {
                lt_curr = broadcast;
                Actualize_Files_List(1);
            }
            else
            {
                if(Get_Current_FileType() != _A_SUBDIR)
                {
                    AUDIO_Stop();
                    Stop_Current_Sample();
                    LoadFile(ped_patsam, Get_Current_FileName());
                    AUDIO_Play();
                }
                else
                {
                    Set_Current_Dir();
                    Read_SMPT();
                    Actualize_Files_List(0);
                }
            }
        }

        // Instruments/synths list slider
        if(gui_action == GUI_CMD_SET_INSTR_SYNTH_LIST_SLIDER)
        {
            Instrs_ykar = Mouse.y - 72;
            Actualize_Instruments_Synths_List(0);
        }

        // Select an instrument/synth
        if(gui_action == GUI_CMD_SET_INSTR_SYNTH_LIST_SELECT)
        {
            ped_patsam = Instrs_index + (Mouse.y - 43) / 12;
            Actualize_Instruments_Synths_List(1);
            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        // --- Sequence ---------------------------------------

        if(gui_action == GUI_CMD_PREVIOUS_POSITION)
        {
            if(Songplaying)
            {
                cPosition_delay--;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition--;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_NEXT_POSITION)
        {
            if(Songplaying)
            {
                cPosition_delay++;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition++;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_SET_PATTERN_LENGTH)
        {
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_PREV_PATT)
        {
            if(Songplaying)
            {
                pSequence[cPosition_delay]--;
            }
            else
            {
                pSequence[cPosition]--;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
            if(Songplaying)
            {
                Anat(cPosition_delay);
            }
            else
            {
                Anat(cPosition);
            }
        }

        if(gui_action == GUI_CMD_NEXT_PATT)
        {
            if(Songplaying)
            {
                pSequence[cPosition_delay]++;
            }
            else
            {
                pSequence[cPosition]++;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
            if(Songplaying)
            {
                Anat(cPosition_delay);
            }
            else
            {
                Anat(cPosition);
            }
        }

        if(gui_action == GUI_CMD_REDUCE_SONG_LENGTH)
        {
            sLength--;
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_INCREASE_SONG_LENGTH)
        {
            sLength++;
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_UPDATE_SEQUENCER)
        {
            Actualize_Sequencer();
        }

        if(gui_action == GUI_CMD_SET_FOCUS_TRACK)
        {
            Actualize_Track_Ed(0);
            Actualize_Track_Fx_Ed(0);
        }

        if(gui_action == GUI_CMD_LOWER_OCTAVE)
        {
            ped_patoct--;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_HIGHER_OCTAVE)
        {
            ped_patoct++;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_DECREASE_STEP_ADD)
        {
            ped_pattad--;
            if(ped_pattad < 0) ped_pattad = 0;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_INCREASE_STEP_ADD)
        {
            ped_pattad++;
            if(ped_pattad > 16) ped_pattad = 16;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_PREV_INSTR)
        {
            ped_patsam--;

            if(snamesel == 2) snamesel = 0;

            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_NEXT_INSTR)
        {
            ped_patsam++;

            if(snamesel == 2) snamesel = 0;

            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_SWITCH_TRACK_MUTE_STATE)
        {
            int tmp_track = Get_Track_Over_Mouse();

            if(CHAN_MUTE_STATE[tmp_track] == 0) CHAN_MUTE_STATE[tmp_track] = 1;
            else CHAN_MUTE_STATE[tmp_track] = 0;

            if(userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(10);
            Actupated(0);
        }

        if(gui_action == GUI_CMD_SWITCH_TRACK_ZOOM_STATE)
        {
            int tmp_track = Get_Track_Over_Mouse();

            Toggle_Track_Zoom(tmp_track);

            // Too big to be zoomed
            if(Is_Track_Zoomed(tmp_track) && Channels_MultiNotes[ped_track] > 11)
            {
                Toggle_Track_Zoom(tmp_track);
            }
            Actupated(0);
        }

        if(gui_action == GUI_CMD_PLAY_SONG)
        {
            SongPlay();
        }

        if(gui_action == GUI_CMD_STOP_SONG)
        {
            is_recording = 0;
            is_recording_2 = 0;
            Nbr_Sub_NoteOff = 0;
            is_editing = 0;
            StartEdit();
            SongStop();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_RECORD_303)
        {
            StartRec();
        }

        if(gui_action == GUI_CMD_EDIT_MODE)
        {
            is_recording = 0;
            is_recording_2 = 0;
            Nbr_Sub_NoteOff = 0;
            SongStop();
            Actupated(0);
            StartEdit();
        }

        if(gui_action == GUI_CMD_RECORD_MODE)
        {
            SongStop();
            is_recording ^= 1;
            is_recording_2 = 0;
            Nbr_Sub_NoteOff = 0;
            is_editing = 0;
            key_record_first_time = TRUE;
            Clear_Midi_Channels_Pool();
            Actupated(0);
            StartEdit();
        }

        if(gui_action == GUI_CMD_CHANGE_BPM_TICKS_NBR)
        {
            Display_Beat_Time();
            Actualize_Master(teac);
        }

        if(gui_action == GUI_CMD_CHANGE_TRACKS_NBR)
        {
            Actualize_Master(teac);
            //Set_Track_Slider(gui_track);
            Actupated(0);
            Draw_Scope();
            Display_Tracks_To_Render();
        }

        if(gui_action == GUI_CMD_SET_TRACK_CUTOFF_FREQ)
        {
            TCut[ped_track] = float(Mouse.x - 88);
            Actualize_Track_Ed(1);
            liveparam = LIVE_PARAM_TRACK_CUTOFF;
            livevalue = (Mouse.x - 88) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_RESONANCE)
        {
            FRez[ped_track] = Mouse.x - 88;
            Actualize_Track_Ed(2);
            liveparam = LIVE_PARAM_TRACK_RESONANCE;
            livevalue = (Mouse.x - 88) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_INERTIA)
        {
            ICut[ped_track] = (Mouse.x - 88.0f) * 0.00006103515625f;
            Actualize_Track_Ed(3);
        }

        if(gui_action == GUI_CMD_UPDATE_TRACK_ED)
        {
            Actualize_Track_Ed(teac);
        }

        if(gui_action == GUI_CMD_SET_TRACK_THRESHOLD)
        {
            DThreshold[ped_track] = float((Mouse.x - 318) * 512);
            Actualize_Track_Ed(7);
            liveparam = LIVE_PARAM_TRACK_THRESHOLD;
            livevalue = (Mouse.x - 318) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_CLAMP)
        {
            DClamp[ped_track] = float((Mouse.x - 318) * 512);
            Actualize_Track_Ed(8);
            liveparam = LIVE_PARAM_TRACK_CLAMP;
            livevalue = (Mouse.x - 318) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_REVERB_SEND)
        {
            DSend[ped_track] = float(((float) Mouse.x - 318) / 128.0f);
            Actualize_Track_Ed(5);
            liveparam = LIVE_PARAM_TRACK_REVERB_SEND;
            livevalue = (Mouse.x - 318) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_PANNING)
        {
            TPan[ped_track] = ((float) Mouse.x - 318) / 128.0f;
            Actualize_Track_Ed(9);
            liveparam = LIVE_PARAM_TRACK_PANNING;
            livevalue = Mouse.x - 318;
        }

        // Tabs select
        if(gui_action == GUI_CMD_SELECT_DISKIO)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_DISKIO_EDIT;
            Draw_DiskIO_Ed(); 
            Actualize_DiskIO_Ed(0);
        }

        if(gui_action == GUI_CMD_SELECT_TRACK_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_TRACK_EDIT;
            Draw_Track_Ed();
            Actualize_Track_Ed(0);
        }

        if(gui_action == GUI_CMD_SELECT_INSTRUMENT_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_INSTRUMENT_EDIT;
            Draw_Instrument_Ed();
            Actualize_Instrument_Ed(2, 0);
        }

        if(gui_action == GUI_CMD_SELECT_FX_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_FX_SETUP_EDIT;
            Draw_Fx_Ed();
            Actualize_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_SELECT_SEQUENCER)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SEQUENCER;
            Draw_Sequencer_Ed();
            Actualize_Seq_Ed();
        }

        if(gui_action == GUI_CMD_SELECT_SCREEN_SETUP_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SETUP_EDIT;
            Draw_Master_Ed();
            Actualize_Master_Ed(0);
        }

        if(gui_action == GUI_CMD_SELECT_SYNTH_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SYNTH_EDIT;
            Draw_Synth_Ed();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_SELECT_TRACK_FX_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_TRACK_FX_EDIT;
            Draw_Track_Fx_Ed();
            Actualize_Track_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_SELECT_SAMPLE_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SAMPLE_EDIT;
            Draw_Sample_Ed();
            Actualize_Wave_Ed(0);
        }

        if(gui_action == GUI_CMD_SELECT_TB303_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_TB303_EDIT;
            Draw_303_Ed();
        }

        if(gui_action == GUI_CMD_SAVE_INSTRUMENT)
        {
            SaveInst();
        }

        if(gui_action == GUI_CMD_MODULE_INFOS)
        {
            ShowInfo();
        }

        if(gui_action == GUI_CMD_REFRESH_TB303_PARAMS)
        {
            Actualize_303_Ed(teac);
        }

        if(gui_action == GUI_CMD_SAVE_303_PATTERN)
        {
            Save303();
        }

        if(gui_action == GUI_CMD_FILELIST_SCROLL)
        {
            Actualize_Files_List(1);
        }

        if(gui_action == GUI_CMD_INSTR_SYNTH_SCROLL)
        {
            Actualize_Instruments_Synths_List(1);
        }

        if(gui_action == GUI_CMD_UPDATE_LOOP_EDITOR_ED)
        {
            Actualize_Instrument_Ed(1, teac);
        }

        if(gui_action == GUI_CMD_SAVE_MODULE)
        {
            Pack_Module(name);
        }

        // Save final mod
        if(gui_action == GUI_CMD_SAVE_FINAL)
        {
            SaveMod(name, TRUE, SAVE_WRITE, NULL);
        }

        // Calculate packed mod size
        if(gui_action == GUI_CMD_CALC_FINAL)
        {
            Final_Mod_Length = TestMod();
            outlong(254, 506, Final_Mod_Length, 7);
        }

        if(gui_action == GUI_CMD_SET_INSTRUMENT_AMPLI)
        {
            SampleVol[ped_patsam][ped_split] = float((Mouse.x - 436) / 32.0f);
            Actualize_Instrument_Ed(0, 1);
        }

        if(gui_action == GUI_CMD_SET_INSTRUMENT_FINETUNE)
        {
            FineTune_Value = ((Mouse.x - 436) - 64) * 2;
            if(FineTune_Value > 127) FineTune_Value = 127;
            if(FineTune_Value < -127) FineTune_Value = -127;
            Finetune[ped_patsam][ped_split] = FineTune_Value;
            Actualize_Instrument_Ed(0, 2);
        }

        if(gui_action == GUI_CMD_SET_INSTRUMENT_DECAY)
        {
            FDecay[ped_patsam][ped_split] = float(Mouse.x - 62) / 8192.0f;
            Actualize_Instrument_Ed(0, 3);
        }

        if(gui_action == GUI_CMD_UPDATE_INSTRUMENT_ED)
        {
            Actualize_Instrument_Ed(0, teac);
        }

        if(gui_action == GUI_CMD_UPDATE_DISKIO_ED)
        {
            Actualize_DiskIO_Ed(0);
        }

        if(gui_action == GUI_CMD_NEW_MODULE)
        {
            Newmod();
        }

        if(gui_action == GUI_CMD_UPDATE_SEQ_ED)
        {
            Actualize_Seq_Ed();
        }

        if(gui_action == GUI_CMD_UPDATE_FX_ED)
        {
            Actualize_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_UPDATE_SETUP_ED)
        {
            Actualize_Master_Ed(teac);
        }

        if(gui_action == GUI_CMD_UPDATE_SYNTH_ED)
        {
            Actualize_Synth_Ed(teac);
        }

        if(gui_action == GUI_CMD_MIDI_NOTE_OFF_1_TRACK)
        {
            mess_box("Notes Off command sent to this track...");
        }

        if(gui_action == GUI_CMD_MIDI_NOTE_OFF_ALL_TRACKS)
        {
            mess_box("Notes Off command sent to all tracks...");
        }

        if(gui_action == GUI_CMD_UPDATE_TRACK_FX_ED)
        {
            Actualize_Track_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_UPDATE_MIDI_303_ED)
        {
            Actualize_303_Ed(teac);
        }

        if(gui_action == GUI_CMD_PREVIOUS_16_INSTR)
        {
            ped_patsam -= 16;
            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_NEXT_16_INSTR)
        {
            ped_patsam += 16;
            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_UPDATE_PATTERN_ED)
        {
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_REDUCE_POSITIONS_10)
        {
            if(Songplaying)
            {
                cPosition_delay -= 10;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition -= 10;
                Bound_Patt_Pos();
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_INCREASE_POSITIONS_10)
        {
            if(Songplaying)
            {
                cPosition_delay += 10;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition += 10;
                Bound_Patt_Pos();
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_REDUCE_PATTERNS_10)
        {
            if(Songplaying)
            {
                if(pSequence[cPosition_delay] > 9) pSequence[cPosition_delay] -= 10;
                else pSequence[cPosition_delay] = 0;
                Anat(cPosition_delay);
            }
            else
            {
                if(pSequence[cPosition] > 9) pSequence[cPosition] -= 10;
                else pSequence[cPosition] = 0;
                Bound_Patt_Pos();          
                Anat(cPosition);
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_INCREASE_PATTERNS_10)
        {
            if(Songplaying)
            {
                if(pSequence[cPosition_delay] < 118) pSequence[cPosition_delay] += 10;
                else pSequence[cPosition_delay] = 127;

                Anat(cPosition_delay);
            }
            else
            {
                if(pSequence[cPosition] < 118) pSequence[cPosition] += 10;
                else pSequence[cPosition] = 127;
                Bound_Patt_Pos();          
                Anat(cPosition);
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_RENDER_WAV)
        {
            WavRenderizer();
        }

        if(gui_action == GUI_CMD_TIMED_REFRESH_SEQUENCER)
        {
            Actualize_Sequencer();
            if(Songplaying) player_pos = cPosition_delay;
            else player_pos = cPosition;
        }

        if(gui_action == GUI_CMD_DELETE_INSTRUMENT)
        {
            DeleteInstrument();
        }

        if(gui_action == GUI_CMD_SAVE_SYNTH)
        {
            SaveSynth();
        }

        if(gui_action == GUI_CMD_PATTERNS_POOL_EXHAUSTED)
        {
            mess_box("Maximum number of patterns reached.");
        }

        if(gui_action == GUI_CMD_REFRESH_WAVE_ED)
        {
            Actualize_Wave_Ed(teac);
        }

        if(gui_action == GUI_CMD_ASK_EXIT)
        {
            Asking_Exit = TRUE;
            Draw_Ask_Exit();
        }

        if(gui_action == GUI_CMD_EXIT)
        {
            SongStop();
            mess_box("Seppuku...");
            return(FALSE);
        }

        gui_action = GUI_CMD_NOP;
    }

    // Draw the main windows layout
    if(redraw_everything == 1)
    {
        if(!display_title)
        {
            SetColor(LOGO_BKCOL);
            Fillrect(0, 0, 800, 600);
            Copy(LOGOPIC, (800 - LOGO_WIDTH) / 2, (600 - LOGO_HEIGHT) / 2, 0, 0, LOGO_WIDTH, LOGO_HEIGHT);
        }
        else
        {
            last_index = -1;
            Gui_Draw_Button_Box(MIN_VUMETER - 4, 6, (MAX_VUMETER - MIN_VUMETER) + 6, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);

            Display_Master_Comp();
            Display_Master_Volume();
            Display_Shuffle();

            Draw_Scope_Files_Button();

            mess_box(tipoftheday);
            Gui_Draw_Button_Box(0, 178, fsize, 4, "", BUTTON_NORMAL | BUTTON_DISABLED);

            Gui_Draw_Button_Box(0, 24, 96, 98, "Player", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 44, 80, 16, "Play Sng./Patt.", BUTTON_NORMAL);
            Gui_Draw_Button_Box(8, 62, 80, 16, "Stop     ", BUTTON_NORMAL);
            StartRec();
            StartEdit();

            Gui_Draw_Button_Box(98, 24, 156, 98, "Sequence", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 44, 80, 16, "Position", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 62, 80, 16, "Pattern", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 80, 80, 16, "Song Length", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 98, 80, 16, "Pattern Lines", BUTTON_NORMAL | BUTTON_DISABLED);

            Gui_Draw_Button_Box(256, 24, 136, 98, "Song Settings", BUTTON_NORMAL | BUTTON_DISABLED);

            Gui_Draw_Button_Box(262, 44, 60, 16, "Tracks", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(262, 62, 60, 16, "Beats/Min.", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(262, 80, 60, 16, "Ticks/Beat", BUTTON_NORMAL | BUTTON_DISABLED);
            Display_Beat_Time();

            //Set_Track_Slider(gui_track);

            NewWav();

            Gui_Draw_Button_Box(0, 124, 392, 52, "", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 152, 80, 16, "Step Add", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(166, 152, 90, 16, "Keyboard Octave", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 134, 80, 16, "Instrument", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(320, 134, 64, 16, "Delete", BUTTON_NORMAL);

            Refresh_UI_Context();

            Actualize_Files_List(0);

            // Vu meters background
            SetColor(COL_BACKGROUND);
            Fillrect(MIN_VUMETER - 1, 9, MAX_VUMETER, 20);

            Actupated(0);
        }
    }

    if(Songplaying && ped_line_delay != player_line)
    {
        if(!sr_isrecording) Actualize_Track_Ed(0);
        Actupated(0);
        player_line = ped_line_delay;
    }

    // Checking for mouse and keyboard events ------------------------------------
    Mouse_Handler();
    Keyboard_Handler();

    // Refresh the sequencer each time the song position is different
    if(Songplaying)
    {
        if(player_pos != cPosition_delay)
        {
            gui_action = GUI_CMD_TIMED_REFRESH_SEQUENCER;
        }
    }

    return(TRUE);
}

// ------------------------------------------------------
// Allocate space for a waveform
void AllocateWave(int n_index, long lenfir, int samplechans)
{
    // Freeing if memory was allocated before -----------------------
    if(SampleType[n_index][ped_split] != 0)
    {
        if(RawSamples[n_index][0][ped_split]) free(RawSamples[n_index][0][ped_split]);
        RawSamples[n_index][0][ped_split] = NULL;
        if(SampleChannels[n_index][ped_split] == 2)
        {
            if(RawSamples[n_index][1][ped_split]) free(RawSamples[n_index][1][ped_split]);
            RawSamples[n_index][1][ped_split] = NULL;
        }
    }

    SampleType[n_index][ped_split] = 1;
    // Gsm by default

#if !defined(__NO_CODEC__)
    SampleCompression[n_index] = SMP_PACK_GSM;
#else
    SampleCompression[n_index] = SMP_PACK_NONE;
#endif

    SampleChannels[n_index][ped_split] = samplechans;
    RawSamples[n_index][0][ped_split] = (short *) malloc(lenfir * 2);

    if(samplechans == 2) RawSamples[n_index][1][ped_split] = (short *) malloc(lenfir * 2);    

    LoopStart[n_index][ped_split] = 0;
    LoopEnd[n_index][ped_split] = lenfir;
    LoopType[n_index][ped_split] = SMP_LOOP_NONE;
    SampleNumSamples[n_index][ped_split] = lenfir;
    Finetune[n_index][ped_split] = 0;
    SampleVol[n_index][ped_split] = 1.0f;
}

// ------------------------------------------------------
// Attempt to load any supported file format
void LoadFile(int Freeindex, const char *str)
{
    long Datalen = 0;
    int ld0 = 0;
    int ld1 = 0;
    int ld2 = 0;
    int ld3 = 0;
    int Freeindex2 = 0;
    FILE *in;
    WaveFile Wav_File;

    int rate = 0;
    int bits = 0;
    short *csamples, *csamples2;
    char st_type = 0;
    short inx = 0;
    int fmtchklen = 0;

    const char *Wavfile = str;

    if(Wavfile != NULL && (in = fopen(Wavfile, "rb")) != NULL)
    {
        char extension[10];
        unsigned extension_New;
        int modext;
        int found_mod;
        int i;
        //char rebext[5];

        fseek(in, 1080, SEEK_SET);
        fread(&modext, sizeof(char), 4, in);

        fseek(in, 0, SEEK_SET);
        fread(extension, sizeof(char), 9, in);

        fseek(in, 0, SEEK_SET);
        fread(&extension_New, sizeof(char), 4, in);

        found_mod = 0;
        for(i = 0; i < sizeof(mt_tags) / sizeof(int); i++)
        {
            if(Swap_32(mt_tags[i]) == modext)
            {
                found_mod = mt_channels[i];
                break;
            }
        }

        if(found_mod)
        {
            sprintf(name, "%s", Wavfile);
            // name / number of channels
            LoadAmigaMod(name, found_mod);
            NewWav();
            fclose(in);
            gui_action = GUI_CMD_NONE;
            Actualize_DiskIO_Ed(0);
            return;
        }

        //fseek(in, 8, SEEK_SET);
        //fread(rebext, sizeof(char), 4, in);

        if(strcmp(extension, "TWNNINS0") == 0 ||
           strcmp(extension, "TWNNINS1") == 0 ||
           strcmp(extension, "TWNNINS2") == 0 ||
           strcmp(extension, "TWNNINS3") == 0 ||
           strcmp(extension, "TWNNINS4") == 0)
        {
           sprintf(instrname, "%s", Wavfile);
            LoadInst(instrname);
            NewWav();
        }
        else if(strcmp(extension, "TWNNSNG2") == 0 ||
                strcmp(extension, "TWNNSNG3") == 0 ||
                strcmp(extension, "TWNNSNG4") == 0 ||
                strcmp(extension, "TWNNSNG5") == 0 ||
                strcmp(extension, "TWNNSNG6") == 0 ||
                strcmp(extension, "TWNNSNG7") == 0 ||
                strcmp(extension, "TWNNSNG8") == 0 ||
                strcmp(extension, "TWNNSNG9") == 0 ||
                strcmp(extension, "TWNNSNGA") == 0 ||
                strcmp(extension, "TWNNSNGB") == 0 ||
                strcmp(extension, "TWNNSNGC") == 0 ||
                extension_New == 'KTRP')
        {
            sprintf(name, "%s", Wavfile);
            LoadMod(name);
            NewWav();
        }
        else if(strcmp(extension, "TWNNSYN0") == 0 ||
                strcmp(extension, "TWNNSYN1") == 0)
        {
            sprintf(synthname, "%s", Wavfile);
            LoadSynth(synthname);
        }
        else if(strcmp(extension, "TWNN3030") == 0 ||
                strcmp(extension, "TWNN3031") == 0)
        {
            sprintf(name303, "%s", Wavfile);
            Load303(name303);
        }
        else
        {
            mess_box("Attempting of loading a wav file...");   

            // We need the length
            if(Wav_File.OpenForRead(Wavfile) == DDC_SUCCESS)
            {
                int bits = Wav_File.BitsPerSample();
                int channels = Wav_File.NumChannels();
                if(channels != 1 && channels != 2)
                {
                    mess_box("Protrekkr can only load mono or stereo samples.");
                }
                else
                {
                    if(bits != 8 &&
                       bits != 12 &&
                       bits != 16 &&
                       bits != 24 &&
                       bits != 32 &&
                       bits != 64)
                    {
                        mess_box("Protrekkr can only load 8, 12, 16, 24, 32 or 64 bits samples.");
                    }
                    else
                    {
                        switch(channels)
                        {
                            case 1:
                                AllocateWave(Freeindex, Wav_File.NumSamples(), 1);
                                csamples = RawSamples[Freeindex][0][ped_split];
                                for(i = 0; i < Wav_File.NumSamples(); i++)
                                {
                                    Wav_File.ReadMonoSample(&csamples[i]);
                                }
                                break;

                            case 2:
                                AllocateWave(Freeindex, Wav_File.NumSamples(), 2);
                                csamples = RawSamples[Freeindex][0][ped_split];
                                csamples2 = RawSamples[Freeindex][1][ped_split];
                                for(i = 0; i < Wav_File.NumSamples(); i++)
                                {
                                    Wav_File.ReadStereoSample(&csamples[i], &csamples2[i]);
                                }
                                break;
                        }
                        LoopType[Freeindex][ped_split] = Wav_File.LoopType();
                        LoopStart[Freeindex][ped_split] = Wav_File.LoopStart();
                        LoopEnd[Freeindex][ped_split] = Wav_File.LoopEnd();

                        sprintf(SampleName[Freeindex][ped_split], "%s", Wavfile); 
                        Actualize_Patterned();
                        Actualize_Instrument_Ed(2, 0);
                        NewWav();
                        switch(bits)
                        {
                            case 64:
                                mess_box("64 bit WAV PCM converted into 16 bit format.");
                                break;
                            case 32:
                                mess_box("32 bit WAV PCM converted into 16 bit format.");
                                break;
                            case 24:
                                mess_box("24 bit WAV PCM converted into 16 bit format.");
                                break;
                            case 12:
                                mess_box("12 bit WAV PCM converted into 16 bit format.");
                                break;
                            case 8:
                                mess_box("8 bit WAV PCM converted into 16 bit format.");
                                break;
                            default:
                                mess_box("16 bit WAV PCM loaded.");
                                break;
                        }
                    }
                }

                Wav_File.Close();
            }
            else
            {
                mess_box("Invalid file format. I accept only '.wav' '.ptk' '.ptp' '.pti' '.pts' or '.mod'.");
            }
        }
        fclose(in);
    }
    else
    {
        mess_box("File loading error. (Probably: file not found)"); 
    }
    gui_action = GUI_CMD_NONE;
    Actualize_DiskIO_Ed(0);
}

// ------------------------------------------------------
// Function to search for a free wave on board
int GetFreeWave(void)
{
    int inico = 0;
    int exiter = 0;
    int valuer = -1;

    do
    {
        if(SampleType[inico] == 0)
        {
            exiter = 1;
            valuer = inico;
        }
        inico++;
    }
    while(exiter == 0);

    return valuer;
}

// ------------------------------------------------------
// Start replaying
void SongPlay(void)
{
    SongStop();
    liveparam = 0;
    livevalue = 0;
    player_pos = -1;

    Post_Song_Init();
    Ptk_Play();

    if(plx == 0)
    {
        Gui_Draw_Button_Box(8, 44, 80, 16, "Playing Song", BUTTON_PUSHED);
        mess_box("Playing song...");
    }
    else
    {
        Gui_Draw_Button_Box(8, 44, 80, 16, "Playing Pattern", BUTTON_PUSHED);
        mess_box("Playing pattern...");
    }
}

// ------------------------------------------------------
// Turn live recording on/off
// "startrec" get it ? haha :(
void StartRec(void)
{
    liveparam = 0;
    livevalue = 0;
    if(sr_isrecording) Gui_Draw_Button_Box(8, 80, 80, 16, "Live Rec: ON", BUTTON_PUSHED);
    else Gui_Draw_Button_Box(8, 80, 80, 16, "Live Rec: OFF", BUTTON_NORMAL);
}

// ------------------------------------------------------
// Turn edit on/off
void StartEdit(void)
{
    if(is_editing && !is_recording)
    {
        Gui_Draw_Button_Box(8, 98, 80, 16, "Editing...", BUTTON_PUSHED);
    }
    if(is_recording)
    {
        Gui_Draw_Button_Box(8, 98, 80, 16, "Recording...", BUTTON_PUSHED);
    }
    if(!is_editing && !is_recording)
    {
        Gui_Draw_Button_Box(8, 98, 80, 16, "Edit/Record", BUTTON_NORMAL);
    }
}

// ------------------------------------------------------
// Stop replaying
void SongStop(void)
{
    Gui_Draw_Button_Box(8, 44, 80, 16, "Play Sng./Patt.", BUTTON_NORMAL);
    mess_box("Ready...");

    // Store the visual position (not the replayed one)
    if(Songplaying)
    {
        cPosition = cPosition_delay;
        ped_line = ped_line_delay;
    }

#if !defined(__NO_MIDI__)
    Midi_AllNotesOff();
#endif

    // Clear all midi channels
    Clear_Midi_Channels_Pool();

    Ptk_Stop();
}

// ------------------------------------------------------
// Create a new song
void Newmod(void)
{
    SongStop();
    Stop_Current_Sample();
    mess_box("Freeing all allocated buffers and restarting...");   

    Free_Samples();

    for(int api = 0; api < MAX_ROWS; api++)
    {
        patternLines[api] = 64;
    }

    Clear_Patterns_Pool();

    nPatterns = 1;

#if !defined(__NO_MIDI__)
    Midi_Reset();
#endif

    Set_Default_Channels_Polyphony();

    init_sample_bank();

    Pre_Song_Init();
    Post_Song_Init();

    sprintf(name, "Untitled");
    sprintf(artist, "Somebody");
    sprintf(style, "Goa Trance");
    namesize = 8;
    sLength = 1;
    BeatsPerMin = 125;
    TicksPerBeat = 4;
    SubCounter = 0;
    PosInTick = 0;
    DelayType = 1;
    player_line = 0;
    mas_vol = 1.0f;
    mas_comp_threshold = 100.0f;
    mas_comp_ratio = 0.0f;
    ped_track = 0;
    ped_patsam = 0;
    ped_col = 0;
    ped_line = 0;
    ped_line_delay = 0;
    cPosition = 0;
    cPosition_delay = 0;

    allow_save = TRUE;

    lchorus_feedback = 0.6f;
    rchorus_feedback = 0.5f;
    lchorus_delay = 10584;
    rchorus_delay = 15876;

    lchorus_counter = 44100;
    rchorus_counter = 44100;
    lchorus_counter2 = 44100 - lchorus_delay;
    rchorus_counter2 = 44100 - rchorus_delay;
    compressor = 0;
    c_threshold = 32;
    DelayType = 1;
    delay_time = 0;
    seditor = 0;

    Final_Mod_Length = 0;

    for(int spl = 0; spl < MAX_TRACKS; spl++)
    {
        CCoef[spl] = float((float) CSend[spl] / 127.0f);
    }
    Actualize_Sequencer();
    Actualize_Patterned();
    Actualize_Master(0);
    if(snamesel == 1 || snamesel == 4 || snamesel == 5) snamesel = 0;
    Actualize_DiskIO_Ed(0);

    Reset_Song_Length();
    Display_Song_Length();
    Draw_Scope();

    mess_box("New song started...");
    NewWav();
    Actupated(0);
    Reset_Tracks_To_Render();
    Refresh_UI_Context();
    
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

// ------------------------------------------------------
// Change a name
char *table_newletter[71] =
{
    NULL, // 0
    "%sa",
    "%sb",
    "%sc",
    "%sd",
    "%se",
    "%sf",
    "%sg",
    "%sh",
    "%si",
    "%sj", // 10
    "%sk",
    "%sl",
    "%sm",
    "%sn",
    "%so",
    "%sp",
    "%sq",
    "%sr",
    "%ss",
    "%st", // 20
    "%su",
    "%sv",
    "%sw",
    "%sx",
    "%sy",
    "%sz",
    "%s0",
    "%s1",
    "%s2",
    "%s3",
    "%s4",
    "%s5",
    "%s6",
    "%s7",
    "%s8",
    "%s9",
    NULL, // 37
    NULL, // 38
    NULL, // 39
    "%s.", // 40
    "%sA",
    "%sB",
    "%sC",
    "%sD",
    "%sE",
    "%sF",
    "%sG",
    "%sH",
    "%sI",
    "%sJ", // 50
    "%sK",
    "%sL",
    "%sM",
    "%sN",
    "%sO",
    "%sP",
    "%sQ",
    "%sR",
    "%sS",
    "%sT", // 60
    "%sU",
    "%sV",
    "%sW",
    "%sX",
    "%sY",
    "%sZ",
    "%s,", // 67
    NULL,  // 68
    "%s-", // 69
    NULL   // 70
};

// ------------------------------------------------------
// Change a letter in a string
void Actualize_Name(int *newletter, char *nam)
{
    int i;

    // Cancel input
    if(newletter[71])
    {
        newletter[71] = FALSE;
        strcpy(nam, cur_input_name);
        snamesel = 0;
        Keyboard_Nbr_Events = 0;
        return;
    }
    // Valid input
    if(newletter[39] && namesize > 0)
    {
        newletter[39] = FALSE;
        snamesel = 0;
        Keyboard_Nbr_Events = 0;
        return;
    }

    for(i = 0; i < 71; i++)
    {
        if(i == 37)
        {
            // Backspace
            if(newletter[i])
            {
                if(namesize > 0)
                {
                    nam[strlen(nam) - 1] = '\0';
                    if(namesize > 0) namesize--;
                }
                newletter[i] = FALSE;
            }
        }
        else
        {
            if(newletter[i])
            {
                if(namesize < 18)
                {
                    if(table_newletter[i]) sprintf(nam, table_newletter[i], nam);
                    namesize++;
                }
                newletter[i] = FALSE;
            }
        }
    }
}

// ------------------------------------------------------
// Render the song as a .wav file
void WavRenderizer(void)
{
    plx = 0;
    char buffer[80];
    sprintf(buffer, "%s.wav", name);
    int Save_CHAN_MUTE_STATE[MAX_TRACKS];
    int i;

    if(!hd_isrecording)
    {
        WaveFile RF;
        RF.OpenForWrite(buffer, 44100, rawrender_32float ? 32 : 16, 2);
        SongStop();
        SDL_Delay(500);
        sprintf(buffer, "Rendering module to '%s.wav' file. Please wait...", name);
        mess_box(buffer);
        SDL_Delay(500);

        ped_line = 0;
        ped_line_delay = 0;
        cPosition = 0;
        cPosition_delay = 0;

        long filesize = 0;
        char bru = FALSE;

        for(i = 0; i < MAX_TRACKS; i++)
        {
            Save_CHAN_MUTE_STATE[i] = CHAN_MUTE_STATE[i];
            CHAN_MUTE_STATE[i] = Tracks_To_Render[i];
        }

        SongPlay();

        while(cPosition > 0 || ped_line > 0 || bru == FALSE)
        {
            if(ped_line > 0) bru = TRUE;
            GetPlayerValues(mas_vol);
            if(rawrender_32float)
            {
                RF.WriteStereoFloatSample(left_float_render, right_float_render);
            }
            else
            {
                RF.WriteStereoSample(left_value, right_value);
            }
            filesize += 4;
        }

        for(i = 0; i < MAX_TRACKS; i++)
        {
            CHAN_MUTE_STATE[i] = Save_CHAN_MUTE_STATE[i];
        }

        RF.Close();
        SongStop();
        int minutes = filesize / 10584000;
        int seconds = (filesize - minutes * 10584000) / 176400;

        sprintf(buffer, "Wav rendering finished. File size: %.2f Megabytes. Playback time: %d'%d''.", float(filesize / 1048576.0f), minutes, seconds);
        mess_box(buffer);

        // Return to the start as all the values will be trashed anyway.
        ped_line = 0;
        ped_line_delay = 0;
        cPosition = 0;
        cPosition_delay = 0;
        Actualize_DiskIO_Ed(0);

        last_index = -1;
        Read_SMPT();
        Actualize_Files_List(0);

        mess_box(buffer);
        Actupated(0);
    }
    rawrender = FALSE;
}

// ------------------------------------------------------
// Delete an instrument
void DeleteInstrument(void)
{
    AUDIO_Stop();
    Gui_Draw_Button_Box(320, 134, 64, 16, "Delete", BUTTON_PUSHED);
    Stop_Current_Sample();
    SDL_Delay(256);

    beatsync[ped_patsam] = FALSE;
    beatlines[ped_patsam] = 16;
    sprintf(nameins[ped_patsam], "Untitled");
    ResetSynthParameters(&PARASynth[ped_patsam]);
    KillInst(ped_patsam);
    mess_box("Instrument deleted.");
    Gui_Draw_Button_Box(320, 134, 64, 16, "Delete", BUTTON_NORMAL);
    RefreshSample();
    NewWav();
    AUDIO_Play();
    Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
    Actualize_Patterned();
}

void Stop_Current_Sample(void)
{
    int i;

    for(char u = 0; u < MAX_TRACKS; u++)
    {
        for(i = 0; i < MAX_POLYPHONY; i++)
        {
            if(sp_channelsample[u][i] == ped_patsam)
            {
                sp_Stage[u][i] = PLAYING_NOSAMPLE;
            }
        }
        Player_FD[u] = 0;
        ramper[u] = 0;
    }
}

// ------------------------------------------------------
// Refresh the view of a sample
void RefreshSample(void)
{
    seditor = 0;
    ped_split = 0;
    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT)
    {
        Draw_Instrument_Ed();
        Actualize_Instrument_Ed(2, 0);
    }
}

// ------------------------------------------------------
// Reset the channels polyphonies  to their default state
void Set_Default_Channels_Polyphony(void)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        Channels_Polyphony[i] = DEFAULT_POLYPHONY;
    }
}

// ------------------------------------------------------
// Show song informations (summing instruments and patterns)
void ShowInfo(void)
{
   char tmp[256];
   int pattsize = nPatterns * PATTERN_LEN;
   int sampsize = 0;

   for(int pp = 0; pp < 128; pp++)
   {
      for(int z = 0; z < 16; z++)
      {
         if(SampleType[pp][z] != 0)
         {
            sampsize += SampleChannels[pp][z] * SampleNumSamples[pp][z];   
         }
      }
   }
   sprintf(tmp, "Sample bank size: %d bytes, pattern bank [%d patterns] size: %d bytes.", sampsize, nPatterns, pattsize);
   mess_box(tmp);
}

// ------------------------------------------------------
// Function called each second mainly for autosaving purposes
Uint32 Timer_CallBack(Uint32 interval, void *param)
{
    if(!display_title)
    {
        wait_title++;
        if(wait_title > 6) Remove_Title();
    }

    if(display_title)
    {
        if(AutoSave)
        {
            wait_AutoSave++;
            // Autosave module
            if(wait_AutoSave > Values_AutoSave[AutoSave])
            {
                strcpy(autosavename, name);
                strcat(autosavename, "_bak");
                Pack_Module(autosavename);
            }
        }
    }

    return(interval);
}

// ------------------------------------------------------
// Validate or cancel an user input
void Actualize_Input(void)
{
    switch(snamesel)
    {
        case 1:
            Actualize_Name(retletter, name);
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        case 2:
            Actualize_Name(retletter, nameins[ped_patsam]);
            gui_action = GUI_CMD_UPDATE_PATTERN_ED;
            break;

        case 3:
            Actualize_Name(retletter, PARASynth[ped_patsam].presetname);
            teac = UPDATE_SYNTH_CHANGE_NAME;
            gui_action = GUI_CMD_UPDATE_SYNTH_ED;
            break;

        case 4:
            Actualize_Name(retletter, artist);
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        case 5:
            Actualize_Name(retletter, style);
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        case 6:
            Actualize_Name(retletter, tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
            teac = 18;
            gui_action = GUI_CMD_UPDATE_MIDI_303_ED;
            break;
    }
}

// ------------------------------------------------------
// Return the channel and sub channel of a played note
LPJAZZ_KEY Get_Jazz_Key_Off(JAZZ_KEY Pool[MAX_TRACKS][MAX_POLYPHONY], int Note)
{
    int i;
    int j;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        for(j = 0; j < MAX_POLYPHONY; j++)
        {
            if(Pool[i][j].Note == Note)
            {
                return(&Pool[i][j]);
            }
        }
    }
    return(NULL);
}

// ------------------------------------------------------
// Return the channel and sub channel of a played note
int Discard_Key_Note_Off(JAZZ_KEY Pool[MAX_TRACKS][MAX_POLYPHONY], int Channel, int Sub_Channel)
{
    int i;
    int j;
    int discarded = 0;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        for(j = 0; j < MAX_POLYPHONY; j++)
        {
            if(Pool[i][j].Channel == Channel && Pool[i][j].Sub_Channel == Sub_Channel)
            {
                Pool[i][j].Sub_Channel = 0;
                Pool[i][j].Channel = 0;
                Pool[i][j].Note = 0;
                discarded++;
            }
        }
    }
    return(discarded);
}

int Raw_Keys_to_Send[] =
{
    0x10,
    3,
    0x11,
    4,
    0x12,
    0x13,
    6,
    0x14,
    7,
    0x15,
    8,
    0x16,
    0x17,
    0xa,
    0x18,
    0xb,
    0x19,
    0x1a,
    0xd,
    0x1b,
    0x2c,
    0x1f,
    0x2d,
    0x20,
    0x2e,
    0x2f,
    0x22,
    0x30,
    0x23,
    0x31,
    0x24,
    0x32,
    0x33,
    0x26,
    0x34,
    0x27,
    0x35
};

// ------------------------------------------------------
// KeyBoard ShortCut Handler
void Keyboard_Handler(void)
{
    int Cur_Position;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    // Exit tracker
    if(Get_LAlt() && Keys[SDLK_F4]) gui_action = GUI_CMD_EXIT;
    // Lower octave
    if(!Get_LShift() && Keys[SDLK_F1]) gui_action = GUI_CMD_LOWER_OCTAVE;
    // Higher octave
    if(!Get_LShift() && Keys[SDLK_F2]) gui_action = GUI_CMD_HIGHER_OCTAVE;

    if(Get_LShift())
    {
        if(Keys[SDLK_F1])
        {
            gui_action = GUI_CMD_SELECT_SEQUENCER;
        }
        if(Keys[SDLK_F2])
        {
            gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
        }
        if(Keys[SDLK_F3])
        {
            gui_action = GUI_CMD_SELECT_SYNTH_EDIT;
        }
        if(Keys[SDLK_F4])
        {
            gui_action = GUI_CMD_SELECT_SAMPLE_EDIT;
        }
        if(Keys[SDLK_F5])
        {
            gui_action = GUI_CMD_SELECT_TB303_EDIT;
        }
        if(Keys[SDLK_F6])
        {
            gui_action = GUI_CMD_SELECT_TRACK_EDIT;
        }
        if(Keys[SDLK_F7])
        {
            gui_action = GUI_CMD_SELECT_TRACK_FX_EDIT;
        }
        if(Keys[SDLK_F8])
        {
            gui_action = GUI_CMD_SELECT_FX_EDIT;
        }
        if(Keys[SDLK_F9])
        {
            gui_action = GUI_CMD_SELECT_DISKIO;
        }
        if(Keys[SDLK_F10])
        {
            gui_action = GUI_CMD_SELECT_SCREEN_SETUP_EDIT;
        }
    }

    // Previous track
    if(Get_LShift() && !Get_LCtrl() && Keys[SDLK_TAB])
    {
        Unselect_Selection();
        ped_track--;
        ped_col = 0;
        Actupated(0);
        gui_action = GUI_CMD_SET_FOCUS_TRACK;
    }

    // Next track
    if(!Get_LShift() && !Get_LCtrl() && Keys[SDLK_TAB])
    {
        Unselect_Selection();
        ped_track++;
        ped_col = 0;
        Actupated(0);
        gui_action = GUI_CMD_SET_FOCUS_TRACK;
    }

    // Next note
#if defined(__WIN32__)
    if(!Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB] && !Key_Unicode)
#endif
#if defined(__LINUX__)
    if(!Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB] && Keys_Sym[SDLK_TAB])
#else
    if(!Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB])
#endif
    {
        Unselect_Selection();
        ped_col += Table_Right_Tab_Notes[ped_col];
        if(ped_col >= (Channels_MultiNotes[ped_track] * 3)) ped_col = 0;
        Actupated(0);
    }

    // Previous note
    if(Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB])
    {
        Unselect_Selection();
        ped_col -= Table_Left_Tab_Notes[ped_col];
        if(ped_col < 0) ped_col = (Channels_MultiNotes[ped_track] * 3) - 3;
        Actupated(0);
    }

    if(!is_recording)
    {
        if(!Get_LShift())
        {
            // Jump to row 0
            if(Keys[SDLK_F5] && ped_line != 0)
            {
                Goto_Row(0);
            }

            // Jump to row 16
            if(Keys[SDLK_F6] && ped_line != 16)
            {
                Goto_Row(16);
            }

            // Jump to row 32
            if(Keys[SDLK_F7] && ped_line != 32)
            {
                Goto_Row(32);
            }

            // Jump to row 48
            if(Keys[SDLK_F8] && ped_line != 48)
            {
                Goto_Row(48);
            }

            // Jump to row 63
            if(Keys[SDLK_F9] && ped_line != 63)
            {
                Goto_Row(63);
            }
        }

        // Top left of pattern
        if(Keys[SDLK_HOME])
        {
            Goto_Top_Left();
        }

        // Bottom right of pattern
        if(Keys[SDLK_END])
        {
            Goto_Bottom_Right();
        }

        // Previous column or previous track
        if(Keys[SDLK_LEFT] && !Get_LCtrl() && !Get_LAlt())
        {
            Goto_Previous_Column();
        }

        // Next column or next track
        if(Keys[SDLK_RIGHT] && !Get_LCtrl() && !Get_LAlt())
        {
            Goto_Next_Column();
        }

        // Previous row
        if(Keys[SDLK_UP] && !Songplaying)
        {
            Goto_Previous_Row();
        }

        // Next row
        if(Keys[SDLK_DOWN] && !Songplaying)
        {
            Goto_Next_Row();
        }

        // Previous page (16 rows)
        if(Keys[SDLK_PAGEUP] && !Songplaying)
        {
            Goto_Previous_Page();
        }

        // Next page (16 rows)
        if(Keys[SDLK_PAGEDOWN] && !Songplaying)
        {
            Goto_Next_Page();
        }

        // Previous pattern
        if((Keys_Sym[SDLK_KP_MINUS]) && pSequence[Cur_Position] > 0)
        {
            gui_action = GUI_CMD_PREV_PATT;
        }

        // Next pattern
        if((Keys_Sym[SDLK_KP_PLUS]) && pSequence[Cur_Position] < 254)
        {
            gui_action = GUI_CMD_NEXT_PATT;
        }

        if(Keys[SDLK_INSERT] && is_editing)
        {
            // INSERT
            if(Get_LShift()) Insert_Pattern_Line(Cur_Position);
            else Insert_Track_Line(ped_track, Cur_Position);
        }

        if(snamesel == 0)
        {
            if(Keys[SDLK_BACKSPACE] && is_editing)
            {
                // BACKSPACE
                if(ped_line)
                {
                    ped_line--;
                    if(Get_LShift()) Remove_Pattern_Line(Cur_Position);
                    else Remove_Track_Line(ped_track, Cur_Position);
                }
            }
        }
    }

    // Lower octave
    if((Keys_Sym[SDLK_KP_DIVIDE]))
    {
        gui_action = GUI_CMD_LOWER_OCTAVE;
        Keys_Sym[SDLK_KP_DIVIDE] = FALSE;
    }
    // Upper octave
    if((Keys_Sym[SDLK_KP_MULTIPLY]))
    {
        gui_action = GUI_CMD_HIGHER_OCTAVE;
        Keys_Sym[SDLK_KP_MULTIPLY] = FALSE;
    }

    int retnote = NOTE_OFF;
    int retnote_raw = NOTE_OFF;
    int retvalue = NOTE_OFF;
    int reelletter = FALSE;

    // -------------------------------------------
    // Editing a name
    if(snamesel > 0)
    {
        if(Keys[SDLK_a])
        {
            retletter[1] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_b])
        {
            retletter[2] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_c])
        {
            retletter[3] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_d])
        {
            retletter[4] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_e])
        {
            retletter[5] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_f])
        {
            retletter[6] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_g])
        {
            retletter[7] = TRUE;
            reelletter = TRUE;
        }     
        if(Keys[SDLK_h])
        {
            retletter[8] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_i])
        {
            retletter[9] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_j])
        {
            retletter[10] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_k])
        {
            retletter[11] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_l])
        {
            retletter[12] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_m])
        {
            retletter[13] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_n])
        {
            retletter[14] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_o])
        {
            retletter[15] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_p])
        {
            retletter[16] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_q])
        {
            retletter[17] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_r])
        {
            retletter[18] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_s])
        {
            retletter[19] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_t])
        {
            retletter[20] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_u])
        {
            retletter[21] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_v])
        {
            retletter[22] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_w])
        {
            retletter[23] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_x])
        {
            retletter[24] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_y])
        {
            retletter[25] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_z])
        {
            retletter[26] = TRUE;
            reelletter = TRUE;
        }

        if(Keys['A'])
        {
            retletter[41] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['B'])
        {
            retletter[42] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['C'])
        {
            retletter[43] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['D'])
        {
            retletter[44] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['E'])
        {
            retletter[45] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['F'])
        {
            retletter[46] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['G'])
        {
            retletter[47] = TRUE;
            reelletter = TRUE;
        }     
        if(Keys['H'])
        {
            retletter[48] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['I'])
        {
            retletter[49] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['J'])
        {
            retletter[50] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['K'])
        {
            retletter[51] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['L'])
        {
            retletter[52] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['M'])
        {
            retletter[53] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['N'])
        {
            retletter[54] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['O'])
        {
            retletter[55] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['P'])
        {
            retletter[56] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Q'])
        {
            retletter[57] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['R'])
        {
            retletter[58] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['S'])
        {
            retletter[59] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['T'])
        {
            retletter[60] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['U'])
        {
            retletter[61] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['V'])
        {
            retletter[62] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['W'])
        {
            retletter[63] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['X'])
        {
            retletter[64] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Y'])
        {
            retletter[65] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Z'])
        {
            retletter[66] = TRUE;
            reelletter = TRUE;
        }

        if(Keys[SDLK_KP0])
        {
            retletter[27] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['0'])
        {
            retletter[27] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP1])
        {
            retletter[28] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['1'])
        {
            retletter[28] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP2])
        {
            retletter[29] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['2'])
        {
            retletter[29] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP3])
        {
            retletter[30] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['3'])
        {
            retletter[30] = TRUE;
            reelletter = TRUE;
        }     
        if(Keys[SDLK_KP4])
        {
            retletter[31] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['4'])
        {
            retletter[31] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP5])
        {
            retletter[32] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['5'])
        {
            retletter[32] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP6])
        {
            retletter[33] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['6'])
        {
            retletter[33] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP7])
        {
            retletter[34] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['7'])
        {
            retletter[34] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP8])
        {
            retletter[35] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['8'])
        {
            retletter[35] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP9])
        {
            retletter[36] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['9'])
        {
            retletter[36] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_BACKSPACE])
        {
            retletter[37] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_SPACE])
        {
            retletter[38] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_RETURN])
        {
            retletter[39] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP_ENTER])
        {
            retletter[39] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_PERIOD])
        {
            retletter[40] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_COMMA])
        {
            retletter[67] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_SLASH])
        {
            retletter[68] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_MINUS])
        {
            retletter[69] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_EQUALS])
        {
            retletter[70] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_ESCAPE])
        {
            retletter[71] = TRUE;
            reelletter = TRUE;
        }
    }

    if(reelletter)
    {
        Actualize_Input();
    }
    // -------------------------------------------

    // ------------------------------------------
    if(!Keys[SDLK_MENU] && !Get_LAlt() && !Get_LCtrl() && !Get_LShift() && snamesel == 0 && !reelletter)
    {
        // Key jazz release
        if(Keys_Raw_Off[0x10]) { Record_Keys[0] = (12 + 1) | 0x80;  Keys_Raw_Off[0x10] = FALSE; Keys_Raw[0x10] = FALSE; }
        if(Keys_Raw_Off[3])    { Record_Keys[1] = (13 + 1) | 0x80;  Keys_Raw_Off[3] = FALSE; Keys_Raw[3] = FALSE; }
        if(Keys_Raw_Off[0x11]) { Record_Keys[2] = (14 + 1) | 0x80;  Keys_Raw_Off[0x11] = FALSE; Keys_Raw[0x11] = FALSE; }
        if(Keys_Raw_Off[4])    { Record_Keys[3] = (15 + 1) | 0x80;  Keys_Raw_Off[4] = FALSE; Keys_Raw[4] = FALSE; }
        if(Keys_Raw_Off[0x12]) { Record_Keys[4] = (16 + 1) | 0x80;  Keys_Raw_Off[0x12] = FALSE; Keys_Raw[0x12] = FALSE; }
        if(Keys_Raw_Off[0x13]) { Record_Keys[5] = (17 + 1) | 0x80;  Keys_Raw_Off[0x13] = FALSE; Keys_Raw[0x13] = FALSE; }
        if(Keys_Raw_Off[6])    { Record_Keys[6] = (18 + 1) | 0x80;  Keys_Raw_Off[6] = FALSE; Keys_Raw[6] = FALSE; }
        if(Keys_Raw_Off[0x14]) { Record_Keys[7] = (19 + 1) | 0x80;  Keys_Raw_Off[0x14] = FALSE; Keys_Raw[0x14] = FALSE; }
        if(Keys_Raw_Off[7])    { Record_Keys[8] = (20 + 1) | 0x80;  Keys_Raw_Off[7] = FALSE; Keys_Raw[7] = FALSE; }
        if(Keys_Raw_Off[0x15]) { Record_Keys[9] = (21 + 1) | 0x80;  Keys_Raw_Off[0x15] = FALSE; Keys_Raw[0x15] = FALSE; }
        if(Keys_Raw_Off[8])    { Record_Keys[10] = (22 + 1) | 0x80; Keys_Raw_Off[8] = FALSE; Keys_Raw[8] = FALSE; }
        if(Keys_Raw_Off[0x16]) { Record_Keys[11] = (23 + 1) | 0x80; Keys_Raw_Off[0x16] = FALSE; Keys_Raw[0x16] = FALSE; }
        if(Keys_Raw_Off[0x17]) { Record_Keys[12] = (24 + 1) | 0x80; Keys_Raw_Off[0x17] = FALSE; Keys_Raw[0x17] = FALSE; }
        if(Keys_Raw_Off[0xa])  { Record_Keys[13] = (25 + 1) | 0x80; Keys_Raw_Off[0xa] = FALSE; Keys_Raw[0xa] = FALSE; }
        if(Keys_Raw_Off[0x18]) { Record_Keys[14] = (26 + 1) | 0x80; Keys_Raw_Off[0x18] = FALSE; Keys_Raw[0x18] = FALSE; }
        if(Keys_Raw_Off[0xb])  { Record_Keys[15] = (27 + 1) | 0x80; Keys_Raw_Off[0xb] = FALSE; Keys_Raw[0xb] = FALSE; }
        if(Keys_Raw_Off[0x19]) { Record_Keys[16] = (28 + 1) | 0x80; Keys_Raw_Off[0x19] = FALSE; Keys_Raw[0x19] = FALSE; }
        if(Keys_Raw_Off[0x1a]) { Record_Keys[17] = (29 + 1) | 0x80; Keys_Raw_Off[0x1a] = FALSE; Keys_Raw[0x1a] = FALSE; }
        if(Keys_Raw_Off[0xd])  { Record_Keys[18] = (30 + 1) | 0x80; Keys_Raw_Off[0xd] = FALSE; Keys_Raw[0xd] = FALSE; }
        if(Keys_Raw_Off[0x1b]) { Record_Keys[19] = (31 + 1) | 0x80; Keys_Raw_Off[0x1b] = FALSE; Keys_Raw[0x1b] = FALSE; }
        if(Keys_Raw_Off[0x2c]) { Record_Keys[20] = (0 + 1) | 0x80;  Keys_Raw_Off[0x2c] = FALSE; Keys_Raw[0x2c] = FALSE; }
        if(Keys_Raw_Off[0x1f]) { Record_Keys[21] = (1 + 1) | 0x80;  Keys_Raw_Off[0x1f] = FALSE; Keys_Raw[0x1f] = FALSE; }
        if(Keys_Raw_Off[0x2d]) { Record_Keys[22] = (2 + 1) | 0x80;  Keys_Raw_Off[0x2d] = FALSE; Keys_Raw[0x2d] = FALSE; }
        if(Keys_Raw_Off[0x20]) { Record_Keys[23] = (3 + 1) | 0x80;  Keys_Raw_Off[0x20] = FALSE; Keys_Raw[0x20] = FALSE; }
        if(Keys_Raw_Off[0x2e]) { Record_Keys[24] = (4 + 1) | 0x80;  Keys_Raw_Off[0x2e] = FALSE; Keys_Raw[0x2e] = FALSE; }
        if(Keys_Raw_Off[0x2f]) { Record_Keys[25] = (5 + 1) | 0x80;  Keys_Raw_Off[0x2f] = FALSE; Keys_Raw[0x2f] = FALSE; }
        if(Keys_Raw_Off[0x22]) { Record_Keys[26] = (6 + 1) | 0x80;  Keys_Raw_Off[0x22] = FALSE; Keys_Raw[0x22] = FALSE; }
        if(Keys_Raw_Off[0x30]) { Record_Keys[27] = (7 + 1) | 0x80;  Keys_Raw_Off[0x30] = FALSE; Keys_Raw[0x30] = FALSE; }
        if(Keys_Raw_Off[0x23]) { Record_Keys[28] = (8 + 1) | 0x80;  Keys_Raw_Off[0x23] = FALSE; Keys_Raw[0x23] = FALSE; }
        if(Keys_Raw_Off[0x31]) { Record_Keys[29] = (9 + 1) | 0x80;  Keys_Raw_Off[0x31] = FALSE; Keys_Raw[0x31] = FALSE; }
        if(Keys_Raw_Off[0x24]) { Record_Keys[30] = (10 + 1) | 0x80; Keys_Raw_Off[0x24] = FALSE; Keys_Raw[0x24] = FALSE; }
        if(Keys_Raw_Off[0x32]) { Record_Keys[31] = (11 + 1) | 0x80; Keys_Raw_Off[0x32] = FALSE; Keys_Raw[0x32] = FALSE; }
        if(Keys_Raw_Off[0x33]) { Record_Keys[32] = (12 + 1) | 0x80; Keys_Raw_Off[0x33] = FALSE; Keys_Raw[0x33] = FALSE; }
        if(Keys_Raw_Off[0x26]) { Record_Keys[33] = (13 + 1) | 0x80; Keys_Raw_Off[0x26] = FALSE; Keys_Raw[0x26] = FALSE; }
        if(Keys_Raw_Off[0x34]) { Record_Keys[34] = (14 + 1) | 0x80; Keys_Raw_Off[0x34] = FALSE; Keys_Raw[0x34] = FALSE; }
        if(Keys_Raw_Off[0x27]) { Record_Keys[35] = (15 + 1) | 0x80; Keys_Raw_Off[0x27] = FALSE; Keys_Raw[0x27] = FALSE; }
        if(Keys_Raw_Off[0x35]) { Record_Keys[36] = (16 + 1) | 0x80; Keys_Raw_Off[0x35] = FALSE; Keys_Raw[0x35] = FALSE; }

        if(Keys[SDLK_z]) retnote = 0;
        if(Keys[SDLK_s]) retnote = 1;
        if(Keys[SDLK_x]) retnote = 2;
        if(Keys[SDLK_d]) retnote = 3;
        if(Keys[SDLK_c]) retnote = 4;
        if(Keys[SDLK_v]) retnote = 5;
        if(Keys[SDLK_g]) retnote = 6;
        if(Keys[SDLK_b]) retnote = 7;
        if(Keys[SDLK_h]) retnote = 8;
        if(Keys[SDLK_n]) retnote = 9;
        if(Keys[SDLK_j]) retnote = 10;
        if(Keys[SDLK_m]) retnote = 11;
        if(Keys[SDLK_COMMA]) retnote = 12;
        if(Keys[SDLK_l]) retnote = 13;
        if(Keys[SDLK_PERIOD]) retnote = 14;
        if(Keys[SDLK_SEMICOLON]) retnote = 15;
        if(Keys[SDLK_SLASH]) retnote = 16;
        if(Keys[SDLK_q]) retnote = 12;
        if(Keys[SDLK_z]) retnote = 13;
        if(Keys[SDLK_w]) retnote = 14;
        if(Keys[SDLK_3]) retnote = 15;
        if(Keys[SDLK_e]) retnote = 16;
        if(Keys[SDLK_r]) retnote = 17;
        if(Keys[SDLK_s]) retnote = 18;
        if(Keys[SDLK_t]) retnote = 19;
        if(Keys[SDLK_6]) retnote = 20;
        if(Keys[SDLK_y]) retnote = 21;
        if(Keys[SDLK_7]) retnote = 22;
        if(Keys[SDLK_u]) retnote = 23;
        if(Keys[SDLK_i]) retnote = 24;
        if(Keys[SDLK_9]) retnote = 25;
        if(Keys[SDLK_o]) retnote = 26;
        if(Keys[SDLK_0]) retnote = 27;
        if(Keys[SDLK_p]) retnote = 28;
        if(Keys[SDLK_DELETE])
        {
            retnote = -1;
            retnote_raw = -1;
        }
        if(Keys[SDLK_INSERT])
        {
            retnote = -2;
            retnote_raw = -2;
        }
        if(Keys[SDLK_RSHIFT])
        {
            retnote = -3;
            retnote_raw = -3;
        }

        // Key jazz
        if(Keys_Raw[0x10]) { retnote_raw = 12; Record_Keys[0] = 12 + 1; }
        if(Keys_Raw[3])    { retnote_raw = 13; Record_Keys[1] = 13 + 1; }
        if(Keys_Raw[0x11]) { retnote_raw = 14; Record_Keys[2] = 14 + 1; }
        if(Keys_Raw[4])    { retnote_raw = 15; Record_Keys[3] = 15 + 1; }
        if(Keys_Raw[0x12]) { retnote_raw = 16; Record_Keys[4] = 16 + 1; }
        if(Keys_Raw[0x13]) { retnote_raw = 17; Record_Keys[5] = 17 + 1; }
        if(Keys_Raw[6])    { retnote_raw = 18; Record_Keys[6] = 18 + 1; }
        if(Keys_Raw[0x14]) { retnote_raw = 19; Record_Keys[7] = 19 + 1; }
        if(Keys_Raw[7])    { retnote_raw = 20; Record_Keys[8] = 20 + 1; }
        if(Keys_Raw[0x15]) { retnote_raw = 21; Record_Keys[9] = 21 + 1; }
        if(Keys_Raw[8])    { retnote_raw = 22; Record_Keys[10] = 22 + 1; }
        if(Keys_Raw[0x16]) { retnote_raw = 23; Record_Keys[11] = 23 + 1; }
        if(Keys_Raw[0x17]) { retnote_raw = 24; Record_Keys[12] = 24 + 1; }
        if(Keys_Raw[0xa])  { retnote_raw = 25; Record_Keys[13] = 25 + 1; }
        if(Keys_Raw[0x18]) { retnote_raw = 26; Record_Keys[14] = 26 + 1; }
        if(Keys_Raw[0xb])  { retnote_raw = 27; Record_Keys[15] = 27 + 1; }
        if(Keys_Raw[0x19]) { retnote_raw = 28; Record_Keys[16] = 28 + 1; }
        if(Keys_Raw[0x1a]) { retnote_raw = 29; Record_Keys[17] = 29 + 1; }
        if(Keys_Raw[0xd])  { retnote_raw = 30; Record_Keys[18] = 30 + 1; }
        if(Keys_Raw[0x1b]) { retnote_raw = 31; Record_Keys[19] = 31 + 1; }
        if(Keys_Raw[0x2c]) { retnote_raw = 0; Record_Keys[20] = 0 + 1; }
        if(Keys_Raw[0x1f]) { retnote_raw = 1; Record_Keys[21] = 1 + 1; }
        if(Keys_Raw[0x2d]) { retnote_raw = 2; Record_Keys[22] = 2 + 1; }
        if(Keys_Raw[0x20]) { retnote_raw = 3; Record_Keys[23] = 3 + 1; }
        if(Keys_Raw[0x2e]) { retnote_raw = 4; Record_Keys[24] = 4 + 1; }
        if(Keys_Raw[0x2f]) { retnote_raw = 5; Record_Keys[25] = 5 + 1; }
        if(Keys_Raw[0x22]) { retnote_raw = 6; Record_Keys[26] = 6 + 1; }
        if(Keys_Raw[0x30]) { retnote_raw = 7; Record_Keys[27] = 7 + 1; }
        if(Keys_Raw[0x23]) { retnote_raw = 8; Record_Keys[28] = 8 + 1; }
        if(Keys_Raw[0x31]) { retnote_raw = 9; Record_Keys[29] = 9 + 1; }
        if(Keys_Raw[0x24]) { retnote_raw = 10; Record_Keys[30] = 10 + 1; }
        if(Keys_Raw[0x32]) { retnote_raw = 11; Record_Keys[31] = 11 + 1; }
        if(Keys_Raw[0x33]) { retnote_raw = 12; Record_Keys[32] = 12 + 1; }
        if(Keys_Raw[0x26]) { retnote_raw = 13; Record_Keys[33] = 13 + 1; }
        if(Keys_Raw[0x34]) { retnote_raw = 14; Record_Keys[34] = 14 + 1; }
        if(Keys_Raw[0x27]) { retnote_raw = 15; Record_Keys[35] = 15 + 1; }
        if(Keys_Raw[0x35]) { retnote_raw = 16; Record_Keys[36] = 16 + 1; }

        // Data columns
        if(Keys_Unicode['0']) retvalue = 0;
        if(Keys[SDLK_KP0]) retvalue = 0;
        if(Keys_Unicode['1']) retvalue = 1;
        if(Keys[SDLK_KP1]) retvalue = 1;
        if(Keys_Unicode['2']) retvalue = 2;
        if(Keys[SDLK_KP2]) retvalue = 2;
        if(Keys_Unicode['3']) retvalue = 3;
        if(Keys[SDLK_KP3]) retvalue = 3;
        if(Keys_Unicode['4']) retvalue = 4;
        if(Keys[SDLK_KP4]) retvalue = 4;
        if(Keys_Unicode['5']) retvalue = 5;
        if(Keys[SDLK_KP5]) retvalue = 5;
        if(Keys_Unicode['6']) retvalue = 6;
        if(Keys[SDLK_KP6]) retvalue = 6;
        if(Keys_Unicode['7']) retvalue = 7;
        if(Keys[SDLK_KP7]) retvalue = 7;
        if(Keys_Unicode['8']) retvalue = 8;
        if(Keys[SDLK_KP8]) retvalue = 8;
        if(Keys_Unicode['9']) retvalue = 9;
        if(Keys[SDLK_KP9]) retvalue = 9;
        if(Keys[SDLK_a]) retvalue = 10;
        if(Keys[SDLK_b]) retvalue = 11;
        if(Keys[SDLK_c]) retvalue = 12;
        if(Keys[SDLK_d]) retvalue = 13;
        if(Keys[SDLK_e]) retvalue = 14;
        if(Keys[SDLK_f]) retvalue = 15;
        if(Keys[SDLK_DELETE]) retvalue = 16;

        int i;

        if(is_recording)
        {
            // Start the real recording
            if(!is_recording_2 && (retnote_raw < NOTE_OFF && retnote_raw > 0) && key_record_first_time)
            {
                // Start recording
                is_recording_2 = 1;
                Nbr_Sub_NoteOff = 0;
                is_record_key = FALSE;
                is_editing = TRUE;
                Songplaying = TRUE;
                ped_line_delay = ped_line;
                key_record_first_time = FALSE;
                old_key_ped_line = ped_line;
                Clear_Midi_Channels_Pool();
            }
        }

        int Record_Key_Pressed = FALSE;
        for(i = 0; i < 37; i++)
        {
            if(Record_Keys[i])
            {
                Record_Key_Pressed = TRUE;
            }
        }

        if(Record_Key_Pressed)
        {
            for(i = 0; i < 37; i++)
            {
                if(Record_Keys[i])
                {
                    int tmp_note = ((Record_Keys[i] & 0x7f) - 1) + ped_patoct * 12;
                    if(tmp_note > NOTE_MAX) tmp_note = NOTE_MAX;

                    if(!(Record_Keys[i] & 0x80))
                    {
                        if(Record_Keys_State[i] == FALSE)
                        {
                            Record_Keys_State[i] = TRUE;

                            if(is_recording_2)
                            {
                                Send_Note(Raw_Keys_to_Send[i], FALSE, FALSE);
                            }

                            Note_Jazz(ped_track, tmp_note);
                        }
                    }
                    else
                    {
                        if(Record_Keys_State[i] == TRUE)
                        {
                            Record_Keys_State[i] = FALSE;
                            Record_Keys[i] = 0;

                            // Do not reach that point
                            if(is_recording_2)
                            {
                                Send_Note(Raw_Keys_to_Send[i] | 0x80, FALSE, FALSE);
                            }

                            Note_Jazz_Off(tmp_note);
                        }
                    }
                }
            }
        }
    }

    // Turn edit mode on/off
    if(Keys[SDLK_SPACE] && snamesel == 0 && pos_space == 1)
    {
        if(Get_LShift())
        {
            gui_action = GUI_CMD_RECORD_MODE;
        }
        else
        {
            is_recording = 0;
            is_editing ^= 1;
            gui_action = GUI_CMD_EDIT_MODE;
        }
    }

    if(pos_space == 0 && !Keys[SDLK_SPACE]) pos_space = 1;

    if(!is_recording)
    {
        // Play song
        if(Keys[SDLK_RCTRL] && snamesel == 0 && po_ctrl2)
        {
            plx = 0;
            po_ctrl2 = FALSE;
            gui_action = GUI_CMD_PLAY_SONG;
        }
        if(!Keys[SDLK_RCTRL] && !po_ctrl2) po_ctrl2 = TRUE;

        if(Keys[SDLK_RALT] && snamesel == 0 && po_alt2)
        {
            plx = 1;
            po_alt2 = FALSE;
            gui_action = GUI_CMD_PLAY_SONG;
        }
        if(!Keys[SDLK_RALT] && !po_alt2) po_alt2 = TRUE;

        // Play song
        if(Get_LAlt() && snamesel == 0 && !po_alt)
        {
            po_alt = TRUE;
        }
        if(!Get_LAlt() && po_alt) po_alt = FALSE;

// This is a nasty hack: we should have a specific ASCII buffer
// instead of using the unicode one.
#if !defined(__MACOSX__)
#define UNICODE_OFFSET1 0x20
#define UNICODE_OFFSET2 0x60
#else
#define UNICODE_OFFSET1 0
#define UNICODE_OFFSET2 0
#endif

        if(Key_Unicode)
        {
            if(Get_LShift())
            {
                if(Keys[SDLK_m - UNICODE_OFFSET1])
                {
                    if(CHAN_MUTE_STATE[ped_track] == 0) CHAN_MUTE_STATE[ped_track] = 1;
                    else CHAN_MUTE_STATE[ped_track] = 0;
                    if(userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(10);
                    Actupated(0);
                }
            }
        }

        if(Get_LCtrl())
        {
            if(!Get_LAlt() && !Get_LShift())
            {
                if(Keys[SDLK_LEFT])
                {
                    if(pSequence[Cur_Position] > 0)
                    {
                        gui_action = GUI_CMD_PREV_PATT;
                    }
                    Keys[SDLK_LEFT] = FALSE;
                }
                if(Keys[SDLK_RIGHT])
                {
                    if(pSequence[Cur_Position] < 254)
                    {
                        gui_action = GUI_CMD_NEXT_PATT;
                    }
                    Keys[SDLK_RIGHT] = FALSE;
                }
            }

            if(Key_Unicode)
            {
                if(Get_LShift())
                {
                    // Solo track
                    if(Keys[SDLK_m - UNICODE_OFFSET2])
                    {
                        Solo_Track(ped_track);
                        // Will unmute the correct track
                        if(CHAN_MUTE_STATE[ped_track] == 0) CHAN_MUTE_STATE[ped_track] = 1;
                        else CHAN_MUTE_STATE[ped_track] = 0;
                        if(userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(10);
                        Actupated(0);
                    }
                }

                // Save
                if(Keys[SDLK_s - UNICODE_OFFSET2])
                {
                    gui_action = GUI_CMD_SAVE_MODULE;
                }

                // Cut selected block
                if(Keys[SDLK_x - UNICODE_OFFSET2] && block_start_track != -1 && block_end_track != -1)
                {
                    Cut_Selection(Cur_Position);
                }

                // Copy selected block
                if(Keys[SDLK_c - UNICODE_OFFSET2] && block_start_track != -1 && block_end_track != -1)
                {
                    Copy_Selection(Cur_Position);
                }

                // Interpolate the values of a selected block
                if(Keys[SDLK_i - UNICODE_OFFSET2] && is_editing)
                {
                    Interpolate_Block(Cur_Position);
                }

                // Randomize the values of a selected block
                if(Keys[SDLK_r - UNICODE_OFFSET2] && is_editing)
                {
                    Randomize_Block(Cur_Position);
                }

                // Paste the block buffer into a pattern
                if(Keys[SDLK_v - UNICODE_OFFSET2] && block_start_track_nibble != -1 && block_end_track_nibble != -1 && is_editing)
                {
                    Paste_Block(Cur_Position);
                }

                // Transpose the selection 1 semitone higher
                if(Keys[SDLK_u - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Semitone_Up_Block(Cur_Position);
                    else Semitone_Up_Block(Cur_Position);
                }

                // Transpose the selection 1 semitone lower
                if(Keys[SDLK_d - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Semitone_Down_Block(Cur_Position);
                    else Semitone_Down_Block(Cur_Position);
                }

                // Select the current track or the current pattern
                if(Keys[SDLK_a - UNICODE_OFFSET2])
                {
                    if(Get_LShift()) Select_Pattern_Block();
                    else Select_Track_Block();
                }
            }
        }

        if(Get_LAlt())
        {
            if(Get_LCtrl())
            {
                // Previous position in sequence
                if(Keys[SDLK_LEFT])
                {
                    gui_action = GUI_CMD_PREVIOUS_POSITION;
                    Keys[SDLK_LEFT] = FALSE;
                }

                // Next position in sequence
                if(Keys[SDLK_RIGHT])
                {
                    gui_action = GUI_CMD_NEXT_POSITION;
                    Keys[SDLK_RIGHT] = FALSE;
                }
            }
            else
            {
                // Previous instrument
                if(Keys[SDLK_LEFT])
                {
                    gui_action = GUI_CMD_PREV_INSTR;
                    Keys[SDLK_LEFT] = FALSE;
                }

                // Next instrument
                if(Keys[SDLK_RIGHT])
                {
                    gui_action = GUI_CMD_NEXT_INSTR;
                    Keys[SDLK_RIGHT] = FALSE;
                }

                // Select the current note/instrument
                if(Keys[SDLK_a])
                {
                    Select_Note_Block();
                }

                // Select the current track or the current pattern
                if(Keys[SDLK_a - UNICODE_OFFSET1])
                {
                    if(Get_LShift()) Select_All_Notes_Block();
                }

            }
        }

        if(!Get_LAlt())
        {
            if(retvalue != NOTE_OFF && is_editing == 1)
            {
                int ped_cell;
                int i;
                int j = (Channels_MultiNotes[ped_track] - 1) * 3;
                // Instrument
                for(i = 0; i < Channels_MultiNotes[ped_track]; i++)
                {
                    if(ped_col == 1 + (i * 3))
                    {
                        i++;
                        break;
                    }
                }
                i--;

                if(ped_col == (1 + (i * 3)) ||
                   ped_col == (3 + j) ||
                   ped_col == (5 + j) ||
                   ped_col == (7 + j) ||
                   ped_col == (9 + j))
                {
                    ped_cell = PATTERN_INSTR1 + (i * 2);                    // instrument
                    if(ped_col == (3 + j)) ped_cell = PATTERN_VOLUME;       // volume
                    if(ped_col == (5 + j)) ped_cell = PATTERN_PANNING;      // panning
                    if(ped_col == (7 + j)) ped_cell = PATTERN_FX;           // fx
                    if(ped_col == (9 + j)) ped_cell = PATTERN_FXDATA;       // fx data

                    ltretvalue = retvalue;
                    xoffseted = (ped_track * PATTERN_BYTES) + (ped_line * PATTERN_ROW_LEN) + ped_cell;

                    int oldval = *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted);

                    if(retvalue < 16)
                    {
                        if(oldval == 255 && ped_col == (1 + (i * 3))) oldval = 0;
                        if(oldval == 255 && ped_col == (3 + j)) oldval = 0;
                        if(oldval == 255 && ped_col == (5 + j)) oldval = 0;
                        oldval = (oldval & 0xf) + (retvalue << 4);
                        *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                        // Max panning
                        if(oldval != 255 && ped_col == (5 + j) &&
                           *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                        {
                            if(oldval != 255 && ped_col == (5 + j) &&
                            *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                            {
                                *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                            }
                        }

                        // Max instrument
                        if(oldval != 255 && ped_col == (1 + (i * 3)) &&
                           *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x7f)
                        {
                            *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x7f;
                        }

                        if(!is_recording) if(!Songplaying)
                        {
                            ped_line += ped_pattad;
                            if(!ped_pattad)
                            {
                                ped_col++;
                                gui_action = GUI_CMD_SET_FOCUS_TRACK;
                            }
                        }
                    }
                    else
                    {
                        if(!Delete_Selection(Cur_Position))
                        {
                            oldval = 0;
                            if(ped_col == (1 + (i * 3))) oldval = 255;
                            if(ped_col == (3 + j)) oldval = 255;
                            if(ped_col == (5 + j)) oldval = 255;
                            *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                            // Max panning
                            if(oldval != 255 && ped_col == (5 + j) &&
                               *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                            {
                                if(oldval != 255 && ped_col == (5 + j) &&
                                   *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                                {
                                    *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                                }
                            }

                            // Max instrument
                            if(oldval != 255 && ped_col == (1 + (i * 3)) &&
                               *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x7f)
                            {
                                *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x7f;
                            }

                            if(!is_recording) if(!Songplaying)
                            {
                                ped_line += ped_pattad;
                                if(!ped_pattad)
                                {
                                    ped_col++;
                                    gui_action = GUI_CMD_SET_FOCUS_TRACK;
                                }
                            }
                        }
                    }
                    Actupated(0);
                }

                for(i = 0; i < Channels_MultiNotes[ped_track]; i++)
                {
                    if(ped_col == 2 +(i * 3))
                    {
                        i++;
                        break;
                    }
                }
                i--;
                if(ped_col == (2 + (i * 3)) ||
                   ped_col == (4 + j) ||
                   ped_col == (6 + j) ||
                   ped_col == (8 + j) ||
                   ped_col == (10 + j))
                {
                    ped_cell = PATTERN_INSTR1 + (i * 2);
                    if(ped_col == (4 + j)) ped_cell = PATTERN_VOLUME;
                    if(ped_col == (6 + j)) ped_cell = PATTERN_PANNING;
                    if(ped_col == (8 + j)) ped_cell = PATTERN_FX;
                    if(ped_col == (10 + j)) ped_cell = PATTERN_FXDATA;

                    ltretvalue = retvalue;
                    xoffseted = (ped_track * PATTERN_BYTES) + (ped_line * PATTERN_ROW_LEN) + ped_cell;
                    int oldval = *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted);

                    if(retvalue < 16)
                    {
                        if(oldval == 255 && ped_col == (2 + (i * 3))) oldval = 0;
                        if(oldval == 255 && ped_col == (4 + j)) oldval = 0;
                        if(oldval == 255 && ped_col == (6 + j)) oldval = 0;
                        oldval = (oldval & 0xf0) + retvalue;
                        *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                        // Max panning
                        if(oldval != 255 && ped_col == (6 + j) &&
                           *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                        {
                            if(oldval != 255 && ped_col == (6 + j) &&
                               *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                            {
                                *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                            }
                        }

                        // Max instrument
                        if(oldval != 255 && ped_col == (2 + (i * 3)) &&
                           *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x7f)
                        {
                            *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x7f;
                        }
    
                        if(!is_recording) if(!Songplaying)
                        {
                            ped_line += ped_pattad;
                            if(!ped_pattad)
                            {
                                ped_col++;
                                gui_action = GUI_CMD_SET_FOCUS_TRACK;
                            }
                        }
                    }
                    else
                    {
                        if(!Delete_Selection(Cur_Position))
                        {
                            oldval = 0;
                            if(ped_col == (2 + (i * 3))) oldval = 255;
                            if(ped_col == (4 + j)) oldval = 255;
                            if(ped_col == (6 + j)) oldval = 255;
                            *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                            // Max panning
                            if(oldval != 255 && ped_col == (6 + j) &&
                               *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                            {
                                if(oldval != 255 && ped_col == (6 + j) &&
                                   *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                                {
                                    *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                                }
                            }

                            // Max instrument
                            if(oldval != 255 && ped_col == (2 + (i * 3)) &&
                               *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x7f)
                            {
                                *(RawPatterns + pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x7f;
                            }

                            if(!is_recording) if(!Songplaying)
                            {
                                ped_line += ped_pattad;
                                if(!ped_pattad)
                                {
                                    ped_col++;
                                    gui_action = GUI_CMD_SET_FOCUS_TRACK;
                                }
                            }
                        }
                    }
                    Actupated(0);
                }
            }
        }
    }

    // --------------------------------------------
    // Enter one or several notes (used for midi and record as well as simple editing)
    if(!Get_LAlt() && !Get_LCtrl() && !Get_LShift() && snamesel == 0 && !reelletter)
    {
        int go_note = FALSE;

        int i;
        for(i = 0; i < Keyboard_Nbr_Events; i++)
        {
            int Idx = Keyboard_Events[i];

            if(!Keyboard_Notes_Type[i])
            {
                // Key editing
                switch(Idx & 0x7f)
                {
                    case 0x10: 
                    case 3:
                    case 0x11:
                    case 4:
                    case 0x12:
                    case 0x13:
                    case 6:
                    case 0x14:
                    case 7:
                    case 0x15:
                    case 8:
                    case 0x16:
                    case 0x17:
                    case 0xa:
                    case 0x18:
                    case 0xb:
                    case 0x19:
                    case 0x1a:
                    case 0xd:
                    case 0x1b:
                    case 0x2c:
                    case 0x1f:
                    case 0x2d:
                    case 0x20:
                    case 0x2e:
                    case 0x2f:
                    case 0x22:
                    case 0x30:
                    case 0x23:
                    case 0x31:
                    case 0x24:
                    case 0x32:
                    case 0x33:
                    case 0x26:
                    case 0x34:
                    case 0x27:
                    case 0x35:
                        go_note = TRUE;
                        break;
                }
            }
            else
            {
                go_note = TRUE;
            }
        }
        // Write a note
        if(go_note)
        {
            int column = 0;
            int in_note = FALSE;

            // Check if this is a legal note column
            for(i = 0; i < Channels_MultiNotes[ped_track]; i++)
            {
                if(ped_col == (i * 3))
                {
                    in_note = TRUE;
                    break;
                }
                column += 2;
            }

            if(in_note)
            {
                int i;
                int move_down = FALSE;
                for(i = 0; i < Keyboard_Nbr_Events; i++)
                {
                    int Idx = Keyboard_Events[i];
                    int pos;
                    int tmp_note;
                    int line;

                    if(!Keyboard_Notes_Type[i])
                    {
                        // Key editing
                        switch(Idx & 0x7f)
                        {
                            case 0x10: { retnote_raw = 12; break; }
                            case 3:    { retnote_raw = 13; break; }
                            case 0x11: { retnote_raw = 14; break; }
                            case 4:    { retnote_raw = 15; break; }
                            case 0x12: { retnote_raw = 16; break; }
                            case 0x13: { retnote_raw = 17; break; }
                            case 6:    { retnote_raw = 18; break; }
                            case 0x14: { retnote_raw = 19; break; }
                            case 7:    { retnote_raw = 20; break; }
                            case 0x15: { retnote_raw = 21; break; }
                            case 8:    { retnote_raw = 22; break; }
                            case 0x16: { retnote_raw = 23; break; }
                            case 0x17: { retnote_raw = 24; break; }
                            case 0xa:  { retnote_raw = 25; break; }
                            case 0x18: { retnote_raw = 26; break; }
                            case 0xb:  { retnote_raw = 27; break; }
                            case 0x19: { retnote_raw = 28; break; }
                            case 0x1a: { retnote_raw = 29; break; }
                            case 0xd:  { retnote_raw = 30; break; }
                            case 0x1b: { retnote_raw = 31; break; }
                            case 0x2c: { retnote_raw = 0; break; }
                            case 0x1f: { retnote_raw = 1; break; }
                            case 0x2d: { retnote_raw = 2; break; }
                            case 0x20: { retnote_raw = 3; break; }
                            case 0x2e: { retnote_raw = 4; break; }
                            case 0x2f: { retnote_raw = 5; break; }
                            case 0x22: { retnote_raw = 6; break; }
                            case 0x30: { retnote_raw = 7; break; }
                            case 0x23: { retnote_raw = 8; break; }
                            case 0x31: { retnote_raw = 9; break; }
                            case 0x24: { retnote_raw = 10; break; }
                            case 0x32: { retnote_raw = 11; break; }
                            case 0x33: { retnote_raw = 12; break; }
                            case 0x26: { retnote_raw = 13; break; }
                            case 0x34: { retnote_raw = 14; break; }
                            case 0x27: { retnote_raw = 15; break; }
                            case 0x35: { retnote_raw = 16; break; }
                            default:
                                goto No_Key;
                        }
                        tmp_note = retnote_raw + (ped_patoct * 12);
                        if(tmp_note > NOTE_MAX) tmp_note = NOTE_MAX;
                    }
                    else
                    {
                        tmp_note = Idx & 0x7f;
                    }

                    line = ped_line;
                    if(is_recording_2) line = ped_line_delay;

                    if(!(Idx & 0x80))
                    {
                        // Note
                        if(retnote > -1)
                        {
                            xoffseted = Get_Pattern_Offset(pSequence[Cur_Position], ped_track, line);

                            // Select the sub channel
                            pos = i;

                            if(is_recording_2 || is_editing)
                            {
                                // Store the note column where it was entered
                                Sub_Channels_NoteOff[ped_track][Nbr_Sub_NoteOff].Note = ((tmp_note + 1) << 8);
                                Sub_Channels_NoteOff[ped_track][Nbr_Sub_NoteOff].Channel = ped_track;
                                if(Keyboard_Notes_Bound[i])
                                {
                                    pos = (ped_col / 3);
                                }
                                else
                                {
                                    pos = (ped_col / 3) + Nbr_Sub_NoteOff;
                                }
                                Sub_Channels_NoteOff[ped_track][Nbr_Sub_NoteOff].Sub_Channel = pos;

                                if(Nbr_Sub_NoteOff < (Channels_MultiNotes[ped_track] - 1)) Nbr_Sub_NoteOff++;
                                else Nbr_Sub_NoteOff = 0;
                            }

                            if(pos > (Channels_MultiNotes[ped_track] - 1))
                            {
                                pos -= Channels_MultiNotes[ped_track];
                            }
                            xoffseted += (pos * 2);

                            if(is_editing)
                            {
                                *(RawPatterns + xoffseted + PATTERN_NOTE1) = tmp_note;
                                *(RawPatterns + xoffseted + PATTERN_INSTR1) = ped_patsam;
                            }
                        }
                        move_down = TRUE;
                    }
                    else
                    {
                        // Note off
                        if(is_recording_2 || is_editing)
                        {
                            LPJAZZ_KEY Channel = Get_Jazz_Key_Off(Sub_Channels_NoteOff, (tmp_note + 1) << 8);
                            if(Channel)
                            {
                                if(is_recording_2)
                                {
                                    xoffseted = Get_Pattern_Offset(pSequence[Cur_Position], Channel->Channel, line);
                                    pos = Channel->Sub_Channel;
                                    if(pos > (Channels_MultiNotes[ped_track] - 1))
                                    {
                                        pos -= Channels_MultiNotes[ped_track];
                                    }
                                    xoffseted += (pos * 2);
                                    *(RawPatterns + xoffseted + PATTERN_NOTE1) = NOTE_OFF;
                                    *(RawPatterns + xoffseted + PATTERN_INSTR1) = NO_INSTR;
                                }
                                // Now discard any remaining note off for that channel/sub channel
                                Nbr_Sub_NoteOff -= Discard_Key_Note_Off(Sub_Channels_NoteOff, Channel->Channel, Channel->Sub_Channel);
                                if(Nbr_Sub_NoteOff < 0) Nbr_Sub_NoteOff = 0;
                            }
                        }
                        if(is_recording_2) move_down = TRUE;
                    }
No_Key:;
                }
                Keyboard_Nbr_Events = 0;
                if(is_editing)
                {
                    if(!Songplaying)
                    {
                        if(move_down)
                        {
                            ped_line += ped_pattad;
                            if(!ped_pattad)
                            {
                                ped_col++;
                                gui_action = GUI_CMD_SET_FOCUS_TRACK;
                            }
                        }
                    }
                }
                if(is_recording_2)
                {
                    if(old_key_ped_line != ped_line)
                    {
                        Actualize_Sequencer();
                    }
                    old_key_ped_line = ped_line;
                }
                Actupated(0);
            }
        }

        // Note off (RSHIFT)
        if(retnote == -3 && is_editing && !is_recording_2)
        {
            int column = 0;
            int in_note = FALSE;
            int i;

            // Check if this is a legal note column
            for(i = 0; i < Channels_MultiNotes[ped_track]; i++)
            {
                if(ped_col == (i * 3))
                {
                    in_note = TRUE;
                    break;
                }
                column += 2;
            }

            if(in_note)
            {
                xoffseted = Get_Pattern_Offset(pSequence[Cur_Position], ped_track, ped_line);

                *(RawPatterns + xoffseted + PATTERN_NOTE1 + column) = NOTE_OFF;
                *(RawPatterns + xoffseted + PATTERN_INSTR1 + column) = NO_INSTR;
    
                if(!is_recording) if(!Songplaying)
                {
                    ped_line += ped_pattad;
                    if(!ped_pattad)
                    {
                        ped_col++;
                        gui_action = GUI_CMD_SET_FOCUS_TRACK;
                    }
                }
                Actupated(0);
            }
        }

        // Delete a note
        if(retnote == -1 && is_editing && !is_recording_2)
        {
            int column = 0;
            int in_note = FALSE;
            int i;

            // Check if this is a legal note column
            for(i = 0; i < Channels_MultiNotes[ped_track]; i++)
            {
                if(ped_col == (i * 3))
                {
                    in_note = TRUE;
                    break;
                }
                column += 2;
            }

            if(in_note)
            {
                xoffseted = Get_Pattern_Offset(pSequence[Cur_Position], ped_track, ped_line);

                if(!Delete_Selection(Cur_Position))
                {
                    // Delete the note under the caret
                    *(RawPatterns + xoffseted + PATTERN_NOTE1 + column) = NO_NOTE;
                    *(RawPatterns + xoffseted + PATTERN_INSTR1 + column) = NO_INSTR;

                    if(!is_recording) if(!Songplaying)
                    {
                        ped_line += ped_pattad;
                        if(!ped_pattad)
                        {
                            ped_col++;
                            gui_action = GUI_CMD_SET_FOCUS_TRACK;
                        }
                    }
                }
                Actupated(0);
            }
        }
    }
}

// ------------------------------------------------------
// Retrieve mouse datas
char zcheckMouse_nobutton(int x, int y, int xs, int ys)
{
    if(Mouse.x > x && Mouse.x < x + xs && Mouse.y > y && Mouse.y < y + ys + 1)
    {
        return(1);
    }
    return(0);
}

char zcheckMouse(int x, int y, int xs, int ys)
{
    if(!gui_pushed && Mouse.x > x && Mouse.x < x + xs && Mouse.y > y && Mouse.y < y + ys + 1)
    {
        gui_lx = x;
        gui_ly = y;
        gui_lxs = xs;
        gui_lys = ys;
        gui_pushed = Mouse.button_oneshot;
        fluzy = -1;
        return(1);
    }
    if(gui_pushed && x == gui_lx && y == gui_ly && xs == gui_lxs && ys == gui_lys)
    {
        return(1);
    }
    return(0);
}

// ------------------------------------------------------
// Mouse Handler
void Mouse_Handler(void)
{
    int Cur_Position;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    if(gui_action == GUI_CMD_NOP) gui_action = GUI_CMD_NONE;

    teac = 0;

    // mouse wheel up
    if(Mouse.wheel == 1)
    {
        // Scroll the pattern
        if(!Songplaying && !is_recording)
        {
            Mouse_Wheel_Pattern_Ed(-MouseWheel_Multiplier);
        }

        // Scroll the files list
        switch(Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:

                // Scroll the instruments/synths lists
                if(zcheckMouse(410, 41, 390, 136) == 1)
                {
                    Instrs_index--;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }

                if(zcheckMouse(394, 58, 16, 103) == 1)
                {
                    Instrs_index--;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:

                // Scroll the files lists
                if(zcheckMouse(410, 41, 390, 136) == 1)
                {
                    lt_index--;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }

                if(zcheckMouse(394, 58, 16, 103) == 1)
                {
                    lt_index--;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }
                break;
        }

        // Scroll the sequences
        if(userscreen == USER_SCREEN_SEQUENCER)
        {
            if(zcheckMouse(257, 466, 26, 90) ||
               zcheckMouse(89, 466, 26, 90) ||
               zcheckMouse(120, 466, 132, 90))
            {
                gui_action = GUI_CMD_PREVIOUS_POSITION;
            }
        }

        // Scroll the knobs
        Mouse_Wheel_303_Ed(MouseWheel_Multiplier);
    }

    // mouse wheel down
    if(Mouse.wheel == -1)
    {
        // Scroll the pattern
        if(!Songplaying && !is_recording)
        {
            Mouse_Wheel_Pattern_Ed(MouseWheel_Multiplier);
        }

        switch(Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:

                // Scroll the instruments/synths lists
                if(zcheckMouse(410, 41, 390, 136) == 1)
                {
                    Instrs_index++;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }

                if(zcheckMouse(394, 58, 16, 103) == 1)
                {
                    Instrs_index++;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:

                // Scroll the files lists
                if(zcheckMouse(410, 41, 390, 136) == 1)
                {
                    lt_index++;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }

                // Scroll the files list
                if(zcheckMouse(394, 58, 16, 103) == 1)
                {
                    lt_index++;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }
                break;
        }

        // Scroll the sequences
        if(userscreen == USER_SCREEN_SEQUENCER)
        {
            if(zcheckMouse(257, 466, 26, 90) ||
               zcheckMouse(89, 466, 26, 90) ||
               zcheckMouse(120, 466, 132, 90))
            {
                gui_action = GUI_CMD_NEXT_POSITION;
            }
        }

        // Scroll the knobs
        Mouse_Wheel_303_Ed(-MouseWheel_Multiplier);
    }

    if(Mouse.button & MOUSE_RIGHT_BUTTON)
    {
        Mouse_Sliders_Right_Pattern_Ed();

        Mouse_Sliders_Right_Instrument_Ed();
    }

    if(Mouse.button & MOUSE_LEFT_BUTTON)
    {
        switch(Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:
                if(zcheckMouse(395, 59, 16, 103)) gui_action = GUI_CMD_SET_INSTR_SYNTH_LIST_SLIDER;
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
                if(zcheckMouse(395, 59, 16, 103)) gui_action = GUI_CMD_SET_FILES_LIST_SLIDER;
                break;
        }

        Mouse_Left_Repeat_Instrument_Ed();

        Mouse_Sliders_303_Ed();

        Mouse_Sliders_Sample_Ed();

        Mouse_Sliders_Instrument_Ed();

        Mouse_Sliders_Master_Ed();

        Mouse_Sliders_Synth_Ed();

        Mouse_Sliders_Track_Fx_Ed();

        Mouse_Sliders_Fx_Ed();

        Mouse_Sliders_Track_Ed();

        Mouse_Sliders_Master_Shuffle();

        Mouse_Sliders_Pattern_Ed();
    }
    else
    {
        // leftbutton
        sas = FALSE;
    }

    // Check Zones for GUI clicks -----------------------------------

    if(Mouse.button_oneshot & MOUSE_LEFT_BUTTON)
    {
        if(display_title)
        {
            // Modules dir.
            if(zcheckMouse(746, 24, 18, 16))
            {
                Scopish = SCOPE_ZONE_MOD_DIR;
                Draw_Scope_Files_Button();
            }

            // Instruments dir.
            if(zcheckMouse(764, 24, 18, 16))
            {
                Scopish = SCOPE_ZONE_INSTR_DIR;
                Draw_Scope_Files_Button();
            }

            // Presets dir.
            if(zcheckMouse(782, 24, 18, 16))
            {
                Scopish = SCOPE_ZONE_PRESET_DIR;
                Draw_Scope_Files_Button();
            }

            // Tracks/Stereo scopes.
            if(zcheckMouse(746, 6, 18, 16))
            {
                if(Scopish == SCOPE_ZONE_SCOPE)
                { 
                    Scopish_LeftRight ^= TRUE;
                }
                Scopish = SCOPE_ZONE_SCOPE;
                Draw_Scope_Files_Button();
            }

            // Instrument list
            if(zcheckMouse(764, 6, 18, 16))
            {
                Scopish = SCOPE_ZONE_INSTR_LIST;
                Draw_Scope_Files_Button();
            }

            // Synth list
            if(zcheckMouse(782, 6, 18, 16))
            {
                Scopish = SCOPE_ZONE_SYNTH_LIST;
                Draw_Scope_Files_Button();
            }
        }

        Mouse_Left_303_Ed();

        Mouse_Left_Sample_Ed();

        // Change current instrument name
        if(zcheckMouse(90, 134, 166, 16) && snamesel == 0)
        {
            snamesel = 2;
            strcpy(cur_input_name, nameins[ped_patsam]);
            sprintf(nameins[ped_patsam], "");
            namesize = 0;
            gui_action = GUI_CMD_UPDATE_PATTERN_ED;
        }

        switch(Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:
                if(zcheckMouse(394, 42, 16, 14))
                {
                    Instrs_index--;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }

                if(zcheckMouse(394, 162, 16, 14))
                {
                    Instrs_index++;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }

                // Select a file
                if(zcheckMouse(412, 43, 226 + restx, 133))
                {
                    gui_action = GUI_CMD_SET_INSTR_SYNTH_LIST_SELECT;
                }
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
                // Files list up
                if(zcheckMouse(394, 42, 16, 14))
                {
                    lt_index--;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }
            
                // Files list down
                if(zcheckMouse(394, 162, 16, 14))
                {
                    lt_index++;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }

                // Select a file
                if(zcheckMouse(412, 43, 226 + restx, 133))
                {
                    gui_action = GUI_CMD_SET_FILES_LIST_SELECT_FILE;
                }
                break;
        }

        if(zcheckMouse(258, 152, 16, 16)) gui_action = GUI_CMD_LOWER_OCTAVE;
        if(zcheckMouse(302, 152, 16, 16)) gui_action = GUI_CMD_HIGHER_OCTAVE;
        if(zcheckMouse(90, 152, 16, 16)) gui_action = GUI_CMD_DECREASE_STEP_ADD;
        if(zcheckMouse(134, 152, 16, 16)) gui_action = GUI_CMD_INCREASE_STEP_ADD;
        if(zcheckMouse(258, 134, 16, 16)) gui_action = GUI_CMD_PREV_INSTR;
        if(zcheckMouse(302, 134, 16, 16)) gui_action = GUI_CMD_NEXT_INSTR;

        // --- Player --------------------------------------------

        if(zcheckMouse(8, 44, 80, 16))
        {
            plx = 0;
            gui_action = GUI_CMD_PLAY_SONG;
        }
        if(zcheckMouse(8, 62, 80, 16))
        {
            gui_action = GUI_CMD_STOP_SONG;
        }

        if(zcheckMouse(8, 80, 80, 16))
        {
            sr_isrecording++;
            if(sr_isrecording > 1) sr_isrecording = 0;
            gui_action = GUI_CMD_RECORD_303;
        }

        if(zcheckMouse(8, 98, 80, 16))
        {
            is_recording = 0;
            is_editing ^= 1;
            gui_action = GUI_CMD_EDIT_MODE;
        }

        // --- Sequence -----------------------------------------

        if(zcheckMouse(188, 44, 16, 16))
        {
            gui_action = GUI_CMD_PREVIOUS_POSITION;
        }
        if(zcheckMouse(232, 44, 16, 16))
        {
            gui_action = GUI_CMD_NEXT_POSITION;
        }
        if(zcheckMouse(188, 62, 16, 16) && pSequence[Cur_Position] > 0)
        {
            gui_action = GUI_CMD_PREV_PATT;
        }
        if(zcheckMouse(232, 62, 16, 16) && pSequence[Cur_Position] < 254)
        {
            gui_action = GUI_CMD_NEXT_PATT;
        }
        if(zcheckMouse(188, 80, 16, 16) && sLength > 1)
        {
            gui_action = GUI_CMD_REDUCE_SONG_LENGTH;
        }
        if(zcheckMouse(232, 80, 16, 16) && sLength < 255)
        {
            gui_action = GUI_CMD_INCREASE_SONG_LENGTH;
        }

        // Decrease the number of lines for this pattern
        if(zcheckMouse(188, 98, 16, 16) && patternLines[pSequence[Cur_Position]] > 1)
        {
            patternLines[pSequence[Cur_Position]]--;
            if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }
        // Increase the number of lines for this pattern
        if(zcheckMouse(232, 98, 16, 16) && patternLines[pSequence[Cur_Position]] < 128)
        {
            patternLines[pSequence[Cur_Position]]++;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        // --- Song Settings --------------------------------------

        // Reduce the number of tracks
        if(zcheckMouse(324, 44, 16, 16) && Songtracks > 1)
        {
            Songtracks--;
            if(Songtracks < 1) Songtracks = 1;
            gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            teac = 4;
        }
        // Increase the number of tracks
        if(zcheckMouse(368, 44, 16, 16) && Songtracks < 16)
        {
            Songtracks++;
            if(Songtracks > 16) Songtracks = 16;
            gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            teac = 4;
        }

        // Reduce the number of BPM
        if(zcheckMouse(324, 62, 16, 16))
        {
            BeatsPerMin--;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }
    
        // Increase the number of BPM
        if(zcheckMouse(368, 62, 16, 16))
        {
            BeatsPerMin++;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }

        // Decrease the number of TPB       
        if(zcheckMouse(324, 80, 16, 16))
        {
            TicksPerBeat--;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }
    
        // Increase the number of TPB
        if(zcheckMouse(368, 80, 16, 16))
        {
            TicksPerBeat++;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }

        // Delete instrument
        if(zcheckMouse(320, 134, 64, 16))
        {
            gui_action = GUI_CMD_DELETE_INSTRUMENT;
        }

        // Tabs
        if(zcheckMouse(396, 429, 64, 16) && userscreen != USER_SCREEN_TRACK_FX_EDIT) gui_action = GUI_CMD_SELECT_TRACK_FX_EDIT;
        if(zcheckMouse(132, 429, 64, 16) && userscreen != USER_SCREEN_SYNTH_EDIT) gui_action = GUI_CMD_SELECT_SYNTH_EDIT;
        if(zcheckMouse(594, 429, 64, 16) && userscreen != USER_SCREEN_SETUP_EDIT) gui_action = GUI_CMD_SELECT_SCREEN_SETUP_EDIT;
        if(zcheckMouse(0, 429, 64, 16) && userscreen != USER_SCREEN_SEQUENCER) gui_action = GUI_CMD_SELECT_SEQUENCER;
        if(zcheckMouse(528, 429, 64, 16) && userscreen != USER_SCREEN_DISKIO_EDIT) gui_action = GUI_CMD_SELECT_DISKIO;
        if(zcheckMouse(330, 429, 64, 16) && userscreen != USER_SCREEN_TRACK_EDIT) gui_action = GUI_CMD_SELECT_TRACK_EDIT;
        if(zcheckMouse(66, 429, 64, 16) && userscreen != USER_SCREEN_INSTRUMENT_EDIT)
        {
            gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
            seditor = 0;
        }
        if(zcheckMouse(462, 429, 64, 16) && userscreen != USER_SCREEN_FX_SETUP_EDIT)
        {
            gui_action = GUI_CMD_SELECT_FX_EDIT;
            teac = 0;
        }  
        if(zcheckMouse(198, 429, 64, 16) && userscreen != USER_SCREEN_SAMPLE_EDIT) gui_action = GUI_CMD_SELECT_SAMPLE_EDIT;
        if(zcheckMouse(264, 429, 64, 16) && userscreen != USER_SCREEN_TB303_EDIT) gui_action = GUI_CMD_SELECT_TB303_EDIT;
        if(zcheckMouse(660, 429, 64, 16))
        {
            if(Asking_Exit) gui_action = GUI_CMD_EXIT;
            else gui_action = GUI_CMD_ASK_EXIT;
        }

        Mouse_Left_Track_Fx_Ed();

        Mouse_Left_Sequencer_Ed();

        Mouse_Left_DiskIO_Ed();

        Mouse_Left_Track_Ed();

        Mouse_Left_Instrument_Ed();

        Mouse_Left_Fx_Ed();

        Mouse_Left_Synth_Ed();

        Mouse_Left_Master_Ed();

        Mouse_Left_Pattern_Ed();
    }

    // Right mouse button
    if(Mouse.button_oneshot & MOUSE_RIGHT_BUTTON)
    {
        gui_action = GUI_CMD_NOP;

        Mouse_Right_Pattern_Ed();

        if(zcheckMouse(188, 98, 16, 16))
        {
            int ltp = patternLines[pSequence[Cur_Position]];
            ltp -= 16;
            if(ltp < 1) ltp = 1;
            patternLines[pSequence[Cur_Position]] = ltp;
            if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        if(zcheckMouse(232, 98, 16, 16))
        {
            int ltp = patternLines[pSequence[Cur_Position]];
            ltp += 16;
            if(ltp > 128) ltp = 128;
            patternLines[pSequence[Cur_Position]] = ltp;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        if(zcheckMouse(258, 134, 16, 16) == 1)
        {
            gui_action = GUI_CMD_PREVIOUS_16_INSTR;
        }
        if(zcheckMouse(302, 134, 16, 16) == 1)
        {
            gui_action = GUI_CMD_NEXT_16_INSTR;
        }

        if(zcheckMouse(8, 44, 80, 16) == 1)
        {
            plx = 1;
            gui_action = GUI_CMD_PLAY_SONG;
        }

        switch(Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:
                if(zcheckMouse(394, 42, 16, 14) == 1)
                {
                    Instrs_index -= 10;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }
                if(zcheckMouse(394, 162, 16, 14) == 1)
                {
                    Instrs_index += 10;
                    gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
                if(zcheckMouse(394, 42, 16, 14) == 1)
                {
                    lt_index -= 10;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }
                if(zcheckMouse(394, 162, 16, 14) == 1)
                {
                    lt_index += 10;
                    gui_action = GUI_CMD_FILELIST_SCROLL;
                }
                break;
        }

        // Reduce the number of tracks
        if(zcheckMouse(324, 44, 16, 16) && Songtracks > 1)
        {
            Songtracks -= 5;
            if(Songtracks < 1) Songtracks = 1;
            gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            teac = 4;
        }
        // Increase the number of tracks
        if(zcheckMouse(368, 44, 16, 16) && Songtracks < 16)
        {
            Songtracks += 5;
            if(Songtracks > 16) Songtracks = 16;
            gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            teac = 4;
        }

        // Reduce the number of BPM by 16
        if(zcheckMouse(324, 62, 16, 16))
        {
            BeatsPerMin -= 16;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }
        // Increase the number of BPM by 16
        if(zcheckMouse(368, 62, 16, 16))
        {
            BeatsPerMin += 16;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }

        // Decrease the number of TPB       
        if(zcheckMouse(324, 80, 16, 16))
        {
            TicksPerBeat -= 10;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }
    
        // Increase the number of TPB
        if(zcheckMouse(368, 80, 16, 16))
        {
            TicksPerBeat += 10;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }

        // Songlength + 10
        if(zcheckMouse(188, 80, 16, 16) == 1 && sLength != 1)
        {
            int tLength = sLength;
            tLength -= 10;
            if(tLength < 1) tLength = 1;
            sLength = tLength;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }
        if(zcheckMouse(232, 80, 16, 16) == 1 && sLength != 255)
        {
            int tLength = sLength;
            tLength += 10;
            if(tLength > 255) tLength = 255;
            sLength = tLength;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // -10 positions
        if(zcheckMouse(188, 44, 16, 16) == 1)
        {
            gui_action = GUI_CMD_REDUCE_POSITIONS_10;
        }
        // +10 positions
        if(zcheckMouse(232, 44, 16, 16) == 1)
        {
            gui_action = GUI_CMD_INCREASE_POSITIONS_10;
        }

        // -10 patterns
        if(zcheckMouse(188, 62, 16, 16) == 1)
        {
            gui_action = GUI_CMD_REDUCE_PATTERNS_10;
        }
        // +10 patterns
        if(zcheckMouse(232, 62, 16, 16) == 1)
        {
            gui_action = GUI_CMD_INCREASE_PATTERNS_10;
        }

        // Record mode
        if(zcheckMouse(8, 98, 80, 16))
        {
            gui_action = GUI_CMD_RECORD_MODE;
        }

        Mouse_Right_Fx_Ed();

        Mouse_Right_303_Ed();

        Mouse_Right_DiskIO_Ed();

        Mouse_Right_Sample_Ed();

        Mouse_Right_Track_Ed();

        Mouse_Right_Instrument_Ed();

        Mouse_Right_Sequencer_Ed();

        Mouse_Right_Synth_Ed();

        Mouse_Right_Master_Ed();

        Reset_Pattern_Scrolling_Horiz();

    } // RIGHT MOUSE
}

// ------------------------------------------------------
// Search the first free slot in the sequences pool
int Search_Free_Pattern(void)
{
    int i;
    int j;
    int found;

    for(i = 0; i < sLength; i++)
    {
        found = FALSE;
        for(j = 0; j < 128; j++)
        {
            if(pSequence[j] == i)
            {
                found = TRUE;
                break;
            }
        }
        if(!found) return(i);
    }
    return(-1);
}

// ------------------------------------------------------
// Next pattern/position
int Next_Line_Pattern_Auto(int *position, int lines, int *line)
{
    int new_pattern;
    int max_value;

    if(*line >= lines)
    {
        // Normal end of pattern
        *line = *line - lines;
        *position += 1;
        if(*position >= sLength - 1)
        {
            sLength++;
            max_value = sLength;
            if(max_value > 255)
            {
                sLength = 255;
                cPosition = 0;
            }
            // Alloc a new pattern position
            new_pattern = Search_Free_Pattern();
            if(new_pattern > -1) pSequence[*position] = new_pattern;
        }
    }
    return(*position);
}

// ------------------------------------------------------
// Return the track associated with the midi channel
int Get_Midi_Channel(int midi_channel)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(midi_channel == CHAN_MIDI_PRG[i])
        {
            return(i);
        }
    }
    return(-1);
}

// ------------------------------------------------------
// Return the first track which isn't allocated in the midi channels pool
int Get_Free_Midi_Sub_Channel(int track)
{
    int i;
    for(i = 0; i < MAX_POLYPHONY; i++)
    {
        if(Alloc_midi_Channels[track][i] == 0) return(i);
    }
    return(-1);
}

// ------------------------------------------------------
// Search & return the midi channel corresponding to the given data
int Search_Corresponding_Midi_Sub_Channel(int track, Uint32 Datas)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(Alloc_midi_Channels[track][i] == (Datas & 0x00ff00))
        {
            return(i);
        }
    }
    return(-1);
}

// ------------------------------------------------------
// Display the current beat time in milliseconds
void Display_Beat_Time(void)
{
    char String[64];
    sprintf(String, "1/4 Beat Time: %d ms.", (int) (15000.0f / (float) BeatsPerMin));
    Gui_Clear_Array(268, 102, 120, 16);
    PrintXY(268, 102, USE_FONT, String);
}

// ------------------------------------------------------
// Display master compressor
void Display_Master_Comp(void)
{
    char string[64];

    if(display_title)
    {
        Gui_Draw_Button_Box(159, 6, 54, 16, "Threshold", BUTTON_NORMAL | BUTTON_DISABLED);
        Realslider_Size(159 + 54, 6, 50, (int) (mas_comp_threshold * 0.5f), TRUE);
        if(mas_comp_ratio <= 0.01f)
        {
            sprintf(string, "Off");
        }
        else
        {
            sprintf(string, "%d%%", (int) (mas_comp_threshold));
        }
        Print_String(string, 159 + 54, 8, 67, BUTTON_TEXT_CENTERED);

        Gui_Draw_Button_Box(283, 6, 41, 16, "Ratio", BUTTON_NORMAL | BUTTON_DISABLED);
        Realslider_Size(283 + 41, 6, 50, (int) (mas_comp_ratio * 0.5f), TRUE);
        if(mas_comp_ratio <= 0.01f)
        {
            sprintf(string, "Off");
        }
        else
        {
            sprintf(string, "%d%%", (int) (mas_comp_ratio));
        }
        Print_String(string, 283 + 41, 8, 67, BUTTON_TEXT_CENTERED);
    }
}
         
// ------------------------------------------------------
// Display master volume
void Display_Master_Volume(void)
{
    char tt[64];

    if(display_title)
    {
        if(mas_vol < 0.01f) mas_vol = 0.01f;
        if(mas_vol > 1.0f) mas_vol = 1.0f;
        Gui_Draw_Button_Box(394, 6, 44, 16, "Mst Vol.", BUTTON_NORMAL | BUTTON_DISABLED);
        Realslider(394 + 44, 6, (int) (mas_vol * 128.0f), TRUE);
        sprintf(tt, "%d%%", (int) (mas_vol * 100.0f));
        Print_String(tt, 394 + 44, 8, 145, BUTTON_TEXT_CENTERED);
    }
}
         
// ------------------------------------------------------
// Display the shuffle amount
void Display_Shuffle(void)
{
    char string[64];

    if(display_title)
    {
        if(shuffle > 100) shuffle = 100;
        if(shuffle < 0) shuffle = 0;
        Gui_Draw_Button_Box(586, 6, 40, 16, "Shuffle", BUTTON_NORMAL | BUTTON_DISABLED);

        Realslider_Size(586 + 40, 6, 100, shuffle, TRUE);
        sprintf(string, "%d%%", shuffle);
        Print_String(string, 586 + 40, 8, 116, BUTTON_TEXT_CENTERED);
    }
}

// ------------------------------------------------------
// Handle the mouse event of the top bar
void Mouse_Sliders_Master_Shuffle(void)
{
    if(display_title)
    {
        // Compressor threshold
        if(zcheckMouse(213, 6, 67, 18))
        {
            Mas_Compressor_Set_Variables((Mouse.x - 223.0f) * 2.0f, mas_comp_ratio);
            Display_Master_Comp();
        }

        // Compressor ratio
        if(zcheckMouse(324, 6, 67, 18))
        {
            Mas_Compressor_Set_Variables(mas_comp_threshold, (Mouse.x - 334.0f) * 2.0f);
            Display_Master_Comp();
        }

        // Master volume
        if(zcheckMouse(438, 6, 148, 18))
        {
            // Normalized
            mas_vol = (Mouse.x - 448.0f) / 128.0f;
            Display_Master_Volume();
        }

        // Shuffle
        if(zcheckMouse(586 + 40, 6, 120, 18))
        {
            shuffle = (int) ((Mouse.x - (586 + 40 + 10)));
            Display_Shuffle();
        }
    }
}

void Actualize_Master(char gode)
{
    if(gode == 0 || gode == 1)
    {
        if(BeatsPerMin < 32) BeatsPerMin = 32;
        if(BeatsPerMin > 255) BeatsPerMin = 255;
        Gui_Draw_Arrows_Number_Box(324, 62, BeatsPerMin, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    }

    if(gode == 0 || gode == 2)
    {
        if(TicksPerBeat < 1) TicksPerBeat = 1;
        if(TicksPerBeat > 16) TicksPerBeat = 16;
        if((TicksPerBeat + 1) < Ticks_Synchro_Left)
        {
            Ticks_Synchro_Left = TicksPerBeat + 1;
            Actualize_Fx_Ed(10);
            Actualize_Fx_Ed(11);
        }
        if((TicksPerBeat + 1) < Ticks_Synchro_Right)
        {
            Ticks_Synchro_Right = TicksPerBeat + 1;
            Actualize_Fx_Ed(10);
            Actualize_Fx_Ed(11);
        }
        Gui_Draw_Arrows_Number_Box2(324, 80, TicksPerBeat, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    }

    if(gode == 0)
    {
        Gui_Draw_Arrows_Number_Box2(324, 44, Songtracks, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    }

    if(gode == 4)
    {
        Gui_Draw_Arrows_Number_Box2(324, 44, Songtracks, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

        if(userscreen == USER_SCREEN_SEQUENCER) Actualize_Seq_Ed();

        SetColor(COL_BACKGROUND);
        bjbox(0, 186, fsize, 242);
        Actupated(0);
    }

    SamplesPerTick = (int) ((60 * SAMPLESPERSEC) / (BeatsPerMin * TicksPerBeat));  
    float SamplesPerBeat = (float) SAMPLESPERSEC / (((float) BeatsPerMin * 4) / 60);
    SamplesPerSub = SamplesPerTick / 6;

    if(userscreen == USER_SCREEN_SETUP_EDIT) Actualize_Master_Ed(3);
}

// ------------------------------------------------------
// Draw the stereo scope
typedef struct
{
    int nbr;
    int offset;
} POS_SCOPE, *LPPOS_SCOPE;

typedef struct
{
    int x_pos;
    int y_pos;
    int y_large;
    int x_max;
} DAT_POS_SCOPE, *LPDAT_POS_SCOPE;

POS_SCOPE Scope_Table[] =
{
    { 0, 0 },
    { 1, 0 },
    { 2, 1 },
    { 3, 3 },
    { 4, 6 },
    { 5, 10 },
    { 6, 15 },
    { 7, 21 },
    { 8, 28 },
    { 9, 36 },
    { 10, 45 },
    { 11, 55 },
    { 12, 66 },
    { 13, 78 },
    { 14, 91 },
    { 15, 105 },
    { 16, 120 },
};

DAT_POS_SCOPE Scope_Table_Dats[] =
{
    // ---
    {          394, 135 / 2, 135 / 2, 405 },                  // 0

    // ---
    {                   394, 135 / 2, 135 / 2, 405 / 2},      // 1
    { 394 + ((405 / 2) * 1), 135 / 2, 135 / 2, 405 / 2},      // 2

    // ---
    {                   394, 135 / 2, 135 / 2, 405 / 3},      // 3
    { 394 + ((405 / 3) * 1), 135 / 2, 135 / 2, 405 / 3},      // 4
    { 394 + ((405 / 3) * 2), 135 / 2, 135 / 2, 405 / 3},      // 5

    // ---
    {                   394, 135 / 2, 135 / 2, 405 / 4},      // 6
    { 394 + ((405 / 4) * 1), 135 / 2, 135 / 2, 405 / 4},      // 7
    { 394 + ((405 / 4) * 2), 135 / 2, 135 / 2, 405 / 4},      // 8
    { 394 + ((405 / 4) * 3), 135 / 2, 135 / 2, 405 / 4},      // 9

    // ---
    {             394, 135 / 2, 135 / 2, 405 / 5},            // 10
    { 394 + ((405 / 5) * 1), 135 / 2, 135 / 2, 405 / 5},      // 11
    { 394 + ((405 / 5) * 2), 135 / 2, 135 / 2, 405 / 5},      // 12
    { 394 + ((405 / 5) * 3), 135 / 2, 135 / 2, 405 / 5},      // 13
    { 394 + ((405 / 5) * 4), 135 / 2, 135 / 2, 405 / 5},      // 14

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 3},      // 15
    { 394 + ((405 / 3) * 1), 135 / 4, 135 / 4, 405 / 3},      // 16
    { 394 + ((405 / 3) * 2), 135 / 4, 135 / 4, 405 / 3},      // 17
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 18
    { 394 + ((405 / 3) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 19
    { 394 + ((405 / 3) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 20

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 4},      // 21
    { 394 + ((405 / 4) * 1), 135 / 4, 135 / 4, 405 / 4},      // 22
    { 394 + ((405 / 4) * 2), 135 / 4, 135 / 4, 405 / 4},      // 23
    { 394 + ((405 / 4) * 3), 135 / 4, 135 / 4, 405 / 4},      // 24
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 25
    { 394 + ((405 / 3) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 26
    { 394 + ((405 / 3) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 27

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 4},      // 28
    { 394 + ((405 / 4) * 1), 135 / 4, 135 / 4, 405 / 4},      // 29
    { 394 + ((405 / 4) * 2), 135 / 4, 135 / 4, 405 / 4},      // 30
    { 394 + ((405 / 4) * 3), 135 / 4, 135 / 4, 405 / 4},      // 31
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 32
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 33
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 34
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 35

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 5},      // 36
    { 394 + ((405 / 5) * 1), 135 / 4, 135 / 4, 405 / 5},      // 37
    { 394 + ((405 / 5) * 2), 135 / 4, 135 / 4, 405 / 5},      // 38
    { 394 + ((405 / 5) * 3), 135 / 4, 135 / 4, 405 / 5},      // 39
    { 394 + ((405 / 5) * 4), 135 / 4, 135 / 4, 405 / 5},      // 40
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 41
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 42
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 43
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 44

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 5},      // 45
    { 394 + ((405 / 5) * 1), 135 / 4, 135 / 4, 405 / 5},      // 46
    { 394 + ((405 / 5) * 2), 135 / 4, 135 / 4, 405 / 5},      // 47
    { 394 + ((405 / 5) * 3), 135 / 4, 135 / 4, 405 / 5},      // 48
    { 394 + ((405 / 5) * 4), 135 / 4, 135 / 4, 405 / 5},      // 49
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 50
    { 394 + ((405 / 5) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 51
    { 394 + ((405 / 5) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 52
    { 394 + ((405 / 5) * 3), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 53
    { 394 + ((405 / 5) * 4), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 54

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 4},      // 55
    { 394 + ((405 / 4) * 1), 135 / 6, 135 / 6, 405 / 4},      // 56
    { 394 + ((405 / 4) * 2), 135 / 6, 135 / 6, 405 / 4},      // 57
    { 394 + ((405 / 4) * 3), 135 / 6, 135 / 6, 405 / 4},      // 58

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 4},      // 59
    { 394 + ((405 / 4) * 1), (135 / 2) + 1, 135 / 6, 405 / 4},      // 60
    { 394 + ((405 / 4) * 2), (135 / 2) + 1, 135 / 6, 405 / 4},      // 61
    { 394 + ((405 / 4) * 3), (135 / 2) + 1, 135 / 6, 405 / 4},      // 62

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 3},      // 63
    { 394 + ((405 / 3) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 3},      // 64
    { 394 + ((405 / 3) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 3},      // 65

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 4},      // 66
    { 394 + ((405 / 4) * 1), 135 / 6, 135 / 6, 405 / 4},      // 67
    { 394 + ((405 / 4) * 2), 135 / 6, 135 / 6, 405 / 4},      // 68
    { 394 + ((405 / 4) * 3), 135 / 6, 135 / 6, 405 / 4},      // 69

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 4},      // 70
    { 394 + ((405 / 4) * 1), (135 / 2) + 1, 135 / 6, 405 / 4},      // 71
    { 394 + ((405 / 4) * 2), (135 / 2) + 1, 135 / 6, 405 / 4},      // 72
    { 394 + ((405 / 4) * 3), (135 / 2) + 1, 135 / 6, 405 / 4},      // 73

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 74
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 75
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 76
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 77

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 5},      // 78
    { 394 + ((405 / 5) * 1), 135 / 6, 135 / 6, 405 / 5},      // 79
    { 394 + ((405 / 5) * 2), 135 / 6, 135 / 6, 405 / 5},      // 80
    { 394 + ((405 / 5) * 3), 135 / 6, 135 / 6, 405 / 5},      // 81
    { 394 + ((405 / 5) * 4), 135 / 6, 135 / 6, 405 / 5},      // 82

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 4},      // 83
    { 394 + ((405 / 4) * 1), (135 / 2) + 1, 135 / 6, 405 / 4},      // 84
    { 394 + ((405 / 4) * 2), (135 / 2) + 1, 135 / 6, 405 / 4},      // 85
    { 394 + ((405 / 4) * 3), (135 / 2) + 1, 135 / 6, 405 / 4},      // 86

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 87
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 88
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 89
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 90

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 5},      // 91
    { 394 + ((405 / 5) * 1), 135 / 6, 135 / 6, 405 / 5},      // 92
    { 394 + ((405 / 5) * 2), 135 / 6, 135 / 6, 405 / 5},      // 93
    { 394 + ((405 / 5) * 3), 135 / 6, 135 / 6, 405 / 5},      // 94
    { 394 + ((405 / 5) * 4), 135 / 6, 135 / 6, 405 / 5},      // 95

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 5},      // 96
    { 394 + ((405 / 5) * 1), (135 / 2) + 1, 135 / 6, 405 / 5},      // 97
    { 394 + ((405 / 5) * 2), (135 / 2) + 1, 135 / 6, 405 / 5},      // 98
    { 394 + ((405 / 5) * 3), (135 / 2) + 1, 135 / 6, 405 / 5},      // 99
    { 394 + ((405 / 5) * 4), (135 / 2) + 1, 135 / 6, 405 / 5},      // 100

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 101
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 102
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 103
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 104

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 5},      // 105
    { 394 + ((405 / 5) * 1), 135 / 6, 135 / 6, 405 / 5},      // 106
    { 394 + ((405 / 5) * 2), 135 / 6, 135 / 6, 405 / 5},      // 107
    { 394 + ((405 / 5) * 3), 135 / 6, 135 / 6, 405 / 5},      // 108
    { 394 + ((405 / 5) * 4), 135 / 6, 135 / 6, 405 / 5},      // 109

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 5},      // 110
    { 394 + ((405 / 5) * 1), (135 / 2) + 1, 135 / 6, 405 / 5},      // 111
    { 394 + ((405 / 5) * 2), (135 / 2) + 1, 135 / 6, 405 / 5},      // 112
    { 394 + ((405 / 5) * 3), (135 / 2) + 1, 135 / 6, 405 / 5},      // 113
    { 394 + ((405 / 5) * 4), (135 / 2) + 1, 135 / 6, 405 / 5},      // 114

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 115
    { 394 + ((405 / 5) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 116
    { 394 + ((405 / 5) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 117
    { 394 + ((405 / 5) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 118
    { 394 + ((405 / 5) * 4), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 119

    // ---
    {                   394, 135 / 8, 135 / 8, 405 / 4},      // 120
    { 394 + ((405 / 4) * 1), 135 / 8, 135 / 8, 405 / 4},      // 121
    { 394 + ((405 / 4) * 2), 135 / 8, 135 / 8, 405 / 4},      // 122
    { 394 + ((405 / 4) * 3), 135 / 8, 135 / 8, 405 / 4},      // 123

    {                   394, (135 / 3) + 4, 135 / 8, 405 / 4},      // 125
    { 394 + ((405 / 4) * 1), (135 / 3) + 4, 135 / 8, 405 / 4},      // 126
    { 394 + ((405 / 4) * 2), (135 / 3) + 4, 135 / 8, 405 / 4},      // 127
    { 394 + ((405 / 4) * 3), (135 / 3) + 4, 135 / 8, 405 / 4},      // 128

    {                   394, (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 130
    { 394 + ((405 / 4) * 1), (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 131
    { 394 + ((405 / 4) * 2), (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 132
    { 394 + ((405 / 4) * 3), (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 133

    {                   394, (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 134
    { 394 + ((405 / 4) * 1), (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 135
    { 394 + ((405 / 4) * 2), (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 136
    { 394 + ((405 / 4) * 3), (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 137
};

char *table_channels_scopes[] =
{
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "L",
    "R",
};

void Draw_Scope(void)
{
    int x;
    int i;
    int pixel_color = COL_SCOPESSAMPLES;
    float pos_in_line;
    float datas;
    LPDAT_POS_SCOPE ptrTbl_Dat;

    if(Scopish == SCOPE_ZONE_SCOPE)
    {
        SetColor(COL_BACKGROUND);
        Fillrect(394, 42, 394 + 405, 42 + 135);

        if(Scopish_LeftRight)
        {
            // Left / Right
            ptrTbl_Dat = &Scope_Table_Dats[Scope_Table[2].offset];
            for(i = 0; i < Scope_Table[2].nbr; i++)
            {
                x = ptrTbl_Dat->x_pos;
                PrintXY(x + 4, 44 + (ptrTbl_Dat->y_pos - ptrTbl_Dat->y_large), USE_FONT_LOW, table_channels_scopes[i + 16]);

                for(int s = 0; s < ptrTbl_Dat->x_max; s++)
                {
                    pos_in_line = ((float) s) / ptrTbl_Dat->x_max;
                    datas = (Scope_Dats_LeftRight[i][(int) (pos_in_line * 128)] / 8192);
                    if(datas < -1.0f) datas = -1.0f;
                    if(datas > 1.0f) datas = 1.0f;
                    int y = 42 + ptrTbl_Dat->y_pos + (int) (datas * ptrTbl_Dat->y_large);
                    DrawPixel(x, y, pixel_color);
                    x++;
                }
                ptrTbl_Dat++;
                pixel_color = COL_SCOPESSAMPLES;
            }
        }
        else
        {
            // Channels
            ptrTbl_Dat = &Scope_Table_Dats[Scope_Table[Songtracks].offset];
            for(i = 0; i < Scope_Table[Songtracks].nbr; i++)
            {
                x = ptrTbl_Dat->x_pos;
                PrintXY(x + 4, 44 + (ptrTbl_Dat->y_pos - ptrTbl_Dat->y_large), USE_FONT_LOW, table_channels_scopes[i]);

                for(int s = 0; s < ptrTbl_Dat->x_max; s++)
                {
                    pos_in_line = ((float) s) / ptrTbl_Dat->x_max;
                    datas = (Scope_Dats[i][(int) (pos_in_line * 128)] / 8192) * mas_vol;
                    if(datas < -1.0f) datas = -1.0f;
                    if(datas > 1.0f) datas = 1.0f;
                    int y = 42 + ptrTbl_Dat->y_pos + (int) (datas * ptrTbl_Dat->y_large);
                    DrawPixel(x, y, pixel_color);
                    x++;
                }
                ptrTbl_Dat++;
                pixel_color = COL_SCOPESSAMPLES;
            }
        }
    }
}

void Draw_Scope_Files_Button(void)
{
    switch(Scopish)
    {
        case SCOPE_ZONE_SCOPE:
            SetColor(COL_BACKGROUND);
            bjbox(394, 42, 405, 135);
            bjbox(394, 24, 370, 17);
            Gui_Draw_Button_Box(394, 24, 350, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
            if(Scopish_LeftRight)
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "T", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "S", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            Gui_Draw_Button_Box(746, 24, 16, 16, "M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(764, 24, 16, 16, "I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(782, 24, 16, 16, "P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(764, 6, 16, 16, "In", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

            break;

        case SCOPE_ZONE_INSTR_LIST:
            SetColor(COL_BACKGROUND);
            bjbox(394, 42, 405, 135);
            bjbox(394, 24, 370, 17);
            Gui_Draw_Button_Box(394, 24, 350, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
            
            Actualize_Instruments_Synths_List(0);
            
            if(Scopish_LeftRight)
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "T", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "S", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }

            Gui_Draw_Button_Box(746, 24, 16, 16, "M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(764, 24, 16, 16, "I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(782, 24, 16, 16, "P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(764, 6, 16, 16, "In", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(782, 6, 16, 16, "Sy", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(394, 42, 16, 14, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(394, 162, 16, 14, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            break;

        case SCOPE_ZONE_SYNTH_LIST:
            SetColor(COL_BACKGROUND);
            bjbox(394, 42, 405, 135);
            bjbox(394, 24, 370, 17);
            Gui_Draw_Button_Box(394, 24, 350, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);

            Actualize_Instruments_Synths_List(0);

            if(Scopish_LeftRight)
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "T", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "S", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }

            Gui_Draw_Button_Box(746, 24, 16, 16, "M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(764, 24, 16, 16, "I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(782, 24, 16, 16, "P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(764, 6, 16, 16, "In", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(782, 6, 16, 16, "Sy", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(394, 42, 16, 14, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(394, 162, 16, 14, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            break;
        
        case SCOPE_ZONE_MOD_DIR:
        case SCOPE_ZONE_INSTR_DIR:
        case SCOPE_ZONE_PRESET_DIR:
            Read_SMPT();
            Dump_Files_List(413, 41);
            Actualize_Files_List(0);
            if(Scopish_LeftRight)
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "T", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(746, 6, 16, 16, "S", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }

            switch(Scopish)
            {
                case SCOPE_ZONE_MOD_DIR:
                    Gui_Draw_Button_Box(746, 24, 16, 16, "M", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 24, 16, 16, "I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 24, 16, 16, "P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 6, 16, 16, "In", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 6, 16, 16, "Sy", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    break;
                case SCOPE_ZONE_INSTR_DIR:
                    Gui_Draw_Button_Box(746, 24, 16, 16, "M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 24, 16, 16, "I", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 24, 16, 16, "P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 6, 16, 16, "In", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 6, 16, 16, "Sy", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    break;
                case SCOPE_ZONE_PRESET_DIR:
                    Gui_Draw_Button_Box(746, 24, 16, 16, "M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 24, 16, 16, "I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 24, 16, 16, "P", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 6, 16, 16, "In", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 6, 16, 16, "Sy", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    break;
            }
    
            Gui_Draw_Button_Box(394, 42, 16, 14, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(394, 162, 16, 14, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            break;
    }
}

// ------------------------------------------------------
// Free all allocated midi channels and sub channels
void Clear_Midi_Channels_Pool(void)
{
    int i;
    int j;

    for(j = 0; j < MAX_POLYPHONY; j++)
    {
        for(i = 0; i < MAX_TRACKS; i++)
        {
            Alloc_midi_Channels[i][j] = 0;
        }
    }
}

// ------------------------------------------------------
// Schedule a note
void Send_Note(int Note, int Raw_Note, int One_Channel)
{
    // Provision to send
    Keyboard_Events[Keyboard_Nbr_Events] = Note;
    Keyboard_Notes_Type[Keyboard_Nbr_Events] = Raw_Note;
    Keyboard_Notes_Bound[Keyboard_Nbr_Events] = One_Channel;
    Keyboard_Nbr_Events++;
    if(Keyboard_Nbr_Events >= 256) Keyboard_Nbr_Events = 0;
}

// ------------------------------------------------------
// Jazz a note man
void Note_Jazz(int track, int note)
{
    // Play the note
    int Sub_Channel = Get_Free_Sub_Channel(track, 16);
    if(Sub_Channel == -1)
    {
        // We didn't have enough channel
        Sub_Channel = 0;
    }
    Sub_Channels_Jazz[track][Sub_Channel].Note = ((note + 1) << 8);
    Sub_Channels_Jazz[track][Sub_Channel].Channel = track;
    Sub_Channels_Jazz[track][Sub_Channel].Sub_Channel = Sub_Channel;
    if(!is_editing || is_recording_2)
    {
        Play_Instrument(track, Sub_Channel,
                        (float) note,
                        ped_patsam,
                        CustomVol[ped_patsam],
                        0, 0, !is_recording, -(Sub_Channel + 1));
    }
}

// ------------------------------------------------------
// Jazz a note off man
void Note_Jazz_Off(int note)
{
    // Play a note off
    LPJAZZ_KEY Channel = Get_Jazz_Key_Off(Sub_Channels_Jazz, (note + 1) << 8);
    if(Channel)
    {
        if(!is_editing || is_recording_2)
        {
            Synthesizer[Channel->Channel][Channel->Sub_Channel].NoteOff();
            if(sp_Stage[Channel->Channel][Channel->Sub_Channel] == PLAYING_SAMPLE)
            {
                sp_Stage[Channel->Channel][Channel->Sub_Channel] = PLAYING_SAMPLE_NOTEOFF;
            }
            // That's a possibility
            noteoff303(Channel->Channel);
        }
        Channel->Note = 0;
        Channel->Channel = 0;
        Channel->Sub_Channel = 0;
    }

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
    if(!is_editing || is_recording_2)
    {
        Midi_NoteOff(ped_track, note);
    }
#endif
#endif

}
