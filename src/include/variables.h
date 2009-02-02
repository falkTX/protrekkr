// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#undef __STAND_ALONE__

// ------------------------------------------------------
// Includes
#include <SDL/SDL.h>

#include "../support/include/interface.h"
#include "../../release/distrib/replay/lib/include/replay.h"

// ------------------------------------------------------
// Constants
#define MAXLOOPTYPE 1
#define FULLYESNO 0

#if !defined(__WIN32__)
#define FALSE 0
#define TRUE 1
#endif

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

#define GUI_CMD_SELECT_DISKIO 60
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
#define GUI_CMD_FILELIST_SCROLL 100
#define GUI_CMD_UPDATE_LOOP_EDITOR_ED 101
#define GUI_CMD_SAVE_MODULE 102

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

#define GUI_CMD_PREVIOUS_16_INSTR 125
#define GUI_CMD_NEXT_16_INSTR 126

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
#define GUI_CMD_REFRESH_WAVE_ED 210
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

#define MIN_VUMETER 4
#define MAX_VUMETER 155
#define LARG_VUMETER 208
#define MIN_PEAK MAX_VUMETER - 12

#if defined(__LINUX__) || defined(__MACOSX__)
#define MAX_PATH 1024
#endif

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
extern long SamplesPerTick;

extern unsigned char *RawPatterns;
extern unsigned char nPatterns;
extern unsigned char sLength;
extern unsigned char pSequence[256];
extern short patternLines[128];
extern char nameins[128][20];
extern char Midiprg[128];
extern unsigned char Synthprg[128];
extern char SampleType[128][16];
extern char SampleCompression[128];
extern char SampleName[128][16][64];
extern char Basenote[128][16];
extern Uint32 LoopStart[128][16];
extern Uint32 LoopEnd[128][16];
extern char LoopType[128][16];
extern Uint32 SampleNumSamples[128][16];
extern char Finetune[128][16];
extern float SampleVol[128][16];
extern float FDecay[128][16];
extern short *RawSamples[128][2][16];
extern char SampleChannels[128][16];

extern float TCut[MAX_TRACKS];
extern float ICut[MAX_TRACKS];
extern float TPan[MAX_TRACKS];
extern int FType[MAX_TRACKS];
extern int FRez[MAX_TRACKS];
extern float DThreshold[MAX_TRACKS];
extern float DClamp[MAX_TRACKS];
extern float DSend[MAX_TRACKS];  
extern int CSend[MAX_TRACKS];

extern char compressor; // 0-->Off 1-->On
extern int c_threshold;
extern int BeatsPerMin;
extern int TicksPerBeat;
extern float mas_vol;
extern float mas_comp_threshold;
extern float mas_comp_ratio;
extern int delay_time;

extern float Feedback;
extern int DelayType;

extern int lchorus_delay;
extern int rchorus_delay;
extern float lchorus_feedback;
extern float rchorus_feedback;
extern int shuffle;

extern bool SACTIVE[256][16];
extern bool SHISTORY[256][16];
extern float CCoef[MAX_TRACKS];
extern int TRACKMIDICHANNEL[MAX_TRACKS];

extern char LFO_ON[MAX_TRACKS];
extern float LFO_RATE[MAX_TRACKS];
extern float LFO_AMPL[MAX_TRACKS];

extern char FLANGER_ON[MAX_TRACKS];
extern float FLANGER_AMOUNT[MAX_TRACKS];
extern float FLANGER_DEPHASE[MAX_TRACKS];
extern float FLANGER_RATE[MAX_TRACKS];
extern float FLANGER_AMPL[MAX_TRACKS];
extern float FLANGER_GR[MAX_TRACKS];
extern float FLANGER_FEEDBACK[MAX_TRACKS];
extern int FLANGER_DELAY[MAX_TRACKS];
extern int FLANGER_OFFSET[MAX_TRACKS];

