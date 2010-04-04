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

// TODO: Maybe this file should be splitted.

// ------------------------------------------------------
// Includes
#include "../extralibs/zlib-1.2.3/zlib.h"

#include "../include/ptk.h"
#include "include/files.h"
#include "include/samples_pack.h"
#include "../ui/include/misc_draw.h"
#include "../editors/include/editor_synth.h"
#include "../editors/include/editor_diskio.h"
#include "../editors/include/editor_reverb.h"
#include "../editors/include/editor_instrument.h"
#include "../editors/include/editor_sequencer.h"
#include "../editors/include/editor_setup.h"
#include "../editors/include/editor_303.h"
#include "../editors/include/editor_sample.h"
#include "../editors/include/editor_fx_setup.h"
#include "../editors/include/editor_track_fx.h"
#include "../editors/include/editor_track.h"
#include "../editors/include/editor_pattern.h"

#include "../../release/distrib/replay/lib/include/endianness.h"

// ------------------------------------------------------
// Structures
typedef struct
{
    int old_note;
    int new_note;
} AMIGA_NOTE, *LPAMIGA_NOTE;

typedef struct
{
   int old_order;
   int new_order;
} INSTR_ORDER, *LPINSTR_ORDER;

#if !defined(__GCC__)
#pragma pack(push)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif
typedef struct
{
   Uint8 Pos;
   Uint8 Row;
   Uint8 Data;
} SYNC_FX, *LPSYNC_FX;
#pragma pack(pop)

// ------------------------------------------------------
// Variables
#if !defined(__WINAMP__)
extern REQUESTER Overwrite_Requester;
extern char OverWrite_Name[1024];
#endif

extern SynthParameters PARASynth[128];

extern char Paste_Across;
extern char Use_Cubic;
extern gear303 tb303engine[2];
extern para303 tb303[2];
extern int patt_highlight;
extern char FullScreen;
extern char AutoSave;
extern int Beveled;
extern char Use_Shadows;
extern char Global_Patterns_Font;
extern int Continuous_Scroll;
extern char *cur_dir;
extern char Scopish_LeftRight;
extern char Jazz_Edit;
extern char Accidental;

int Mod_Length;
int Mod_Simulate;
Uint8 *Mod_Memory;
int Mod_Mem_Pos;
int Final_Mod_Length;
extern int wait_AutoSave;

int mt_tags[13] =
{
    '.K.M', 'NHC5', 'NHC6', 'NHC7',
    'NHC8', 'NHC9', 'HC01', 'HC11',
    'HC21', 'HC31', 'HC41', 'HC51',
    'HC61'
};

int mt_channels[13] =
{
    4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
};

float mt_pannels[16] =
{
    0.35f, 0.65f, 0.65f, 0.35f,
    0.35f, 0.65f, 0.65f, 0.35f,
    0.35f, 0.65f, 0.65f, 0.35f,
    0.35f, 0.65f, 0.65f, 0.35f
};

AMIGA_NOTE mt_period_conv[] =
{
    { 13696, 0 },
    { 12928, 1 },
    { 12192, 2 },
    { 11520, 3 },
    { 10848, 4 },
    { 10240, 5 },
    { 9664,  6 },
    { 9120,  7 },
    { 8608,  8 },
    { 8128,  9 },
    { 7680, 10 },
    { 7248, 11 },

    { 6848, 12 },
    { 6464, 13 },
    { 6096, 14 },
    { 5760, 15 },
    { 5424, 16 },
    { 5120, 17 },
    { 4832, 18 },
    { 4560, 19 },
    { 4304, 20 },
    { 4064, 21 },
    { 3840, 22 },
    { 3624, 23 },

    { 3424, 24 },
    { 3232, 25 },
    { 3048, 26 },
    { 2880, 27 },
    { 2712, 28 },
    { 2560, 29 },
    { 2416, 30 },
    { 2280, 31 },
    { 2152, 32 },
    { 2032, 33 },
    { 1920, 34 },
    { 1812, 35 },

    { 1712, 36 },
    { 1616, 37 },
    { 1524, 38 },
    { 1440, 39 },
    { 1356, 40 },
    { 1280, 41 },
    { 1208, 42 },
    { 1140, 43 },
    { 1076, 44 },
    { 1016, 45 },
    {  960, 46 },
    {  906, 47 },

    {  856, 48 },
    {  808, 49 },
    {  762, 50 },
    {  720, 51 },
    {  678, 52 },
    {  640, 53 },
    {  604, 54 },
    {  570, 55 },
    {  538, 56 },
    {  508, 57 },
    {  480, 58 },
    {  453, 59 },
    
    {  428, 60 },
    {  404, 61 },
    {  381, 62 },
    {  360, 63 },
    {  339, 64 },
    {  320, 65 },
    {  302, 66 },
    {  285, 67 },
    {  269, 68 },
    {  254, 69 },
    {  240, 70 },
    {  226, 71 },
    
    {  214, 72 },
    {  202, 73 },
    {  190, 74 },
    {  180, 75 },
    {  170, 76 },
    {  160, 77 },
    {  151, 78 },
    {  143, 79 },
    {  135, 80 },
    {  127, 81 },
    {  120, 82 },
    {  113, 83 },

    {  107, 84 },
    {  101, 85 },
    {   95, 86 },
    {   90, 87 },
    {   85, 88 },
    {   80, 89 },
    {   75, 90 },
    {   71, 91 },
    {   67, 92 },
    {   63, 93 },
    {   60, 94 },
    {   57, 95 },

    {   53, 96 },
    {   50, 97 },
    {   47, 98 },
    {   45, 99 },
    {   42, 100 },
    {   40, 101 },
    {   38, 102 },
    {   36, 103 },
    {   34, 104 },
    {   32, 105 },
    {   30, 106 },
    {   28, 107 },

    {   26, 108 },
    {   25, 109 },
    {   23, 110 },
    {   22, 111 },
    {   21, 112 },
    {   20, 113 },
    {   18, 114 },
    {   17, 115 },
    {   16, 116 },
    {   15, 117 },
    {   15, 118 },
    {   14, 119 },

}; 

INSTR_ORDER Used_Instr[MAX_INSTRS];
INSTR_ORDER Used_Instr2[MAX_INSTRS];
INSTR_ORDER Used_Instr3[MAX_INSTRS];
unsigned char Old_pSequence[MAX_SEQUENCES];
int nbr_User_Instr;

int Muted_Tracks[MAX_TRACKS];
int nbr_muted;
FILE *Out_constants;

FILE *Out_FX;
int Rec_Fx = 0;
LPSYNC_FX Sync_Fx;

// ------------------------------------------------------
// Functions
int Read_Mod_Data(void *Datas, int Unit, int Length, FILE *Handle);
int Read_Mod_Data_Swap(void *Datas, int Unit, int Length, FILE *Handle);
int Write_Mod_Data(void *Datas, int Unit, int Length, FILE *Handle);
int Write_Mod_Data_Swap(void *Datas, int Unit, int Length, FILE *Handle);
void Pack_Sample(FILE *FileHandle, short *Sample, int Size, char Pack_Type, int BitRate);
short *Unpack_Sample(FILE *FileHandle, int Dest_Length, char Pack_Type, int BitRate);
void Read_Synth_Params(int (*Read_Function)(void *, int ,int, FILE *),
                       int (*Read_Function_Swap)(void *, int ,int, FILE *),
                       FILE *in,
                       int idx,
                       int read_disto,
                       int read_lfo_adsr,
                       int new_version,
                       int Env_Modulation,
                       int New_Env,
                       int Ntk_Beta);
void Write_Synth_Params(int (*Write_Function)(void *, int ,int, FILE *),
                        int (*Write_Function_Swap)(void *, int ,int, FILE *),
                        FILE *in,
                        int idx);
void Swap_Short_Buffer(short *buffer, int size);
short *Swap_New_Sample(short *buffer, int sample, int bank);
void Write_Unpacked_Sample(int (*Write_Function)(void *, int ,int, FILE *),
                           FILE *in, int sample, int bank);
void Swap_Sample(short *buffer, int sample, int bank);
int Write_Data(void *value, int size, int amount, FILE *handle);
int Write_Data_Swap(void *value, int size, int amount, FILE *handle);
int Read_Data(void *value, int size, int amount, FILE *handle);
int Read_Data_Swap(void *value, int size, int amount, FILE *handle);
void Load_303_Data(int (*Read_Function)(void *, int ,int, FILE *),
                   int (*Read_Function_Swap)(void *, int ,int, FILE *),
                   FILE *in, int unit, int pattern);
void Save_303_Data(int (*Write_Function)(void *, int ,int, FILE *),
                   int (*Write_Function_Swap)(void *, int ,int, FILE *),
                   FILE *in, int unit, int pattern);
