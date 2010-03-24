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

#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#undef __STAND_ALONE__

// ------------------------------------------------------
// Includes
#include <SDL/SDL.h>

#include "../support/include/main.h"
#include "../../release/distrib/replay/lib/include/replay.h"

// ------------------------------------------------------
// Constants
#define MAXLOOPTYPE 1
#define FULLYESNO 0

#if !defined(__WIN32__) && !defined(__AROS__)
#define FALSE 0
#define TRUE 1
#endif

#define MAX_TRACKS_LARGE 10
#define MAX_TRACKS_NORMAL 14

#define GUI_CMD_NONE 0

#define GUI_CMD_SET_FILES_LIST_SLIDER 1
#define GUI_CMD_SET_FILES_LIST_SELECT_FILE 2

#define GUI_CMD_PREVIOUS_POSITION 3
#define GUI_CMD_NEXT_POSITION 4
#define GUI_CMD_SET_PATTERN_LENGTH 5
#define GUI_CMD_PREV_PATT 7
#define GUI_CMD_NEXT_PATT 8
#define GUI_CMD_REDUCE_SONG_LENGTH 9
#define GUI_CMD_INCREASE_SONG_LENGTH 10
#define GUI_CMD_UPDATE_SEQUENCER 11
#define GUI_CMD_SET_FOCUS_TRACK 15
#define GUI_CMD_PREV_INSTR 25
#define GUI_CMD_NEXT_INSTR 26
#define GUI_CMD_SWITCH_TRACK_MUTE_STATE 27
#define GUI_CMD_PLAY_SONG 28
#define GUI_CMD_STOP_SONG 29
#define GUI_CMD_RECORD_303 30
#define GUI_CMD_CHANGE_BPM_TICKS_NBR 40
#define GUI_CMD_CHANGE_TRACKS_NBR 41
#define GUI_CMD_SET_TRACK_CUTOFF_FREQ 50
#define GUI_CMD_SET_TRACK_RESONANCE 51
#define GUI_CMD_SET_TRACK_INERTIA 52
#define GUI_CMD_UPDATE_TRACK_ED 53
#define GUI_CMD_SET_TRACK_THRESHOLD 54
#define GUI_CMD_SET_TRACK_CLAMP 55
#define GUI_CMD_SET_TRACK_REVERB_SEND 56
#define GUI_CMD_SET_TRACK_PANNING 57
#define GUI_CMD_CALC_LENGTH 58

#define GUI_CMD_SELECT_REVERB_EDIT 59
#define GUI_CMD_SELECT_DISKIO_EDIT 60
#define GUI_CMD_SELECT_TRACK_EDIT 61
#define GUI_CMD_SELECT_INSTRUMENT_EDIT 62
#define GUI_CMD_SELECT_FX_EDIT 63
#define GUI_CMD_SELECT_SEQUENCER 64
#define GUI_CMD_SELECT_SCREEN_SETUP_EDIT 65
#define GUI_CMD_SELECT_SYNTH_EDIT 66
#define GUI_CMD_SELECT_TRACK_FX_EDIT 67
#define GUI_CMD_SELECT_SAMPLE_EDIT 68
#define GUI_CMD_SMALL_EDIT 69
#define GUI_CMD_SAVE_INSTRUMENT 70
#define GUI_CMD_MODULE_INFOS 71
#define GUI_CMD_SELECT_TB303_EDIT 72
#define GUI_CMD_REFRESH_TB303_PARAMS 73

#define GUI_CMD_SELECT_LARGE_PATTERNS 74

#define GUI_CMD_SWITCH_TRACK_LARGE_STATE 75
#define GUI_CMD_SWITCH_TRACK_SMALL_STATE 76

#define GUI_CMD_EXPORT_WAV 77

#define GUI_CMD_SET_FILES_LIST_PLAY_WAV 78

#define GUI_CMD_FILELIST_SCROLL 100
#define GUI_CMD_UPDATE_LOOP_EDITOR_ED 101
#define GUI_CMD_SAVE_MODULE 102

#define GUI_CMD_INSTR_SYNTH_SCROLL 103

#define GUI_CMD_SET_INSTRUMENT_AMPLI 104
#define GUI_CMD_SET_INSTRUMENT_FINETUNE 105
#define GUI_CMD_SET_INSTRUMENT_DECAY 106

#define GUI_CMD_UPDATE_INSTRUMENT_ED 107
#define GUI_CMD_UPDATE_DISKIO_ED 108