extern float foff2[MAX_TRACKS];
extern float foff1[MAX_TRACKS];

extern int TRACKSTATE[MAX_TRACKS]; // 0->Normal 1->Muted

extern char Songtracks;
extern bool Disclap[MAX_TRACKS];

extern char cur_input_name[1024];

extern char artist[20];
extern char style[20];

extern bool beatsync[128];
extern short beatlines[128];

extern float REVERBFILTER;

extern float CustomVol[128];

extern int ped_track;

extern int ped_patsam;
extern int ped_row;
extern int ped_line;
extern int ped_line_delay;
extern int cPosition;
extern int cPosition_delay;
extern int gui_track;

extern unsigned int lchorus_counter;
extern unsigned int rchorus_counter;
extern unsigned int lchorus_counter2;
extern unsigned int rchorus_counter2;

extern char userscreen;

extern int snamesel;

extern int last_index;

extern int CONSOLE_WIDTH;
extern int CONSOLE_HEIGHT;
extern int Scopish;

extern int Pos_Tbl_Synth_OSC1;
extern int Pos_Tbl_Synth_OSC2;
extern int Pos_Tbl_Synth_VCF;
extern int Pos_Tbl_Synth_LFO1;
extern int Pos_Tbl_Synth_LFO2;
extern int Pos_Tbl_Synth_ENV1;
extern int Pos_Tbl_Synth_ENV2;

extern bool draw_sampled_wave;
extern bool draw_sampled_wave2;
extern bool draw_sampled_wave3;
extern char ped_split;
extern int resty;
extern int rs_coef;

extern SDL_Surface *Main_Screen;
extern int CONSOLE_HEIGHT2;

extern float decays[MAX_COMB_FILTERS][2];
extern int delays[MAX_COMB_FILTERS]; // delays for the comb filters
extern int counters[MAX_COMB_FILTERS];

extern float delay_left_buffer[MAX_COMB_FILTERS][100000];
extern float delay_right_buffer[MAX_COMB_FILTERS][100000];

extern float delay_left_final;
extern float delay_right_final;

extern char num_echoes;
extern int rev_counter;

extern float allBuffer_L[5760];
extern float allBuffer_L2[5760];
extern float allBuffer_L3[5760];
extern float allBuffer_L4[5760];
extern float allBuffer_L5[5760];
extern float allBuffer_L6[5760];

extern int delayedCounter;
extern int delayedCounter2;
extern int delayedCounter3;
extern int delayedCounter4;
extern int delayedCounter5;
extern int delayedCounter6;

extern int currentCounter;

extern float left_float;
extern float right_float;

extern unsigned char track3031;
extern unsigned char track3032;

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

extern bool trkchan;

extern char seditor;
extern char actuloop;

extern char visiblecolums;
extern int VIEWLINE;
extern int VIEWLINE2;
extern int YVIEW; 
extern char is_editing;
extern char is_recording;
extern char is_recording_2;
extern char is_record_key;

extern int block_start_track;
extern int block_start_track_nibble;
extern int save_block_start_track_nibble;
extern int block_end_track;
extern int block_end_track_nibble;
extern int block_start;
extern int block_end;
extern int block_in_selection;

extern int swap_block_start_track;
extern int swap_block_start_track_nibble;
extern int swap_block_end_track;
extern int swap_block_end_track_nibble;
extern int swap_block_start;
extern int swap_block_end;

extern char sr_isrecording;

extern int Songplaying;

extern int ped_pattad;
extern int ped_patoct;

extern int restx;
extern int resty;
extern int fsize;

extern unsigned int SamplesPerSub;

// Render as .wav
extern bool rawrender;

extern bool sas;

extern float Scope_Dats[MAX_TRACKS][128];
extern float Scope_Dats_LeftRight[2][128];

extern int Asking_Exit;

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

extern Uint32 Alloc_midi_Channels[MAX_TRACKS];