short *Get_WaveForm(int Instr_Nbr, int Channel, int Split);
void Load_Reverb_Data(int (*Read_Function)(void *, int ,int, FILE *),
                      int (*Read_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in);
void Load_Pattern_Data(int (*Read_Function)(void *, int ,int, FILE *),
                      int (*Read_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in);
void Save_Reverb_Data(int (*Write_Function)(void *, int ,int, FILE *),
                      int (*Write_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in);
void Save_Pattern_Data(int (*Write_Function)(void *, int ,int, FILE *),
                      int (*Write_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in);

// ------------------------------------------------------
// Prepare the tracker interface once a module has been loaded
void Init_Tracker_Context_After_ModLoad(void)
{
#if !defined(__WINAMP__)
    Track_Under_Caret = 0;
    Current_Sample = 0;
    Column_Under_Caret = 0;
#endif

    Pattern_Line = 0;
    Pattern_Line_Visual = 0;
    Song_Position = 0;
    Song_Position_Visual = 0;

    Final_Mod_Length = 0;

    Post_Song_Init();

#if !defined(__WINAMP__)
    Draw_Scope();
    gui_track = 0;
#endif

    lchorus_counter = MIX_RATE;
    rchorus_counter = MIX_RATE;
    lchorus_counter2 = MIX_RATE - lchorus_delay;
    rchorus_counter2 = MIX_RATE - rchorus_delay;
    Initreverb();

    Mas_Compressor_Set_Variables(mas_comp_threshold, mas_comp_ratio);
    Reset_Song_Length();

#if !defined(__WINAMP__)
    Display_Song_Length();

    Scopish = SCOPE_ZONE_MOD_DIR;
    //Draw_Scope_Files_Button();

    Reset_Tracks_To_Render();

    Refresh_UI_Context();
#endif

}

#if !defined(__WINAMP__)
// ------------------------------------------------------
// Convert an amiga note for our own purpose
int Conv_Amiga_Note(int note)
{
    int i;
    for(i = 0; i < sizeof(mt_period_conv) / sizeof(AMIGA_NOTE); i++)
    {
        if(note == mt_period_conv[i].old_note)
        {
            return(mt_period_conv[i].new_note);
        }
    }
    return(121);
}

// ------------------------------------------------------
// Scale an amiga protracker .mod effect data
float Scale_AmigaMod_Value(int value, float scale1, float scale2)
{
    float newvalue = (float) value;
    newvalue /= scale1;
    newvalue *= scale2;
    if(newvalue > scale2) newvalue = scale2;
    return(newvalue);
}

// ------------------------------------------------------
// Load a .mod file
void LoadAmigaMod(char *Name, const char *FileName, int channels)
{
    FILE *in;
    int t_hi;
    int t_lo;
    int swrite;
    int tps;
    int pwrite;
    int li2;
    int pw2;
    //float volume;
    Uint32 x;
    //int ramp;
    //float ramp_vol;
    int i;
    int j;
    int k;
    int vib_chan[16];
    int vib_last[16];
    int arp_chan[16];
    int cur_tempo = 125;
    int cur_speed = 6;
    int found_tempo;
    int found_speed;
    int last_speed;
    int last_tempo;
    // -1 = -16
    // -2 = -24
    // -3 = -32
    // -4 = -48
    // -5 = -64
    // -6 = -80
    // -7 = -96
    // -8 = -112
    char FineTune_Table[] =
    {
        0,  16,  32,  48,  64,  80,  96,  112,
       -112, -96, -80, -64, -48, -32, -24, -16
    };

    for(i = 0; i < 16; i++)
    {
        vib_chan[i] = 0;
        vib_last[i] = 0;
        arp_chan[i] = 0;
    }

    in = fopen(FileName, "rb");
    if(in != NULL)
    {
        Songplaying = FALSE;

        Free_Samples();
        Clear_Patterns_Pool();

#if !defined(__NO_MIDI__) && !defined(__WINAMP__)
        Midi_Reset();
#endif

        init_sample_bank();
        Pre_Song_Init();

        // Read the title
        memset(Name, 0, 21);
        sprintf(style, "Converted");
        fread(Name, sizeof(char), 20, in);

        if(!strlen(Name))
        {
            strcpy(Name, FileName);
            i = strlen(Name) - 1;
            while(i)
            {
                if(Name[i] == '.')
                {
                    for(j = i; Name[j]; j++)
                    {
                        Name[j] = 0;
                    }
                    break;
                }
                i--;
            }
        }

        // Remove suspicious chars
        for(j = 0; Name[j]; j++)
        {
            if(!isalnum(Name[j])) Name[j] = '_';
        }
        
        Songtracks = channels;

        for(swrite = 0; swrite < 31; swrite++)
        {
            SampleType[swrite][0] = 1;
            memset(nameins[swrite], 0, 20);
            fread(&nameins[swrite], sizeof(char), 19, in);

            // Remove suspicious chars
            for(j = 0; nameins[swrite][j]; j++)
            {
                if(!isalnum(nameins[swrite][j])) nameins[swrite][j] = '_';
            }
        
            sprintf(SampleName[swrite][0], "Untitled.wav");

            // Jump over 3 unhandled bytes for PTK samplename.
            fseek(in, 3, SEEK_CUR);

            Clear_Instrument_Dat(swrite, 0, 0);

            SampleNumSamples[swrite][0] = (int) (fgetc(in) << 8) + (int) fgetc(in);
            SampleNumSamples[swrite][0] *= 2;
            fread(&Finetune[swrite][0], sizeof(char), 1, in);

            Finetune[swrite][0] = FineTune_Table[Finetune[swrite][0] & 0xf];
            Sample_Vol[swrite] = Scale_AmigaMod_Value(fgetc(in), 63.0f, 0.99f);
            Sample_Amplify[swrite][0] = 0.5f;
            Basenote[swrite][0] = DEFAULT_BASE_NOTE - 5 + 12 + 12 + 12 + 12 - 2;

            // Calculate/Adapt AMIGA loop points to ptk LoopPoints
            LoopStart[swrite][0] = (int) (fgetc(in) << 8) + (int) fgetc(in);
            LoopStart[swrite][0] *= 2;
            long Replen = ((int) (fgetc(in) << 8) + (int) fgetc(in));
            Replen *= 2;

            if(Replen > 2)
            {
                LoopEnd[swrite][0] = LoopStart[swrite][0] + Replen;
                LoopType[swrite][0] = SMP_LOOP_FORWARD;
            }
            else
            {
                LoopEnd[swrite][0] = SampleNumSamples[swrite][0];
                LoopType[swrite][0] = SMP_LOOP_NONE;
            }

            if(LoopEnd[swrite][0] > SampleNumSamples[swrite][0])
            {
                LoopEnd[swrite][0] = LoopEnd[swrite][0] - SampleNumSamples[swrite][0];
            }
        } // FOR

        fread(&sLength, sizeof(unsigned char), 1, in);
        fgetc(in);

        last_speed = 4;
        last_tempo = 125;

        // PATTERN PROGRESSION;
        nPatterns = 1;

        for(tps = 0; tps < 128; tps++)
        {
            pSequence[tps] = fgetc(in);
            if(pSequence[tps] >= nPatterns) nPatterns = pSequence[tps] + 1;
            patternLines[tps] = 64;
        }

        // JUMP OVER THE HEADER
        fseek(in, 4, SEEK_CUR);

        for(pwrite = 0; pwrite < sLength; pwrite++)
        {
            // All 64 rows of the .mod
            for(li2 = 0; li2 < 64; li2++)
            {
                fseek(in, 1084 + (pSequence[pwrite] * (4 * channels * 64) + (4 * channels * li2)), SEEK_SET);

                // Precalc the speed for that row
                for(pw2 = 0; pw2 < channels; pw2++)
                {
                    int tmo = Get_Pattern_Offset(pSequence[pwrite], pw2, li2);
                    int f_byte = fgetc(in);
                    int t_sample = f_byte >> 4;
                    int t_period = (f_byte - (t_sample << 4)) * 256 + (int) fgetc(in);
                    int t_byte = fgetc(in);
                    int t_sample2 = t_byte >> 4;
                    int Cmd = t_byte - (t_sample2 << 4);
                    int Cmd_Dat = fgetc(in);

                    switch(Cmd)
                    {
                        // SPEED
                        case 0xf:
                            if(Cmd_Dat > 31) cur_tempo = Cmd_Dat;
                            else cur_speed = Cmd_Dat;

                            // Get the speed to find
                            int found_exact_st;
                            int found_free_chan;
                            float speed_to_find = cur_tempo / 125.0f * 50.0f / (float) cur_speed;
                            speed_to_find *= 60.0f;

                            // Now we need to find the exact match for that speed
                            // the ugly big bad brute force way >:D
                
                            Cmd = 0;
                            Cmd_Dat = 0;

                            // Loop over speeds
                            found_exact_st = FALSE;
                            for(j = 1; j < 32; j++)
                            {
                                // Loop over tempos
                                for(i = 20; i < 255; i++)
                                {
                                    if(speed_to_find == (((float) i * (float) j)))
                                    {
                                        found_exact_st = TRUE;
                                        found_tempo = i;
                                        found_speed = j;
                                        // Always use the 2nd fx to set the tempo
                                        
                                        // We can put this on any track so look for a free one

                                        found_free_chan = FALSE;
                                        // Check if we already have recorded it
                                        for(k = 0; k < channels; k++)
                                        {
                                            int free_chan = Get_Pattern_Offset(pSequence[pwrite], k, li2);
                                            if(*(RawPatterns + free_chan + PATTERN_FX2) == 0xf0 &&
                                               *(RawPatterns + free_chan + PATTERN_FXDATA2) == found_tempo)
                                            {
                                                found_free_chan = TRUE;
                                            }
                                        }

                                        if(!found_free_chan)
                                        {
                                            for(k = 0; k < channels; k++)
                                            {
                                                int free_chan = Get_Pattern_Offset(pSequence[pwrite], k, li2);
                                                if(!*(RawPatterns + free_chan + PATTERN_FX2))
                                                {
                                                    found_free_chan = TRUE;
                                                    if(last_tempo != found_tempo)
                                                    {
                                                        *(RawPatterns + free_chan + PATTERN_FX2) = 0xf0;
                                                        *(RawPatterns + free_chan + PATTERN_FXDATA2) = found_tempo;
                                                        last_tempo = found_tempo;
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                        if(!found_free_chan)
                                        {
                                            // Overwrite is the worst case scenario
                                            if(last_tempo != found_tempo)
                                            {
                                                *(RawPatterns + tmo + PATTERN_FX2) = 0xf0;
                                                *(RawPatterns + tmo + PATTERN_FXDATA2) = found_tempo;
                                                last_tempo = found_tempo;
                                            }
                                        }
                                        Cmd = 0xf;
                                        Cmd_Dat = found_speed;
                                        break;
                                    }
                                }
                                if(found_exact_st) break;
                            }
                            if(last_speed != Cmd_Dat)
                            {
                                *(RawPatterns + tmo + PATTERN_FX) = Cmd;
                                *(RawPatterns + tmo + PATTERN_FXDATA) = Cmd_Dat;
                                last_speed = Cmd_Dat;
                            }
                            break;
                    } // Pattern FX adapter end.
                }

                // Jump to the pattern to retrieve
                fseek(in, 1084 + (pSequence[pwrite] * (4 * channels * 64) + (4 * channels * li2)), SEEK_SET);

                for(pw2 = 0; pw2 < channels; pw2++)
                {
                    int tmo = Get_Pattern_Offset(pSequence[pwrite], pw2, li2);
                    int f_byte = fgetc(in);
                    int t_sample = f_byte >> 4;
                    int t_period = (f_byte - (t_sample << 4)) * 256 + (int) fgetc(in);
                    int t_byte = fgetc(in);
                    int t_sample2 = t_byte >> 4;
                    int Cmd = t_byte - (t_sample2 << 4);
                    t_sample = (t_sample << 4) + t_sample2;
                    if(t_sample == 0) t_sample = 255;
                    if(t_sample != 255) t_sample--;

                    int Note = 121;                     // No note
                    int Cmd_Dat = fgetc(in);

                    // Period Conversion Table
                    Note = Conv_Amiga_Note(t_period);

                    *(RawPatterns + tmo + PATTERN_NOTE1) = Note;
                    *(RawPatterns + tmo + PATTERN_INSTR1) = t_sample;

                    // There was a vibrato
                    if(vib_chan[pw2] == TRUE)
                    {
                        // But no more
                        if(Cmd != 4 && Cmd != 6)
                        {
                            if(*(RawPatterns + tmo + PATTERN_FX2) == 0x1d)
                            {
                                vib_chan[pw2] = FALSE;
                            }
                            else
                            {
                                // Stop it or delay it
                                if(!*(RawPatterns + tmo + PATTERN_FX2))
                                {
                                    *(RawPatterns + tmo + PATTERN_FX2) = 0x1d;
                                    *(RawPatterns + tmo + PATTERN_FXDATA2) = 0;
                                    vib_chan[pw2] = FALSE;
                                }
                            }
                        }
                    }

                    // There was an arpeggio
                    if(arp_chan[pw2] == TRUE)
                    {
                        // But no more
                        if(Cmd != 0 || (Cmd == 0 && Cmd_Dat == 0))
                        {
                            if(*(RawPatterns + tmo + PATTERN_FX2) == 0x1b)
                            {
                                arp_chan[pw2] = FALSE;
                            }
                            else
                            {
                                // Stop it or delay it
                                if(!*(RawPatterns + tmo + PATTERN_FX2))
                                {
                                    *(RawPatterns + tmo + PATTERN_FX2) = 0x1b;
                                    *(RawPatterns + tmo + PATTERN_FXDATA2) = 0;
                                    arp_chan[pw2] = FALSE;
                                }
                            }
                        }
                    }

                    // (Speed has already been converted)
                    if(Cmd != 0xf)
                    {
                        // Pattern effect adapter:
                        switch(Cmd)
                        {
                            // ARPEGGIO
                            case 0:
                                if(Cmd_Dat)
                                {
                                    // Our arpeggios are different from theirs
                                    Cmd = 0x1b;
                                    arp_chan[pw2] = TRUE;
                                }
                                else Cmd = 0;
                                break;

                            // TODO: effects like 1 2 3 5 6 a depend on the current speed in .mods.
                            //       (the faster the speed, the less effect).

                            // PITCH UP
                            case 1:
                                Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                break;

                            // PITCH DOWN
                            case 2:
                                Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                break;

                            // TONE PORTAMENTO
                            case 3:
                                Cmd = 5;
                                break;

                            // VIBRATO
                            case 4:
                                Cmd = 0x1d;
                                // Our vibratos are different from theirs
                                if(Cmd_Dat == 0) Cmd_Dat = vib_last[pw2];
                                else vib_last[pw2] = Cmd_Dat;
                                vib_chan[pw2] = TRUE;
                                break;

                            // TONE PORTAMENTO + VOL SLIDE
                            case 5:
                                // Use the 2nd fx slot to mix both commands
                                if(Cmd_Dat >= 16)
                                {
                                    // Vol SlideUp
                                    *(RawPatterns + tmo + PATTERN_FX2) = 0x19;
                                    *(RawPatterns + tmo + PATTERN_FXDATA2) = (int) Scale_AmigaMod_Value(Cmd_Dat >> 4, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                }
                                else
                                {
                                    *(RawPatterns + tmo + PATTERN_FX2) = 0x1a;
                                    *(RawPatterns + tmo + PATTERN_FXDATA2) = (int) Scale_AmigaMod_Value(Cmd_Dat & 0xf, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                }
                                // (The portamento retains the value initiated with command 3)
                                Cmd = 5;
                                Cmd_Dat = 0;
                                break;

                            // VIBRATO + VOL SLIDE
                            case 6:
                                // Use the 2nd fx slot to mix both commands
                                if(Cmd_Dat >= 16)
                                {
                                    // Vol SlideUp
                                    *(RawPatterns + tmo + PATTERN_FX2) = 0x19;
                                    *(RawPatterns + tmo + PATTERN_FXDATA2) = (int) Scale_AmigaMod_Value(Cmd_Dat >> 4, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                }
                                else
                                {
                                    *(RawPatterns + tmo + PATTERN_FX2) = 0x1a;
                                    *(RawPatterns + tmo + PATTERN_FXDATA2) = (int) Scale_AmigaMod_Value(Cmd_Dat & 0xf, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                }
                                // (The vibrato retains the value initiated with command 4)
                                Cmd = 0x1d;
                                Cmd_Dat = vib_last[pw2];
                                vib_chan[pw2] = TRUE;
                                break;

                            // DEMOSYNCHRO SIGNAL ?
                            case 8:
                                // Let's assume it's a fasttracker 1 module
                                // Which could handle panning.
                                if(channels > 4)
                                {
                                    Cmd_Dat >>= 1;
                                    *(RawPatterns + tmo + PATTERN_PANNING) = Cmd_Dat;
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }
                                else
                                {
                                    Cmd = 7;
                                }
                                break;

                            // SAMPLE OFFSET
                            case 9:
                                // We use 256 bytes instead of 128
                                //Cmd_Dat <<= 1;
                                break;

                            // VOLSLIDE
                            case 0xa:
                                if(Cmd_Dat >= 16)
                                {
                                    Cmd = 0x19; // Vol SlideUp
                                    Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat >> 4, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                }
                                else
                                {
                                    Cmd = 0x1a; // Vol Slide Down
                                    Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat & 0xf, (31 - (cur_speed - 1)) * 2.5f, 255.0f);
                                }
                                break;

                            // POSITION JUMP
                            case 0xb:
                                Cmd = 0x1f;
                                break;

                            // VOLUME
                            case 0xc:
                                Cmd = 3;
                                Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat, 63.0f, 255.0f);
                                break;

                            // PATTERN BREAK
                            case 0xd:
                                // Protracker handles this as BCD
                                // So convert it to hex
                                t_hi = (Cmd_Dat >> 4);
                                t_lo = Cmd_Dat - (t_hi << 4);
                                Cmd_Dat = (t_hi * 10) + t_lo;
                                break;

                            // EXX Special Effects
                            // [not a full implementation of all Protracker MOD Exx commands]
                            case 0xe:
                                Cmd = 0;

                                // FILTER (not supported)
                                if(Cmd_Dat >= 0x00 && Cmd_Dat < 0x10)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // FINE PORTAMENTO UP
                                if(Cmd_Dat >= 0x10 && Cmd_Dat < 0x20)
                                {
                                    Cmd = 0x22;
                                    Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat - 0x10, 15.0f, 255.0f);
                                }

                                // FINE PORTAMENTO DOWN
                                if(Cmd_Dat >= 0x20 && Cmd_Dat < 0x30)
                                {
                                    Cmd = 0x23;
                                    Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat - 0x20, 15.0f, 255.0f);
                                }

                                // SET GLISS CONTROL (not supported)
                                if(Cmd_Dat >= 0x30 && Cmd_Dat < 0x40)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // SET VIBRATO CONTROL (not supported)
                                if(Cmd_Dat >= 0x40 && Cmd_Dat < 0x50)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // SET FINETUNE (not supported)
                                if(Cmd_Dat >= 0x50 && Cmd_Dat < 0x60)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // PATTERN LOOP
                                if(Cmd_Dat >= 0x60 && Cmd_Dat < 0x70)
                                {
                                    Cmd = 0x06;
                                    Cmd_Dat = Cmd_Dat - 0x60;
                                }

                                // SET TREMOLO CONTROL (not supported)
                                if(Cmd_Dat >= 0x70 && Cmd_Dat < 0x80)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // NOTHING
                                if(Cmd_Dat >= 0x80 && Cmd_Dat < 0x90)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // NOTE RETRIGGER
                                if(Cmd_Dat >= 0x90 && Cmd_Dat < 0xa0)
                                {
                                    Cmd = 0xe;
                                    Cmd_Dat = Cmd_Dat - 0x90;
                                }

                                // FINEVOLUME SLIDEUP
                                if(Cmd_Dat >= 0xa0 && Cmd_Dat < 0xb0)
                                {
                                    Cmd = 0x20;
                                    Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat - 0xa0, 16.0f, 255.0f);
                                }

                                // FINEVOLUME SLIDEDOWN
                                if(Cmd_Dat >= 0xb0 && Cmd_Dat < 0xc0)
                                {
                                    Cmd = 0x21;
                                    Cmd_Dat = (int) Scale_AmigaMod_Value(Cmd_Dat - 0xb0, 16.0f, 255.0f);
                                }

                                // NOTE CUT
                                if(Cmd_Dat >= 0xc0 && Cmd_Dat < 0xd0)
                                {
                                    *(RawPatterns + tmo + PATTERN_VOLUME) = (Cmd_Dat & 0xf) | 0xf0;
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // NOTE DELAY (not supported)
                                if(Cmd_Dat >= 0xd0 && Cmd_Dat < 0xe0)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // PATTERN DELAY (not supported)
                                if(Cmd_Dat >= 0xe0 && Cmd_Dat < 0xf0)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }

                                // FUNKIT (not supported)
                                if(Cmd_Dat >= 0xf0 && Cmd_Dat <= 0xff)
                                {
                                    Cmd = 0;
                                    Cmd_Dat = 0;
                                }
                                break;

                        } // Pattern FX adapter end.

                        *(RawPatterns + tmo + PATTERN_FX) = Cmd;
                        *(RawPatterns + tmo + PATTERN_FXDATA) = Cmd_Dat;
                    }
                }
            }
        }

        fseek(in, 1084 + (nPatterns * (4 * channels * 64)), SEEK_SET);

        // Load the samples
        for(swrite = 0; swrite < 31; swrite++)
        {
            if(SampleNumSamples[swrite][0] > 8)
            {
                // Reserving space for 16-Bit Signed Short Data.
                AllocateWave(swrite, 0, SampleNumSamples[swrite][0], 1, FALSE);
                for(x = 0; x < SampleNumSamples[swrite][0]; x++)
                {
                    *(RawSamples[swrite][0][0] + x) = (short) fgetc(in) << 8;
                }
                *(RawSamples[swrite][0][0]) = 0;

/*#define MOD_RAMP_SIZE 40

                // Ramp volume at the end of the sample (if there's no loop)
                if(LoopType[swrite][0] == SMP_LOOP_NONE)
                {
                    ramp_vol = 1.0f;
                    for(ramp = (x - MOD_RAMP_SIZE);
                        ramp < x; ramp++)
                    {
                        volume = (float) (*(RawSamples[swrite][0][0] + ramp));
                        volume /= 32767;
                        volume *= ramp_vol;

                        *(RawSamples[swrite][0][0] + ramp) = (short) (volume * 32767.0f);
                        ramp_vol -= 1.0f / (float) (MOD_RAMP_SIZE - 1.0f);
                    }
                    ramp_vol = 0.0f;
                    for(ramp = 0;
                        ramp < MOD_RAMP_SIZE; ramp++)
                    {
                        volume = (float) (*(RawSamples[swrite][0][0] + ramp));
                        volume /= 32767;
                        volume *= ramp_vol;

                        *(RawSamples[swrite][0][0] + ramp) = (short) (volume * 32767.0f);
                        ramp_vol += 1.0f / (float) (MOD_RAMP_SIZE - 1.0f);
                    }
                }*/
            }
            else
            {
                // NO SAMPLE
                SampleType[swrite][0] = 0;
            }
        }

        for(i = 0; i < channels; i++)
        {
            TPan[i] = mt_pannels[i];
            ComputeStereo(i);
        }

        Use_Cubic = NONE_INT;
        BeatsPerMin = 125;
        TicksPerBeat = 4;
        mas_vol = 0.75f;
        mas_comp_threshold = 100.0f;
        mas_comp_ratio = 0.0f;

        fclose(in);

        Load_Old_Reverb_Presets(0);

        Init_Tracker_Context_After_ModLoad();
    }
    else
    {
        Status_Box("Protracker module loading failed. (Probably: file not found)");
    }

    Clear_Input();
}
#endif // __WINAMP__


// ------------------------------------------------------
// Load the old presets
void Load_Old_Reverb_Presets(int Type)
{
    int i;

    switch(Type)
    {
        case 0:
            decays[0] = 20.0f;
            decays[1] =  0.0f;
            decays[2] = 15.0f;
            decays[3] =  0.0f;
            decays[4] =  9.0f;
            decays[5] =  1.0f;
            decays[6] =  8.0f;
            decays[7] =  1.0f;
            decays[8] =  4.0f;
            decays[9] =  1.0f;

            delays[0] = 1000;
            delays[1] = 1100; 
            delays[2] = 1200;
            delays[3] = 1300;
            delays[4] = 1400;
            delays[5] = 1800;
            delays[6] = 1900;
            delays[7] = 2000;
            delays[8] = 2100;
            delays[9] = 2200;

            num_echoes = 10;
            break;

        case 1:
            decays[0] =   7.0f;
            decays[1] = -13.0f;
            decays[2] =  25.0f;
            decays[3] =  31.0f;
            decays[4] =  20.0f;
            decays[5] =  28.0f;
            decays[6] = -21.0f;
            decays[7] =  18.0f;
            decays[8] = -13.0f;
            decays[9] =   9.0f;

            delays[0] = 1000;
            delays[1] = 1600; 
            delays[2] = 2100;
            delays[3] = 2400;
            delays[4] = 2290;
            delays[5] = 2350;
            delays[6] = 2400;
            delays[7] = 2500;
            delays[8] = 2680;
            delays[9] = 3410;

            num_echoes = 10;
            break;

        case 2:
            decays[0] =  1.0f;
            decays[1] =  1.0f;
            decays[2] =  9.0f;
            decays[3] = 12.0f;
            decays[4] = 22.0f;
            decays[5] =  1.0f;
            decays[6] = 15.0f;
            decays[7] =  1.0f;
            decays[8] =  7.0f;
            decays[9] =  2.0f;

            delays[0] =  100;
            delays[1] =  200; 
            delays[2] =  300;
            delays[3] = 1000;
            delays[4] = 1190;
            delays[5] = 1250;
            delays[6] = 1300;
            delays[7] = 1400;
            delays[8] = 1580;
            delays[9] = 1610;

            num_echoes = 10;
            break;

        case 3:
            decays[0] = 22.0f;
            decays[1] =  5.0f;
            decays[2] = 12.0f;
            decays[3] =  3.0f;

            delays[0] = 2000;
            delays[1] = 4400; 
            delays[2] = 5000;
            delays[3] = 6200;

            num_echoes = 4;
            break;

        case 4:
            decays[0] = 11.0f;
            decays[1] =  0.0f;
            decays[2] = 31.0f;
            decays[3] =  0.0f;

            delays[0] = 3012;
            delays[1] = 4012;
            delays[2] = 4022;
            delays[3] = 5232;

            num_echoes = 4;
            break;

        case 5:
            decays[0] =   7.0f;
            decays[1] = -13.0f;
            decays[2] =  25.0f;
            decays[3] =  31.0f;
            decays[4] =  20.0f;
            decays[5] =  28.0f;
            decays[6] = -21.0f;
            decays[7] =  18.0f;
            decays[8] = -13.0f;
            decays[9] =   9.0f;

            delays[0] = 20;
            delays[1] = 600;  
            delays[2] = 100;
            delays[3] = 400;
            delays[4] = 290;
            delays[5] = 1350;
            delays[6] = 400;
            delays[7] = 1500;
            delays[8] = 1680;
            delays[9] = 1410;

            num_echoes = 10;
            break;

        case 6:
            decays[0] =   7.0f;
            decays[1] = -13.0f;
            decays[2] =  25.0f;
            decays[3] =  31.0f;
            decays[4] =  20.0f;
            decays[5] =  28.0f;
            decays[6] = -21.0f;
            decays[7] =  18.0f;
            decays[8] = -13.0f;
            decays[9] =  12.0f;

            delays[0] = 20;
            delays[1] = 600;  
            delays[2] = 700;
            delays[3] = 800;
            delays[4] = 990;
            delays[5] = 1350;
            delays[6] = 1400;
            delays[7] = 1500;
            delays[8] = 1680;
            delays[9] = 1910;

            num_echoes = 10;
            break;
    }
    // / 64.0f
    for(i = 0; i < num_echoes; i++)
    {
        decays[i] = decays[i] * 0.015625f;
    }
}

// ------------------------------------------------------
// Load a module file
int LoadMod(char *FileName)
{
    int Ye_Old_Phony_Value;
    int New_adsr = FALSE;
    int New_Comp = FALSE;
    int Portable = FALSE;
    int Poly = FALSE;
    int Multi = FALSE;
    int Sel_Interpolation = FALSE;
    int New_Reverb = FALSE;
    int Env_Modulation = FALSE;
    int New_Env = FALSE;
    int Fx2 = FALSE;

    char Comp_Flag;

    int i;
    int j;
    int k;
    int Old_Ntk;
    int Ntk_Beta;
    unsigned char *TmpPatterns;
    unsigned char *TmpPatterns_Tracks;
    unsigned char *TmpPatterns_Notes;
    unsigned char *TmpPatterns_Rows;
    int Old_Bug = TRUE;
    int new_disto = FALSE;
    int Pack_Scheme = FALSE;
    int Mp3_Scheme = FALSE;
    int tps_pos;
    int tps_trk;
    int twrite;
    int fake_value;
    int Packed_Size;
    int UnPacked_Size;
    int Flanger_Bug = FALSE;
    unsigned char *Packed_Module = NULL;

    Mod_Simulate = LOAD_READ;
    Mod_Mem_Pos = 0;
    Mod_Memory = NULL;

    FILE *in;
    in = fopen(FileName, "rb");
    Old_Ntk = FALSE;
    Ntk_Beta = FALSE;

    if(in != NULL)
    {

#if !defined(__WINAMP__)
        Status_Box("Attempting to load the song file...");
#endif

        Songplaying = FALSE;

        // Reading and checking extension...
        char extension[10];
        Read_Data(extension, sizeof(char), 9, in);

        switch(extension[7])
        {
            case 'J':
                Flanger_Bug = TRUE;
            case 'I':
                Fx2 = TRUE;
            case 'H':
                New_Env = TRUE;
            case 'G':
                Env_Modulation = TRUE;
            case 'F':
                New_Reverb = TRUE;
            case 'E':
                Sel_Interpolation = TRUE;
            case 'D':
                Mp3_Scheme = TRUE;
            case 'C':
                Multi = TRUE;
            case 'B':
                Poly = TRUE;
            case 'A':
                Portable = TRUE;
            case '9':
                Mod_Simulate = LOAD_READMEM;
            case '8':
                New_Comp = TRUE;
            case '7':
                New_adsr = TRUE;
            case '6':
                Pack_Scheme = TRUE;
            case '5':
                new_disto = TRUE;
            case '4':
                Old_Bug = FALSE;
            case '3':
                goto Read_Mod_File;

            // Old noisetrekker
            case '2':
                Old_Ntk = TRUE;

            // Noisetrekker Beta (1.6)
            case '1':
                Ntk_Beta = TRUE;
        }

Read_Mod_File:

#if !defined(__WINAMP__)
        Status_Box("Loading song -> Header...");
#endif
        Free_Samples();

        mas_comp_threshold = 100.0f;
        mas_comp_ratio = 0.0f;

#if !defined(__WINAMP__)
        allow_save = TRUE;
#endif

        Clear_Patterns_Pool();

#if !defined(__NO_MIDI__) && !defined(__WINAMP__)
        Midi_Reset();
#endif

        init_sample_bank();
        Pre_Song_Init();

        // Load the module into memory and depack it
        if(Mod_Simulate == LOAD_READMEM)
        {
            Packed_Size = Get_File_Size(in) - 9;
            Packed_Module = (unsigned char *) malloc(Packed_Size);
            if(Packed_Module)
            {
                if(Poly)
                {
                    Read_Data_Swap(&UnPacked_Size, sizeof(int), 1, in);
                }
                else
                {
                    UnPacked_Size = Packed_Size * 10;
                }
                Read_Data(Packed_Module, sizeof(char), Packed_Size, in);
                Mod_Memory = Depack_Data(Packed_Module, Packed_Size, UnPacked_Size);
                Mod_Mem_Pos = 0;
                free(Packed_Module);
            }
        }

        Read_Mod_Data(FileName, sizeof(char), 20, in);
        Read_Mod_Data(&nPatterns, sizeof(char), 1, in);

        Songtracks = MAX_TRACKS;
        Read_Mod_Data(&sLength, sizeof(char), 1, in);

        Use_Cubic = CUBIC_INT;

        if(Sel_Interpolation)
        {
            Read_Mod_Data(&Use_Cubic, sizeof(char), 1, in);
        }

        Read_Mod_Data(pSequence, sizeof(char), 256, in);

        Clear_Patterns_Pool();

        // Load the patterns rows infos
        if(!Ntk_Beta)
        {
            for(i = 0; i < MAX_ROWS; i++)
            {
                Read_Mod_Data_Swap(&patternLines[i], sizeof(short), 1, in);
            }
        }

        // Multi notes
        if(Multi)
        {
            Read_Mod_Data(Channels_MultiNotes, sizeof(char), MAX_TRACKS, in);
        }

        // Load the patterns data
        int Bytes_Per_Track = PATTERN_BYTES;
        if(!Multi)
        {
            Bytes_Per_Track = 6;
        }

        TmpPatterns = RawPatterns;
        for(int pwrite = 0; pwrite < nPatterns; pwrite++)
        {
            TmpPatterns_Rows = TmpPatterns + (pwrite * PATTERN_LEN);
            int Rows_To_Read = MAX_ROWS;
            if(Ntk_Beta) Rows_To_Read = 64;

            for(j = 0; j < Rows_To_Read; j++)
            {
                // Bytes / track
                for(k = 0; k < Songtracks; k++)
                {
                    // Tracks
                    TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                    // Rows
                    TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                    if(Multi)
                    {
                        for(i = 0; i < MAX_POLYPHONY; i++)
                        {
                            Read_Mod_Data(TmpPatterns_Notes + PATTERN_NOTE1 + (i * 2), sizeof(char), 1, in);
                            Read_Mod_Data(TmpPatterns_Notes + PATTERN_INSTR1 + (i * 2), sizeof(char), 1, in);
                        }
                    }
                    else
                    {
                        Read_Mod_Data(TmpPatterns_Notes + PATTERN_NOTE1, sizeof(char), 1, in);
                        Read_Mod_Data(TmpPatterns_Notes + PATTERN_INSTR1, sizeof(char), 1, in);
                    }
                    Read_Mod_Data(TmpPatterns_Notes + PATTERN_VOLUME, sizeof(char), 1, in);
                    Read_Mod_Data(TmpPatterns_Notes + PATTERN_PANNING, sizeof(char), 1, in);
                    Read_Mod_Data(TmpPatterns_Notes + PATTERN_FX, sizeof(char), 1, in);
                    Read_Mod_Data(TmpPatterns_Notes + PATTERN_FXDATA, sizeof(char), 1, in);
                    if(Fx2)
                    {
                        Read_Mod_Data(TmpPatterns_Notes + PATTERN_FX2, sizeof(char), 1, in);
                        Read_Mod_Data(TmpPatterns_Notes + PATTERN_FXDATA2, sizeof(char), 1, in);
                    }

                }
            }
        }

#if !defined(__WINAMP__)
        Status_Box("Loading song -> Sample data...");
#endif
        for(int swrite = 0; swrite < MAX_INSTRS; swrite++)
        {
            Read_Mod_Data(&nameins[swrite], sizeof(char), 20, in);
            Read_Mod_Data(&Midiprg[swrite], sizeof(char), 1, in);
            Read_Mod_Data(&Synthprg[swrite], sizeof(char), 1, in);

            PARASynth[swrite].disto = 0;

            PARASynth[swrite].lfo1_attack = 0;
            PARASynth[swrite].lfo1_decay = 0;
            PARASynth[swrite].lfo1_sustain = 128;
            PARASynth[swrite].lfo1_release = 0x10000;

            PARASynth[swrite].lfo2_attack = 0;
            PARASynth[swrite].lfo2_decay = 0;
            PARASynth[swrite].lfo2_sustain = 128;
            PARASynth[swrite].lfo2_release = 0x10000;

            Read_Synth_Params(Read_Mod_Data, Read_Mod_Data_Swap, in, swrite,
                              new_disto, New_adsr, Portable, Env_Modulation, New_Env, Ntk_Beta);

            // Fix some very old Ntk bugs
            if(PARASynth[swrite].lfo1_period > 128) PARASynth[swrite].lfo1_period = 128;
            if(PARASynth[swrite].lfo2_period > 128) PARASynth[swrite].lfo2_period = 128;
            if(Old_Ntk)
            {
                if(PARASynth[swrite].ptc_glide < 1) PARASynth[swrite].ptc_glide = 64;
                if(PARASynth[swrite].glb_volume < 1) PARASynth[swrite].glb_volume = 64;
            }

            // Compression type
            if(Pack_Scheme)
            {
                Read_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);
                if(Mp3_Scheme)
                {
                    switch(SampleCompression[swrite])
                    {
                        case SMP_PACK_MP3:
                            Read_Mod_Data(&Mp3_BitRate[swrite], sizeof(char), 1, in);
                            break;

                        case SMP_PACK_AT3:
                            Read_Mod_Data(&At3_BitRate[swrite], sizeof(char), 1, in);
                            break;
                    }
                }
            }
            for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
            {
                Read_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite])
                {
                    if(Old_Bug) Read_Mod_Data(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                    else Read_Mod_Data(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                    Read_Mod_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);

                    Read_Mod_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);

                    Read_Mod_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                    Read_Mod_Data_Swap(&Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                    Read_Mod_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                    AllocateWave(swrite, slwrite, SampleNumSamples[swrite][slwrite], 1, FALSE);
                    Read_Mod_Data(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                    Swap_Sample(RawSamples[swrite][0][slwrite], swrite, slwrite);
                    *RawSamples[swrite][0][slwrite] = 0;

                    // Stereo flag
                    Read_Mod_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short) + 8);
                        memset(RawSamples[swrite][1][slwrite], 0, SampleNumSamples[swrite][slwrite] * sizeof(short) + 8);
                        Read_Mod_Data(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                        Swap_Sample(RawSamples[swrite][1][slwrite], swrite, slwrite);
                        *RawSamples[swrite][1][slwrite] = 0;
                    }
                } // Exist Sample
            }
        }

#if !defined(__WINAMP__)
        Status_Box("Loading song -> Track info, patterns and sequence...");   
#endif

        Set_Default_Channels_Polyphony();

        // Reading Track Properties
        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data_Swap(&TCut[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&ICut[twrite], sizeof(float), 1, in);
            if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
            if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;
            Read_Mod_Data_Swap(&TPan[twrite], sizeof(float), 1, in);
            ComputeStereo(twrite);
            Read_Mod_Data_Swap(&FType[twrite], sizeof(int), 1, in);
            Read_Mod_Data_Swap(&FRez[twrite], sizeof(int), 1, in);
            Read_Mod_Data_Swap(&DThreshold[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&DClamp[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&DSend[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&CSend[twrite], sizeof(int), 1, in);
            if(Poly) Read_Mod_Data(&Channels_Polyphony[twrite], sizeof(char), 1, in);
        }

        // Reading mod properties
        Read_Mod_Data_Swap(&compressor, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&c_threshold, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&BeatsPerMin, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&TicksPerBeat, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&mas_vol, sizeof(float), 1, in);
        if(mas_vol < 0.01f) mas_vol = 0.01f;
        if(mas_vol > 1.0f) mas_vol = 1.0f;

        if(New_Comp)
        {
            Comp_Flag = 0;
            Read_Mod_Data(&Comp_Flag, sizeof(char), 1, in);
            if(Comp_Flag)
            {
                Read_Mod_Data_Swap(&mas_comp_threshold, sizeof(float), 1, in);
                if(mas_comp_threshold < 0.01f) mas_comp_threshold = 0.01f;
                if(mas_comp_threshold > 100.0f) mas_comp_threshold = 100.0f;

                Read_Mod_Data_Swap(&mas_comp_ratio, sizeof(float), 1, in);
                if(mas_comp_ratio < 0.01f) mas_comp_ratio = 0.01f;
                if(mas_comp_ratio > 100.0f) mas_comp_ratio = 100.0f;
            }
        }

        if(!New_Reverb) Read_Mod_Data_Swap(&delay_time, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&Feedback, sizeof(float), 1, in);
        if(!New_Reverb) Read_Mod_Data_Swap(&DelayType, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&lchorus_delay, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&rchorus_delay, sizeof(int), 1, in);
        Read_Mod_Data_Swap(&lchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Data_Swap(&rchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Data_Swap(&shuffle, sizeof(int), 1, in);

        // Load the new reverb data
        if(New_Reverb)
        {
            Load_Reverb_Data(Read_Mod_Data, Read_Mod_Data_Swap, in);
        }

        // Reading track part sequence
        for(tps_pos = 0; tps_pos < 256; tps_pos++)
        {
            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Read_Mod_Data(&CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char), 1, in);
                CHAN_HISTORY_STATE[tps_pos][tps_trk] = FALSE;
            }
        }

        for(int spl = 0; spl < Songtracks; spl++)
        {
            CCoef[spl] = float((float) CSend[spl] / 127.0f);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data_Swap(&CHAN_MIDI_PRG[twrite], sizeof(int), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(&LFO_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Data_Swap(&LFO_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&LFO_AMPL[twrite], sizeof(float), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(&FLANGER_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Data_Swap(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&FLANGER_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
            FLANGER_OFFSET[twrite] = 8192;
            FLANGER_OFFSET2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
            FLANGER_OFFSET1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
        }

        if(!Flanger_Bug)
        {
            Read_Mod_Data_Swap(&FLANGER_DEPHASE, sizeof(float), 1, in);
        }

        for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
        {
            Read_Mod_Data_Swap(&CHAN_MUTE_STATE[tps_trk], sizeof(int), 1, in);
        }

        Read_Mod_Data(&Songtracks, sizeof(char), 1, in);

        for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
        {
            Read_Mod_Data(&Disclap[tps_trk], sizeof(char), 1, in);
            if(!Portable) Read_Mod_Data(&fake_value, sizeof(char), 1, in);
        }

        if(!Ntk_Beta)       // Nothing like that in ntk beta
        {
            Read_Mod_Data(artist, sizeof(char), 20, in);
            Read_Mod_Data(style, sizeof(char), 20, in);

            if(!Portable) Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);

            Read_Mod_Data(beatsync, sizeof(char), MAX_INSTRS, in);

            for(i = 0; i < MAX_INSTRS; i++)
            {
                Read_Mod_Data_Swap(&beatlines[i], sizeof(short), 1, in);
            }

            Read_Mod_Data_Swap(&REVERBFILTER, sizeof(float), 1, in);

            for(i = 0; i < MAX_INSTRS; i++)
            {
                Read_Mod_Data_Swap(&Sample_Vol[i], sizeof(float), 1, in);
            }

            if(!Portable) Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);

            // Read the 303 datas
            for(j = 0; j < 2; j++)
            {
                Read_Mod_Data(&tb303[j].enabled, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].selectedpattern, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].tune, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].cutoff, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].resonance, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].envmod, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].decay, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].accent, sizeof(char), 1, in);
                Read_Mod_Data(&tb303[j].waveform, sizeof(char), 1, in);
                if(Portable)
                {
                    for(i = 0; i < 32; i++)
                    {
                        Load_303_Data(Read_Mod_Data, Read_Mod_Data_Swap, in, j, i);
                    }
                }
                else
                {
                    Read_Mod_Data(&tb303[j].patternlength, sizeof(char), 32, in);
                    Read_Mod_Data(&tb303[j].tone, sizeof(char), 32 * 16, in);
                    Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);
                    Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);
                    Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);
                    for(k = 0; k < 32; k++)
                    {
                        for(i = 0; i < 16; i++)
                        {
                            Read_Mod_Data_Swap(&tb303[j].flag[k][i], sizeof(struct flag303), 1, in);
                        }                            
                    }
                    Read_Mod_Data(&tb303[j].pattern_name, sizeof(char), 32 * 20, in);
                }
            }
            Read_Mod_Data_Swap(&tb303engine[0].tbVolume, sizeof(float), 1, in);
            Read_Mod_Data_Swap(&tb303engine[1].tbVolume, sizeof(float), 1, in);
            Read_Mod_Data(&tb303engine[0].hpf, sizeof(char), 1, in);
            Read_Mod_Data(&tb303engine[1].hpf, sizeof(char), 1, in);
        }

        fclose(in);

        if(!New_Reverb)
        {
            // Set the reverb to one of the old presets
            Load_Old_Reverb_Presets(DelayType);
        }

        // Init the tracker context
        Init_Tracker_Context_After_ModLoad();

#if !defined(__WINAMP__)
        Status_Box("Module loaded sucessfully...");
#endif

    }
    else
    {

#if !defined(__WINAMP__)
        Status_Box("Module loading failed. (Probably: file not found)");
#endif

        return(FALSE);
    }

#if !defined(__WINAMP__)
    Clear_Input();
    if(Mod_Memory) free(Mod_Memory);
#endif

    return(TRUE);
}