#define GUI_CMD_NEW_MODULE 109
#define GUI_CMD_UPDATE_SEQ_ED 110
#define GUI_CMD_UPDATE_FX_ED 112
#define GUI_CMD_CHANGE_SHUFFLE 113
#define GUI_CMD_UPDATE_SETUP_ED 114

#define GUI_CMD_REDUCE_PATTERNS_10 115
#define GUI_CMD_INCREASE_PATTERNS_10 116

#define GUI_CMD_SAVE_FINAL 117
#define GUI_CMD_CALC_FINAL 118
#define GUI_CMD_SAVE_303_PATTERN 119

#define GUI_CMD_SET_INSTR_SYNTH_LIST_SLIDER 120
#define GUI_CMD_SET_INSTR_SYNTH_LIST_SELECT 121

#define GUI_CMD_UPDATE_REVERB_ED 122

#define GUI_CMD_PREVIOUS_16_INSTR 125
#define GUI_CMD_NEXT_16_INSTR 126

#define GUI_CMD_SAVE_REVERB 127
#define GUI_CMD_SAVE_PATTERN 128

#define GUI_CMD_UPDATE_SYNTH_ED 150

#define GUI_CMD_MIDI_NOTE_OFF_1_TRACK 151
#define GUI_CMD_MIDI_NOTE_OFF_ALL_TRACKS 152

#define GUI_CMD_UPDATE_TRACK_FX_ED 154
#define GUI_CMD_UPDATE_MIDI_303_ED 155

#define GUI_CMD_RENDER_WAV 169

#define GUI_CMD_REDUCE_POSITIONS_10 170
#define GUI_CMD_INCREASE_POSITIONS_10 171

#define GUI_CMD_INCREASE_STEP_ADD 172
#define GUI_CMD_DECREASE_STEP_ADD 173

#define GUI_CMD_UPDATE_PATTERN_ED 174

#define GUI_CMD_HIGHER_OCTAVE 175
#define GUI_CMD_LOWER_OCTAVE 176

#define GUI_CMD_TIMED_REFRESH_SEQUENCER 177

#define GUI_CMD_DELETE_INSTRUMENT 201
#define GUI_CMD_SAVE_SYNTH 202
#define GUI_CMD_PATTERNS_POOL_EXHAUSTED 204
#define GUI_CMD_TURN_SCOPES_ON 205
#define GUI_CMD_RAND_SYNTH 206
#define GUI_CMD_REFRESH_SAMPLE_ED 210
#define GUI_CMD_RECORD_MODE 251
#define GUI_CMD_EDIT_MODE 252
#define GUI_CMD_ASK_EXIT 253
#define GUI_CMD_EXIT 254

#define GUI_CMD_NOP 255

#define USER_SCREEN_DISKIO_EDIT 0
#define USER_SCREEN_TRACK_EDIT 1
#define USER_SCREEN_INSTRUMENT_EDIT 2
#define USER_SCREEN_FX_SETUP_EDIT 3
#define USER_SCREEN_SEQUENCER 4
#define USER_SCREEN_SETUP_EDIT 5
#define USER_SCREEN_SYNTH_EDIT 6
#define USER_SCREEN_TRACK_FX_EDIT 7
#define USER_SCREEN_SAMPLE_EDIT 8
#define USER_SCREEN_TB303_EDIT 9
#define USER_SCREEN_REVERB_EDIT 10
#define USER_SCREEN_LARGE_PATTERN 11

#define INPUT_NONE 0
#define INPUT_MODULE_NAME 1
#define INPUT_INSTRUMENT_NAME 2
#define INPUT_SYNTH_NAME 3
#define INPUT_MODULE_ARTIST 4
#define INPUT_MODULE_STYLE 5
#define INPUT_303_PATTERN 6
#define INPUT_REVERB_NAME 7
#define INPUT_SELECTION_NAME 8

#define MARKER_START_SET 1
#define MARKER_END_SET 2