extern char *ExePath;

extern int allow_save;

// ------------------------------------------------------
// Functions
void ResetFilters(char tr);
void MidiReset(void);
void MidiAllNotesOff(void);
void WavRenderizer(void);
void DoEffects_tick0(void);
void DoEffects(void);
void Newmod(void);
void GetBackMouse(void);
void SeqFill(int st, int en, bool n);
void PutBackMouse(void);
void Read_SMPT(void);
void DumpList(int xr, int yr, int y);
void Initreverb();
void ltActualize(int modeac);
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
void GetPlayerValues(float master_coef);
void Check_Loops(void);
void Skincopy(int xd, int yd, int xs, int ys, int w, int h);
void Go303(void);
void knob(int x, int y, unsigned char number);
void Fire303(unsigned char number, char unit);
void number303(unsigned char number, int x, int y);
void LoadRebirthMod(void);
void noteoff303(char strack);
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
void StopIfSp(void);
void draw_lfoed(void);
void draw_pated_highlight(int track, int line, int petrack, int row);
void draw_tracked(void);
void DeleteInstrument(void);
void Sp_Player(void);             
void Sp_Playwave(int channel, float note, int sample, float vol, unsigned int offset, int glide, int Play_Entire);
void Pre_Song_Init(void);
void Compressor_work(void);
void StartRec(void);
void StartEdit(void);
void ComputeCoefs(int freq, int r, int t);
void blitnote(int x, int y, int note, int y1, int y2);
void Letter(int x, int y, char ltr, int ys, int y2);
void live303(int pltr_eff_row, int pltr_dat_row);
float Filter(float x, char i);
float Kutoff(int v);
float Reonance(float v);
float Bandwidth(int v);
float ApplyLfo(float cy, char trcy);
int f2i(float d);
int GetFreeWave(void);
char zcheckMouse_nobutton(int x, int y, int xs, int ys);
char zcheckMouse(int x, int y, int xs, int ys);
float int_filter2p(char ch, float input, float f, float q, float q2);
float filter2p(char ch, float input, float f, float q);
float filterhp(char ch, float input, float f, float q);
float filterhp2(char ch, float input, float f, float q);
float filter2px(char ch, float input, float f, float q);
float filter2p24d(char ch, float input, float f, float q);
float filterRingMod(char ch, float input, float f, float q);
float filterRingModStereo(char ch, float input);
float filterWater(char ch, float input, float f, float q);
float filterWaterStereo(char ch ,float input, float f, float q);
float filterBellShaped(char ch, float input, float f, float q, float g);
float filterDelta(char ch, float input, float f, float q);
//float filterDeltaStereo(char ch, float input, float f, float q);
void Mas_Compressor_Set_Variables(float treshold, float ratio);

void FadeToBlack(void);
void IniCsParNames(void);
void UpSynth(int peac,int number);
void allPassInit(float miliSecs);
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
void Mark_Block_End(int start_nibble, int start_track, int start_line, int Modif);
int Delete_Selection(int Position);
void Copy_Selection(int Position);
void Cut_Selection(int Position);
void Interpolate_Block(int Position);
void Paste_Block(int Position);
void Randomize_Block(int Position);
void Seminote_Up_Block(int Position);
void Seminote_Down_Block(int Position);
void Instrument_Seminote_Up_Block(int Position);
void Instrument_Seminote_Down_Block(int Position);
void Select_Track_Block(void);
void Select_Pattern_Block(void);
void Unselect_Selection(void);
void Select_Block_Keyboard(int Type);
void Insert_Track_Line(int track, int Position);
void Insert_Pattern_Line(int Position);
void Remove_Track_Line(int track, int Position);
void Remove_Pattern_Line(int Position);
void Calc_selection(void);
void Next_Line_Pattern_Auto(void);
int Get_Free_Channel(void);
int Search_Corresponding_Channel(Uint32 Datas);
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