// ------------------------------------------------------
// Load and decode a packed sample
short *Unpack_Sample(FILE *FileHandle, int Dest_Length, char Pack_Type, int BitRate)
{
    int Packed_Length;

#if !defined(__NO_CODEC__)
    short *Dest_Buffer;
#endif

    Uint8 *Packed_Read_Buffer;

    Read_Mod_Data(&Packed_Length, sizeof(int), 1, FileHandle);
    if(Packed_Length == -1)
    {
        // Sample wasn't packed
        Packed_Read_Buffer = (Uint8 *) malloc(Dest_Length * 2 + 8);
        memset(Packed_Read_Buffer, 0, Dest_Length * 2 + 8);
        Read_Mod_Data(Packed_Read_Buffer, sizeof(char), Dest_Length * 2, FileHandle);
        return((short *) Packed_Read_Buffer);
    }
    else
    {

#if !defined(__NO_CODEC__)
        Packed_Read_Buffer = (Uint8 *) malloc(Packed_Length);
        // Read the packer buffer
        Read_Mod_Data(Packed_Read_Buffer, sizeof(char), Packed_Length, FileHandle);
        Dest_Buffer = (short *) malloc(Dest_Length * 2 + 8);
        memset(Dest_Buffer, 0, Dest_Length * 2 + 8);

        switch(Pack_Type)
        {
            case SMP_PACK_AT3:
                UnpackAT3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length, BitRate);
                break;
            case SMP_PACK_GSM:
                UnpackGSM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
            case SMP_PACK_MP3:
                UnpackMP3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length, BitRate);
                break;
            case SMP_PACK_TRUESPEECH:
                UnpackTrueSpeech(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
            case SMP_PACK_ADPCM:
                UnpackADPCM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
            case SMP_PACK_8BIT:
                Unpack8Bit(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
        }
        free(Packed_Read_Buffer);
        return(Dest_Buffer);
#else
        // No codec support
        return(NULL);
#endif

    }
}

// ------------------------------------------------------
// Save a packed sample
#if !defined(__WINAMP__)
void Pack_Sample(FILE *FileHandle, short *Sample, int Size, char Pack_Type, int BitRate)
{
    int PackedLen = 0;
    short *PackedSample = NULL;

#if !defined(__NO_CODEC__)
    short *AlignedSample;
    int Aligned_Size;

    switch(Pack_Type)
    {
        case SMP_PACK_AT3:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = ToAT3(Sample, PackedSample, Size * 2, BitRate);
            break;

        case SMP_PACK_GSM:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = ToGSM(Sample, PackedSample, Size * 2);
            break;

        case SMP_PACK_MP3:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = ToMP3(Sample, PackedSample, Size * 2, BitRate);
            break;

        case SMP_PACK_TRUESPEECH:
            Aligned_Size = (Size * 2) + 0x400;
            AlignedSample = (short *) malloc(Aligned_Size + 8);
            if(AlignedSample)
            {
                memset(AlignedSample, 0, Aligned_Size + 8);
                memcpy(AlignedSample, Sample, Size * 2);
                // Size must be aligned
                PackedSample = (short *) malloc(Aligned_Size + 8);
                if(PackedSample)
                {
                    memset(PackedSample, 0, Aligned_Size + 8);
                    PackedLen = ToTrueSpeech(AlignedSample, PackedSample, Aligned_Size);
                }
                free(AlignedSample);
            }
            break;

        case SMP_PACK_ADPCM:
            Aligned_Size = (Size * 2) + 0x1000;
            AlignedSample = (short *) malloc(Aligned_Size + 8);
            if(AlignedSample)
            {
                memset(AlignedSample, 0, Aligned_Size + 8);
                memcpy(AlignedSample, Sample, Size * 2);
                // Size must be aligned
                PackedSample = (short *) malloc(Aligned_Size + 8);
                if(PackedSample)
                {
                    memset(PackedSample, 0, Aligned_Size + 8);
                    PackedLen = ToADPCM(AlignedSample, PackedSample, Aligned_Size);
                }
                free(AlignedSample);
            }
            break;

        case SMP_PACK_8BIT:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = To8Bit(Sample, PackedSample, Size);
            break;

        case SMP_PACK_NONE:
            PackedLen = 0;
            break;
    }
    if(PackedLen)
    {
        // Write the encoded length
        Write_Mod_Data(&PackedLen, sizeof(char), 4, FileHandle);
        // Write the encoded datas
        Write_Mod_Data(PackedSample, sizeof(char), PackedLen, FileHandle);
    }
    else
    {
#endif
        // Couldn't pack (too small or user do not want that to happen)
        PackedLen = -1;
        Write_Mod_Data(&PackedLen, sizeof(char), 4, FileHandle);
        Write_Mod_Data(Sample, sizeof(char), Size * 2, FileHandle);

#if !defined(__NO_CODEC__)
    }
#endif

#if !defined(__NO_CODEC__)
    if(PackedSample) free(PackedSample);
#endif

}

// ------------------------------------------------------
// Write data into a module file
int Write_Mod_Data(void *Datas, int Unit, int Length, FILE *Handle)
{
    switch(Mod_Simulate)
    {
        case SAVE_WRITE:
            Write_Data(Datas, Unit, Length, Handle);
            break;

        case SAVE_CALCLEN:
            Mod_Length += Unit * Length;
            break;

        case SAVE_WRITEMEM:
            memcpy(Mod_Memory + Mod_Mem_Pos, Datas, Unit * Length);
            Mod_Mem_Pos += Unit * Length;
            break;
    }
    return(0);
}

// ------------------------------------------------------
// Write data into a module file (handling bytes swapping)
int Write_Mod_Data_Swap(void *Datas, int Unit, int Length, FILE *Handle)
{
    short sswap_value;
    int iswap_value;
    short *svalue;
    int *ivalue;
    
    switch(Mod_Simulate)
    {
        case SAVE_WRITE:
            Write_Data_Swap(Datas, Unit, Length, Handle);
            break;

        case SAVE_CALCLEN:
            Mod_Length += Unit * Length;
            break;

        case SAVE_WRITEMEM:
            switch(Unit)
            {
                case 2:
                    svalue = (short *) Datas;
                    sswap_value = Swap_16(*svalue);
                    memcpy(Mod_Memory + Mod_Mem_Pos, &sswap_value, Unit * Length);
                    Mod_Mem_Pos += Unit * Length;
                    break;

                case 4:
                    ivalue = (int *) Datas;
                    iswap_value = Swap_32(*ivalue);
                    memcpy(Mod_Memory + Mod_Mem_Pos, &iswap_value, Unit * Length);
                    Mod_Mem_Pos += Unit * Length;
                    break;

                default:
                    printf("Invalid writing size.\n");
                    break;
            }
            break;
    }
    return(0);
}
#endif // __WINAMP__

// ------------------------------------------------------
// Read data from a module file
int Read_Mod_Data(void *Datas, int Unit, int Length, FILE *Handle)
{
    switch(Mod_Simulate)
    {
        case LOAD_READ:
            Read_Data(Datas, Unit, Length, Handle);
            break;

        case LOAD_READMEM:
            memcpy(Datas, Mod_Memory + Mod_Mem_Pos, Unit * Length);
            Mod_Mem_Pos += Unit * Length;
            break;
    }
    return(0);
}

// ------------------------------------------------------
// Read data from a module file
int Read_Mod_Data_Swap(void *Datas, int Unit, int Length, FILE *Handle)
{
    short svalue;
    int ivalue;

    switch(Mod_Simulate)
    {
        case LOAD_READ:
            Read_Data_Swap(Datas, Unit, Length, Handle);
            break;

        case LOAD_READMEM:
            switch(Unit)
            {
                case 2:
                    memcpy(&svalue, Mod_Memory + Mod_Mem_Pos, Unit * Length);
                    svalue = Swap_16(svalue);
                    *((short *) Datas) = (int) svalue;
                    Mod_Mem_Pos += Unit * Length;
                    break;

                case 4:
                    memcpy(&ivalue, Mod_Memory + Mod_Mem_Pos, Unit * Length);
                    ivalue = Swap_32(ivalue);
                    *((int *) Datas) = (int) ivalue;
                    Mod_Mem_Pos += Unit * Length;
                    break;

                default:
                    printf("Invalid reading size.\n");
                    break;
            }
            break;
    }
    return(0);
}

// ------------------------------------------------------
// Check if a file exists
#if !defined(__WINAMP__)
int File_Exist(char *Format, char *Directory, char *FileName)
{
    FILE *in;
    char Temph[MAX_PATH];

    sprintf(Temph, Format, Directory, FileName);

    in = fopen(Temph, "rb");
    if(in)
    {
        sprintf(OverWrite_Name, "File '%s' already exists, overwrite ?", Temph);
        Overwrite_Requester.Text = OverWrite_Name;
        fclose(in);
        return(TRUE);
    }
    return(FALSE);
}
#endif

// ------------------------------------------------------
// Return the size of an opened file
int Get_File_Size(FILE *Handle)
{
    int File_Size;
    int Current_Pos;

    Current_Pos = ftell(Handle);
    fseek(Handle, 0, SEEK_END);
    File_Size = ftell(Handle);
    fseek(Handle, Current_Pos, SEEK_SET);
    return(File_Size);
}

#if !defined(__WINAMP__)
// ------------------------------------------------------
// module saving related functions
int Get_Instr_New_Order(int instr)
{
    int i;

    for(i = 0; i < MAX_INSTRS; i++)
    {
        if(Used_Instr[i].old_order == instr) return(Used_Instr[i].new_order);
    }
    return(-1);
}

int Search_Sequence(int sequence_idx)
{
    int i;

    for(i = 0; i < MAX_SEQUENCES; i++)
    {
        if(Old_pSequence[i] == sequence_idx)
        {
            return(i);
        }
    }
    return(-1);
}

int Track_Is_Muted(int track)
{
    int i;

    for(i = 0; i < nbr_muted; i++)
    {
        if(Muted_Tracks[i] == track) return(TRUE);
    }
    return(FALSE);
}

void Save_Constant(char *Name, int value)
{
    if(Out_constants)
    {
        if(value) fprintf(Out_constants, "#define %s\n", Name);
        else fprintf(Out_constants, "// #define %s\n", Name);
    }
}

// Look for a fx already recorded for that pos/row
int Check_FX(int pos, int row)
{
    int i;

    for(i = 0; i < Rec_Fx; i++)
    {
        if(pos == Sync_Fx[i].Pos &&
           row == Sync_Fx[i].Row)
        {
            return(i);
        }
    }
    return(-1);
}

void Save_FX(int pos, int row, int data)
{
    if(Out_FX)
    {
        fwrite(&pos, 1, 1, Out_FX);
        fwrite(&row, 1, 1, Out_FX);
        fwrite(&data, 1, 1, Out_FX);
    }
}