#define LIVE_PARAM_TRACK_CUTOFF 1
#define LIVE_PARAM_TRACK_RESONANCE 2
#define LIVE_PARAM_TRACK_REVERB_SEND 3
#define LIVE_PARAM_TRACK_THRESHOLD 4
#define LIVE_PARAM_TRACK_CLAMP 5
#define LIVE_PARAM_TRACK_PANNING 6
#define LIVE_PARAM_303_1_CUTOFF 7
#define LIVE_PARAM_303_2_CUTOFF 8
#define LIVE_PARAM_303_1_RESONANCE 9
#define LIVE_PARAM_303_2_RESONANCE 10
#define LIVE_PARAM_303_1_ENVMOD 11
#define LIVE_PARAM_303_2_ENVMOD 12
#define LIVE_PARAM_303_1_DECAY 13
#define LIVE_PARAM_303_2_DECAY 14
#define LIVE_PARAM_303_1_TUNE 15
#define LIVE_PARAM_303_2_TUNE 16
#define LIVE_PARAM_303_1_ACCENT 17
#define LIVE_PARAM_303_2_ACCENT 18

#define SCOPE_ZONE_SCOPE 0
#define SCOPE_ZONE_MOD_DIR 1
#define SCOPE_ZONE_INSTR_DIR 2
#define SCOPE_ZONE_PRESET_DIR 3
#define SCOPE_ZONE_INSTR_LIST 4
#define SCOPE_ZONE_SYNTH_LIST 5
#define SCOPE_ZONE_REVERB_DIR 6
#define SCOPE_ZONE_PATTERN_DIR 7
#define SCOPE_ZONE_SAMPLE_DIR 8

#define MIN_VUMETER 22
#define MAX_VUMETER 155
#define LARG_VUMETER 134
#define MIN_PEAK MAX_VUMETER - 12

#define MAX_PATH 260

#define ZZAAPP_ALL 1
#define ZZAAPP_PATTERNS 2
#define ZZAAPP_INSTRUMENTS 3
#define ZZAAPP_SYNTHS 4
#define ZZAAPP_303 5

// ------------------------------------------------------
// Structures
typedef struct
{
    int Start_Row;
    int Start_Col;
    int End_Row;
    int End_Col;
    int cStatus;
} MARKER, *LPMARKER;

// ------------------------------------------------------
// Variables
extern int SamplesPerTick;

extern char cur_input_name[1024];

extern char artist[20];
extern char style[20];

extern int ped_track;

extern int ped_patsam;
extern int ped_col;
extern int ped_line;
extern int ped_line_delay;
extern int cPosition;
extern int cPosition_delay;
extern int gui_track;

extern char userscreen;

extern int snamesel;

extern int last_index;

extern int CONSOLE_WIDTH;
extern int CHANNELS_WIDTH;
extern int CONSOLE_HEIGHT;
extern int Scopish;

extern int Pos_Tbl_Synth_OSC1;
extern int Pos_Tbl_Synth_OSC2;
extern int Pos_Tbl_Synth_VCF;
extern int Pos_Tbl_Synth_LFO1;
extern int Pos_Tbl_Synth_LFO2;
extern int Pos_Tbl_Synth_ENV1;
extern int Pos_Tbl_Synth_ENV2;

extern char draw_sampled_wave;
extern char draw_sampled_wave2;
extern char draw_sampled_wave3;
extern char ped_split;
extern int resty;
extern int rs_coef;

extern SDL_Surface *Main_Screen;
extern int CONSOLE_HEIGHT2;

extern float left_float_render;
extern float right_float_render;

extern SDL_Surface *PFONT;
extern SDL_Surface *FONT;
extern SDL_Surface *FONT_LOW;

extern int gui_action;
extern char teac;

extern MOUSE Mouse;

extern int fluzy;

extern signed char c_midiin;
extern signed char c_midiout;

extern int namesize;

extern int liveparam;
extern int livevalue;

extern int Final_Mod_Length;

extern char name[MAX_PATH];
extern char artist[20];

extern char style[20];

extern char trkchan;

extern char seditor;
extern char actuloop;

extern char Visible_Columns;
extern int VIEWLINE;
extern int VIEWLINE2;
extern int YVIEW; 
extern char is_editing;
extern char is_recording;
extern char is_recording_2;
extern char is_record_key;

extern char sr_isrecording;

extern int Songplaying;

extern int ped_pattad;
extern int ped_patoct;

extern int restx;
extern int resty;
extern int fsize;

extern unsigned int SamplesPerSub;

// Render as .wav
extern char rawrender;
extern char rawrender_32float;
extern int rawrender_range;
extern int rawrender_from;
extern int rawrender_to;

extern char sas;

extern float Scope_Dats[MAX_TRACKS][128];
extern float Scope_Dats_LeftRight[2][128];

extern SDL_Surface *SKIN303;
extern SDL_Surface *LOGOPIC;
extern int LOGO_WIDTH;
extern int LOGO_HEIGHT;
extern int LOGO_BKCOL;