// ------------------------------------------------------
// Save a packed (.ptp) module
// (Only the samples are actually (if requested) packed,
//  the rest is just "demangled" to ease packers compression ratio).
int SaveMod_Ptp(FILE *in, int Simulate, char *FileName)
{
    unsigned char *TmpPatterns;
    unsigned char *TmpPatterns_Tracks;
    unsigned char *TmpPatterns_Notes;
    unsigned char *TmpPatterns_Rows;
    unsigned int New_Extension = 'KTRP';
    char char_value;
    char FileName_FX[MAX_PATH];
    int i;
    int j;
    int k;
    int l;
    int twrite;
    int tps_trk;
    char Store_303_1 = FALSE;
    char Store_303_2 = FALSE;
    int Store_Gsm = FALSE;
    int Store_Mp3 = FALSE;
    int Store_TrueSpeech = FALSE;
    int Store_ADPCM = FALSE;
    int Store_At3 = FALSE;
    int Store_8Bit = FALSE;
    int Store_Flanger = FALSE;
    int Store_Disclap = FALSE;
    int Store_LFO = FALSE;
    int Store_Instr_Waveform_Osc1 = FALSE; 
    int Store_Instr_Waveform_Osc2 = FALSE;
    int Store_Synth_Disto = FALSE;
    int Store_Synth_Osc2 = FALSE;
    int Store_Synth_Osc3 = FALSE;

    int Store_Synth_Phase_Osc1 = FALSE;
    int Store_Synth_Phase_Osc2 = FALSE;

    int Store_Synth_Lfo1 = FALSE;
    int Store_Synth_Lfo2 = FALSE;
    int Store_Synth_Lfo1_Pitch = FALSE;
    int Store_Synth_Lfo2_Pitch = FALSE;
    
    int Store_Synth_Env1 = FALSE;
    int Store_Synth_Env2 = FALSE;
    int Store_Synth_Env1_Pitch = FALSE;
    int Store_Synth_Env2_Pitch = FALSE;
    
    int Store_Instruments = FALSE;

    int Store_Volume_Column = FALSE;
    int Store_FX_NoteCut = FALSE;
    int Store_FX_PitchUp = FALSE;
    int Store_FX_PitchDown = FALSE;
    int Store_FX_SetVolume = FALSE;
    int Store_FX_TranceSlicer = FALSE;
    int Store_FX_TranceGlider = FALSE;
    int Store_FX_PatternLoop = FALSE;
    int Store_FX_SetCutOff = FALSE;
    int Store_FX_SetSampleOffset = FALSE;
    int Store_FX_SetRandomCutOff = FALSE;
    int Store_FX_SlideUpCutOff = FALSE;
    int Store_FX_SlideDownCutOff = FALSE;
    int Store_FX_PatternBreak = FALSE;
    int Store_FX_NoteRetrigger = FALSE;
    int Store_FX_SetBPM = FALSE;
    int Store_FX_SetSpeed = FALSE;
    int Store_FX_SendToDelayCommand = FALSE;
    int Store_FX_SendToReverbCommand = FALSE;
    int Store_FX_SetDistortionThreshold = FALSE;
    int Store_FX_SetDistortionClamp = FALSE;
    int Store_FX_SetFilterResonance = FALSE;
    int Store_FX_SetFilterType = FALSE;
    int Store_FX_ResetFilterLfo = FALSE;
    int Store_FX_AutoFadeIn = FALSE;
    int Store_FX_AutoFadeOut = FALSE;
    int Store_FX_VolumeSlideUp = FALSE;
    int Store_FX_VolumeSlideDown = FALSE;
    int Store_FX_SetGlobalVolume = FALSE;
    int Store_FX_Arpeggio = FALSE;
    int Store_FX_Vibrato = FALSE;
    int Store_FX_Reverse = FALSE;
    int Store_FX_PosJump = FALSE;
    int Store_FX_FineVolumeSlideUp = FALSE;
    int Store_FX_FineVolumeSlideDown = FALSE;
    int Store_FX_FinePitchUp = FALSE;
    int Store_FX_FinePitchDown = FALSE;
    int Store_FX_SwitchFlanger = FALSE;

    int Store_Synth = FALSE;

    int Store_TrackFilters = FALSE;

    int Store_Filter_LoHiBand = FALSE;
    int Store_Filter_Lo24 = FALSE;
    int Store_Filter_Lo48 = FALSE;
    int Store_Filter_Lp24 = FALSE;
    int Store_Filter_AModM = FALSE;
    int Store_Filter_AModS = FALSE;
    int Store_Filter_SingleM = FALSE;
    int Store_Filter_SingleS = FALSE;
    int Store_Filter_Eqm15 = FALSE;
    int Store_Filter_Eqm6 = FALSE;
    int Store_Filter_Eqp6 = FALSE;
    int Store_Filter_Eqp15 = FALSE;
    int Store_Filter_Delta = FALSE;
    int Store_Filter_DistL = FALSE;
    int Store_Filter_DistM = FALSE;
    int Store_Filter_DistH = FALSE;
    int Store_Filter_Dist = FALSE;
    int Store_Filter_Hp12M = FALSE;
    int Store_Filter_Hp12S = FALSE;
    int Store_Filter_Hp24M = FALSE;

    int Store_Synth_Filter = FALSE;
    int Store_Synth_Filter_Lo = FALSE;
    int Store_Synth_Filter_Hi = FALSE;
    int Store_Synth_Sin = FALSE;
    int Store_Synth_Saw = FALSE;
    int Store_Synth_Pulse = FALSE;
    int Store_Synth_WhiteNoise = FALSE;
    int Store_Synth_PinkNoise = FALSE;

    int Number_Fx = 0;
    int Empty_Var = 0;

    unsigned char New_pSequence[256];
    unsigned short New_patternLines[256];
    unsigned char *New_RawPatterns;
    unsigned char done_pattern[256];
    int int_pattern;
    int Real_SongTracks;
    int pwrite;
    int swrite;
    char Constant_Filename[96];

    Out_constants = NULL;
    Out_FX = NULL;
    wait_AutoSave = 0;

    if(!Simulate)
    {
        sprintf(Constant_Filename, "ptk_properties.h");
        Out_constants = fopen(Constant_Filename, "w");
        fprintf(Out_constants, "// Constants for %s.ptp module\n", FileName);
        fprintf(Out_constants, "// Generated for %s replay routine\n", VERSION);
        fprintf(Out_constants, "// Use this file to compile a custom PtkReplay library\n\n", VERSION);
    }

    New_RawPatterns = (unsigned char *) malloc(PATTERN_POOL_SIZE);
    if(!New_RawPatterns) return(FALSE);

    // Writing header & name...
    Write_Mod_Data(&New_Extension, sizeof(char), 4, in);

    // Re-arrange the patterns sequence
    int_pattern = 0;
    memset(done_pattern, 0, sizeof(done_pattern));
    memset(Old_pSequence, -1, sizeof(Old_pSequence));

    for(i = 0; i < sLength; i++)
    {
        if(!done_pattern[pSequence[i]])
        {
            memcpy(New_RawPatterns + (int_pattern * PATTERN_LEN),
                   RawPatterns + (pSequence[i] * PATTERN_LEN),
                   PATTERN_LEN);
            New_patternLines[int_pattern] = patternLines[pSequence[i]];
            Old_pSequence[i] = pSequence[i];
            New_pSequence[i] = int_pattern;
        }
        else
        {
            // Pattern is already known
            New_pSequence[i] = New_pSequence[Search_Sequence(pSequence[i])];
        }
        if(!done_pattern[pSequence[i]])
        {
            int_pattern++;
            done_pattern[pSequence[i]] = TRUE;
        }
    }

    nbr_muted = 0;
    for(i = 0; i < MAX_TRACKS; i++)
    {
        Muted_Tracks[i] = -1;
    }

    // Check which tracks are muted
    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
    {
        if(CHAN_MUTE_STATE[tps_trk])
        {
            Muted_Tracks[nbr_muted] = tps_trk;
            nbr_muted++;
        }
    }
    Real_SongTracks = Songtracks - nbr_muted;

    char_value = (char) int_pattern;
    Write_Mod_Data(&char_value, sizeof(char), 1, in);
    
    // Number of tracks is stored here in .ptp format
    char_value = (char) Real_SongTracks;
    Write_Mod_Data(&char_value, sizeof(char), 1, in);
    
    Write_Mod_Data(&sLength, sizeof(char), 1, in);

    Write_Mod_Data(&Use_Cubic, sizeof(char), 1, in);

    // Patterns sequence
    Write_Mod_Data(New_pSequence, sizeof(char), sLength, in);

    for(i = 0; i < int_pattern; i++)
    {
        char_value = (char) New_patternLines[i];
        Write_Mod_Data(&char_value, sizeof(char), 1, in);
    }

    char_value = (char) Real_SongTracks;
    Write_Mod_Data(Channels_MultiNotes, sizeof(char), char_value, in);

    // Check the instruments
    nbr_User_Instr = 0;
    TmpPatterns = New_RawPatterns;
    for(i = 0; i < MAX_INSTRS; i++)
    {
        Used_Instr[i].new_order = -1;
        Used_Instr[i].old_order = -1;
        Used_Instr2[i].old_order = -1;
    }
    for(pwrite = 0; pwrite < int_pattern; pwrite++)
    {
        TmpPatterns_Rows = TmpPatterns + (pwrite * PATTERN_LEN);
        for(i = 0; i < PATTERN_BYTES; i++)
        {   // Datas
            for(k = 0; k < Songtracks; k++)
            {   // Tracks
                if(!Track_Is_Muted(k))
                {
                    TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                    for(j = 0; j < New_patternLines[pwrite]; j++)
                    {   // Rows
                        TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                        // Store the used instrument
                        switch(i)
                        {
                            case  PATTERN_INSTR1:
                            case  PATTERN_INSTR2:
                            case  PATTERN_INSTR3:
                            case  PATTERN_INSTR4:
                            case  PATTERN_INSTR5:
                            case  PATTERN_INSTR6:
                            case  PATTERN_INSTR7:
                            case  PATTERN_INSTR8:
                            case  PATTERN_INSTR9:
                            case  PATTERN_INSTR10:
                            case  PATTERN_INSTR11:
                            case  PATTERN_INSTR12:
                            case  PATTERN_INSTR13:
                            case  PATTERN_INSTR14:
                            case  PATTERN_INSTR15:
                            case  PATTERN_INSTR16:
                                if(TmpPatterns_Notes[i] < MAX_INSTRS)
                                {
                                    if(Used_Instr[TmpPatterns_Notes[i]].new_order == -1)
                                    {
                                        Used_Instr[TmpPatterns_Notes[i]].new_order = nbr_User_Instr;
                                        Used_Instr[TmpPatterns_Notes[i]].old_order = TmpPatterns_Notes[i];
                                        Used_Instr2[nbr_User_Instr].old_order = TmpPatterns_Notes[i];
                                        nbr_User_Instr++;
                                    }
                                }
                                break;
                            
                            case PATTERN_FX:
                            case PATTERN_FX2:
                                // Count the number of synchro fxs
                                if(TmpPatterns_Notes[i] == 0x7)
                                {
                                    Number_Fx++;
                                }
                        }
                    }
                }
            }
        }
    }

    // Add the prgsynth instruments
    int synth_instr_remap;
    for(i = 0; i < MAX_INSTRS; i++)
    {
        switch(Synthprg[i])
        {
            case SYNTH_WAVE_OFF:
                break;
            case SYNTH_WAVE_CURRENT:
                Store_Synth = TRUE;
                synth_instr_remap = i;
                break;
            default:
                Store_Synth = TRUE;
                synth_instr_remap = Synthprg[i] - 2;
                break;
        }
        if(Synthprg[i] != SYNTH_WAVE_OFF)
        {
            if(Used_Instr[synth_instr_remap].new_order == -1)
            {
                Used_Instr[synth_instr_remap].new_order = nbr_User_Instr;
                Used_Instr[synth_instr_remap].old_order = synth_instr_remap;
                Used_Instr2[nbr_User_Instr].old_order = synth_instr_remap;
                nbr_User_Instr++;
            }
        }
    }

    if(!Simulate)
    {
        // Create the sync fx file data
        Sync_Fx = NULL;
        if(Number_Fx)
        {
            Rec_Fx = 0;
            sprintf(FileName_FX, "%s.psm", FileName);
            Out_FX = fopen(FileName_FX, "wb");

            // Save the FX data
            for(l = 0; l < sLength; l++)
            {
                TmpPatterns_Rows = New_RawPatterns + (New_pSequence[l] * PATTERN_LEN);
                for(i = 0; i < PATTERN_BYTES; i++)
                {   // Datas
                    for(k = 0; k < Songtracks; k++)
                    {   // Tracks
                        if(!Track_Is_Muted(k))
                        {
                            TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                            for(j = 0; j < New_patternLines[New_pSequence[l]]; j++)
                            {   // Rows
                                TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                                if(i == PATTERN_FX || i == PATTERN_FX2)
                                {
                                    // Don't save FX 7
                                    if(TmpPatterns_Notes[i] == 0x7)
                                    {
                                        Rec_Fx++;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Sync_Fx = (LPSYNC_FX) malloc(Rec_Fx * sizeof(SYNC_FX));
            if(Sync_Fx)
            {
                memset(Sync_Fx, -1, Rec_Fx * sizeof(SYNC_FX));
                LPSYNC_FX TmpSync_Fx = Sync_Fx;
                int cur_fx_nbr;
                int real_fx_nbr = 0;

                // Save the FX data
                for(l = 0; l < sLength; l++)
                {
                    TmpPatterns_Rows = New_RawPatterns + (New_pSequence[l] * PATTERN_LEN);
                    for(i = 0; i < PATTERN_BYTES; i++)
                    {   // Datas
                        for(k = 0; k < Songtracks; k++)
                        {   // Tracks
                            if(!Track_Is_Muted(k))
                            {
                                TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                                for(j = 0; j < New_patternLines[New_pSequence[l]]; j++)
                                {   // Rows
                                    TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                                    if(i == PATTERN_FX || i == PATTERN_FX2)
                                    {
                                        // Don't save FX 7
                                        if(TmpPatterns_Notes[i] == 0x7)
                                        {
                                            // Discard multiple fxs / line
                                            cur_fx_nbr = Check_FX(l, j);
                                            if(cur_fx_nbr == -1)
                                            {
                                                TmpSync_Fx->Pos = l;
                                                TmpSync_Fx->Row = j;
                                                TmpSync_Fx->Data = TmpPatterns_Notes[i + 1];
                                                TmpSync_Fx++;
                                                real_fx_nbr++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Write_Data(&real_fx_nbr, sizeof(short), 1, Out_FX);
                for(i = 0; i < real_fx_nbr; i++)
                {
                    Save_FX(Sync_Fx[i].Pos, Sync_Fx[i].Row, Sync_Fx[i].Data);
                }
                free(Sync_Fx);
            }
        }
    }

    // Look for the used fx
    TmpPatterns = New_RawPatterns;
    for(pwrite = 0; pwrite < int_pattern; pwrite++)
    {
        TmpPatterns_Rows = TmpPatterns + (pwrite * PATTERN_LEN);
        for(i = 0; i < PATTERN_BYTES; i++)
        {   // Datas
            for(k = 0; k < Songtracks; k++)
            {   // Tracks
                if(!Track_Is_Muted(k))
                {
                    TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                    for(j = 0; j < New_patternLines[pwrite]; j++)
                    {   // Rows
                        TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                        // Check the volume column
                        if(i == PATTERN_VOLUME)
                        {
                            if(TmpPatterns_Notes[i] != 0xff)
                            {
                                if(TmpPatterns_Notes[i] & 0xf0 == 0xf0)
                                {
                                    Store_FX_NoteCut = TRUE;
                                }
                                if(TmpPatterns_Notes[i] <= 64)
                                {
                                    Store_Volume_Column = TRUE;
                                }
                            }
                        }

                        // Check the effects column
                        if(i == PATTERN_FX || i == PATTERN_FX2)
                        {
                            switch(TmpPatterns_Notes[i])
                            {
                                // $01 Pitch Up 
                                case 0x1:
                                    Store_FX_PitchUp = TRUE;
                                    break;

                                // $02 Pitch Down
                                case 0x2:
                                    Store_FX_PitchDown = TRUE;
                                    break;

                                // $03 Set volume
                                case 0x3:
                                    Store_FX_SetVolume = TRUE;
                                    break;

                                // $04 Trance slicer
                                case 0x4:
                                    Store_FX_TranceSlicer = TRUE;
                                    break;

                                // $05 Glider
                                case 0x5:
                                    Store_FX_TranceGlider = TRUE;
                                    break;

                                // $06 Pattern loop
                                case 0x6:
                                    Store_FX_PatternLoop = TRUE;
                                    break;

                                // $08 SetCutOff
                                case 0x8:
                                    Store_FX_SetCutOff = TRUE;
                                    break;

                                // $09 SetSampleOffset
                                case 0x9:
                                    Store_FX_SetSampleOffset = TRUE;
                                    break;

                                // $0a SetRandomCutOff
                                case 0xa:
                                    Store_FX_SetRandomCutOff = TRUE;
                                    break;

                                // $0b SlideUpCutOff
                                case 0xb:
                                    Store_FX_SlideUpCutOff = TRUE;
                                    break;

                                // $0c SlideDownCutOff
                                case 0xc:
                                    Store_FX_SlideDownCutOff = TRUE;
                                    break;

                                // $0d PatternBreak
                                case 0xd:
                                    Store_FX_PatternBreak = TRUE;
                                    break;

                                // $0e Note retrigger
                                case 0xe:
                                    Store_FX_NoteRetrigger = TRUE;
                                    break;

                                // $f0 Set BPM
                                case 0xf0:
                                    Store_FX_SetBPM = TRUE;
                                    break;

                                // $0f Set speed
                                case 0xf:
                                    Store_FX_SetSpeed = TRUE;
                                    break;

                                // $10 Send to delay Command
                                case 0x10:
                                    Store_FX_SendToDelayCommand = TRUE;
                                    break;

                                // $11 Send to reverb Command
                                case 0x11:
                                    Store_FX_SendToReverbCommand = TRUE;
                                    break;

                                // $12 Set distortion Threshold
                                case 0x12:
                                    Store_FX_SetDistortionThreshold = TRUE;
                                    break;

                                // $13 Set distortion clamp
                                case 0x13:
                                    Store_FX_SetDistortionClamp = TRUE;
                                    break;

                                // $14 Set filter resonance
                                case 0x14:
                                    Store_FX_SetFilterResonance = TRUE;
                                    break;

                                // $15 Set filter Type
                                case 0x15:
                                    Store_FX_SetFilterType = TRUE;
                                    switch(TmpPatterns_Notes[i + 1])
                                    {
                                        case 0:
                                        case 1:
                                        case 2:
                                        case 3:
                                            Store_Filter_LoHiBand = TRUE;
                                            break;
                                        case 4:
                                            break;
                                        case 5:
                                            Store_Filter_Lo24 = TRUE;
                                            break;
                                        case 6:
                                            Store_Filter_Lo48 = TRUE;
                                            break;
                                        case 7:
                                            Store_Filter_Lp24 = TRUE;
                                            break;
                                        case 8:
                                            Store_Filter_AModM = TRUE;
                                            break;
                                        case 9:
                                            Store_Filter_AModS = TRUE;
                                            break;
                                        case 10:
                                            Store_Filter_SingleM = TRUE;
                                            break;
                                        case 11:
                                            Store_Filter_SingleS = TRUE;
                                            break;
                                        case 12:
                                            Store_Filter_Eqm15 = TRUE;
                                            break;
                                        case 13:
                                            Store_Filter_Eqm6 = TRUE;
                                            break;
                                        case 14:
                                            Store_Filter_Eqp6 = TRUE;
                                            break;
                                        case 15:
                                            Store_Filter_Eqp15 = TRUE;
                                            break;
                                        case 16:
                                            Store_Filter_Delta = TRUE;
                                            break;
                                        case 17:
                                            Store_Filter_DistL = TRUE;
                                            break;
                                        case 18:
                                            Store_Filter_DistM = TRUE;
                                            break;
                                        case 19:
                                            Store_Filter_DistH = TRUE;
                                            break;
                                        case 20:
                                            Store_Filter_Dist = TRUE;
                                            break;
                                        case 21:
                                            Store_Filter_Hp12M = TRUE;
                                            break;
                                        case 22:
                                            Store_Filter_Hp12S = TRUE;
                                            break;
                                        case 23:
                                            Store_Filter_Hp24M = TRUE;
                                            break;
                                    }
                                    break;

                                // $16 Reset filter lfo
                                case 0x16:
                                    Store_FX_ResetFilterLfo = TRUE;
                                    break;

                                // $17 Auto fade in xx ticks
                                case 0x17:
                                    Store_FX_AutoFadeIn = TRUE;
                                    break;

                                // $18 Auto fade out xx ticks
                                case 0x18:
                                    Store_FX_AutoFadeOut = TRUE;
                                    break;

                                // $19 Volume slide up
                                case 0x19:
                                    Store_FX_VolumeSlideUp = TRUE;
                                    break;

                                // $1a Volume slide down
                                case 0x1a:
                                    Store_FX_VolumeSlideDown = TRUE;
                                    break;

                                // $1c Arpeggio
                                case 0x1b:
                                    Store_FX_Arpeggio = TRUE;
                                    break;

                                // $1c Set global volume
                                case 0x1c:
                                    Store_FX_SetGlobalVolume = TRUE;
                                    break;

                                // $1d Vibrato
                                case 0x1d:
                                    Store_FX_Vibrato = TRUE;
                                    break;

                                // $1e Reverse playing way
                                case 0x1e:
                                    Store_FX_Reverse = TRUE;
                                    break;

                                // $1f Position jump
                                case 0x1f:
                                    Store_FX_PosJump = TRUE;
                                    break;

                                // $20 Fine volume slide up
                                case 0x20:
                                    Store_FX_FineVolumeSlideUp = TRUE;
                                    break;

                                // $20 Fine volume slide down
                                case 0x21:
                                    Store_FX_FineVolumeSlideDown = TRUE;
                                    break;

                                // $22 Fine pitch up
                                case 0x22:
                                    Store_FX_FinePitchUp = TRUE;
                                    break;

                                // $23 Fine pitch down
                                case 0x23:
                                    Store_FX_FinePitchDown = TRUE;
                                    break;

                                // $24 Switch flanger
                                case 0x24:
                                    Store_FX_SwitchFlanger = TRUE;
                                    break;

                                // $31 First TB303 control
                                case 0x31:
                                    Store_303_1 = TRUE;
                                    break;

                                // $32 Second TB303 control
                                case 0x32:
                                    Store_303_2 = TRUE;
                                    break;
                            }
                        }
                        switch(i)
                        {
                            case PATTERN_FX:
                            case PATTERN_FX2:
                                // Don't save FX 7
                                if(TmpPatterns_Notes[i] == 0x7)
                                {
                                    Write_Mod_Data(&Empty_Var, sizeof(char), 1, in);
                                }
                                else
                                {
                                    Write_Mod_Data(TmpPatterns_Notes + i, sizeof(char), 1, in);
                                }
                                break;

                            case PATTERN_FXDATA:
                            case PATTERN_FXDATA2:
                                // Don't save Fx 7 datas
                                if(TmpPatterns_Notes[i - 1] == 0x7)
                                {
                                    Write_Mod_Data(&Empty_Var, sizeof(char), 1, in);
                                }
                                else
                                {
                                    Write_Mod_Data(TmpPatterns_Notes + i, sizeof(char), 1, in);
                                }
                                break;

                            case PATTERN_INSTR1:
                            case PATTERN_INSTR2:
                            case PATTERN_INSTR3:
                            case PATTERN_INSTR4:
                            case PATTERN_INSTR5:
                            case PATTERN_INSTR6:
                            case PATTERN_INSTR7:
                            case PATTERN_INSTR8:
                            case PATTERN_INSTR9:
                            case PATTERN_INSTR10:
                            case PATTERN_INSTR11:
                            case PATTERN_INSTR12:
                            case PATTERN_INSTR13:
                            case PATTERN_INSTR14:
                            case PATTERN_INSTR15:
                            case PATTERN_INSTR16:
                                // Replace the instrument order
                                if(TmpPatterns_Notes[i] < MAX_INSTRS)
                                {
                                    TmpPatterns_Notes[i] = Get_Instr_New_Order(TmpPatterns_Notes[i]);
                                }
                                // no break on purpose

                            default:
                                Write_Mod_Data(TmpPatterns_Notes + i, sizeof(char), 1, in);
                                break;
                        }
                    }
                }
            }
        }
    }

    switch(Use_Cubic)
    {
        case CUBIC_INT:
            Save_Constant("PTK_USE_CUBIC", TRUE);
            Save_Constant("PTK_USE_SPLINE", FALSE);
            break;
        case SPLINE_INT:
            Save_Constant("PTK_USE_CUBIC", FALSE);
            Save_Constant("PTK_USE_SPLINE", TRUE);
            break;
        default:
            Save_Constant("PTK_USE_CUBIC", FALSE);
            Save_Constant("PTK_USE_SPLINE", FALSE);
            break;
    }

    Save_Constant("PTK_303", Store_303_1 | Store_303_2);

    Save_Constant("PTK_FX_0", Store_FX_PitchUp | Store_FX_PitchDown |
                              Store_FX_SetVolume | Store_FX_TranceSlicer |
                              Store_FX_TranceGlider);

    Save_Constant("PTK_FX_X", Store_FX_SetCutOff |
                              Store_FX_SetRandomCutOff |
                              Store_FX_SlideUpCutOff |
                              Store_FX_SlideDownCutOff |
                              Store_FX_NoteRetrigger |
                              Store_FX_SetBPM |
                              Store_FX_SetSpeed |
                              Store_FX_SendToDelayCommand |
                              Store_FX_SendToReverbCommand |
                              Store_FX_SetDistortionThreshold |
                              Store_FX_SetDistortionClamp |
                              Store_FX_SetFilterResonance |
                              Store_FX_SetFilterType |
                              Store_FX_ResetFilterLfo |
                              Store_FX_AutoFadeIn |
                              Store_FX_AutoFadeOut |
                              Store_FX_VolumeSlideUp |
                              Store_FX_VolumeSlideDown |
                              Store_FX_SetGlobalVolume
                             );

    Save_Constant("PTK_VOLUME_COLUMN", Store_Volume_Column);

    Save_Constant("PTK_FX_NOTECUT", Store_FX_NoteCut);
    Save_Constant("PTK_FX_PITCHUP", Store_FX_PitchUp);
    Save_Constant("PTK_FX_PITCHDOWN", Store_FX_PitchDown);
    Save_Constant("PTK_FX_SETVOLUME", Store_FX_SetVolume);
    Save_Constant("PTK_FX_TRANCESLICER", Store_FX_TranceSlicer);
    Save_Constant("PTK_FX_TRANCEGLIDER", Store_FX_TranceGlider);

    Save_Constant("PTK_FX_PATTERNLOOP", Store_FX_PatternLoop);
    Save_Constant("PTK_FX_SETCUTOFF", Store_FX_SetCutOff);
    Save_Constant("PTK_FX_SETSAMPLEOFFSET", Store_FX_SetSampleOffset);
    Save_Constant("PTK_FX_SETRANDOMCUTOFF", Store_FX_SetRandomCutOff);
    Save_Constant("PTK_FX_SLIDEUPCUTOFF", Store_FX_SlideUpCutOff);
    Save_Constant("PTK_FX_SLIDEDOWNCUTOFF", Store_FX_SlideDownCutOff);
    Save_Constant("PTK_FX_PATTERNBREAK", Store_FX_PatternBreak);
    Save_Constant("PTK_FX_NOTERETRIGGER", Store_FX_NoteRetrigger);
    Save_Constant("PTK_FX_SETBPM", Store_FX_SetBPM);
    Save_Constant("PTK_FX_SETSPEED", Store_FX_SetSpeed);
    Save_Constant("PTK_FX_SENDTODELAYCOMMAND", Store_FX_SendToDelayCommand);
    Save_Constant("PTK_FX_SENDTOREVERBCOMMAND", Store_FX_SendToReverbCommand);
    Save_Constant("PTK_FX_SETDISTORTIONTHRESHOLD", Store_FX_SetDistortionThreshold);
    Save_Constant("PTK_FX_SETDISTORTIONCLAMP", Store_FX_SetDistortionClamp);
    Save_Constant("PTK_FX_SETFILTERRESONANCE", Store_FX_SetFilterResonance);
    Save_Constant("PTK_FX_SETFILTERTYPE", Store_FX_SetFilterType);
    Save_Constant("PTK_FX_RESETFILTERLFO", Store_FX_ResetFilterLfo);
    Save_Constant("PTK_FX_AUTOFADEIN", Store_FX_AutoFadeIn);
    Save_Constant("PTK_FX_AUTOFADEOUT", Store_FX_AutoFadeOut);
    Save_Constant("PTK_FX_AUTOFADEMODE", Store_FX_AutoFadeIn | Store_FX_AutoFadeOut);
    Save_Constant("PTK_FX_VOLUMESLIDEUP", Store_FX_VolumeSlideUp);
    Save_Constant("PTK_FX_VOLUMESLIDEDOWN", Store_FX_VolumeSlideDown);
    Save_Constant("PTK_FX_SETGLOBALVOLUME", Store_FX_SetGlobalVolume);
    Save_Constant("PTK_FX_ARPEGGIO", Store_FX_Arpeggio);
    Save_Constant("PTK_FX_VIBRATO", Store_FX_Vibrato);
    Save_Constant("PTK_FX_REVERSE", Store_FX_Reverse);
    Save_Constant("PTK_FX_POSJUMP", Store_FX_PosJump);
    Save_Constant("PTK_FX_FINEVOLUMESLIDEUP", Store_FX_FineVolumeSlideUp);
    Save_Constant("PTK_FX_FINEVOLUMESLIDEDOWN", Store_FX_FineVolumeSlideDown);
    Save_Constant("PTK_FX_FINEPITCHUP", Store_FX_FinePitchUp);
    Save_Constant("PTK_FX_FINEPITCHDOWN", Store_FX_FinePitchDown);
    Save_Constant("PTK_FX_SWITCHFLANGER", Store_FX_SwitchFlanger);

    // Special but only at tick 0
    Save_Constant("PTK_FX_TICK0", Store_FX_Vibrato | Store_FX_Arpeggio |
                                  Store_FX_PatternLoop | Store_FX_Reverse);

    // Remap the used instruments
    for(i = 0; i < MAX_INSTRS; i++)
    {
        switch(Synthprg[i])
        {
            case SYNTH_WAVE_OFF:
            case SYNTH_WAVE_CURRENT:
                break;

            default:
                Synthprg[i] = Get_Instr_New_Order(Synthprg[i] - 2) + 2;
                break;
        }
    }

    Write_Mod_Data(&nbr_User_Instr, sizeof(int), 1, in);

    // Writing sample data
    for(i = 0; i < MAX_INSTRS; i++)
    {
        // Check if it was used at pattern level
        swrite = Used_Instr2[i].old_order;
        if(swrite != -1)
        {
            Write_Mod_Data(&Synthprg[swrite], sizeof(char), 1, in);

            if(Synthprg[swrite])
            {

                Write_Mod_Data(&PARASynth[swrite].osc1_waveform, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].osc2_waveform, sizeof(char), 1, in);
                if(PARASynth[swrite].osc1_waveform == WAVEFORM_WAV) Store_Instr_Waveform_Osc1 = TRUE;
                if(PARASynth[swrite].osc2_waveform == WAVEFORM_WAV) Store_Instr_Waveform_Osc2 = TRUE;

                if(PARASynth[swrite].osc1_waveform == WAVEFORM_SIN) Store_Synth_Sin = TRUE;
                if(PARASynth[swrite].osc2_waveform == WAVEFORM_SIN) Store_Synth_Sin = TRUE;
                if(PARASynth[swrite].osc1_waveform == WAVEFORM_SAW) Store_Synth_Saw = TRUE;
                if(PARASynth[swrite].osc2_waveform == WAVEFORM_SAW) Store_Synth_Saw = TRUE;
                if(PARASynth[swrite].osc1_waveform == WAVEFORM_PULSE) Store_Synth_Pulse = TRUE;
                if(PARASynth[swrite].osc2_waveform == WAVEFORM_PULSE) Store_Synth_Pulse = TRUE;
                if(PARASynth[swrite].osc1_waveform == WAVEFORM_WHITE) Store_Synth_WhiteNoise = TRUE;
                if(PARASynth[swrite].osc2_waveform == WAVEFORM_WHITE) Store_Synth_WhiteNoise = TRUE;
                if(PARASynth[swrite].osc1_waveform == WAVEFORM_PINK) Store_Synth_PinkNoise = TRUE;
                if(PARASynth[swrite].osc2_waveform == WAVEFORM_PINK) Store_Synth_PinkNoise = TRUE;

                if(PARASynth[swrite].osc2_waveform != WAVEFORM_NONE) Store_Synth_Osc2 = TRUE;

                if(PARASynth[swrite].osc1_pw != 256) Store_Synth_Phase_Osc1 = TRUE;
                if(PARASynth[swrite].osc2_pw != 256) Store_Synth_Phase_Osc2 = TRUE;

                float fvalue;
                int ivalue;
                int64 i64value;

                fvalue = (float) (PARASynth[swrite].osc1_pw - 256) / 256.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = (float) (PARASynth[swrite].osc2_pw - 256) / 256.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = (float) (PARASynth[swrite].osc2_detune - 64.0f) * 0.0625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
    
                fvalue = (float) PARASynth[swrite].osc2_finetune * 0.0078125f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = (float) PARASynth[swrite].vcf_cutoff * 0.0078125f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                
                fvalue = (float) PARASynth[swrite].vcf_resonance * 0.0078125f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                Write_Mod_Data(&PARASynth[swrite].vcf_type, sizeof(char), 1, in);
                if(PARASynth[swrite].vcf_type != 2) Store_Synth_Filter = TRUE;
                if(PARASynth[swrite].vcf_type == 0) Store_Synth_Filter_Lo = TRUE;
                if(PARASynth[swrite].vcf_type == 1) Store_Synth_Filter_Hi = TRUE;

                fvalue = ((float) (PARASynth[swrite].env1_attack + 1)) / 512.0f;
                if(fvalue < 0.1f) fvalue = 0.1f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                
                fvalue = ((float) (PARASynth[swrite].env1_decay + 1)) / 512.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = (float) PARASynth[swrite].env1_sustain * 0.0078125f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = ((float) (PARASynth[swrite].env1_release + 1)) / 512.0f;
                if(fvalue < 0.15f) fvalue = 0.15f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = ((float) (PARASynth[swrite].env2_attack + 1)) / 512.0f;
                if(fvalue < 0.1f) fvalue = 0.1f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = ((float) (PARASynth[swrite].env2_decay + 1)) / 512.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = (float) (PARASynth[swrite].env2_sustain * 0.0078125f);
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = ((float) (PARASynth[swrite].env2_release + 1)) / 512.0f;
                if(fvalue < 0.15f) fvalue = 0.15f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = (float) (PARASynth[swrite].lfo1_period * 2) + 1;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                ivalue = (int) (((float) SamplesPerTick * 0.000277f * fvalue));
                Write_Mod_Data(&ivalue, sizeof(int), 1, in);

                fvalue = (float) (PARASynth[swrite].lfo2_period * 2) + 1;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                ivalue = (int) (((float) SamplesPerTick * 0.000277f * fvalue));
                Write_Mod_Data(&ivalue, sizeof(int), 1, in);

                if(Store_Synth_Phase_Osc1)
                {
                    if(PARASynth[swrite].lfo1_osc1_pw != 64) Store_Synth_Lfo1 = TRUE;
                }
                if(Store_Synth_Phase_Osc2)
                {
                    if(PARASynth[swrite].lfo1_osc2_pw != 64) Store_Synth_Lfo1 = TRUE;
                }
                if(PARASynth[swrite].lfo1_osc1_pitch != 64) Store_Synth_Lfo1 = TRUE;
                if(PARASynth[swrite].lfo1_osc2_pitch != 64) Store_Synth_Lfo1 = TRUE;
                if(PARASynth[swrite].lfo1_osc1_pitch != 64) Store_Synth_Lfo1_Pitch = TRUE;
                if(PARASynth[swrite].lfo1_osc2_pitch != 64) Store_Synth_Lfo1_Pitch = TRUE;
                if(PARASynth[swrite].lfo1_osc1_volume != 64) Store_Synth_Lfo1 = TRUE;
                if(PARASynth[swrite].lfo1_osc2_volume != 64) Store_Synth_Lfo1 = TRUE;
                if(PARASynth[swrite].lfo1_vcf_cutoff != 64) Store_Synth_Lfo1 = TRUE;
                if(PARASynth[swrite].lfo1_vcf_resonance != 64) Store_Synth_Lfo1 = TRUE;

                fvalue = ((float) PARASynth[swrite].lfo1_osc1_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo1_osc2_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo1_osc1_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo1_osc2_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo1_osc1_volume - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo1_osc2_volume - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo1_vcf_cutoff - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo1_vcf_resonance - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                if(Store_Synth_Phase_Osc1)
                {
                    if(PARASynth[swrite].lfo2_osc1_pw != 64) Store_Synth_Lfo2 = TRUE;
                }
                if(Store_Synth_Phase_Osc2)
                {
                    if(PARASynth[swrite].lfo2_osc2_pw != 64) Store_Synth_Lfo2 = TRUE;
                }
                if(PARASynth[swrite].lfo2_osc1_pitch != 64) Store_Synth_Lfo2 = TRUE;
                if(PARASynth[swrite].lfo2_osc2_pitch != 64) Store_Synth_Lfo2 = TRUE;
                if(PARASynth[swrite].lfo2_osc1_pitch != 64) Store_Synth_Lfo2_Pitch = TRUE;
                if(PARASynth[swrite].lfo2_osc2_pitch != 64) Store_Synth_Lfo2_Pitch = TRUE;
                if(PARASynth[swrite].lfo2_osc1_volume != 64) Store_Synth_Lfo2 = TRUE;
                if(PARASynth[swrite].lfo2_osc2_volume != 64) Store_Synth_Lfo2 = TRUE;
                if(PARASynth[swrite].lfo2_vcf_cutoff != 64) Store_Synth_Lfo2 = TRUE;
                if(PARASynth[swrite].lfo2_vcf_resonance != 64) Store_Synth_Lfo2 = TRUE;

                fvalue = ((float) PARASynth[swrite].lfo2_osc1_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo2_osc2_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo2_osc1_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo2_osc2_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo2_osc1_volume - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo2_osc2_volume - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo2_vcf_cutoff - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].lfo2_vcf_resonance - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                if(Store_Synth_Phase_Osc1)
                {
                    if(PARASynth[swrite].env1_osc1_pw != 64) Store_Synth_Env1 = TRUE;
                }
                if(Store_Synth_Phase_Osc2)
                {
                    if(PARASynth[swrite].env1_osc2_pw != 64) Store_Synth_Env1 = TRUE;
                }
                if(PARASynth[swrite].env1_osc1_pitch != 64) Store_Synth_Env1 = TRUE;
                if(PARASynth[swrite].env1_osc2_pitch != 64) Store_Synth_Env1 = TRUE;
                if(PARASynth[swrite].env1_osc1_pitch != 64) Store_Synth_Env1_Pitch = TRUE;
                if(PARASynth[swrite].env1_osc2_pitch != 64) Store_Synth_Env1_Pitch = TRUE;
                if(PARASynth[swrite].env1_osc1_volume != 127) Store_Synth_Env1 = TRUE;
                if(PARASynth[swrite].env1_osc2_volume != 127) Store_Synth_Env1 = TRUE;
                if(PARASynth[swrite].env1_vcf_cutoff != 64) Store_Synth_Env1 = TRUE;
                if(PARASynth[swrite].env1_vcf_resonance != 64) Store_Synth_Env1 = TRUE;

                fvalue = ((float) PARASynth[swrite].env1_osc1_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env1_osc2_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env1_osc1_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env1_osc2_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env1_osc1_volume - 64) * 0.015625f;
                //if((PARASynth[swrite].env1_osc1_volume - 64) == 0) fvalue = 1.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env1_osc2_volume - 64) * 0.015625f;
                //if((PARASynth[swrite].env1_osc2_volume - 64) == 0) fvalue = 1.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env1_vcf_cutoff - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env1_vcf_resonance - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                if(Store_Synth_Phase_Osc1)
                {
                    if(PARASynth[swrite].env2_osc1_pw != 64) Store_Synth_Env2 = TRUE;
                }
                if(Store_Synth_Phase_Osc2)
                {
                    if(PARASynth[swrite].env2_osc2_pw != 64) Store_Synth_Env2 = TRUE;
                }
                if(PARASynth[swrite].env2_osc1_pitch != 64) Store_Synth_Env2 = TRUE;
                if(PARASynth[swrite].env2_osc2_pitch != 64) Store_Synth_Env2 = TRUE;
                if(PARASynth[swrite].env2_osc1_pitch != 64) Store_Synth_Env2_Pitch = TRUE;
                if(PARASynth[swrite].env2_osc2_pitch != 64) Store_Synth_Env2_Pitch = TRUE;
                if(PARASynth[swrite].env2_osc1_volume != 127) Store_Synth_Env2 = TRUE;
                if(PARASynth[swrite].env2_osc2_volume != 127) Store_Synth_Env2 = TRUE;
                if(PARASynth[swrite].env2_vcf_cutoff != 64) Store_Synth_Env2 = TRUE;
                if(PARASynth[swrite].env2_vcf_resonance != 64) Store_Synth_Env2 = TRUE;

                fvalue = ((float) PARASynth[swrite].env2_osc1_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env2_osc2_pw - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env2_osc1_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env2_osc2_pitch - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env2_osc1_volume - 64) * 0.015625f;
                //if((PARASynth[swrite].env2_osc1_volume - 64) == 0) fvalue = 1.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env2_osc2_volume - 64) * 0.015625f;
                //if((PARASynth[swrite].env2_osc2_volume - 64) == 0) fvalue = 1.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env2_vcf_cutoff - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) PARASynth[swrite].env2_vcf_resonance - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = ((float) PARASynth[swrite].osc3_volume - 64) * 0.015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                Write_Mod_Data(&PARASynth[swrite].osc3_switch, sizeof(char), 1, in);
                if(PARASynth[swrite].osc3_switch) Store_Synth_Osc3 = TRUE;

                fvalue = ((float) PARASynth[swrite].ptc_glide * (float) PARASynth[swrite].ptc_glide) * 0.0000015625f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                i64value = (int64) ((double) fvalue * 4294967296.0);
                Write_Mod_Data(&i64value, sizeof(int64), 1, in);

                fvalue = ((float) PARASynth[swrite].glb_volume) * 0.0078125f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = (((float) PARASynth[swrite].disto)) + 1.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                if(PARASynth[swrite].disto) Store_Synth_Disto = TRUE;

                fvalue = ((float) (PARASynth[swrite].lfo1_attack + 1)) / 512.0f;
                if(fvalue < 0.1f) fvalue = 0.1f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) (PARASynth[swrite].lfo1_decay + 1)) / 512.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = (float) PARASynth[swrite].lfo1_sustain * 0.0078125f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) (PARASynth[swrite].lfo1_release + 1)) / 512.0f;
                if(fvalue < 0.15f) fvalue = 0.15f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);

                fvalue = ((float) (PARASynth[swrite].lfo2_attack + 1)) / 512.0f;
                if(fvalue < 0.1f) fvalue = 0.1f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) (PARASynth[swrite].lfo2_decay + 1)) / 512.0f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = (float) PARASynth[swrite].lfo2_sustain * 0.0078125f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
                fvalue = ((float) (PARASynth[swrite].lfo2_release + 1)) / 512.0f;
                if(fvalue < 0.15f) fvalue = 0.15f;
                Write_Mod_Data(&fvalue, sizeof(float), 1, in);
            }

#if defined(__NO_CODEC__)
            char No_Comp = SMP_PACK_NONE;
            Write_Mod_Data(&No_Comp, sizeof(char), 1, in);
#else
            Write_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);
            switch(SampleCompression[swrite])
            {
                case SMP_PACK_MP3:
                    Write_Mod_Data(&Mp3_BitRate[swrite], sizeof(char), 1, in);
                    break;

                case SMP_PACK_AT3:
                    Write_Mod_Data(&At3_BitRate[swrite], sizeof(char), 1, in);
                    break;
            }
#endif

            // Compression type

            // 16 splits / instrument
            for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
            {
                Write_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite])
                {
                    Store_Instruments = TRUE;
                    int Apply_Interpolation = FALSE;

#if !defined(__NO_CODEC__)
                    // Check if any of the packing scheme has been used
                    switch(SampleCompression[swrite])
                    {
                        case SMP_PACK_AT3:
                            Store_At3 = TRUE;
                            Apply_Interpolation = TRUE;
                            break;

                        case SMP_PACK_GSM:
                            Store_Gsm = TRUE;
                            Apply_Interpolation = TRUE;
                            break;

                        case SMP_PACK_MP3:
                            Store_Mp3 = TRUE;
                            Apply_Interpolation = TRUE;
                            break;

                        case SMP_PACK_TRUESPEECH:
                            Store_TrueSpeech = TRUE;
                            Apply_Interpolation = TRUE;
                            break;

                        case SMP_PACK_ADPCM:
                            Store_ADPCM = TRUE;
                            Apply_Interpolation = TRUE;
                            break;

                        case SMP_PACK_8BIT:
                            Store_8Bit = TRUE;
                            Apply_Interpolation = TRUE;
                            break;
                    }
#endif

                    Uint32 Real_Len = SampleNumSamples[swrite][slwrite];
                    Uint32 Calc_Len = SampleNumSamples[swrite][slwrite];
                    Uint32 Loop_Start = LoopStart[swrite][slwrite];
                    Uint32 Loop_End = LoopEnd[swrite][slwrite];
                    Uint32 iSmp;
                    short *Smp_Dats = NULL;

                    if(Apply_Interpolation)
                    {
                        Loop_Start /= 2;
                        Loop_Start *= 2;

                        Loop_End /= 2;
                        Loop_End *= 2;

                        Calc_Len /= 2;
                        Calc_Len *= 2;
                    }

                    // Don't write past the looping point
                    if(LoopType[swrite][slwrite])
                    {
                        Calc_Len = Loop_End + 2;
                    }

                    Write_Mod_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                    
                    Write_Mod_Data(&Loop_Start, sizeof(int), 1, in);
                    Write_Mod_Data(&Loop_End, sizeof(int), 1, in);
                    Write_Mod_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                    
                    Write_Mod_Data(&Calc_Len, sizeof(int), 1, in);
                    Write_Mod_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                    Write_Mod_Data(&Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                    Write_Mod_Data(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                    if(Apply_Interpolation)
                    {
                        Calc_Len /= 2;
                        Smp_Dats = (short *) malloc(Real_Len * 2 + 8);
                        memset(Smp_Dats, 0, Real_Len * 2 + 8);
                        // Halve the sample
                        short *Sample = Get_WaveForm(swrite, 0, slwrite);
                        for(iSmp = 0; iSmp < Calc_Len; iSmp++)
                        {
                            Smp_Dats[iSmp] = *(Sample + (iSmp * 2));
                        }
                        Pack_Sample(in,
                                    Smp_Dats,
                                    Calc_Len,
                                    SampleCompression[swrite],
                                    SampleCompression[swrite] == SMP_PACK_MP3 ?
                                            Type_Mp3_BitRate[Mp3_BitRate[swrite]] :
                                            Type_At3_BitRate[At3_BitRate[swrite]]
                                   );
                    }
                    else
                    {
                        Pack_Sample(in,
                                    Get_WaveForm(swrite, 0, slwrite),
                                    Calc_Len,
                                    SampleCompression[swrite],
                                    SampleCompression[swrite] == SMP_PACK_MP3 ?
                                            Type_Mp3_BitRate[Mp3_BitRate[swrite]] :
                                            Type_At3_BitRate[At3_BitRate[swrite]]
                                   );
                    }
                    // Stereo mode ?
                    Write_Mod_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        if(Apply_Interpolation)
                        {
                            // Halve the sample
                            short *Sample = Get_WaveForm(swrite, 1, slwrite);
                            for(iSmp = 0; iSmp < Calc_Len; iSmp++)
                            {
                                Smp_Dats[iSmp] = *(Sample + (iSmp * 2));
                            }
                            Pack_Sample(in,
                                        Smp_Dats,
                                        Calc_Len,
                                        SampleCompression[swrite],
                                        SampleCompression[swrite] == SMP_PACK_MP3 ?
                                                Type_Mp3_BitRate[Mp3_BitRate[swrite]] :
                                                Type_At3_BitRate[At3_BitRate[swrite]]
                                       );
                        }
                        else
                        {
                            Pack_Sample(in,
                                        Get_WaveForm(swrite, 1, slwrite),
                                        Calc_Len,
                                        SampleCompression[swrite],
                                        SampleCompression[swrite] == SMP_PACK_MP3 ?
                                                Type_Mp3_BitRate[Mp3_BitRate[swrite]] :
                                                Type_At3_BitRate[At3_BitRate[swrite]]
                                       );
                        }
                    }
                    free(Smp_Dats);
                }   // Exist Sample
            }
        }
    }

    Save_Constant("PTK_INSTRUMENTS", Store_Instruments);

    Save_Constant("PTK_SYNTH", Store_Synth);

    Save_Constant("PTK_SYNTH_PHASE1", Store_Synth_Phase_Osc1);
    Save_Constant("PTK_SYNTH_PHASE2", Store_Synth_Phase_Osc2);

    Save_Constant("PTK_SYNTH_LFO1", Store_Synth_Lfo1);
    Save_Constant("PTK_SYNTH_LFO2", Store_Synth_Lfo2);

    Save_Constant("PTK_SYNTH_ENV1", Store_Synth_Env1);
    Save_Constant("PTK_SYNTH_ENV2", Store_Synth_Env2);

    Save_Constant("PTK_SYNTH_LFO1_PITCH", Store_Synth_Lfo1_Pitch);
    Save_Constant("PTK_SYNTH_LFO2_PITCH", Store_Synth_Lfo2_Pitch);

    Save_Constant("PTK_SYNTH_ENV1_PITCH", Store_Synth_Env1_Pitch);
    Save_Constant("PTK_SYNTH_ENV2_PITCH", Store_Synth_Env2_Pitch);

    Save_Constant("PTK_SYNTH_PITCH", Store_Synth_Lfo1_Pitch | Store_Synth_Env1_Pitch |
                                     Store_Synth_Lfo2_Pitch | Store_Synth_Env2_Pitch
                 );

    Save_Constant("PTK_SYNTH_DISTO", Store_Synth_Disto);
    Save_Constant("PTK_SYNTH_OSC2", Store_Synth_Osc2);
    Save_Constant("PTK_SYNTH_OSC3", Store_Synth_Osc3);
    Save_Constant("PTK_SYNTH_FILTER", Store_Synth_Filter);
    Save_Constant("PTK_SYNTH_FILTER_LO", Store_Synth_Filter_Lo);
    Save_Constant("PTK_SYNTH_FILTER_HI", Store_Synth_Filter_Hi);

    Save_Constant("PTK_SYNTH_SIN", Store_Synth_Sin);
    Save_Constant("PTK_SYNTH_SAW", Store_Synth_Saw);
    Save_Constant("PTK_SYNTH_PULSE", Store_Synth_Pulse);
    Save_Constant("PTK_SYNTH_WHITE", Store_Synth_WhiteNoise);
    Save_Constant("PTK_SYNTH_PINK", Store_Synth_PinkNoise);

    //Save_Constant("PTK_WAVEFORM", Store_Instr_Waveform_Osc1 | Store_Instr_Waveform_Osc2);
    //Save_Constant("PTK_WAVEFORM_OSC1", Store_Instr_Waveform_Osc1);
    //Save_Constant("PTK_WAVEFORM_OSC2", Store_Instr_Waveform_Osc2);

    Save_Constant("PTK_GSM", Store_Gsm);
    Save_Constant("PTK_MP3", Store_Mp3);
    Save_Constant("PTK_TRUESPEECH", Store_TrueSpeech);
    Save_Constant("PTK_ADPCM", Store_ADPCM);
    Save_Constant("PTK_AT3", Store_At3);
    Save_Constant("PTK_8BIT", Store_8Bit);

    Write_Mod_Data(&compressor, sizeof(char), 1, in);

    for(twrite = 0; twrite < Songtracks; twrite++)
    {
        if(!Track_Is_Muted(twrite))
        {
            if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
            if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;
            Write_Mod_Data(&TCut[twrite], sizeof(float), 1, in);
            Write_Mod_Data(&ICut[twrite], sizeof(float), 1, in);
            Write_Mod_Data(&TPan[twrite], sizeof(float), 1, in);
            Write_Mod_Data(&FType[twrite], sizeof(int), 1, in);
            // On of them must be != 4
            if(FType[twrite] != 4) Store_TrackFilters = TRUE;

            switch(FType[twrite])
            {
                case 0:
                case 1:
                case 2:
                case 3:
                    Store_Filter_LoHiBand = TRUE;
                    break;
                case 4:
                    break;
                case 5:
                    Store_Filter_Lo24 = TRUE;
                    break;
                case 6:
                    Store_Filter_Lo48 = TRUE;
                    break;
                case 7:
                    Store_Filter_Lp24 = TRUE;
                    break;
                case 8:
                    Store_Filter_AModM = TRUE;
                    break;
                case 9:
                    Store_Filter_AModS = TRUE;
                    break;
                case 10:
                    Store_Filter_SingleM = TRUE;
                    break;
                case 11:
                    Store_Filter_SingleS = TRUE;
                    break;
                case 12:
                    Store_Filter_Eqm15 = TRUE;
                    break;
                case 13:
                    Store_Filter_Eqm6 = TRUE;
                    break;
                case 14:
                    Store_Filter_Eqp6 = TRUE;
                    break;
                case 15:
                    Store_Filter_Eqp15 = TRUE;
                    break;
                case 16:
                    Store_Filter_Delta = TRUE;
                    break;
                case 17:
                    Store_Filter_DistL = TRUE;
                    break;
                case 18:
                    Store_Filter_DistM = TRUE;
                    break;
                case 19:
                    Store_Filter_DistH = TRUE;
                    break;
                case 20:
                    Store_Filter_Dist = TRUE;
                    break;
                case 21:
                    Store_Filter_Hp12M = TRUE;
                    break;
                case 22:
                    Store_Filter_Hp12S = TRUE;
                    break;
                case 23:
                    Store_Filter_Hp24M = TRUE;
                    break;
            }
            Write_Mod_Data(&FRez[twrite], sizeof(int), 1, in);
            Write_Mod_Data(&DThreshold[twrite], sizeof(float), 1, in);
            Write_Mod_Data(&DClamp[twrite], sizeof(float), 1, in);
            if(compressor)
            {
                Write_Mod_Data(&DSend[twrite], sizeof(float), 1, in);
            }
            Write_Mod_Data(&CSend[twrite], sizeof(int), 1, in);
            Write_Mod_Data(&Channels_Polyphony[twrite], sizeof(char), 1, in);
        }
    }

    // Writing mod properties
    Write_Mod_Data(&c_threshold, sizeof(int), 1, in);
    Write_Mod_Data(&BeatsPerMin, sizeof(int), 1, in);
    Write_Mod_Data(&TicksPerBeat, sizeof(int), 1, in);
    if(mas_vol < 0.01f) mas_vol = 0.01f;
    if(mas_vol > 1.0f) mas_vol = 1.0f;
    Write_Mod_Data(&mas_vol, sizeof(float), 1, in);

    float threshold = mas_comp_threshold;
    float ratio = mas_comp_ratio;
    char Comp_Flag;

    if(threshold < 0.01f) threshold = 0.01f;
    if(threshold > 100.0f) threshold = 100.0f;
    if(ratio < 0.01f) ratio = 0.01f;
    if(ratio > 100.0f) ratio = 100.0f;

    if(ratio > 0.01f)
    {
        threshold *= 0.001f;
        ratio *= 0.01f;
        Comp_Flag = 1;
        Write_Mod_Data(&Comp_Flag, sizeof(char), 1, in);

        Write_Mod_Data(&threshold, sizeof(float), 1, in);
        Write_Mod_Data(&ratio, sizeof(float), 1, in);
        Save_Constant("PTK_LIMITER", TRUE);
    }
    else
    {
        Comp_Flag = 0;
        Write_Mod_Data(&Comp_Flag, sizeof(char), 1, in);
        Save_Constant("PTK_LIMITER", FALSE);
    }

    Write_Mod_Data(&Feedback, sizeof(float), 1, in);
    
    if(compressor)
    {
        Save_Reverb_Data(Write_Mod_Data, Write_Mod_Data, in);
    }

    Write_Mod_Data(&lchorus_delay, sizeof(int), 1, in);
    Write_Mod_Data(&rchorus_delay, sizeof(int), 1, in);
    Write_Mod_Data(&lchorus_feedback, sizeof(float), 1, in);
    Write_Mod_Data(&rchorus_feedback, sizeof(float), 1, in);
    Write_Mod_Data(&shuffle, sizeof(int), 1, in);

    Save_Constant("PTK_TRACKFILTERS", Store_TrackFilters);

    Save_Constant("PTK_FILTER_LOHIBAND", Store_Filter_LoHiBand);

    Save_Constant("PTK_FILTER_LO24", Store_Filter_Lo24);
    Save_Constant("PTK_FILTER_LO48", Store_Filter_Lo48);
    Save_Constant("PTK_FILTER_LP24", Store_Filter_Lp24);
    Save_Constant("PTK_FILTER_AMODM", Store_Filter_AModM);
    Save_Constant("PTK_FILTER_AMODS", Store_Filter_AModS);
    Save_Constant("PTK_FILTER_SINGLEM", Store_Filter_SingleM);
    Save_Constant("PTK_FILTER_SINGLES", Store_Filter_SingleS);
    Save_Constant("PTK_FILTER_EQM15", Store_Filter_Eqm15);
    Save_Constant("PTK_FILTER_EQM6", Store_Filter_Eqm6);
    Save_Constant("PTK_FILTER_EQP6", Store_Filter_Eqp6);
    Save_Constant("PTK_FILTER_EQP15", Store_Filter_Eqp15);
    Save_Constant("PTK_FILTER_DELTA", Store_Filter_Delta);
    Save_Constant("PTK_FILTER_DISTL", Store_Filter_DistL);
    Save_Constant("PTK_FILTER_DISTM", Store_Filter_DistM);
    Save_Constant("PTK_FILTER_DISTH", Store_Filter_DistH);
    Save_Constant("PTK_FILTER_DIST", Store_Filter_Dist);
    Save_Constant("PTK_FILTER_HP12M", Store_Filter_Hp12M);
    Save_Constant("PTK_FILTER_HP12S", Store_Filter_Hp12S);
    Save_Constant("PTK_FILTER_HP24M", Store_Filter_Hp24M);

    Save_Constant("PTK_PROC_FILTER", Store_Filter_LoHiBand);
    Save_Constant("PTK_PROC_FILTER2P", Store_Filter_Lo24 | Store_Filter_Lo48 | Store_Filter_Lp24);
    Save_Constant("PTK_PROC_FILTER2P24D", Store_Filter_Lo48 | Store_Filter_Lp24);
    Save_Constant("PTK_PROC_FILTERRINGMOD", Store_Filter_AModM | Store_Filter_AModS);
    Save_Constant("PTK_PROC_FILTERRINGMODSTEREO", Store_Filter_AModS);
    Save_Constant("PTK_PROC_FILTERWATER", Store_Filter_SingleM | Store_Filter_SingleS);
    Save_Constant("PTK_PROC_FILTERWATERSTEREO", Store_Filter_SingleS);
    Save_Constant("PTK_PROC_FILTERBELLSHAPED", Store_Filter_Eqm15 | Store_Filter_Eqm6 | Store_Filter_Eqp6 | Store_Filter_Eqp15);
    Save_Constant("PTK_PROC_FILTERDELTA", Store_Filter_Delta);
    Save_Constant("PTK_PROC_FILTERINT2P", Store_Filter_DistL | Store_Filter_DistM | Store_Filter_DistH | Store_Filter_Dist);
    Save_Constant("PTK_PROC_FILTERHP", Store_Filter_Hp12M | Store_Filter_Hp12S | Store_Filter_Hp24M);
    Save_Constant("PTK_PROC_FILTERHP2", Store_Filter_Hp12S | Store_Filter_Hp24M);

    Save_Constant("PTK_SHUFFLE", shuffle != 0 ? TRUE : FALSE);

    Save_Constant("PTK_COMPRESSOR", compressor);

    for(int tps_pos = 0; tps_pos < sLength; tps_pos++)
    {
        for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
        {
            if(!Track_Is_Muted(tps_trk))
            {
                Write_Mod_Data(&CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char), 1, in);
            }
        }
    }

    for(twrite = 0; twrite < Songtracks; twrite++)
    {
        if(!Track_Is_Muted(twrite))
        {
            Write_Mod_Data(&LFO_ON[twrite], sizeof(char), 1, in);
            if(LFO_ON[twrite])
            {
                Store_LFO = TRUE;
                Write_Mod_Data(&LFO_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Data(&LFO_AMPL[twrite], sizeof(float), 1, in);
            }
        }
    }
    Save_Constant("PTK_LFO", Store_LFO);

    for(twrite = 0; twrite < Songtracks; twrite++)
    {
        if(!Track_Is_Muted(twrite))
        {
            Write_Mod_Data(&FLANGER_ON[twrite], sizeof(char), 1, in);
            if(FLANGER_ON[twrite])
            {
                Store_Flanger = TRUE;
                Write_Mod_Data(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                Write_Mod_Data(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                Write_Mod_Data(&FLANGER_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Data(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
                Write_Mod_Data(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                Write_Mod_Data(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
            }
        }
    }
    Save_Constant("PTK_FLANGER", Store_Flanger);

    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
    {
        if(!Track_Is_Muted(tps_trk))
        {
            if(Disclap[tps_trk])
            {
                Store_Disclap = TRUE;
            }
        }
    }
    Save_Constant("PTK_DISCLAP", Store_Disclap);

    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
    {
        if(!Track_Is_Muted(tps_trk))
        {
            Write_Mod_Data(&Disclap[tps_trk], sizeof(char), 1, in);
        }
    }

    char Instrs;
    
    Instrs = Store_Instruments;
    
    Write_Mod_Data(&Instrs, sizeof(char), 1, in);

    if(Instrs)
    {
        Write_Mod_Data(beatsync, sizeof(char), MAX_INSTRS, in);
        Write_Mod_Data(beatlines, sizeof(short), MAX_INSTRS, in);
    }
    Write_Mod_Data(&REVERBFILTER, sizeof(float), 1, in);

    if(Instrs)
    {
        Write_Mod_Data(Sample_Vol, sizeof(float), MAX_INSTRS, in);
    }

    Write_Mod_Data(&Store_303_1, sizeof(char), 1, in);
    if(Store_303_1)
    {
        Write_Mod_Data(&tb303[0].selectedpattern, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[0].tune, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[0].cutoff, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[0].resonance, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[0].envmod, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[0].decay, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[0].accent, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[0].waveform, sizeof(char), 1, in);
        Write_Mod_Data(tb303[0].patternlength, sizeof(char), 32, in);
        Write_Mod_Data(tb303[0].tone, sizeof(char), 32 * 16, in);
        Write_Mod_Data(tb303[0].flag, sizeof(struct flag303), 32 * 16, in);
    }

    Write_Mod_Data(&Store_303_2, sizeof(char), 1, in);
    if(Store_303_2)
    {
        Write_Mod_Data(&tb303[1].selectedpattern, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[1].tune, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[1].cutoff, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[1].resonance, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[1].envmod, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[1].decay, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[1].accent, sizeof(char), 1, in);
        Write_Mod_Data(&tb303[1].waveform, sizeof(char), 1, in);
        Write_Mod_Data(tb303[1].patternlength, sizeof(char), 32, in);
        Write_Mod_Data(tb303[1].tone, sizeof(char), 32 * 16, in);
        Write_Mod_Data(tb303[1].flag, sizeof(struct flag303), 32 * 16, in);
    }
    
    if(Store_303_1) Write_Mod_Data(&tb303engine[0].tbVolume, sizeof(float), 1, in);
    if(Store_303_2) Write_Mod_Data(&tb303engine[1].tbVolume, sizeof(float), 1, in);
    if(Store_303_1) Write_Mod_Data(&tb303engine[0].hpf, sizeof(char), 1, in);
    if(Store_303_2) Write_Mod_Data(&tb303engine[1].hpf, sizeof(char), 1, in);

    free(New_RawPatterns);

    if(Out_FX) fclose(Out_FX);
    if(Out_constants) fclose(Out_constants);

    return(TRUE);
}

void rtrim_string(char *string, int maxlen)
{
    int len = strlen(string);

    while(len < maxlen)
    {
        string[len] = 0;
        len++;
    }
}

// ------------------------------------------------------
// Entry point function for modules saving
int SaveMod(char *FileName, int NewFormat, int Simulate, Uint8 *Memory)
{
    FILE *in;
    int i;
    int j;
    int k;
    int l;
    char Temph[MAX_PATH];
    int Ok_Memory = TRUE;
    char Comp_Flag = TRUE;
    unsigned char *cur_pattern_col;
    unsigned char *cur_pattern;

    int twrite;
    int tps_trk;
    
    Mod_Length = 0;
    Mod_Mem_Pos = 0;
    Mod_Simulate = SAVE_WRITE;
    Mod_Memory = Memory;

    // Store the different possible modes
    if(Simulate)
    {
        Mod_Simulate = Simulate;
    }

    if(!Simulate)
    {
        if(NewFormat)
        {
            sprintf(Temph, "Saving '%s.ptp' song in modules directory...", FileName);
            Status_Box(Temph);
            sprintf(Temph, "%s"SLASH"%s.ptp", Dir_Mods, FileName);
        }
        else
        {
            sprintf(Temph, "Saving '%s.ptk' song in modules directory...", FileName);
            Status_Box(Temph);
            sprintf(Temph, "%s"SLASH"%s.ptk", Dir_Mods, FileName);
        }
        in = fopen(Temph, "wb");
    }
    else
    {
        in = (FILE *) -1;
    }

    if(in != NULL)
    {
        if(NewFormat)
        {
            // .ptp
            Ok_Memory = SaveMod_Ptp(in, Simulate, FileName);
        }
        else
        {
            // .ptk
            if(strlen(FileName)) rtrim_string(FileName, 20);
            Write_Mod_Data(FileName, sizeof(char), 20, in);

            // Calc the real number of patterns
            nPatterns = 0;
            for(i = 0 ; i < sLength; i++)
            {
                if((pSequence[i] + 1) > nPatterns)
                {
                    nPatterns = pSequence[i] + 1;
                }
            }

            Write_Mod_Data(&nPatterns, sizeof(char), 1, in);
            Write_Mod_Data(&sLength, sizeof(char), 1, in);
            Write_Mod_Data(&Use_Cubic, sizeof(char), 1, in);

            Write_Mod_Data(pSequence, sizeof(char), MAX_SEQUENCES, in);

            Swap_Short_Buffer(patternLines, MAX_ROWS);
            Write_Mod_Data(patternLines, sizeof(short), MAX_ROWS, in);
            Swap_Short_Buffer(patternLines, MAX_ROWS);

            Write_Mod_Data(Channels_MultiNotes, sizeof(char), MAX_TRACKS, in);

            // Clean the unused patterns garbage (doesn't seem to do much)
            for(i = Songtracks; i < MAX_TRACKS; i++)
            {
                // Next column
                cur_pattern_col = RawPatterns + (i * PATTERN_BYTES);
                for(j = 0; j < nPatterns; j++)
                {
                    // Next pattern
                    cur_pattern = cur_pattern_col + (j * PATTERN_LEN);
                    for(k = 0; k < patternLines[j]; k++)
                    {
                        for(l = 0; l < MAX_POLYPHONY; l += 2)
                        {
                            cur_pattern[PATTERN_NOTE1 + i] = 121;
                            cur_pattern[PATTERN_INSTR1 + i] = 255;
                        }

                        cur_pattern[PATTERN_VOLUME] = 255;
                        cur_pattern[PATTERN_PANNING] = 255;
                        cur_pattern[PATTERN_FX] = 0;
                        cur_pattern[PATTERN_FXDATA] = 0;
                        cur_pattern[PATTERN_FX2] = 0;
                        cur_pattern[PATTERN_FXDATA2] = 0;
                        // Next line
                        cur_pattern += PATTERN_ROW_LEN;
                    }
                }
            }

            for(int pwrite = 0; pwrite < nPatterns; pwrite++)
            {
                Write_Mod_Data(RawPatterns + (pwrite * PATTERN_LEN),
                               sizeof(char), PATTERN_LEN, in);
            }

            // Writing sample data
            for(int swrite = 0; swrite < MAX_INSTRS; swrite++)
            {
                rtrim_string(nameins[swrite], 20);
                Write_Mod_Data(nameins[swrite], sizeof(char), 20, in);
                Write_Mod_Data(&Midiprg[swrite], sizeof(char), 1, in);
                Write_Mod_Data(&Synthprg[swrite], sizeof(char), 1, in);

                Write_Synth_Params(Write_Mod_Data, Write_Mod_Data_Swap, in, swrite);

                // Compression type
                Write_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);
                switch(SampleCompression[swrite])
                {
                    case SMP_PACK_MP3:
                        Write_Mod_Data(&Mp3_BitRate[swrite], sizeof(char), 1, in);
                        break;

                    case SMP_PACK_AT3:
                        Write_Mod_Data(&At3_BitRate[swrite], sizeof(char), 1, in);
                        break;
                }

                // 16 splits / instrument
                for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
                {
                    Write_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleType[swrite][slwrite])
                    {
                        rtrim_string(SampleName[swrite][slwrite], 64);
                        Write_Mod_Data(SampleName[swrite][slwrite], sizeof(char), 64, in);
                        Write_Mod_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                        
                        Write_Mod_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                        
                        Write_Mod_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                        Write_Mod_Data_Swap(&Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                        Write_Mod_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                        // All samples are converted into 16 bits
                        Write_Unpacked_Sample(Write_Mod_Data, in, swrite, slwrite);
                    }// Exist Sample
                }
            }

            // Writing Track Properties
            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data_Swap(&TCut[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&ICut[twrite], sizeof(float), 1, in);

                Write_Mod_Data_Swap(&TPan[twrite], sizeof(float), 1, in);

                Write_Mod_Data_Swap(&FType[twrite], sizeof(int), 1, in);
                Write_Mod_Data_Swap(&FRez[twrite], sizeof(int), 1, in);
                Write_Mod_Data_Swap(&DThreshold[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&DClamp[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&DSend[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&CSend[twrite], sizeof(int), 1, in);
                Write_Mod_Data(&Channels_Polyphony[twrite], sizeof(char), 1, in);
            }

            // Writing mod properties
            Write_Mod_Data_Swap(&compressor, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&c_threshold, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&BeatsPerMin, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&TicksPerBeat, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&mas_vol, sizeof(float), 1, in);
            
            Write_Mod_Data(&Comp_Flag, sizeof(char), 1, in);
            Write_Mod_Data_Swap(&mas_comp_threshold, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&mas_comp_ratio, sizeof(float), 1, in);

            Write_Mod_Data_Swap(&Feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&lchorus_delay, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&rchorus_delay, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&lchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&rchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&shuffle, sizeof(int), 1, in);

            // Save the reverb data
            Save_Reverb_Data(Write_Mod_Data, Write_Mod_Data_Swap, in);

            // Writing part sequence data
            for(int tps_pos = 0; tps_pos < MAX_SEQUENCES; tps_pos++)
            {
                for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
                {
                    Write_Mod_Data(&CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char), 1, in);
                }
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data_Swap(&CHAN_MIDI_PRG[twrite], sizeof(int), 1, in);
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data(&LFO_ON[twrite], sizeof(char), 1, in);
                Write_Mod_Data_Swap(&LFO_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&LFO_AMPL[twrite], sizeof(float), 1, in);
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data(&FLANGER_ON[twrite], sizeof(char), 1, in);
                Write_Mod_Data_Swap(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&FLANGER_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
            }

            // Was a bug
            //Write_Mod_Data_Swap(&FLANGER_DEPHASE, sizeof(float), 1, in);

            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Write_Mod_Data_Swap(&CHAN_MUTE_STATE[tps_trk], sizeof(int), 1, in);
            }
            Write_Mod_Data(&Songtracks, sizeof(char), 1, in);

            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Write_Mod_Data(&Disclap[tps_trk], sizeof(char), 1, in);
            }

            rtrim_string(artist, 20);
            Write_Mod_Data(artist, sizeof(char), 20, in);
            rtrim_string(style, 20);
            Write_Mod_Data(style, sizeof(char), 20, in);

            Write_Mod_Data(beatsync, sizeof(char), 128, in);

            for(i = 0; i < 128; i++)
            {
                Write_Mod_Data_Swap(&beatlines[i], sizeof(short), 1, in);
            }
            Write_Mod_Data_Swap(&REVERBFILTER, sizeof(float), 1, in);

            for(i = 0; i < 128; i++)
            {
                Write_Mod_Data_Swap(&Sample_Vol[i], sizeof(float), 1, in);
            }

            // Include the patterns names
            for(i = 0; i < 32; i++)
            {
                rtrim_string(tb303[0].pattern_name[i], 20);
                rtrim_string(tb303[1].pattern_name[i], 20);
            }
            
            for(j = 0; j < 2; j++)
            {
                Write_Mod_Data(&tb303[j].enabled, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].selectedpattern, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].tune, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].cutoff, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].resonance, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].envmod, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].decay, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].accent, sizeof(char), 1, in);
                Write_Mod_Data(&tb303[j].waveform, sizeof(char), 1, in);
                for(i = 0; i < 32; i++)
                {
                    Save_303_Data(Write_Mod_Data, Write_Mod_Data_Swap, in, j, i); 
                }
            }

            Write_Mod_Data_Swap(&tb303engine[0].tbVolume, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&tb303engine[1].tbVolume, sizeof(float), 1, in);
            Write_Mod_Data(&tb303engine[0].hpf, sizeof(char), 1, in);
            Write_Mod_Data(&tb303engine[1].hpf, sizeof(char), 1, in);
        }

        if(!Simulate)
        {
            fclose(in);
            last_index = -1;
            Read_SMPT();
            Actualize_Files_List(0);

            if(Ok_Memory)
            {
                char name[128];
                if(NewFormat)
                {
                    sprintf(name, "Module '%s.ptp' saved succesfully.", FileName);
                }
                else
                {
                    sprintf(name, "Module '%s.ptk' saved succesfully.", FileName);
                }
                Status_Box(name);
            }
        }
        if(!Ok_Memory) Status_Box("Not enough memory.");
    }
    else
    {
        if(!Simulate) Status_Box("Module save failed.");   
    }

    return(Mod_Length);
}

// ------------------------------------------------------
// Load a synth preset
void LoadSynth(char *FileName)
{
    FILE *in;
    int new_version = FALSE;
    int Env_Modulation = FALSE;
    int New_Env = FALSE;

    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        switch(extension[7])
        {
            case '3':
                New_Env = TRUE;
            case '2':
                Env_Modulation = TRUE;
            case '1':
                new_version = TRUE;
            case '0':
                break;
        }

        Status_Box("Loading Synthesizer -> structure...");
        ResetSynthParameters(&PARASynth[Current_Sample]);

        PARASynth[Current_Sample].disto = 0;

        PARASynth[Current_Sample].lfo1_attack = 0;
        PARASynth[Current_Sample].lfo1_decay = 0;
        PARASynth[Current_Sample].lfo1_sustain = 128;
        PARASynth[Current_Sample].lfo1_release = 0x10000;

        PARASynth[Current_Sample].lfo2_attack = 0;
        PARASynth[Current_Sample].lfo2_decay = 0;
        PARASynth[Current_Sample].lfo2_sustain = 128;
        PARASynth[Current_Sample].lfo2_release = 0x10000;

        Read_Synth_Params(Read_Data, Read_Data_Swap, in, Current_Sample,
                          TRUE, TRUE, new_version, Env_Modulation, New_Env, FALSE);

        // Fix some old Ntk bugs
        if(PARASynth[Current_Sample].lfo1_period > 128) PARASynth[Current_Sample].lfo1_period = 128;
        if(PARASynth[Current_Sample].lfo2_period > 128) PARASynth[Current_Sample].lfo2_period = 128;

        Synthprg[Current_Sample] = SYNTH_WAVE_CURRENT;
        sprintf(nameins[Current_Sample],PARASynth[Current_Sample].presetname);
        Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);

        Actualize_Instrument_Ed(0, 0);
        Actualize_DiskIO_Ed(0);
        Actualize_Patterned();
        Status_Box("Synthesizer program loaded ok.");
        fclose(in);
    }
    else
    {
        Status_Box("Synthesizer program loading failed. (Probably: file not found)");
    }
}

// ------------------------------------------------------
// Save the current synth preset
void SaveSynth(void)
{
    FILE *in;
    char Temph[MAX_PATH];
    char extension[10];

    sprintf(extension, "TWNNSYN3");
    sprintf (Temph, "Saving '%s.pts' synthesizer program in presets directory...", PARASynth[Current_Sample].presetname);
    Status_Box(Temph);

    sprintf(Temph, "%s"SLASH"%s.pts", Dir_Presets, PARASynth[Current_Sample].presetname);

    in = fopen(Temph, "wb");
    if(in != NULL)
    {
        Write_Data(extension, sizeof(char), 9, in);
        Write_Synth_Params(Write_Data, Write_Data_Swap, in, Current_Sample);
        fclose(in);

        Read_SMPT();
        last_index = -1;
        Actualize_Files_List(0);
        Status_Box("Synthesizer program saved succesfully."); 
    }
    else
    {
        Status_Box("Synthesizer program save failed.");
    }

    Clear_Input();
}

// ------------------------------------------------------
// Load an instrument
void LoadInst(char *FileName)
{
    int old_bug = FALSE;
    int Pack_Scheme = FALSE;
    int Mp3_Scheme = FALSE;
    int new_adsr = FALSE;
    int tight = FALSE;
    int Env_Modulation = FALSE;
    int New_Env = FALSE;
    int Glob_Vol = FALSE;

    Status_Box("Attempting to load an instrument file...");
    FILE *in;
    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        Read_Data(extension, sizeof(char), 9, in);

        switch(extension[7])
        {
            case '8':
                Glob_Vol = TRUE;
            case '7':
                New_Env = TRUE;
            case '6':
                Env_Modulation = TRUE;
            case '5':
                Mp3_Scheme = TRUE;
            case '4':
                tight = TRUE;
            case '3':
                new_adsr = TRUE;
            case '2':
                Pack_Scheme = TRUE;
            case '1':
                break;
            case '0':
                old_bug = TRUE;
                break;
        }

        KillInst(Current_Sample);
        Status_Box("Loading Instrument -> Header..."); 
        Read_Data(&nameins[Current_Sample], sizeof(char), 20, in);

        // Reading sample data
        Status_Box("Loading Instrument -> Sample data...");

        int swrite = Current_Sample;

        Read_Data(&Midiprg[swrite], sizeof(char), 1, in);
        Read_Data(&Synthprg[swrite], sizeof(char), 1, in);

        PARASynth[swrite].disto = 0;

        PARASynth[swrite].lfo1_attack = 0;
        PARASynth[swrite].lfo1_decay = 0;
        PARASynth[swrite].lfo1_sustain = 128;
        PARASynth[swrite].lfo1_release = 0x10000;

        PARASynth[swrite].lfo2_attack = 0;
        PARASynth[swrite].lfo2_decay = 0;
        PARASynth[swrite].lfo2_sustain = 128;
        PARASynth[swrite].lfo2_release = 0x10000;

        Read_Synth_Params(Read_Data, Read_Data_Swap, in, swrite,
                          !old_bug, new_adsr, tight, Env_Modulation, New_Env, FALSE);

        // Gsm by default
        if(Pack_Scheme)
        {
            Read_Data(&SampleCompression[swrite], sizeof(char), 1, in);
        }
        else
        {
            SampleCompression[swrite] = SMP_PACK_GSM;
        }
        // Load the bitrate
        if(Mp3_Scheme)
        {
            switch(SampleCompression[swrite])
            {
                case SMP_PACK_MP3:
                    Read_Data(&Mp3_BitRate[swrite], sizeof(char), 1, in);
                    break;

                case SMP_PACK_AT3:
                    Read_Data(&At3_BitRate[swrite], sizeof(char), 1, in);
                    break;
            }
        }

        Sample_Vol[swrite] = 1.0f;
        if(Glob_Vol)
        {
            Read_Data_Swap(&Sample_Vol[swrite], sizeof(float), 1, in);
        }

        for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
        {
            Read_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
            if(SampleType[swrite][slwrite])
            {
                if(old_bug) Read_Data(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                else Read_Data(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                Read_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                
                Read_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                Read_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                Read_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                
                Read_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                Read_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                Read_Data_Swap(&Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                Read_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                AllocateWave(swrite, slwrite, SampleNumSamples[swrite][slwrite], 1, FALSE);

                // Samples data
                Read_Data(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                Swap_Sample(RawSamples[swrite][0][slwrite], swrite, slwrite);
                *RawSamples[swrite][0][slwrite] = 0;

                // Number of channel(s)
                Read_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                if(SampleChannels[swrite][slwrite] == 2)
                {
                    RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short) + 8);
                    memset(RawSamples[swrite][1][slwrite], 0, SampleNumSamples[swrite][slwrite] * sizeof(short) + 8);
                    Read_Data(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                    Swap_Sample(RawSamples[swrite][1][slwrite], swrite, slwrite);
                    *RawSamples[swrite][1][slwrite] = 0;
                }
            } // Exist Sample
        }
        fclose(in);
        Actualize_Patterned();
        Actualize_Instrument_Ed(2, 0);
        Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        Status_Box("Instrument loaded ok.");
    } else {
        Status_Box("Instrument loading failed. (Probably: file not found)");
    }
    
    Clear_Input();
}

// ------------------------------------------------------
// Save the current instrument
void SaveInst(void)
{
    FILE *in;
    char Temph[MAX_PATH];
    char extension[10];
    char synth_prg;
    int synth_save;

    sprintf(extension, "TWNNINS8");
    sprintf (Temph, "Saving '%s.pti' instrument in instruments directory...", nameins[Current_Sample]);
    Status_Box(Temph);
    sprintf(Temph, "%s"SLASH"%s.pti", Dir_Instrs, nameins[Current_Sample]);

    in = fopen(Temph, "wb");
    if(in != NULL)
    {
        // Writing header & name...
        Write_Data(extension, sizeof(char), 9, in);
        rtrim_string(nameins[Current_Sample], 20);
        Write_Data(nameins[Current_Sample], sizeof(char), 20, in);

        // Writing sample data
        int swrite = Current_Sample;

        Write_Data(&Midiprg[swrite], sizeof(char), 1, in);
        switch(Synthprg[swrite])
        {
            case SYNTH_WAVE_OFF:
            case SYNTH_WAVE_CURRENT:
                synth_prg = Synthprg[swrite];
                synth_save = swrite;
                break;
            default:
                synth_prg = SYNTH_WAVE_CURRENT;
                synth_save = Synthprg[swrite] - 2;
                break;
        }

        Write_Data(&synth_prg, sizeof(char), 1, in);

        Write_Synth_Params(Write_Data, Write_Data_Swap, in, swrite);

        Write_Data(&SampleCompression[swrite], sizeof(char), 1, in);
        switch(SampleCompression[swrite])
        {
            case SMP_PACK_MP3:
                Write_Data(&Mp3_BitRate[swrite], sizeof(char), 1, in);
                break;

            case SMP_PACK_AT3:
                Write_Data(&At3_BitRate[swrite], sizeof(char), 1, in);
                break;
        }

        Write_Data_Swap(&Sample_Vol[swrite], sizeof(float), 1, in);

        swrite = synth_save;
        for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
        {
            Write_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
            if(SampleType[swrite][slwrite])
            {
                rtrim_string(SampleName[swrite][slwrite], 64);
                Write_Data(SampleName[swrite][slwrite], sizeof(char), 64, in);
                Write_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                
                Write_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                Write_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                Write_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                
                Write_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                Write_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                Write_Data_Swap(&Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                Write_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                Write_Unpacked_Sample(Write_Data, in, swrite, slwrite);

            } // Exist Sample
        }
        fclose(in);

        Read_SMPT();
        last_index = -1;
        Actualize_Files_List(0);
        Actualize_Patterned();
        Status_Box("Instrument saved succesfully."); 
    }
    else
    {
        Status_Box("Instrument save failed.");
    }

    Clear_Input();
}

// ------------------------------------------------------
// Load a 303 pattern
void Load303(char *FileName)
{
    FILE *in;
    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        if(strcmp(extension, "TWNN3031") == 0)
        {
            // Ok, extension matched!
            Status_Box("Loading 303 pattern...");

            Load_303_Data(Read_Data, Read_Data_Swap, in, sl3, tb303[sl3].selectedpattern);

            Actualize_303_Ed(0);
            Status_Box("303 pattern loaded ok.");
        }
        else
        {
            Status_Box("That file is not a "TITLE" 303 pattern file...");
        }
        fclose(in);
    }
    else
    {
        Status_Box("303 pattern loading failed. (Probably: file not found)");
    }
}

// ------------------------------------------------------
// Save a 303 pattern
void Save303(void)
{
    FILE *in;
    char Temph[MAX_PATH];
    char extension[10];

    sprintf(extension, "TWNN3031");
    sprintf(Temph, "Saving '%s.303' pattern in patterns directory...",
            tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
    Status_Box(Temph);
    sprintf(Temph, "%s"SLASH"%s.303", Dir_Patterns,
            tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
    in = fopen(Temph, "wb");

    if(in != NULL)
    {
        Write_Data(extension, sizeof(char), 9, in);

        Save_303_Data(Write_Data, Write_Data_Swap, in, sl3, tb303[sl3].selectedpattern);

        fclose(in);
        Read_SMPT();
        last_index = -1;
        Actualize_Files_List(0);
        Status_Box("303 pattern saved succesfully.");   
    }
    else
    {
        Status_Box("303 pattern save failed.");
    }

    Clear_Input();
}

// ------------------------------------------------------
// Pack a module
Uint8 *Pack_Data(Uint8 *Memory, int *Size)
{
    z_stream c_stream; // compression stream
    int Comp_Len = -1;
    Uint8 *Final_Mem_Out = (Uint8 *) malloc(*Size);

    memset(&c_stream, 0, sizeof(c_stream));
    deflateInit(&c_stream, Z_BEST_COMPRESSION);
    c_stream.next_in = (Bytef *) Memory;
    c_stream.next_out = Final_Mem_Out;
    while (c_stream.total_in != *Size)
    {
        c_stream.avail_in = 1;
        c_stream.avail_out = 1;
        deflate(&c_stream, Z_NO_FLUSH);
    }
    for (;;)
    {
        c_stream.avail_out = 1;
        if (deflate(&c_stream, Z_FINISH) == Z_STREAM_END) break;
    }
    deflateEnd(&c_stream);
    *Size = c_stream.total_out;
    return(Final_Mem_Out);
}
#endif // __WINAMP__

// ------------------------------------------------------
// Depack a compressed module
Uint8 *Depack_Data(Uint8 *Memory, int Sizen, int Size_Out)
{
    z_stream d_stream;

    Uint8 *Test_Mem = (Uint8 *) malloc(Size_Out);
    if(Test_Mem)
    {
        memset(&d_stream, 0, sizeof(d_stream));
        d_stream.next_in = (Uint8 *) Memory;
        d_stream.next_out = Test_Mem;
        inflateInit(&d_stream);
        while (d_stream.total_out < Size_Out &&
               d_stream.total_in < Sizen)
        {
            d_stream.avail_in = 1;
            d_stream.avail_out = 1;
            if (inflate(&d_stream, Z_NO_FLUSH) == Z_STREAM_END) break;
        }
        inflateEnd(&d_stream);
    }
    return(Test_Mem);
}

// ------------------------------------------------------
// Save a packed .ptk module
#if !defined(__WINAMP__)
int Pack_Module(char *FileName)
{
    FILE *output;
    char name[128];
    char extension[10];
    char Temph[MAX_PATH];
    Uint8 *Final_Mem_Out;
    int Depack_Size;

    if(!strlen(FileName))
    {
        sprintf(name, "Can't save module without a name...");
        Status_Box(name);
        return(FALSE);
    }

    sprintf(Temph, "%s"SLASH"%s.ptk", Dir_Mods, FileName);

    int Len = SaveMod("", FALSE, SAVE_CALCLEN, NULL);

    Depack_Size = Len;

    Uint8 *Final_Mem = (Uint8 *) malloc(Len);
    SaveMod(FileName, FALSE, SAVE_WRITEMEM, Final_Mem);

    Final_Mem_Out = Pack_Data(Final_Mem, &Len);

    output = fopen(Temph, "wb");
    if(output)
    {
        sprintf(extension, "TWNNSNGJ");
        Write_Data(extension, sizeof(char), 9, output);
        Write_Data_Swap(&Depack_Size, sizeof(int), 1, output);
        Write_Data(Final_Mem_Out, sizeof(char), Len, output);
        fclose(output);
        sprintf(name, "Module '%s.ptk' saved succesfully.", FileName);
    }
    else
    {
        sprintf(name, "Module save failed.");
    }
    if(Final_Mem_Out) free(Final_Mem_Out);
    if(Final_Mem) free(Final_Mem);

    Clear_Input();
    Status_Box(name);
    Read_SMPT();
    last_index = -1;
    Actualize_Files_List(0);
    return(FALSE);
}

// ------------------------------------------------------
// Return the length of a compressed module
int TestMod(void)
{
    Uint8 *Final_Mem_Out;
    int Len = SaveMod("", TRUE, SAVE_CALCLEN, NULL);

    Uint8 *Final_Mem = (Uint8 *) malloc(Len);
    SaveMod("", TRUE, SAVE_WRITEMEM, Final_Mem);

    Final_Mem_Out = Pack_Data(Final_Mem, &Len);

    if(Final_Mem_Out) free(Final_Mem_Out);
    if(Final_Mem) free(Final_Mem);
    return(Len);
}

// ------------------------------------------------------
// Save the configuration
void SaveConfig(void)
{
    FILE *out;
    char extension[10];
    char FileName[MAX_PATH];
    int i;
    int Real_Palette_Idx;
    char KeyboardName[MAX_PATH];
    signed char phony = -1;

    sprintf(extension, "TWNNCFGB");
    Status_Box("Saving 'ptk.cfg'...");

    sprintf(FileName, "%s"SLASH"ptk.cfg", ExePath);

    memset(KeyboardName, 0, sizeof(KeyboardName));
    sprintf(KeyboardName, "%s", Keyboard_Name);

    out = fopen(FileName, "wb");
    if(out != NULL)
    {
        Write_Data(extension, sizeof(char), 9, out);
        Write_Data_Swap(&Current_Edit_Steps, sizeof(Current_Edit_Steps), 1, out);
        Write_Data_Swap(&patt_highlight, sizeof(patt_highlight), 1, out);
        Write_Data_Swap(&AUDIO_Milliseconds, sizeof(AUDIO_Milliseconds), 1, out);

#if defined(__NO_MIDI__)
        Write_Data(&phony, sizeof(phony), 1, out);
#else
        Write_Data(&c_midiin, sizeof(c_midiin), 1, out);
#endif

#if defined(__NO_MIDI__)
        Write_Data(&phony, sizeof(phony), 1, out);
#else
        Write_Data(&c_midiout, sizeof(c_midiout), 1, out);
#endif

        Write_Data_Swap(&MouseWheel_Multiplier, sizeof(MouseWheel_Multiplier), 1, out);
        Write_Data(&Rows_Decimal, sizeof(Rows_Decimal), 1, out);
        Write_Data(&FullScreen, sizeof(FullScreen), 1, out);

        for(i = 0; i < NUMBER_COLORS; i++)
        {
            Real_Palette_Idx = Idx_Palette[i];
            Write_Data(&Ptk_Palette[Real_Palette_Idx].r, sizeof(char), 1, out);
            Write_Data(&Ptk_Palette[Real_Palette_Idx].g, sizeof(char), 1, out);
            Write_Data(&Ptk_Palette[Real_Palette_Idx].b, sizeof(char), 1, out);
        }
        Write_Data(&See_Prev_Next_Pattern, sizeof(See_Prev_Next_Pattern), 1, out);
        Write_Data_Swap(&Beveled, sizeof(Beveled), 1, out);
        Write_Data_Swap(&Continuous_Scroll, sizeof(Continuous_Scroll), 1, out);
        Write_Data(&AutoSave, sizeof(AutoSave), 1, out);
        
        Write_Data(&Dir_Mods, sizeof(Dir_Mods), 1, out);
        Write_Data(&Dir_Instrs, sizeof(Dir_Instrs), 1, out);
        Write_Data(&Dir_Presets, sizeof(Dir_Presets), 1, out);
        Write_Data(&Dir_Reverbs, sizeof(Dir_Reverbs), 1, out);
        Write_Data(&Dir_Patterns, sizeof(Dir_Patterns), 1, out);
        Write_Data(&Dir_Samples, sizeof(Dir_Samples), 1, out);
        Write_Data(KeyboardName, MAX_PATH, 1, out);

        Write_Data(&rawrender_32float, sizeof(char), 1, out);
        Write_Data(&Patterns_Lines, sizeof(char), 1, out);
        Write_Data(&Scopish_LeftRight, sizeof(char), 1, out);
        
        Write_Data(&Paste_Across, sizeof(char), 1, out);
        Write_Data(&Jazz_Edit, sizeof(char), 1, out);
        Write_Data(&Accidental, sizeof(char), 1, out);

        Write_Data(&Use_Shadows, sizeof(char), 1, out);
        Write_Data(&Global_Patterns_Font, sizeof(char), 1, out);

        fclose(out);

        Read_SMPT();
        last_index = -1;
        Actualize_Files_List(0);
        Status_Box("Configuration file saved succesfully.");  
    }
    else
    {
        Status_Box("Configuration file save failed.");
    }
}

// ---------------------------------------------------------------------------
// Save the configuration
// ---------------------------------------------------------------------------
void LoadConfig(void)
{
    FILE *in;
    int i;
    int Real_Palette_Idx;
    char FileName[MAX_PATH];
    char KeyboardName[MAX_PATH];
    signed char phony = -1;

    sprintf(FileName, "%s"SLASH"ptk.cfg", ExePath);

    memset(KeyboardName, 0, sizeof(KeyboardName));

    in = fopen(FileName, "rb");
    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];

        Read_Data(extension, sizeof(char), 9, in);
        if(strcmp(extension, "TWNNCFGB") == 0)
        {
            Read_Data_Swap(&Current_Edit_Steps, sizeof(Current_Edit_Steps), 1, in);
            Read_Data_Swap(&patt_highlight, sizeof(patt_highlight), 1, in);
            Read_Data_Swap(&AUDIO_Milliseconds, sizeof(AUDIO_Milliseconds), 1, in);

#if defined(__NO_MIDI__)
            Read_Data(&phony, sizeof(phony), 1, in);
#else
            Read_Data(&c_midiin, sizeof(c_midiin), 1, in);
#endif

#if defined(__NO_MIDI__)
            Read_Data(&phony, sizeof(phony), 1, in);
#else
            Read_Data(&c_midiout, sizeof(c_midiout), 1, in);
#endif

            Read_Data_Swap(&MouseWheel_Multiplier, sizeof(MouseWheel_Multiplier), 1, in);
            Read_Data(&Rows_Decimal, sizeof(Rows_Decimal), 1, in);
            Read_Data(&FullScreen, sizeof(FullScreen), 1, in);

            for(i = 0; i < NUMBER_COLORS; i++)
            {
                Real_Palette_Idx = Idx_Palette[i];
                Read_Data(&Ptk_Palette[Real_Palette_Idx].r, sizeof(char), 1, in);
                Read_Data(&Ptk_Palette[Real_Palette_Idx].g, sizeof(char), 1, in);
                Read_Data(&Ptk_Palette[Real_Palette_Idx].b, sizeof(char), 1, in);
                Ptk_Palette[Real_Palette_Idx].unused = 0;
            }
            Read_Data(&See_Prev_Next_Pattern, sizeof(See_Prev_Next_Pattern), 1, in);
            Read_Data_Swap(&Beveled, sizeof(Beveled), 1, in);
            Read_Data_Swap(&Continuous_Scroll, sizeof(Continuous_Scroll), 1, in);
            Read_Data(&AutoSave, sizeof(AutoSave), 1, in);
            Read_Data(&Dir_Mods, sizeof(Dir_Mods), 1, in);
            Read_Data(&Dir_Instrs, sizeof(Dir_Instrs), 1, in);
            Read_Data(&Dir_Presets, sizeof(Dir_Presets), 1, in);
            Read_Data(&Dir_Reverbs, sizeof(Dir_Reverbs), 1, in);
            Read_Data(&Dir_Patterns, sizeof(Dir_Patterns), 1, in);
            Read_Data(&Dir_Samples, sizeof(Dir_Samples), 1, in);
            Read_Data(KeyboardName, MAX_PATH, 1, in);

            Read_Data(&rawrender_32float, sizeof(char), 1, in);
            Read_Data(&Patterns_Lines, sizeof(char), 1, in);
            Read_Data(&Scopish_LeftRight, sizeof(char), 1, in);

            Read_Data(&Paste_Across, sizeof(char), 1, in);
            Read_Data(&Jazz_Edit, sizeof(char), 1, in);
            Read_Data(&Accidental, sizeof(char), 1, in);

            Read_Data(&Use_Shadows, sizeof(char), 1, in);
            Read_Data(&Global_Patterns_Font, sizeof(char), 1, in);

            if(Patterns_Lines == DISPLAYED_LINES_LARGE)
            {
                VIEWLINE = 22;
                VIEWLINE2 = -22;
                YVIEW = 372;
                Patterns_Lines_Offset = 132;
                userscreen = USER_SCREEN_LARGE_PATTERN;
                curr_tab_highlight = USER_SCREEN_DISKIO_EDIT;
            }
            else
            {
                VIEWLINE = 15;
                VIEWLINE2 = -13;
                YVIEW = 300;
                Patterns_Lines_Offset = 0;
                userscreen = USER_SCREEN_DISKIO_EDIT;
                curr_tab_highlight = USER_SCREEN_DISKIO_EDIT;
            }
        }
        fclose(in);
    }
    sprintf(Keyboard_Name, "%s", KeyboardName);

    // Set default dirs if nothing
    if(!strlen(Dir_Mods))
    {
        GETCWD(Dir_Mods, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Mods, "\\modules");
#else
        strcat(Dir_Mods, "/modules");
#endif

    }
    if(!strlen(Dir_Instrs))
    {
        GETCWD(Dir_Instrs, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Instrs, "\\instruments");
#else
        strcat(Dir_Instrs, "/instruments");
#endif

    }
    if(!strlen(Dir_Presets))
    {
        GETCWD(Dir_Presets, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Presets, "\\presets");
#else
        strcat(Dir_Presets, "/presets");
#endif

    }

    if(!strlen(Dir_Reverbs))
    {
        GETCWD(Dir_Reverbs, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Reverbs, "\\reverbs");
#else
        strcat(Dir_Reverbs, "/reverbs");
#endif

    }

    if(!strlen(Dir_Patterns))
    {
        GETCWD(Dir_Patterns, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Patterns, "\\patterns");
#else
        strcat(Dir_Patterns, "/patterns");
#endif

    }

    if(!strlen(Dir_Samples))
    {
        GETCWD(Dir_Samples, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Samples, "\\samples");
#else
        strcat(Dir_Samples, "/samples");
#endif

    }

    cur_dir = Dir_Mods;
}
#endif // __WINAMP__

// ------------------------------------------------------
// Switch the endianness of a 16 bit buffer
// (size is the number of short elements, not bytes)
void Swap_Short_Buffer(short *buffer, int size)
{
#if defined(__BIG_ENDIAN__)
    int i;

    for(i = 0; i < size; i++)
    {
        buffer[i] = Swap_16(buffer[i]);
    }
#endif
}

// ------------------------------------------------------
// Switch the endianness of a sample
void Swap_Sample(short *buffer, int sample, int bank)
{
#if defined(__BIG_ENDIAN__)
    Swap_Short_Buffer(buffer, SampleNumSamples[sample][bank]);
#endif
}

// ------------------------------------------------------
// Create a new buffer and switch the endianness of a sample
short *Swap_New_Sample(short *buffer, int sample, int bank)
{
#if defined(__BIG_ENDIAN__)
    short *new_buffer = (short *) malloc(SampleNumSamples[sample][bank] * sizeof(short) + 8);
    memset(new_buffer, 0, SampleNumSamples[sample][bank] * sizeof(short) + 8);
    memcpy(new_buffer, buffer, SampleNumSamples[sample][bank] * sizeof(short));
    Swap_Sample(new_buffer, sample, bank);
    return(new_buffer);
#else
    return(NULL);
#endif
}

// ------------------------------------------------------
// Save a given unpacked sample
void Write_Unpacked_Sample(int (*Write_Function)(void *, int ,int, FILE *),
                           FILE *in, int sample, int bank)
{
    short *swap_buffer;

    swap_buffer = Swap_New_Sample(Get_WaveForm(sample, 0, bank), sample, bank);
    if(swap_buffer)
    {
        Write_Function(swap_buffer, sizeof(short), SampleNumSamples[sample][bank], in);
        free(swap_buffer);
    }
    else
    {
        Write_Function(Get_WaveForm(sample, 0, bank), sizeof(short), SampleNumSamples[sample][bank], in);
    }

    Write_Function(&SampleChannels[sample][bank], sizeof(char), 1, in);
    if(SampleChannels[sample][bank] == 2)
    {
        swap_buffer = Swap_New_Sample(Get_WaveForm(sample, 1, bank), sample, bank);
        if(swap_buffer)
        {
            Write_Function(swap_buffer, sizeof(short), SampleNumSamples[sample][bank], in);
            free(swap_buffer);
        }
        else
        {
            Write_Function(Get_WaveForm(sample, 1, bank), sizeof(short), SampleNumSamples[sample][bank], in);
        }
    }
}

// ------------------------------------------------------
// Load the data of a synth instrument
// (The new version (v4) use correct data aligment)
void Read_Synth_Params(int (*Read_Function)(void *, int ,int, FILE *),
                       int (*Read_Function_Swap)(void *, int ,int, FILE *),
                       FILE *in,
                       int idx,
                       int read_disto,
                       int read_lfo_adsr,
                       int new_version,
                       int Env_Modulation,
                       int New_Env,
                       int Ntk_Beta)
{
    if(!new_version)
    {
        if(read_disto && read_lfo_adsr)
        {
            Read_Function(&PARASynth[idx], sizeof(SynthParameters), 1, in);
        }
        else
        {
            if(read_disto)
            {
                Read_Function(&PARASynth[idx], sizeof(SynthParameters) - 32, 1, in);
            }
            else
            {
                if(Ntk_Beta)
                {
                    Read_Function(&PARASynth[idx].presetname, sizeof(char), 20, in);
        
                    Read_Function(&PARASynth[idx].osc1_waveform, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].osc2_waveform, sizeof(char), 1, in);
        
                    Read_Function_Swap(&PARASynth[idx].osc1_pw, sizeof(int), 1, in);
                    Read_Function_Swap(&PARASynth[idx].osc2_pw, sizeof(int), 1, in);
        
                    Read_Function(&PARASynth[idx].osc2_detune, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].osc2_finetune, sizeof(char), 1, in);
        
                    Read_Function(&PARASynth[idx].vcf_cutoff, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].vcf_resonance, sizeof(char), 1, in);
                    char phony;
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].vcf_type, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
    
                    Read_Function_Swap(&PARASynth[idx].env1_attack, sizeof(int), 1, in);
                    Read_Function_Swap(&PARASynth[idx].env1_decay, sizeof(int), 1, in);
                    Read_Function(&PARASynth[idx].env1_sustain, sizeof(char), 1, in);   //44
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
    
                    Read_Function_Swap(&PARASynth[idx].env1_release, sizeof(int), 1, in);
                    Read_Function_Swap(&PARASynth[idx].env2_attack, sizeof(int), 1, in);
                    Read_Function_Swap(&PARASynth[idx].env2_decay, sizeof(int), 1, in);
                    Read_Function(&PARASynth[idx].env2_sustain, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function_Swap(&PARASynth[idx].env2_release, sizeof(int), 1, in);//61
    
                    Read_Function_Swap(&PARASynth[idx].lfo1_period, sizeof(int), 1, in);
                    Read_Function_Swap(&PARASynth[idx].lfo2_period, sizeof(int), 1, in);

                    Read_Function(&PARASynth[idx].lfo1_osc1_pw, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo1_osc2_pw, sizeof(char), 1, in);
                    PARASynth[idx].lfo1_osc1_pitch = 0x40;
                    PARASynth[idx].lfo1_osc2_pitch = 0x40;
                    Read_Function(&PARASynth[idx].lfo1_osc1_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo1_osc2_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo1_vcf_cutoff, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo1_vcf_resonance, sizeof(char), 1, in);

                    Read_Function(&PARASynth[idx].lfo2_osc1_pw, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo2_osc2_pw, sizeof(char), 1, in);
                    PARASynth[idx].lfo2_osc1_pitch = 0x40;
                    PARASynth[idx].lfo2_osc2_pitch = 0x40;
                    Read_Function(&PARASynth[idx].lfo2_osc1_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo2_osc2_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo2_vcf_cutoff, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].lfo2_vcf_resonance, sizeof(char), 1, in);//81
                    Read_Function(&PARASynth[idx].env1_osc1_pw, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env1_osc2_pw, sizeof(char), 1, in);
                    PARASynth[idx].env1_osc1_pitch = 0x40;
                    PARASynth[idx].env1_osc2_pitch = 0x40;
                    Read_Function(&PARASynth[idx].env1_osc1_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env1_osc2_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env1_vcf_cutoff, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env1_vcf_resonance, sizeof(char), 1, in);

                    Read_Function(&PARASynth[idx].env2_osc1_pw, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env2_osc2_pw, sizeof(char), 1, in);
                    PARASynth[idx].env2_osc1_pitch = 0x40;
                    PARASynth[idx].env2_osc2_pitch = 0x40;
                    Read_Function(&PARASynth[idx].env2_osc1_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env2_osc2_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env2_vcf_cutoff, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].env2_vcf_resonance, sizeof(char), 1, in);

                    Read_Function(&PARASynth[idx].osc3_volume, sizeof(char), 1, in);
                    Read_Function(&PARASynth[idx].osc3_switch, sizeof(char), 1, in);//96
                    Read_Function(&phony, sizeof(char), 1, in);
                    Read_Function(&phony, sizeof(char), 1, in);
                    PARASynth[idx].ptc_glide = 0;
                    PARASynth[idx].glb_volume = 127;
                }
                else
                {
                    Read_Function(&PARASynth[idx], sizeof(SynthParameters) - 4 - 32, 1, in);
                }
            }
        }

        PARASynth[idx].osc1_pw = Swap_32(PARASynth[idx].osc1_pw);
        PARASynth[idx].osc2_pw = Swap_32(PARASynth[idx].osc2_pw);

        PARASynth[idx].env1_attack = Swap_32(PARASynth[idx].env1_attack);
        PARASynth[idx].env1_decay = Swap_32(PARASynth[idx].env1_decay);
        PARASynth[idx].env1_release = Swap_32(PARASynth[idx].env1_release);
    
        PARASynth[idx].env2_attack = Swap_32(PARASynth[idx].env2_attack);
        PARASynth[idx].env2_decay = Swap_32(PARASynth[idx].env2_decay);
        PARASynth[idx].env2_release = Swap_32(PARASynth[idx].env2_release);
    
        PARASynth[idx].lfo1_period = Swap_32(PARASynth[idx].lfo1_period);
        PARASynth[idx].lfo2_period = Swap_32(PARASynth[idx].lfo2_period);

        if(read_lfo_adsr)
        {
            PARASynth[idx].lfo1_attack = Swap_32(PARASynth[idx].lfo1_attack);
            PARASynth[idx].lfo1_decay = Swap_32(PARASynth[idx].lfo1_decay);
            PARASynth[idx].lfo1_release = Swap_32(PARASynth[idx].lfo1_release);

            PARASynth[idx].lfo2_attack = Swap_32(PARASynth[idx].lfo2_attack);
            PARASynth[idx].lfo2_decay = Swap_32(PARASynth[idx].lfo2_decay);
            PARASynth[idx].lfo2_release = Swap_32(PARASynth[idx].lfo2_release);
        }
    }
    else
    {
        Read_Function(&PARASynth[idx].presetname, sizeof(char), 20, in);
        
        Read_Function(&PARASynth[idx].osc1_waveform, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].osc2_waveform, sizeof(char), 1, in);
        
        Read_Function_Swap(&PARASynth[idx].osc1_pw, sizeof(int), 1, in);
        Read_Function_Swap(&PARASynth[idx].osc2_pw, sizeof(int), 1, in);
        
        Read_Function(&PARASynth[idx].osc2_detune, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].osc2_finetune, sizeof(char), 1, in);
        
        Read_Function(&PARASynth[idx].vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].vcf_resonance, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].vcf_type, sizeof(char), 1, in);
    
        Read_Function_Swap(&PARASynth[idx].env1_attack, sizeof(int), 1, in);
        Read_Function_Swap(&PARASynth[idx].env1_decay, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].env1_sustain, sizeof(char), 1, in);
        Read_Function_Swap(&PARASynth[idx].env1_release, sizeof(int), 1, in);
    
        Read_Function_Swap(&PARASynth[idx].env2_attack, sizeof(int), 1, in);
        Read_Function_Swap(&PARASynth[idx].env2_decay, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].env2_sustain, sizeof(char), 1, in);
        Read_Function_Swap(&PARASynth[idx].env2_release, sizeof(int), 1, in);
    
        Read_Function_Swap(&PARASynth[idx].lfo1_period, sizeof(int), 1, in);
        Read_Function_Swap(&PARASynth[idx].lfo2_period, sizeof(int), 1, in);

        Read_Function(&PARASynth[idx].lfo1_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].lfo2_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].env1_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].env2_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].osc3_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].osc3_switch, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].ptc_glide, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].glb_volume, sizeof(char), 1, in);

        if(read_disto)
        {
            Read_Function(&PARASynth[idx].disto, sizeof(char), 1, in);
        }

        if(read_lfo_adsr)
        {
            Read_Function_Swap(&PARASynth[idx].lfo1_attack, sizeof(int), 1, in);
            Read_Function_Swap(&PARASynth[idx].lfo1_decay, sizeof(int), 1, in);
            Read_Function(&PARASynth[idx].lfo1_sustain, sizeof(char), 1, in);
            Read_Function_Swap(&PARASynth[idx].lfo1_release, sizeof(int), 1, in);

            Read_Function_Swap(&PARASynth[idx].lfo2_attack, sizeof(int), 1, in);
            Read_Function_Swap(&PARASynth[idx].lfo2_decay, sizeof(int), 1, in);
            Read_Function(&PARASynth[idx].lfo2_sustain, sizeof(char), 1, in);
            Read_Function_Swap(&PARASynth[idx].lfo2_release, sizeof(int), 1, in);
        }
    }

    if(!Env_Modulation)
    {
        char Swap;

        Swap = PARASynth[idx].env1_osc2_volume;
        PARASynth[idx].env1_osc2_volume = PARASynth[idx].env2_osc1_volume;
        PARASynth[idx].env2_osc1_volume = Swap;
    }

    if(!New_Env)
    {
        // In old versions a 64 value meant ENV = 1.0
        // In newer ones it's a normal value (0.0)
        if(PARASynth[idx].env1_osc1_volume == 64) PARASynth[idx].env1_osc1_volume = 127;
        if(PARASynth[idx].env1_osc2_volume == 64) PARASynth[idx].env1_osc2_volume = 127;
        if(PARASynth[idx].env2_osc1_volume == 64) PARASynth[idx].env2_osc1_volume = 127;
        if(PARASynth[idx].env2_osc2_volume == 64) PARASynth[idx].env2_osc2_volume = 127;
    }
}

// ------------------------------------------------------
// Save the data of a synth instrument
void Write_Synth_Params(int (*Write_Function)(void *, int ,int, FILE *),
                        int (*Write_Function_Swap)(void *, int ,int, FILE *),
                        FILE *in,
                        int idx)
{
    Write_Function(&PARASynth[idx].presetname, sizeof(char), 20, in);

    Write_Function(&PARASynth[idx].osc1_waveform, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].osc2_waveform, sizeof(char), 1, in);

    Write_Function_Swap(&PARASynth[idx].osc1_pw, sizeof(int), 1, in);
    Write_Function_Swap(&PARASynth[idx].osc2_pw, sizeof(int), 1, in);

    Write_Function(&PARASynth[idx].osc2_detune, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].osc2_finetune, sizeof(char), 1, in);

    Write_Function(&PARASynth[idx].vcf_cutoff, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].vcf_resonance, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].vcf_type, sizeof(char), 1, in);

    Write_Function_Swap(&PARASynth[idx].env1_attack, sizeof(int), 1, in);
    Write_Function_Swap(&PARASynth[idx].env1_decay, sizeof(int), 1, in);
    Write_Function(&PARASynth[idx].env1_sustain, sizeof(char), 1, in);
    Write_Function_Swap(&PARASynth[idx].env1_release, sizeof(int), 1, in);

    Write_Function_Swap(&PARASynth[idx].env2_attack, sizeof(int), 1, in);
    Write_Function_Swap(&PARASynth[idx].env2_decay, sizeof(int), 1, in);
    Write_Function(&PARASynth[idx].env2_sustain, sizeof(char), 1, in);
    Write_Function_Swap(&PARASynth[idx].env2_release, sizeof(int), 1, in);

    Write_Function_Swap(&PARASynth[idx].lfo1_period, sizeof(int), 1, in);
    Write_Function_Swap(&PARASynth[idx].lfo2_period, sizeof(int), 1, in);

    Write_Function(&PARASynth[idx].lfo1_osc1_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo1_osc2_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo1_osc1_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo1_osc2_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo1_osc1_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo1_osc2_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo1_vcf_cutoff, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo1_vcf_resonance, sizeof(char), 1, in);

    Write_Function(&PARASynth[idx].lfo2_osc1_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo2_osc2_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo2_osc1_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo2_osc2_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo2_osc1_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo2_osc2_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo2_vcf_cutoff, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].lfo2_vcf_resonance, sizeof(char), 1, in);

    Write_Function(&PARASynth[idx].env1_osc1_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env1_osc2_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env1_osc1_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env1_osc2_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env1_osc1_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env1_osc2_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env1_vcf_cutoff, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env1_vcf_resonance, sizeof(char), 1, in);

    Write_Function(&PARASynth[idx].env2_osc1_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env2_osc2_pw, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env2_osc1_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env2_osc2_pitch, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env2_osc1_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env2_osc2_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env2_vcf_cutoff, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].env2_vcf_resonance, sizeof(char), 1, in);

    Write_Function(&PARASynth[idx].osc3_volume, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].osc3_switch, sizeof(char), 1, in);

    Write_Function(&PARASynth[idx].ptc_glide, sizeof(char), 1, in);
    Write_Function(&PARASynth[idx].glb_volume, sizeof(char), 1, in);

    Write_Function(&PARASynth[idx].disto, sizeof(char), 1, in);

    Write_Function_Swap(&PARASynth[idx].lfo1_attack, sizeof(int), 1, in);
    Write_Function_Swap(&PARASynth[idx].lfo1_decay, sizeof(int), 1, in);
    Write_Function(&PARASynth[idx].lfo1_sustain, sizeof(char), 1, in);
    Write_Function_Swap(&PARASynth[idx].lfo1_release, sizeof(int), 1, in);

    Write_Function_Swap(&PARASynth[idx].lfo2_attack, sizeof(int), 1, in);
    Write_Function_Swap(&PARASynth[idx].lfo2_decay, sizeof(int), 1, in);
    Write_Function(&PARASynth[idx].lfo2_sustain, sizeof(char), 1, in);
    Write_Function_Swap(&PARASynth[idx].lfo2_release, sizeof(int), 1, in);
}