extern int MouseWheel_Multiplier;
extern char Rows_Decimal;
extern char See_Prev_Next_Pattern;

extern MARKER Patterns_Marker;

extern int LastProgram[MAX_TRACKS];

extern Uint32 Alloc_midi_Channels[MAX_TRACKS][MAX_POLYPHONY];

extern char *ExePath;

extern int allow_save;

extern char Channels_Polyphony[MAX_TRACKS];
extern char Channels_MultiNotes[MAX_TRACKS];

// ------------------------------------------------------
// Functions
void ResetFilters(int tr);
void WavRenderizer(void);
void DoEffects_tick0(void);
void DoEffects_tick0_b(void);
void DoEffects(void);
void Newmod(void);
void GetBackMouse(void);
void SeqFill(int st, int en, char n);
void PutBackMouse(void);
void Read_SMPT(void);
void Initreverb();
void init_sample_bank(void);
void AllocateWave(int n_index, long lenfir, int samplechans);
void LoadFile(int Freeindex, const char *str);
void out_hexchar(int x,int y, int number, char smith, int color);
void out_note(int x,int y, int note, int color);
void RefreshSample(void);
void value_box(int x, int y, int val, int flags);
void value_box3(int x, int y, char val, int flags);
void value_box4(int x, int y, char val);
void Actualize_Track_Ed(char gode);
void Actualize_Songname(int *newletter, char *nam);
void GetPlayerValues(void);
void Check_Loops(void);
void Skincopy(int xd, int yd, int xs, int ys, int w, int h);
void Go303(void);
void knob(int x, int y, unsigned char number);
void ShowInfo(void);
void guiDial2(const char *str);

void out_decchar(int x, int y, int number, char smith);

void out_nibble(int x, int y, int color, int number);
void SongPlay(void);
void SongStop(void);
void Free_Samples(void);
void draw_pated(int track, int line, int petrack, int row);
void Actupated(int modac);

void KillInst(int inst_nbr);
void Stop_Current_Sample(void);
void draw_lfoed(void);
void draw_tracked(void);
void DeleteInstrument(void);
void Sp_Player(void);             
void Pre_Song_Init(void);
void Compressor_work(void);
void StartRec(void);
void StartEdit(void);
void ComputeCoefs(int freq, int r, int t);
void live303(int pltr_eff_row, int pltr_dat_row);
float Filter(float x, char i);
float Kutoff(int v);
float Reonance(float v);
float Bandwidth(int v);
float ApplyLfo(float cy, char trcy);
int GetFreeWave(void);
char zcheckMouse_nobutton(int x, int y, int xs, int ys);
char zcheckMouse(int x, int y, int xs, int ys);

void FadeToBlack(void);
void IniCsParNames(void);
void UpSynth(int peac,int number);
void ComputeStereo(char channel);
void Keyboard_Handler(void);
void Mouse_Handler(void);
int Get_Track_Over_Mouse(void);
int Get_Line_Over_Mouse(void);
int Get_Column_Over_Mouse(void);
void Set_Track_Slider(int pos);
#define BLOCK_MARK_TRACKS 1
#define BLOCK_MARK_ROWS 2
void Mark_Block_Start(int start_nibble, int start_track, int start_line);
void Mark_Block_End(int end_nibble, int start_track, int start_line, int Modif);
int Delete_Selection(int Position);
void Insert_Selection(int Cur_Track, int Position);
void Remove_Selection(int Cur_Track, int Position);
void Copy_Selection(int Position);
void Cut_Selection(int Position);
void Interpolate_Block(int Position);
void Randomize_Block(int Position);
void Semitone_Up_Block(int Position);
void Semitone_Down_Block(int Position);
void Instrument_Semitone_Up_Block(int Position);
void Instrument_Semitone_Down_Block(int Position);
void Unselect_Selection(void);
void Select_Block_Keyboard(int Type);
void Calc_selection(void);
int Next_Line_Pattern_Auto(int *position, int lines, int *line);
int Get_Free_Midi_Sub_Channel(int track);
int Search_Corresponding_Midi_Sub_Channel(int track, Uint32 Datas);
int Get_Midi_Channel(int midi_channel);
void Draw_Scope_Files_Button(void);

void Actualize_Master(char gode);
void Display_Master_Comp(void);
void Display_Master_Volume(void);
void Display_Shuffle(void);
void Actualize_Input(void);
void Draw_Scope(void);

void Remove_Title(void);

#endif