// ------------------------------------------------------
// Load a 303 pattern
void Load_303_Data(int (*Read_Function)(void *, int ,int, FILE *),
                   int (*Read_Function_Swap)(void *, int ,int, FILE *),
                   FILE *in, int unit, int pattern)
{
    int i;

    Read_Function(&tb303[unit].patternlength[pattern], sizeof(char), 1, in);
    Read_Function(&tb303[unit].tone[pattern], sizeof(char), 16, in);
    for(i = 0; i < 16; i++)
    {
        Read_Function_Swap(&tb303[unit].flag[pattern][i], sizeof(struct flag303), 1, in);
    }
    Read_Function(&tb303[unit].pattern_name[pattern], sizeof(char), 20, in);
}

// ------------------------------------------------------
// Save a 303 pattern
void Save_303_Data(int (*Write_Function)(void *, int ,int, FILE *),
                   int (*Write_Function_Swap)(void *, int ,int, FILE *),
                   FILE *in, int unit, int pattern)
{
    int i;

    Write_Function(&tb303[unit].patternlength[pattern], sizeof(char), 1, in);
    Write_Function(&tb303[unit].tone[pattern], sizeof(char), 16, in);
    for(i = 0; i < 16; i++)
    {
        Write_Function_Swap(&tb303[unit].flag[pattern][i], sizeof(struct flag303), 1, in);
    }
    Write_Function(&tb303[unit].pattern_name[pattern], sizeof(char), 20, in);
}

// ------------------------------------------------------
// Write data into a file
int Write_Data(void *value, int size, int amount, FILE *handle)
{
    return(fwrite(value, size, amount, handle));
}

// ------------------------------------------------------
// Write data into a file taking care of the endianness
int Write_Data_Swap(void *value, int size, int amount, FILE *handle)
{
    short sswap_value;
    int iswap_value;
    short *svalue;
    int *ivalue;

    switch(size)
    {
        case 2:
            svalue = (short *) value;
            sswap_value = Swap_16(*svalue);
            return(Write_Data(&sswap_value, size, amount, handle));

        case 4:
            ivalue = (int *) value;
            iswap_value = Swap_32(*ivalue);
            return(Write_Data(&iswap_value, size, amount, handle));

        default:
            printf("Invalid writing size.\n");
            break;
    }
    return(TRUE);
}

// ------------------------------------------------------
// Read data from a file
int Read_Data(void *value, int size, int amount, FILE *handle)
{
    return(fread(value, size, amount, handle));
}

// ------------------------------------------------------
// Read data from a file taking care of the endianness
int Read_Data_Swap(void *value, int size, int amount, FILE *handle)
{
    short svalue;
    int ivalue;
    int ret_value;

    switch(size)
    {
        case 2:
            ret_value = Read_Data(&svalue, size, amount, handle);
            svalue = Swap_16(svalue);
            *((short *) value) = (int) svalue;
            return(ret_value);

        case 4:
            ret_value = Read_Data(&ivalue, size, amount, handle);
            ivalue = Swap_32(ivalue);
            *((int *) value) = (int) ivalue;
            return(ret_value);

        default:
            printf("Invalid reading size.\n");
            break;
    }
    return(0);
}

// -------------------------------------
// Calculate the length of the song in hours:minutes:seconds
int song_Seconds;
int song_Minutes;
int song_Hours;

int Calc_Length(void)
{
    int i;
    int k;
    int l;
    int pos_patt;
    int patt_cmd[MAX_FX];
    int patt_datas[MAX_FX];
    Uint8 *Cur_Patt;
    float Ticks = (float) TicksPerBeat;
    float BPM = (float) BeatsPerMin;
    int rep_pos = 0;
    int rep_counter = -1;
    int have_break = 255;
    int PosTicks;
    int shuffle_switch;
    int shuffle_stp = shuffle;
    double len;
    int nbr_ticks;
    int Samples;
    int ilen;
    int early_exit = FALSE;

    shuffle_switch = -1;
    Samples = (int) ((60 * MIX_RATE) / (BeatsPerMin * TicksPerBeat));
    if(shuffle_switch == 1) shuffle_stp = -((Samples * shuffle) / 200);
    else shuffle_stp = (Samples * shuffle) / 200;

    PosTicks = 0;
    nbr_ticks = 0;
    len = 0;
    i = 0;
    while(i < sLength)
    {
        if(have_break < MAX_ROWS) pos_patt = have_break;
        else pos_patt = 0;
        have_break = 255;
        while(pos_patt < patternLines[pSequence[i]])
        {
            Cur_Patt = RawPatterns + (pSequence[i] * PATTERN_LEN) + (pos_patt * PATTERN_ROW_LEN);
            if(!PosTicks)
            {
                for(k = 0; k < Songtracks; k++)
                {
                    // Check if there's a pattern loop command
                    // or a change in the tempo/ticks
                    patt_cmd[0] = Cur_Patt[PATTERN_FX];
                    patt_datas[0] = Cur_Patt[PATTERN_FXDATA];
                    patt_cmd[1] = Cur_Patt[PATTERN_FX2];
                    patt_datas[1] = Cur_Patt[PATTERN_FXDATA2];

                    for(l = 0; l < MAX_FX; l++)
                    {
                        switch(patt_cmd[l])
                        {
                            case 0x6:
                                if(!patt_datas[l])
                                {
                                    rep_counter = -1;
                                    rep_pos = pos_patt;
                                }
                                else
                                {
                                    if(rep_counter == -1)
                                    {
                                        rep_counter = (int) patt_datas[l];
                                        pos_patt = rep_pos;
                                    }
                                    else
                                    {
                                        // count
                                        rep_counter--;
                                        if(rep_counter)
                                        {
                                            pos_patt = rep_pos;
                                        }
                                        else
                                        {
                                            rep_counter = -1;
                                            rep_pos = 0;
                                        }
                                    }
                                }
                                break;

                            case 0xd:
                                if(patt_datas[l] < MAX_ROWS) have_break = patt_datas[l];
                                break;
                        
                            case 0x1f:
                                // Avoid looping the song when jumping
                                if(i == (sLength - 1) || patt_datas[l] <= i)
                                {
                                    early_exit = TRUE;
                                }
                                i = patt_datas[l];
                                // Was there a break already ?
                                if(have_break >= MAX_ROWS) have_break = 0;
                                break;
                        
                            case 0xf:
                                Ticks = (float) patt_datas[l];
                                break;
    
                            case 0xf0:
                                BPM = (float) patt_datas[l];
                                break;
                        }
                    }
                    Cur_Patt += PATTERN_BYTES;
                }
            }
            Samples = (int) ((60 * MIX_RATE) / (BPM * Ticks));

            PosTicks++;
            if(PosTicks > Samples + shuffle_stp)
            {
                shuffle_switch = -shuffle_switch;

                if(shuffle_switch == 1)
                {
                    shuffle_stp = -((Samples * shuffle) / 200);
                }
                else
                {
                    shuffle_stp = (Samples * shuffle) / 200;
                }
                len += PosTicks - 1;

                nbr_ticks++;
                PosTicks = 0;
                if(have_break > 127)
                {
                    pos_patt++;
                }
                else
                {
                    // End the pattern here
                    pos_patt = patternLines[pSequence[i]];
                    rep_counter = -1;
                    rep_pos = 0;
                }
            }
        }
        if(early_exit) break;
        i++;
    }
    len /= MIX_RATE;

    ilen = (int) len;

    song_Seconds = (int) ilen;
    song_Seconds %= 60;
    song_Minutes = (ilen / 60);
    song_Hours = ilen / 60 / 24;

#if !defined(__WINAMP__)
    Display_Song_Length();
#endif

    return((int) (len * 1000));
}

void Reset_Song_Length(void)
{
    song_Hours = 0;
    song_Minutes = 0;
    song_Seconds = 0;
}

// ------------------------------------------------------
// Return the data of an unpacked sample
short *Get_WaveForm(int Instr_Nbr, int Channel, int Split)
{
#if !defined(__WINAMP__) && !defined(__NO_CODEC__)
    if(SamplesSwap[Instr_Nbr])
    {
        return(RawSamples_Swap[Instr_Nbr][Channel][Split]); 
    }
    else
#endif
    {
        return(RawSamples[Instr_Nbr][Channel][Split]); 
    }
}

// ------------------------------------------------------
// Load the data from a reverb file (or a module)
void Load_Reverb_Data(int (*Read_Function)(void *, int ,int, FILE *),
                      int (*Read_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in)
{
    int i;

    Read_Function(&num_echoes, sizeof(char), 1, in);

    for(i = 0; i < MAX_COMB_FILTERS; i++)
    {
        delays[i] = 1000;
        decays[i] = 0.0f;
    }

    for(i = 0; i < num_echoes; i++)
    {
        Read_Function_Swap(&delays[i], sizeof(int), 1, in);
    }
    for(i = 0; i < num_echoes; i++)
    {
        Read_Function_Swap(&decays[i], sizeof(float), 1, in);
    }
}

// ------------------------------------------------------
// Save the data to a reverb file (or a module)
#if !defined(__WINAMP__)
void Save_Reverb_Data(int (*Write_Function)(void *, int ,int, FILE *),
                      int (*Write_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in)
{
    int i;

    Write_Function(&num_echoes, sizeof(char), 1, in);

    // Save the reverb data
    for(i = 0; i < num_echoes; i++)
    {
        Write_Function_Swap(&delays[i], sizeof(int), 1, in);
    }
    for(i = 0; i < num_echoes; i++)
    {
        Write_Function_Swap(&decays[i], sizeof(float), 1, in);
    }
}

// ------------------------------------------------------
// Load a reverb file
void LoadReverb(char *FileName)
{
    FILE *in;
    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        if(strcmp(extension, "TWNNREV1") == 0)
        {
            // Ok, extension matched!
            Status_Box("Loading Reverb data...");

            Read_Data(Reverb_Name, sizeof(char), 20, in);

            Load_Reverb_Data(Read_Data, Read_Data_Swap, in);

            Initreverb();

            Actualize_Reverb_Ed(0);

            Status_Box("Reverb data loaded ok.");
        }
        else
        {
            Status_Box("That file is not a "TITLE" Reverb file...");
        }
        fclose(in);
    }
    else
    {
        Status_Box("Reverb data loading failed. (Probably: file not found)");
    }
}

// ------------------------------------------------------
// Save a reverb file
void SaveReverb(void)
{
    FILE *in;
    char Temph[96];
    char extension[10];

    sprintf(extension, "TWNNREV1");
    sprintf(Temph, "Saving '%s.prv' data in reverbs directory...", Reverb_Name);
    Status_Box(Temph);
    sprintf(Temph, "%s"SLASH"%s.prv", Dir_Reverbs, Reverb_Name);

    in = fopen(Temph, "wb");
    if(in != NULL)
    {
        Write_Data(extension, sizeof(char), 9, in);
        Write_Data(Reverb_Name, sizeof(char), 20, in);

        Save_Reverb_Data(Write_Data, Write_Data_Swap, in);

        fclose(in);
        Read_SMPT();
        last_index = -1;
        Actualize_Files_List(0);
        Status_Box("Reverb data saved succesfully.");   
    }
    else
    {
        Status_Box("Reverb data save failed.");
    }

    Clear_Input();
}

// ------------------------------------------------------
// Load a pattern file
void LoadPattern(char *FileName)
{
    FILE *in;

    if(!is_editing)
    {
        Status_Box("Edit mode isn't turned on.");
        return;
    }

    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        if(strcmp(extension, "TWNNBLK1") == 0)
        {
            // Ok, extension matched!
            Status_Box("Loading Pattern data...");

            Read_Data(Selection_Name, sizeof(char), 20, in);

            Load_Pattern_Data(Read_Data, Read_Data_Swap, in);

            Actupated(0);

            Status_Box("Pattern data loaded ok.");
        }
        else
        {
            Status_Box("That file is not a "TITLE" Pattern file...");
        }
        fclose(in);
    }
    else
    {
        Status_Box("Pattern data loading failed. (Probably: file not found)");
    }
}

// ------------------------------------------------------
// Save a pattern block file
void SavePattern(void)
{
    FILE *in;
    char Temph[96];
    char extension[10];

    sprintf(extension, "TWNNBLK1");
    sprintf(Temph, "Saving '%s.ppb' data in patterns directory...", Selection_Name);
    Status_Box(Temph);
    sprintf(Temph, "%s"SLASH"%s.ppb", Dir_Patterns, Selection_Name);

    in = fopen(Temph, "wb");
    if(in != NULL)
    {
        Write_Data(extension, sizeof(char), 9, in);
        Write_Data(Selection_Name, sizeof(char), 20, in);

        Save_Pattern_Data(Write_Data, Write_Data_Swap, in);

        fclose(in);
        Read_SMPT();
        last_index = -1;
        Actualize_Files_List(0);
        Status_Box("Pattern data saved succesfully.");   
    }
    else
    {
        Status_Box("Pattern data save failed.");
    }

    Clear_Input();
}

// ------------------------------------------------------
// Save the data to a pattern file
void Save_Pattern_Data(int (*Write_Function)(void *, int ,int, FILE *),
                      int (*Write_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in)
{
    int Cur_Position = Get_Song_Position();

    int Old_Curr_Buff_Block = Curr_Buff_Block;

    Copy_Buff(NBR_COPY_BLOCKS - 1, Curr_Buff_Block);
    Curr_Buff_Block = NBR_COPY_BLOCKS - 1;
    Copy_Selection_To_Buffer(Cur_Position);

    Calc_selection();

    Write_Function(Buff_MultiNotes[Curr_Buff_Block], sizeof(char), MAX_TRACKS, in);
    Write_Function_Swap(&b_buff_xsize[Curr_Buff_Block], sizeof(int), 1, in);
    Write_Function_Swap(&b_buff_ysize[Curr_Buff_Block], sizeof(int), 1, in);
    Write_Function_Swap(&start_buff_nibble[Curr_Buff_Block], sizeof(int), 1, in);
    int Block_Len = PATTERN_LEN;

    unsigned char *Final_Mem_Out = Pack_Data(BuffBlock[Curr_Buff_Block], &Block_Len);
    if(Final_Mem_Out)
    {
        Write_Function_Swap(&Block_Len, sizeof(int), 1, in);
        Write_Function(Final_Mem_Out, sizeof(char), Block_Len, in);
        free(Final_Mem_Out);
    }
    Curr_Buff_Block = Old_Curr_Buff_Block;
    Actupated(0);
}

// ------------------------------------------------------
// Load the data from a pattern file
void Load_Pattern_Data(int (*Read_Function)(void *, int ,int, FILE *),
                      int (*Read_Function_Swap)(void *, int ,int, FILE *),
                      FILE *in)
{
    int Cur_Position = Get_Song_Position();

    int Old_Curr_Buff_Block = Curr_Buff_Block;
    Curr_Buff_Block = NBR_COPY_BLOCKS - 1;

    Read_Function(Buff_MultiNotes[Curr_Buff_Block], sizeof(char), MAX_TRACKS, in);
    Read_Function_Swap(&b_buff_xsize[Curr_Buff_Block], sizeof(int), 1, in);
    Read_Function_Swap(&b_buff_ysize[Curr_Buff_Block], sizeof(int), 1, in);
    Read_Function_Swap(&start_buff_nibble[Curr_Buff_Block], sizeof(int), 1, in);

    int Size_In;
    int Size_Out = PATTERN_LEN;
    Read_Function_Swap(&Size_In, sizeof(int), 1, in);

    unsigned char *Pack_Mem = (unsigned char *) malloc(Size_In);
    if(Pack_Mem)
    {
        Read_Function(Pack_Mem, sizeof(char), Size_In, in);
        unsigned char *Final_Mem_Out = Depack_Data(Pack_Mem, Size_In, Size_Out);
        if(Final_Mem_Out)
        {
            Buff_Full[Curr_Buff_Block] = TRUE;
            memcpy(BuffBlock[Curr_Buff_Block], Final_Mem_Out, Size_Out);
            Paste_Block(Cur_Position, Paste_Across, FALSE);
            free(Final_Mem_Out);
        }
        free(Pack_Mem);
    }
    Curr_Buff_Block = Old_Curr_Buff_Block;
    Actupated(0);
}

// ------------------------------------------------------
// Clear any pending input
void Clear_Input(void)
{
    if(snamesel == INPUT_303_PATTERN)
    {
        snamesel = INPUT_NONE;
        Actualize_303_Ed(0);
    }

    if(snamesel == INPUT_SYNTH_NAME)
    {
        snamesel = INPUT_NONE;
        Actualize_Synth_Ed(0);
    }

    if(snamesel == INPUT_MODULE_NAME ||
       snamesel == INPUT_MODULE_ARTIST ||
       snamesel == INPUT_MODULE_STYLE)
    {
        snamesel = INPUT_NONE;
        Actualize_DiskIO_Ed(0);
    }

    if(snamesel == INPUT_INSTRUMENT_NAME)
    {
        snamesel = INPUT_NONE;
        Actualize_Patterned();
    }

    if(snamesel == INPUT_REVERB_NAME)
    {
        snamesel = INPUT_NONE;
        Actualize_Reverb_Ed(0);
    }

    if(snamesel == INPUT_SELECTION_NAME)
    {
        snamesel = INPUT_NONE;
        Actualize_Seq_Ed(0);
    }
}
#endif
