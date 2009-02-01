// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "../extralibs/zlib-1.2.3/zlib.h"

#include "../include/variables.h"
#include "../include/main.h"
#include "include/files.h"
#include "include/samples_pack.h"
#include "../ui/include/misc_draw.h"
#include "../editors/include/editor_synth.h"
#include "../editors/include/editor_diskio.h"
#include "../editors/include/editor_instrument.h"
#include "../editors/include/editor_sequencer.h"
#include "../editors/include/editor_setup.h"
#include "../editors/include/editor_303.h"
#include "../editors/include/editor_sample.h"
#include "../editors/include/editor_fx_setup.h"
#include "../editors/include/editor_track_fx.h"
#include "../editors/include/editor_track.h"
#include "../editors/include/editor_pattern.h"

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

#if !defined(MINGW)
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
extern SynthParameters PARASynth[128];

extern gear303 tb303engine[2];
extern para303 tb303[2];
extern int patt_highlight;
extern char FullScreen;
extern char AutoSave;
extern int Beveled;
extern int Continuous_Scroll;
extern char Dir_Mods[MAX_PATH];
extern char Dir_Instrs[MAX_PATH];
extern char Dir_Presets[MAX_PATH];
extern char *cur_dir;

//char appbuffer[MAX_PATH];

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

AMIGA_NOTE mt_period_conv[] =
{
    { 1712, 1 },
    { 1616, 2 },
    { 1524, 3 },
    { 1440, 4 },
    { 1356, 5 },
    { 1280, 6 },
    { 1208, 7 },
    { 1140, 8 },
    { 1076, 9 },
    { 1016, 10 },
    { 960, 11 },
    { 906, 12 },

    { 856, 13 },
    { 808, 14 },
    { 762, 15 },
    { 720, 16 },
    { 678, 17 },
    { 640, 18 },
    { 604, 19 },
    { 570, 20 },
    { 538, 21 },
    { 508, 22 },
    { 480, 23 },
    { 453, 24 },
    { 428, 25 },
    { 404, 26 },
    { 381, 27 },
    { 360, 28 },
    { 339, 29 },
    { 320, 30 },
    { 302, 31 },
    { 285, 32 },
    { 269, 33 },
    { 254, 34 },
    { 240, 35 },
    { 226, 36 },
    { 214, 37 },
    { 202, 38 },
    { 190, 39 },
    { 180, 40 },
    { 170, 41 },
    { 160, 42 },
    { 151, 43 },
    { 143, 44 },
    { 135, 45 },
    { 127, 46 },
    { 120, 47 },
    { 113, 48 },

    { 107, 49 },
    { 101, 50 },
    { 95, 51 },
    { 90, 52 },
    { 85, 53 },
    { 75, 54 },
    { 71, 55 },
    { 67, 56 },
    { 63, 57 },
    { 60, 58 },
    { 56, 59 },
};

INSTR_ORDER Used_Instr[128];
INSTR_ORDER Used_Instr2[128];
INSTR_ORDER Used_Instr3[128];
unsigned char Old_pSequence[256];
int nbr_User_Instr;

int Muted_Tracks[MAX_TRACKS];
int nbr_muted;
FILE *Out_constants;

FILE *Out_FX;
int Rec_Fx = 0;
LPSYNC_FX Sync_Fx;

// ------------------------------------------------------
// Functions
void Write_Mod_Datas(const void *Datas, int Unit, int Length, FILE *Handle);
void Pack_Sample(FILE *FileHandle, short *Sample, int Size, char Pack_Type);
short *Unpack_Sample(FILE *FileHandle, int Dest_Length, char Pack_Type);

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
// Clear all patterns
void Clean_Up_Patterns_Pool(void)
{
    for(int i = 0; i < PBLEN; i += 6)
    {
        *(RawPatterns + i) = 121;
        *(RawPatterns + i + 1) = 255;
        *(RawPatterns + i + 2) = 255;
        *(RawPatterns + i + 3) = 255;
        *(RawPatterns + i + 4) = 0;
        *(RawPatterns + i + 5) = 0;
    }
}

// ------------------------------------------------------
// Prepare the tracker interface once a module has been loaded
void Init_Tracker_Context_After_ModLoad(void)
{
    ped_track = 0;
    ped_patsam = 0;
    ped_row = 0;
    ped_line_delay = 0;
    cPosition = 0;
    cPosition_delay = 0;

    Final_Mod_Length = 0;

    Post_Song_Init();
    Draw_Scope();

    gui_track = 0;
    Set_Track_Slider(gui_track);
    lchorus_counter = 44100;
    rchorus_counter = 44100;
    lchorus_counter2 = 44100 - lchorus_delay;
    rchorus_counter2 = 44100 - rchorus_delay;
    Initreverb();

    Mas_Compressor_Set_Variables(mas_comp_threshold, mas_comp_ratio);
    Reset_Song_Length();
    Display_Song_Length();

    Scopish = SCOPE_ZONE_MOD_DIR;
    Draw_Scope_Files_Button();

    Reset_Tracks_To_Render();

    Refresh_UI_Context();
}

// ------------------------------------------------------
// Load a .mod file
void LoadAmigaMod(char *FileName, int channels)
{
    FILE *in;
    int t_hi;
    int t_lo;
    int swrite;
    int tps;
    int pwrite;
    int li2;
    int pw2;

    SongStop();

    in = fopen(FileName, "rb");
    if(in != NULL) {
        Free_Samples();
        Clean_Up_Patterns_Pool();

#ifndef NOMIDI
        MidiReset();
#endif

        init_sample_bank();
        Pre_Song_Init();

        fread(FileName, sizeof(char), 20, in);

        Songtracks = channels;

        for(swrite = 0; swrite < 31; swrite++)
        {
            SampleType[swrite][0] = 1;
            fread(&nameins[swrite], sizeof(char), 19, in);
            sprintf(SampleName[swrite][0], "%s ", nameins[swrite]);

            // Jump over 3 unhandled bytes for PTK samplename.
            fseek(in, 3, SEEK_CUR);

            SampleNumSamples[swrite][0] = (int) (fgetc(in) * 256) + (int) fgetc(in);
            SampleNumSamples[swrite][0] *= 2;
            fread(&Finetune[swrite][0], sizeof(char), 1, in);
            Finetune[swrite][0] *= 16;
            CustomVol[swrite] = (float) fgetc(in) * 0.015625f;
            SampleVol[swrite][0] = 1.0f;
            Basenote[swrite][0] = 48 - 5 + 12;

            SampleType[swrite][0] = 0;    // NO SAMPLE

            // Calculate/Adapt AMIGA loop points to ntk LoopPoints:

            LoopStart[swrite][0] = ((int) (fgetc(in) * 256) + (int) fgetc(in)) * 2;
            long Replen = ((int) (fgetc(in) * 256) + (int) fgetc(in)) * 2;

            if(Replen > 2)
            {
                LoopEnd[swrite][0] = LoopStart[swrite][0] + Replen;
                LoopType[swrite][0] = SMP_LOOP_FORWARD;
            }
            else
            {
                LoopEnd[swrite][0] = SampleNumSamples[swrite][0] - 1;
                LoopType[swrite][0] = SMP_LOOP_NONE;
            }

            if(LoopEnd[swrite][0] >= SampleNumSamples[swrite][0])
            {
                LoopEnd[swrite][0] = SampleNumSamples[swrite][0] - 1;
            }
        } // FOR

        fread(&sLength, sizeof(unsigned char), 1, in);
        fgetc(in);

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

        for(pwrite = 0; pwrite < nPatterns; pwrite++)
        {
            for(li2 = 0; li2 < 64; li2++)
            {
                for(pw2 = 0; pw2 < channels; pw2++)
                {
                    int tmo = (pwrite * 12288) + (li2 * 96) + (pw2 * 6);
                    int f_byte = fgetc(in);
                    int t_sample = f_byte >> 4;
                    int t_period = (f_byte - (t_sample << 4)) * 256 + (int) fgetc(in);
                    int t_byte = fgetc(in);
                    int t_sample2 = t_byte >> 4;
                    int t_command = t_byte - (t_sample2 << 4);
                    t_sample = (t_sample << 4) + t_sample2;
                    if(t_sample == 0) t_sample = 255;
                    if(t_sample != 255) t_sample--;

                    int t_note = 121;
                    int t_argu = fgetc(in);

                    // Period Conversion Table
                    t_note = Conv_Amiga_Note(t_period);

                    *(RawPatterns + tmo) = t_note;
                    *(RawPatterns + tmo + 1) = t_sample;

                    // Pattern effect adapter:
                    switch(t_command)
                    {
                        // SPEED [not accurate]
                        case 0xf:
                            if(t_argu > 31)
                            {
                                t_command = 240;
                            }
                            else
                            {
                                switch(t_argu)
                                {
                                    case 1: t_argu = 9; break;
                                    case 2: t_argu = 8; break;
                                    case 3: t_argu = 7; break;
                                    case 4: t_argu = 6; break;
                                    case 5: t_argu = 5; break;
                                    case 6: t_argu = 4; break;
                                    case 7: t_argu = 3; break;
                                    case 8: t_argu = 2; break;
                                    case 9: t_argu = 1; break;
                                    case 10: t_argu = 1; break;
                                    case 11: t_argu = 1; break;
                                    case 12: t_argu = 1; break;
                                    case 13: t_argu = 1; break;
                                    case 14: t_argu = 1; break;
                                    case 15: t_argu = 1; break;
                                    case 16: t_argu = 1; break;
                                    case 17: t_argu = 1; break;
                                    case 18: t_argu = 1; break;
                                    case 19: t_argu = 1; break;
                                    case 20: t_argu = 1; break;
                                    case 21: t_argu = 1; break;
                                    case 22: t_argu = 1; break;
                                    case 23: t_argu = 1; break;
                                    case 24: t_argu = 1; break;
                                    case 25: t_argu = 1; break;
                                    case 26: t_argu = 1; break;
                                    case 27: t_argu = 1; break;
                                    case 28: t_argu = 1; break;
                                    case 29: t_argu = 1; break;
                                    case 30: t_argu = 1; break;
                                    case 31: t_argu = 1; break;
                                }
                            }
                            break;

                        // ARPEGGIO
                        case 0:
                            t_command = 0x1b;
                            break;

                        // GLIDE
                        case 3:
                            t_command = 5;
                            break;

                        // VIBRATO [Not supported by NTK, ntk wasn't to sound like a violin :P ].
                        case 4:
                            t_command = 0;
                            t_argu = 0;
                            break;

                        // NO TREMOLO EITHER
                        case 6:
                            t_command = 0;
                            t_argu = 0;
                            break;

                        // DEMOSYNCHRO SIGNAL???
                        case 8:
                            t_command = 7;
                            break;

                        // Volume Sliders
                        case 0xa:
                            if(t_argu >= 16)
                            {
                                t_command = 25; // Vol SlideUp
                                t_argu = (t_argu >> 4) * 4;
                            }
                            else
                            {
                                t_command = 26; // Vol Slide Down
                                t_argu *= 4;
                            }
                            break;

                        // VOLUME
                        case 0xc:
                            t_command = 3;
                            t_argu *= 8;
                            if(t_argu > 255) t_argu = 255;
                            break;

                        // Pattern Jump Amiga Mod stylee
                        case 0xd:
                            t_hi = t_argu / 16;
                            t_lo = t_argu - t_hi * 16;
                            t_argu = t_hi * 10 + t_lo;
                            break;

                        // EXX Special Effects
                        // [not a full implementation of all Protracker MOD Exx commands]
                        case 0xe:
                            t_command = 0;

                            // PATTERN LOOP
                            if(t_argu >= 0x60 && t_argu < 0x70)
                            {
                                t_command = 0x06;
                                t_argu = t_argu - 0x60;
                            }

                            // FINEVOLUME SLIDEUP.
                            if(t_argu > 0xa0 && t_argu < 0xb0)
                            {
                                t_command = 0x19;
                                t_argu = t_argu - 0xa0;
                            }

                            // FINEVOLUME SLIDEDOWN.
                            if(t_argu > 0xb0 && t_argu < 0xc0)
                            {
                                t_command = 0x1a;
                                t_argu = t_argu - 0xb0;
                            }

                            // NOTE RETRIGGER
                            if(t_argu > 0x90 && t_argu < 0xa0)
                            {
                                t_command = 0xe;
                                t_argu = t_argu - 0x90;
                            }

                            // NOTE CUT
                            if(t_argu > 0xc0 && t_argu < 0xd0)
                            {
                                *(RawPatterns + tmo + 2) = (t_argu & 0xf) | 0xf0;
                                t_command = 0;
                                t_argu = 0;
                            }
                            break;

                    } // Pattern FX adapter end.

                    *(RawPatterns + tmo + 4) = t_command;
                    *(RawPatterns + tmo + 5) = t_argu;
                }
            }
        }

        for(swrite = 0; swrite < 31; swrite++)
        {
            if(SampleNumSamples[swrite][0] > 8)
            {
                SampleChannels[swrite][0] = 1;
                SampleType[swrite][0] = 1;

                // Reserving space for 16-Bit Signed Short Data.
                RawSamples[swrite][0][0] = (short *) malloc((SampleNumSamples[swrite][0] * 2));
                for(Uint32 x = 0; x < SampleNumSamples[swrite][0]; x++)
                {
                    *(RawSamples[swrite][0][0] + x) = (short) fgetc(in) << 8;
                }
                *RawSamples[swrite][0][0] = 0;
            }
            else
            {
                // NO SAMPLE
                SampleType[swrite][0] = 0;
            }
        }

        TPan[0] = 0.3f;
        TPan[1] = 0.7f;
        TPan[2] = 0.8f;
        TPan[3] = 0.2f;
        ComputeStereo(0);
        ComputeStereo(1);
        ComputeStereo(2);
        ComputeStereo(3);

        BeatsPerMin = 125;
        TicksPerBeat = 4;
        mas_vol = 0.75f;
        mas_comp_threshold = 100.0f;
        mas_comp_ratio = 0.0f;

        fclose(in);

        Init_Tracker_Context_After_ModLoad();
    }
    else
    {
        mess_box("Protracker module loading failed. (Probably: file not found)");
    }

    if(snamesel == 1 || snamesel == 4 || snamesel == 5) snamesel = 0;
}

// ------------------------------------------------------
// Load a module file
void LoadMod(char *FileName)
{
    int Phony_Old_Value;
    int Old_Mod_Format = FALSE;
    int New_adsr = FALSE;
    int New_Comp = FALSE;
    int Comp_Flag;

    int i;
    int j;
    int k;
    int Old_303;
    unsigned char *TmpPatterns;
    unsigned char *TmpPatterns_Tracks;
    unsigned char *TmpPatterns_Notes;
    unsigned char *TmpPatterns_Rows;
    int Old_Bug = TRUE;
    int new_disto = FALSE;
    int Pack_Scheme = FALSE;
    int tps_pos;
    int tps_trk;
    int nbr_instr;
    int twrite;
    int fake_value;
    int Packed_Size;
    unsigned char *Packed_Module = NULL;

    Mod_Simulate = LOAD_READ;
    Mod_Mem_Pos = 0;
    Mod_Memory = NULL;

    SongStop();
    mess_box("Attempting to load the song file...");

    FILE *in;
    in = fopen(FileName, "rb");
    Old_303 = FALSE;

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        switch(extension[7])
        {
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
                Old_Mod_Format = TRUE;
                goto Read_Mod_File;

            // Old noisetrekker
            case '2':
                Old_303 = TRUE;
                Old_Mod_Format = TRUE;
                goto Read_Mod_File;
        }

        // Read a possible packed module ID
        unsigned int New_Extension;
        fseek(in, 0, SEEK_SET);
        fread(&New_Extension, sizeof(char), 4, in);

        if(New_Extension == 'KTRP')
        {
            Old_Mod_Format = FALSE;
            New_Comp = TRUE;
            Read_Mod_File:
            mess_box("Loading song -> Header"); 
            Free_Samples();

            mas_comp_threshold = 100.0f;
            mas_comp_ratio = 0.0f;

            allow_save = Old_Mod_Format;

            Clean_Up_Patterns_Pool();

#ifndef NOMIDI
            MidiReset();
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
                    fread(Packed_Module, sizeof(unsigned char), Packed_Size, in);
                    Mod_Memory = Depack_Data(Packed_Module, Packed_Size);
                    Mod_Mem_Pos = 0;
                    free(Packed_Module);
                }
            }

            if(Old_Mod_Format) Read_Mod_Datas(FileName, sizeof(char), 20, in);
            else memset(FileName, 0, 20);
            Read_Mod_Datas(&nPatterns, sizeof(unsigned char), 1, in);
            Songtracks = MAX_TRACKS;
            if(!Old_Mod_Format) Read_Mod_Datas(&Songtracks, sizeof(char), 1, in);
            Read_Mod_Datas(&sLength, sizeof(unsigned char), 1, in);

            if(Old_Mod_Format) Read_Mod_Datas(pSequence, sizeof(unsigned char), 256, in);
            else Read_Mod_Datas(pSequence, sizeof(unsigned char), sLength, in);

            memset(RawPatterns, 0, (12288 * nPatterns));

            if(Old_Mod_Format)
            {
                Read_Mod_Datas(patternLines, sizeof(short), 128, in);
            }
            else
            {
                for(i = 0; i < 128; i++)
                {
                    patternLines[i] = 64;
                }
                for(i = 0; i < nPatterns; i++)
                {
                    Read_Mod_Datas(&patternLines[i], sizeof(char), 1, in);
                }
            }
            if(Old_Mod_Format)
            {
                for(int pwrite = 0; pwrite < nPatterns; pwrite++)
                {
                    Read_Mod_Datas(RawPatterns + pwrite * 12288, 12288, 1, in);
                }
            }
            else
            {
                TmpPatterns = RawPatterns;
                for(int pwrite = 0; pwrite < nPatterns; pwrite++)
                {
                    TmpPatterns_Rows = TmpPatterns + (pwrite * 12288);
                    for(i = 0; i < 6; i++)
                    {   // Bytes / track
                        for(k = 0; k < Songtracks; k++)
                        {   // Tracks
                            TmpPatterns_Tracks = TmpPatterns_Rows + (k * 6);
                            for(j = 0; j < patternLines[pwrite]; j++)
                            {   // Rows
                                TmpPatterns_Notes = TmpPatterns_Tracks + (j * (MAX_TRACKS * 6));
                                Read_Mod_Datas(TmpPatterns_Notes + i, 1, 1, in);
                            }
                        }
                    }
                }
            }

            mess_box("Loading song -> Sample data");
            if(Old_Mod_Format)
            {
                for(int swrite = 0; swrite < 128; swrite++)
                {
                    Read_Mod_Datas(&nameins[swrite], sizeof(char), 20, in);
                    Read_Mod_Datas(&Midiprg[swrite], sizeof(char), 1, in);
                    Read_Mod_Datas(&Synthprg[swrite], sizeof(unsigned char), 1, in);

                    PARASynth[swrite].disto = 0;

                    PARASynth[swrite].lfo1_attack = 0;
                    PARASynth[swrite].lfo1_decay = 0;
                    PARASynth[swrite].lfo1_sustain = 128;
                    PARASynth[swrite].lfo1_release = 0x10000;

                    PARASynth[swrite].lfo2_attack = 0;
                    PARASynth[swrite].lfo2_decay = 0;
                    PARASynth[swrite].lfo2_sustain = 128;
                    PARASynth[swrite].lfo2_release = 0x10000;

                    // BEWARE: struct is aligned on 4 bytes !!!
                    if(!new_disto)
                    {
                        Read_Mod_Datas(&PARASynth[swrite], sizeof(SynthParameters) - 4 - 32, 1, in);
                    }
                    else
                    {
                        if(New_adsr)
                        {
                            Read_Mod_Datas(&PARASynth[swrite], sizeof(SynthParameters), 1, in);
                        }
                        else
                        {
                            Read_Mod_Datas(&PARASynth[swrite], sizeof(SynthParameters) - 32, 1, in);
                        }
                    }
                    // Compression type
                    if(Pack_Scheme) Read_Mod_Datas(&SampleCompression[swrite], sizeof(char), 1, in);

                    for(int slwrite = 0; slwrite < 16; slwrite++)
                    {
                        Read_Mod_Datas(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                        if(SampleType[swrite][slwrite] != 0)
                        {
                            if(Old_Bug) Read_Mod_Datas(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                            else Read_Mod_Datas(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                            Read_Mod_Datas(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Datas(&LoopStart[swrite][slwrite], sizeof(long), 1, in);
                            Read_Mod_Datas(&LoopEnd[swrite][slwrite], sizeof(long), 1, in);
                            Read_Mod_Datas(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Datas(&SampleNumSamples[swrite][slwrite], sizeof(long), 1, in);
                            Read_Mod_Datas(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Datas(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                            Read_Mod_Datas(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                            RawSamples[swrite][0][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * 2);
                            Read_Mod_Datas(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                            *RawSamples[swrite][0][slwrite] = 0;
                            // Stereo flag
                            Read_Mod_Datas(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                            if(SampleChannels[swrite][slwrite] == 2) {
                                RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * 2);
                                Read_Mod_Datas(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                                *RawSamples[swrite][1][slwrite] = 0;
                            }
                        }// Exist Sample
                    }
                }
            }
            else
            {
                // Packed format
                Read_Mod_Datas(&nbr_instr, sizeof(int), 1, in);

                for(int swrite = 0; swrite < nbr_instr; swrite++)
                {
                    memset(nameins[swrite], 0, 20);
                    Read_Mod_Datas(&Synthprg[swrite], sizeof(unsigned char), 1, in);

                    if(Synthprg[swrite])
                    {
                        Read_Mod_Datas(&PARASynth[swrite].osc1_waveform, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].osc2_waveform, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].osc1_pw, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].osc2_pw, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].osc2_detune, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].osc2_finetune, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].vcf_resonance, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].vcf_type, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].env1_attack, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_decay, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_sustain, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_release, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_attack, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_decay, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_sustain, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_release, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_period, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_period, sizeof(int), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].lfo1_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_osc2_pw, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_osc2_pitch, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_osc2_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].lfo2_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_osc2_pw, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_osc2_pitch, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_osc2_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].env1_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_osc2_pw, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_osc2_pitch, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_osc2_volume, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].env1_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env1_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].env2_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_osc2_pw, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].env2_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_osc2_pitch, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].env2_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_osc2_volume, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].env2_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].env2_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].osc3_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].osc3_switch, sizeof(bool), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].ptc_glide, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].glb_volume, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].disto, sizeof(char), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].lfo1_attack, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_decay, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_sustain, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo1_release, sizeof(int), 1, in);

                        Read_Mod_Datas(&PARASynth[swrite].lfo2_attack, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_decay, sizeof(int), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_sustain, sizeof(char), 1, in);
                        Read_Mod_Datas(&PARASynth[swrite].lfo2_release, sizeof(int), 1, in);
                    }
                    else
                    {
                        memset(&PARASynth[swrite], 0, sizeof(SynthParameters));
                    }

                    // Compression type
                    Read_Mod_Datas(&SampleCompression[swrite], sizeof(char), 1, in);

                    for(int slwrite = 0; slwrite < 16; slwrite++)
                    {
                        Read_Mod_Datas(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                        if(SampleType[swrite][slwrite] != 0)
                        {
                            int Apply_Interpolation;
                            Uint32 Save_Len;
                            Uint32 iSmp;
                            short Sample1;
                            short Sample2;
                            short *Sample_Buffer = NULL;
                            short *Sample_Dest_Buffer;
                            // No samples names in packed modules
                            memset(SampleName[swrite][slwrite], 0, 64);
                            Read_Mod_Datas(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Datas(&LoopStart[swrite][slwrite], sizeof(long), 1, in);
                            Read_Mod_Datas(&LoopEnd[swrite][slwrite], sizeof(long), 1, in);
                            Read_Mod_Datas(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Datas(&SampleNumSamples[swrite][slwrite], sizeof(long), 1, in);
                            Read_Mod_Datas(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Datas(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                            Read_Mod_Datas(&FDecay[swrite][slwrite], sizeof(float), 1, in);
                            Save_Len = SampleNumSamples[swrite][slwrite];

                            Apply_Interpolation = SampleCompression[swrite] == SMP_PACK_NONE ? FALSE : TRUE;

                            if(Apply_Interpolation)
                            {
                                Save_Len /= 2;
                                Sample_Buffer = Unpack_Sample(in, Save_Len, SampleCompression[swrite]);
                                Sample_Dest_Buffer = (short *) malloc((Save_Len * 2 * sizeof(short)) + 2);
                                if(Sample_Buffer)
                                {
                                    // Interpolate samples
                                    for(iSmp = 0; iSmp < Save_Len; iSmp++)
                                    {
                                        Sample1 = Sample_Buffer[iSmp];
                                        Sample2 = Sample_Buffer[iSmp + 1];
                                        if(iSmp == Save_Len - 1)
                                        {
                                            if(LoopType[swrite][slwrite])
                                            {
                                                Sample2 = Sample_Dest_Buffer[LoopStart[swrite][slwrite]];
                                            }
                                            else
                                            {
                                                Sample2 = Sample_Buffer[iSmp];
                                            }
                                        }
                                        Sample_Dest_Buffer[(iSmp * 2)] = Sample1;
                                        Sample_Dest_Buffer[(iSmp * 2) + 1] = Sample1 + ((Sample2 - Sample1) / 2);
                                    }
                                }
                                RawSamples[swrite][0][slwrite] = Sample_Dest_Buffer;
                            }
                            else
                            {
                                RawSamples[swrite][0][slwrite] = Unpack_Sample(in, Save_Len, SampleCompression[swrite]);
                            }
                            *(RawSamples[swrite][0][slwrite]) = 0;

                            // Stereo flag
                            Read_Mod_Datas(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                            if(SampleChannels[swrite][slwrite] == 2)
                            {
                                if(Apply_Interpolation)
                                {
                                    Sample_Buffer = Unpack_Sample(in, Save_Len, SampleCompression[swrite]);
                                    Sample_Dest_Buffer = (short *) malloc((Save_Len * 2 * sizeof(short)) + 2);
                                    if(Sample_Buffer)
                                    {
                                        for(iSmp = 0; iSmp < Save_Len; iSmp++)
                                        {
                                            Sample1 = Sample_Buffer[iSmp];
                                            Sample2 = Sample_Buffer[iSmp + 1];
                                            if(LoopType[swrite][slwrite])
                                            {
                                                Sample2 = Sample_Dest_Buffer[LoopStart[swrite][slwrite]];
                                            }
                                            else
                                            {
                                                Sample2 = Sample_Buffer[iSmp];
                                            }
                                            Sample_Dest_Buffer[(iSmp * 2)] = Sample1;
                                            Sample_Dest_Buffer[(iSmp * 2) + 1] = Sample1 + ((Sample2 - Sample1) / 2);
                                        }
                                    }
                                    RawSamples[swrite][1][slwrite] = Sample_Dest_Buffer;
                                }
                                else
                                {
                                    RawSamples[swrite][1][slwrite] = Unpack_Sample(in, Save_Len, SampleCompression[swrite]);
                                }
                                *RawSamples[swrite][1][slwrite] = 0;
                            }
                            if(Sample_Buffer) free(Sample_Buffer);
                        }
                    }
                }
            }

            mess_box("Loading song -> Track info, patterns and sequence.");   

            // Reading Track Properties
            for(twrite = 0; twrite < Songtracks; twrite++)
            {
                Read_Mod_Datas(&TCut[twrite], sizeof(float), 1, in);
                Read_Mod_Datas(&ICut[twrite], sizeof(float), 1, in);
                if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
                if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;

                Read_Mod_Datas(&TPan[twrite], sizeof(float), 1, in);
                ComputeStereo(twrite);
                Read_Mod_Datas(&FType[twrite], sizeof(int), 1, in);
                Read_Mod_Datas(&FRez[twrite], sizeof(int), 1, in);
                Read_Mod_Datas(&DThreshold[twrite], sizeof(float), 1, in);
                Read_Mod_Datas(&DClamp[twrite], sizeof(float), 1, in);
                Read_Mod_Datas(&DSend[twrite], sizeof(float), 1, in);
                Read_Mod_Datas(&CSend[twrite], sizeof(int), 1, in);
            }

            // Reading mod properties
            Read_Mod_Datas(&compressor, sizeof(int), 1, in);
            Read_Mod_Datas(&c_threshold, sizeof(int), 1, in);
            Read_Mod_Datas(&BeatsPerMin, sizeof(int), 1, in);
            Read_Mod_Datas(&TicksPerBeat, sizeof(int),1, in);
            Read_Mod_Datas(&mas_vol, sizeof(float), 1, in);
            if(mas_vol < 0.01f) mas_vol = 0.01f;
            if(mas_vol > 1.0f) mas_vol = 1.0f;

            if(New_Comp)
            {
                Comp_Flag = 0;
                Read_Mod_Datas(&Comp_Flag, sizeof(char), 1, in);
                if(Comp_Flag)
                {
                    Read_Mod_Datas(&mas_comp_threshold, sizeof(float), 1, in);
                    if(mas_comp_threshold < 0.01f) mas_comp_threshold = 0.01f;
                    if(mas_comp_threshold > 100.0f) mas_comp_threshold = 100.0f;
                    Read_Mod_Datas(&mas_comp_ratio, sizeof(float), 1, in);
                    if(mas_comp_ratio < 0.01f) mas_comp_ratio = 0.01f;
                    if(mas_comp_ratio > 100.0f) mas_comp_ratio = 100.0f;
                }
            }

            Read_Mod_Datas(&delay_time, sizeof(int), 1, in);
            Read_Mod_Datas(&Feedback, sizeof(float), 1, in);
            Read_Mod_Datas(&DelayType, sizeof(int), 1, in);
            Read_Mod_Datas(&lchorus_delay, sizeof(int), 1, in);
            Read_Mod_Datas(&rchorus_delay, sizeof(int), 1, in);
            Read_Mod_Datas(&lchorus_feedback, sizeof(float), 1, in);
            Read_Mod_Datas(&rchorus_feedback, sizeof(float), 1, in);
            Read_Mod_Datas(&shuffle, sizeof(int), 1, in);

            // Reading track part sequence

            // Reset'em first
            if(Old_Mod_Format)
            {
                for(tps_pos = 0; tps_pos < 256; tps_pos++)
                {
                    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                    {
                        Read_Mod_Datas(&SACTIVE[tps_pos][tps_trk], sizeof(bool), 1, in);
                    }
                }
            }
            else
            {
                for(tps_pos = 0; tps_pos < 256; tps_pos++)
                {
                    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                    {
                        SACTIVE[tps_pos][tps_trk] = true;
                        SHISTORY[tps_pos][tps_trk] = false;
                    }
                }
                for(tps_pos = 0; tps_pos < sLength; tps_pos++)
                {
                    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                    {
                        Read_Mod_Datas(&SACTIVE[tps_pos][tps_trk], sizeof(bool), 1, in);
                        SHISTORY[tps_pos][tps_trk] = false;
                    }
                }
            }

            for(int spl = 0; spl < Songtracks; spl++)
            {
                CCoef[spl] = float((float) CSend[spl] / 127.0f);
            }

            if(Old_Mod_Format)
            {
                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Datas(&TRACKMIDICHANNEL[twrite], sizeof(int), 1, in);
                }
            }

            if(Old_Mod_Format)
            {

                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Datas(&LFO_ON[twrite], sizeof(char), 1, in);
                    Read_Mod_Datas(&LFO_RATE[twrite], sizeof(float), 1, in);
                    Read_Mod_Datas(&LFO_AMPL[twrite], sizeof(float), 1, in);
                }

                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Datas(&FLANGER_ON[twrite], sizeof(char), 1, in);
                    Read_Mod_Datas(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                    Read_Mod_Datas(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                    Read_Mod_Datas(&FLANGER_RATE[twrite], sizeof(float), 1, in);
                    Read_Mod_Datas(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
                    Read_Mod_Datas(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                    Read_Mod_Datas(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
                    FLANGER_OFFSET[twrite] = 8192;
                    foff2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                    foff1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                }

            }
            else
            {

                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Datas(&LFO_ON[twrite], sizeof(char), 1, in);
                    if(LFO_ON[twrite])
                    {
                        Read_Mod_Datas(&LFO_RATE[twrite], sizeof(float), 1, in);
                        Read_Mod_Datas(&LFO_AMPL[twrite], sizeof(float), 1, in);
                    }
                }

                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Datas(&FLANGER_ON[twrite], sizeof(char), 1, in);
                    if(FLANGER_ON[twrite])
                    {
                        Read_Mod_Datas(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                        Read_Mod_Datas(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                        Read_Mod_Datas(&FLANGER_RATE[twrite], sizeof(float), 1, in);
                        Read_Mod_Datas(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
                        Read_Mod_Datas(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                        Read_Mod_Datas(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
                        FLANGER_OFFSET[twrite] = 8192;
                        foff2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                        foff1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                    }
                }
            }

            if(Old_Mod_Format)
            {

                Read_Mod_Datas(&FLANGER_DEPHASE, sizeof(float), 1, in);

                for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                {
                    Read_Mod_Datas(&TRACKSTATE[tps_trk], sizeof(int), 1, in);
                }
                Read_Mod_Datas(&Songtracks, sizeof(char), 1, in);
                for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
                {
                    Read_Mod_Datas(&Disclap[tps_trk], sizeof(bool), 1, in);
                    Read_Mod_Datas(&fake_value, sizeof(bool), 1, in);
                }
            }
            else
            {
                for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                {
                    Read_Mod_Datas(&Disclap[tps_trk], sizeof(bool), 1, in);
                }
            }

            if(Old_Mod_Format) Read_Mod_Datas(artist, sizeof(char), 20, in);
            else memset(artist, 0, 20);
            if(Old_Mod_Format) Read_Mod_Datas(style, sizeof(char), 20, in);
            else memset(style, 0, 20);
            if(Old_Mod_Format) Read_Mod_Datas(&Phony_Old_Value, sizeof(char), 1, in);
            Read_Mod_Datas(beatsync, sizeof(bool), 128, in);
            Read_Mod_Datas(beatlines, sizeof(short), 128, in);
            Read_Mod_Datas(&REVERBFILTER, sizeof(float), 1, in);

            Read_Mod_Datas(CustomVol, sizeof(float), 128, in);
            if(Old_Mod_Format) Read_Mod_Datas(&Phony_Old_Value, sizeof(bool), 1, in);

            // Read the 303 datas
            if(Old_Mod_Format)
            {
                if(Old_303)
                {
                    Read_Mod_Datas(&tb303[0], sizeof(para303) - (32 * 20), 1, in);
                    Read_Mod_Datas(&tb303[1], sizeof(para303) - (32 * 20), 1, in);
                }
                else
                {
                    Read_Mod_Datas(&tb303[0], sizeof(para303), 1, in);
                    Read_Mod_Datas(&tb303[1], sizeof(para303), 1, in);
                }  
                Read_Mod_Datas(&tb303engine[0].tbVolume, sizeof(float), 1, in);
                Read_Mod_Datas(&tb303engine[1].tbVolume, sizeof(float), 1, in);
                Read_Mod_Datas(&tb303engine[0].hpf, sizeof(bool), 1, in);
                Read_Mod_Datas(&tb303engine[1].hpf, sizeof(bool), 1, in);
            }
            else
            {
                Reset_303_Parameters(&tb303[0]);
                Reset_303_Parameters(&tb303[1]);

                Read_Mod_Datas(&tb303[0].enabled, sizeof(unsigned char), 1, in);
                if(tb303[0].enabled)
                {
                    Read_Mod_Datas(&tb303[0].selectedpattern, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].tune, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].cutoff, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].resonance, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].envmod, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].decay, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].accent, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].waveform, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[0].patternlength, sizeof(unsigned char), 32, in);
                    Read_Mod_Datas(&tb303[0].tone, sizeof(unsigned char), 32 * 16, in);
                    Read_Mod_Datas(&tb303[0].flag, sizeof(struct flag303), 32 * 16, in);
                }

                Read_Mod_Datas(&tb303[1].enabled, sizeof(unsigned char), 1, in);
                if(tb303[1].enabled)
                {
                    Read_Mod_Datas(&tb303[1].selectedpattern, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].tune, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].cutoff, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].resonance, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].envmod, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].decay, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].accent, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].waveform, sizeof(unsigned char), 1, in);
                    Read_Mod_Datas(&tb303[1].patternlength, sizeof(unsigned char), 32, in);
                    Read_Mod_Datas(&tb303[1].tone, sizeof(unsigned char), 32 * 16, in);
                    Read_Mod_Datas(&tb303[1].flag, sizeof(struct flag303), 32 * 16, in);
                }
                if(tb303[0].enabled) Read_Mod_Datas(&tb303engine[0].tbVolume, sizeof(float), 1, in);
                if(tb303[1].enabled) Read_Mod_Datas(&tb303engine[1].tbVolume, sizeof(float), 1, in);
                if(tb303[0].enabled) Read_Mod_Datas(&tb303engine[0].hpf, sizeof(bool), 1, in);
                if(tb303[1].enabled) Read_Mod_Datas(&tb303engine[1].hpf, sizeof(bool), 1, in);
            }

            fclose(in);

            // Init the tracker context
            Init_Tracker_Context_After_ModLoad();
        }
        else
        {
            mess_box("That file is not a suitable module file...");
        }
    }
    else
    {
        mess_box("Module loading failed. (Probably: file not found)");
    }

    if(snamesel == 1 || snamesel == 4 || snamesel == 5) snamesel = 0;
    if(Mod_Memory) free(Mod_Memory);
}

// ------------------------------------------------------
// Load and decode a packed sample
short *Unpack_Sample(FILE *FileHandle, int Dest_Length, char Pack_Type)
{
    int Packed_Length;

#ifndef NOCODEC
    short *Dest_Buffer;
#endif

    Uint8 *Packed_Read_Buffer;

    fread(&Packed_Length, sizeof(char), 4, FileHandle);
    if(Packed_Length == -1)
    {
        // Sample wasn't packed
        Packed_Read_Buffer = (Uint8 *) malloc(Dest_Length * 2);
        fread(Packed_Read_Buffer, sizeof(char), Dest_Length * 2, FileHandle);
        return((short *) Packed_Read_Buffer);
    }
    else
    {

#ifndef NOCODEC
        Packed_Read_Buffer = (Uint8 *) malloc(Packed_Length);
        // Read the packer buffer
        fread(Packed_Read_Buffer, sizeof(char), Packed_Length, FileHandle);
        Dest_Buffer = (short *) malloc(Dest_Length * 2);
        memset(Dest_Buffer, 0, Dest_Length * 2);

        switch(Pack_Type)
        {
            case SMP_PACK_AT3:
                //UnpackAT3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
            case SMP_PACK_GSM:
                UnpackGSM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
            case SMP_PACK_MP3:
                UnpackMP3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
            case SMP_PACK_TRUESPEECH:
                UnpackTrueSpeech(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
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
void Pack_Sample(FILE *FileHandle, short *Sample, int Size, char Pack_Type)
{
    int Sample_Pack_Value = FALSE;
    int PackedLen = 0;
    short *PackedSample = NULL;

#ifndef NOCODEC
    short *AlignedSample;
    int Aligned_Size;

    switch(Pack_Type)
    {
        case SMP_PACK_AT3:
            PackedSample = (short *) malloc(Size * 2);
            memset(PackedSample, 0, Size * 2);
            //PackedLen = ToAT3(Sample, PackedSample, Size * 2);
            break;
        case SMP_PACK_GSM:
            PackedSample = (short *) malloc(Size * 2);
            memset(PackedSample, 0, Size * 2);
            PackedLen = ToGSM(Sample, PackedSample, Size * 2);
            break;
        case SMP_PACK_MP3:
            PackedSample = (short *) malloc(Size * 2);
            memset(PackedSample, 0, Size * 2);
            PackedLen = ToMP3(Sample, PackedSample, Size * 2);
            break;
        case SMP_PACK_TRUESPEECH:
            Aligned_Size = (Size * 2) + 0x400;
            AlignedSample = (short *) malloc(Aligned_Size);
            if(AlignedSample)
            {
                memset(AlignedSample, 0, Aligned_Size);
                memcpy(AlignedSample, Sample, Size * 2);
                // Size must be aligned
                PackedSample = (short *) malloc(Aligned_Size);
                if(PackedSample)
                {
                    memset(PackedSample, 0, Aligned_Size);
                    PackedLen = ToTrueSpeech(AlignedSample, PackedSample, Aligned_Size);
                }
                free(AlignedSample);
            }
            break;
        case SMP_PACK_NONE:
            PackedLen = 0;
            break;
    }
    if(PackedLen)
    {
        // Write the encoded length
        Write_Mod_Datas(&PackedLen, sizeof(char), 4, FileHandle);
        // Write the encoded datas
        Write_Mod_Datas(PackedSample, sizeof(char), PackedLen, FileHandle);
    }
    else
    {
#endif
        // Couldn't pack (too small or user do not want that to happens)
        PackedLen = -1;
        Write_Mod_Datas(&PackedLen, sizeof(char), 4, FileHandle);
        Write_Mod_Datas(Sample, sizeof(char), Size * 2, FileHandle);

#ifndef NOCODEC
    }
#endif

#ifndef NOCODEC
    if(PackedSample) free(PackedSample);
#endif

}

void Write_Mod_Datas(const void *Datas, int Unit, int Length, FILE *Handle)
{
    switch(Mod_Simulate)
    {
        case SAVE_WRITE:
            fwrite(Datas, Unit, Length, Handle);
            break;

        case SAVE_CALCLEN:
            Mod_Length += Unit * Length;
            break;

        case SAVE_WRITEMEM:
            memcpy(Mod_Memory + Mod_Mem_Pos, Datas, Unit * Length);
            Mod_Mem_Pos += Unit * Length;
            break;
    }
}

void Read_Mod_Datas(void *Datas, int Unit, int Length, FILE *Handle)
{
    switch(Mod_Simulate)
    {
        case LOAD_READ:
            fread(Datas, Unit, Length, Handle);
            break;

        case LOAD_READMEM:
            memcpy(Datas, Mod_Memory + Mod_Mem_Pos, Unit * Length);
            Mod_Mem_Pos += Unit * Length;
            break;
    }
}

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

// ------------------------------------------------------
// Save as module file
int Get_Instr_New_Order(int instr)
{
    int i;

    for(i = 0; i < 128; i++)
    {
        if(Used_Instr[i].old_order == instr) return(Used_Instr[i].new_order);
    }
    return(-1);
}

int Search_Sequence(int sequence_idx)
{
    int i;

    for(i = 0; i < 256; i++)
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

int SaveMod_Ptp(FILE *in, int Simulate, char *FileName)
{
    unsigned char *TmpPatterns;
    unsigned char *TmpPatterns_Tracks;
    unsigned char *TmpPatterns_Notes;
    unsigned char *TmpPatterns_Rows;
    unsigned int New_Extension = 'KTRP';
    char FileName_FX[MAX_PATH];
    int i;
    int j;
    int k;
    int l;
    int twrite;
    int tps_trk;
    int Store_303_1 = FALSE;
    int Store_303_2 = FALSE;
    int Store_Gsm = FALSE;
    int Store_Mp3 = FALSE;
    int Store_TrueSpeech = FALSE;
    int Store_At3 = FALSE;
    int Store_Flanger = FALSE;
    int Store_LFO = FALSE;
    int Store_Instr_Waveform_Osc1 = FALSE; 
    int Store_Instr_Waveform_Osc2 = FALSE;
    int Store_Synth_Disto = FALSE;
    int Store_Synth_Osc3 = FALSE;

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
    int Store_FX_Arpeggio = FALSE;

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

    New_RawPatterns = (unsigned char *) malloc(PBLEN);
    if(!New_RawPatterns) return(FALSE);

    // Writing header & name...
    Write_Mod_Datas(&New_Extension, sizeof(char), 4, in);

    // Re-arrange the patterns sequence
    int_pattern = 0;
    memset(done_pattern, 0, sizeof(done_pattern));
    memset(Old_pSequence, -1, sizeof(Old_pSequence));

    for(i = 0; i < sLength; i++)
    {
        if(!done_pattern[pSequence[i]])
        {
            memcpy(New_RawPatterns + (int_pattern * 12288), RawPatterns + (pSequence[i] * 12288), 12288);
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
        if(TRACKSTATE[tps_trk])
        {
            Muted_Tracks[nbr_muted] = tps_trk;
            nbr_muted++;
        }
    }
    Real_SongTracks = Songtracks - nbr_muted;

    Write_Mod_Datas(&int_pattern, sizeof(unsigned char), 1, in);
    // Number of tracks is stored here for new format
    Write_Mod_Datas(&Real_SongTracks, sizeof(char), 1, in);
    Write_Mod_Datas(&sLength, sizeof(unsigned char), 1, in);
    // Patterns sequence
    Write_Mod_Datas(New_pSequence, sizeof(unsigned char), sLength, in);

    for(i = 0; i < int_pattern; i++)
    {
        Write_Mod_Datas(&New_patternLines[i], sizeof(char), 1, in);
    }

    // Check the instruments
    nbr_User_Instr = 0;
    TmpPatterns = New_RawPatterns;
    for(i = 0; i < 128; i++)
    {
        Used_Instr[i].new_order = -1;
        Used_Instr[i].old_order = -1;
        Used_Instr2[i].old_order = -1;
    }
    for(pwrite = 0; pwrite < int_pattern; pwrite++)
    {
        TmpPatterns_Rows = TmpPatterns + (pwrite * 12288);
        for(i = 0; i < 6; i++)
        {   // Datas
            for(k = 0; k < Songtracks; k++)
            {   // Tracks
                if(!Track_Is_Muted(k))
                {
                    TmpPatterns_Tracks = TmpPatterns_Rows + (k * 6);
                    for(j = 0; j < New_patternLines[pwrite]; j++)
                    {   // Rows
                        TmpPatterns_Notes = TmpPatterns_Tracks + (j * (MAX_TRACKS * 6));
                        // Store the used instrument
                        if(i == 1)
                        {
                            if(TmpPatterns_Notes[i] < 128)
                            {
                                if(Used_Instr[TmpPatterns_Notes[i]].new_order == -1)
                                {
                                    Used_Instr[TmpPatterns_Notes[i]].new_order = nbr_User_Instr;
                                    Used_Instr[TmpPatterns_Notes[i]].old_order = TmpPatterns_Notes[i];
                                    Used_Instr2[nbr_User_Instr].old_order = TmpPatterns_Notes[i];
                                    nbr_User_Instr++;
                                }
                            }
                        }
                        // Count the number of synchro fxs
                        if(i == 4)
                        {
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
    for(i = 0; i < 128; i++)
    {
        switch(Synthprg[i])
        {
            case SYNTH_WAVE_OFF:
                break;
            case SYNTH_WAVE_CURRENT:
                synth_instr_remap = i;
                break;
            default:
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
            sprintf(FileName_FX, "%s.psy", FileName);
            Out_FX = fopen(FileName_FX, "wb");

            // Save the FX data
            for(l = 0; l < sLength; l++)
            {
                TmpPatterns_Rows = New_RawPatterns + (New_pSequence[l] * 12288);
                for(i = 0; i < 6; i++)
                {   // Datas
                    for(k = 0; k < Songtracks; k++)
                    {   // Tracks
                        if(!Track_Is_Muted(k))
                        {
                            TmpPatterns_Tracks = TmpPatterns_Rows + (k * 6);
                            for(j = 0; j < New_patternLines[New_pSequence[l]]; j++)
                            {   // Rows
                                TmpPatterns_Notes = TmpPatterns_Tracks + (j * (MAX_TRACKS * 6));
                                if(i == 4)
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
                    TmpPatterns_Rows = New_RawPatterns + (New_pSequence[l] * 12288);
                    for(i = 0; i < 6; i++)
                    {   // Datas
                        for(k = 0; k < Songtracks; k++)
                        {   // Tracks
                            if(!Track_Is_Muted(k))
                            {
                                TmpPatterns_Tracks = TmpPatterns_Rows + (k * 6);
                                for(j = 0; j < New_patternLines[New_pSequence[l]]; j++)
                                {   // Rows
                                    TmpPatterns_Notes = TmpPatterns_Tracks + (j * (MAX_TRACKS * 6));
                                    if(i == 4)
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

                fwrite(&real_fx_nbr, 2, 1, Out_FX);
                for(i = 0; i < real_fx_nbr; i++)
                {
                    Save_FX(Sync_Fx[i].Pos, Sync_Fx[i].Row, Sync_Fx[i].Data);
                }
                free(Sync_Fx);
            }
        }
    }


    TmpPatterns = New_RawPatterns;
    for(pwrite = 0; pwrite < int_pattern; pwrite++)
    {
        TmpPatterns_Rows = TmpPatterns + (pwrite * 12288);
        for(i = 0; i < 6; i++)
        {   // Datas
            for(k = 0; k < Songtracks; k++)
            {   // Tracks
                if(!Track_Is_Muted(k))
                {
                    TmpPatterns_Tracks = TmpPatterns_Rows + (k * 6);
                    for(j = 0; j < New_patternLines[pwrite]; j++)
                    {   // Rows
                        TmpPatterns_Notes = TmpPatterns_Tracks + (j * (MAX_TRACKS * 6));
                        // Check the volume column
                        if(i == 2)
                        {
                            if(TmpPatterns_Notes[i] != 0xff)
                            {
                                if(TmpPatterns_Notes[i] & 0xf0 == 0xf0)
                                {
                                    Store_FX_NoteCut = TRUE;
                                }
                            }
                        }

                        // Check the effects column
                        if(i == 4)
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

                                // $1a Arpeggio
                                case 0x1b:
                                    Store_FX_Arpeggio = TRUE;
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
                            case 4:
                                // Don't save FX 7
                                if(TmpPatterns_Notes[i] == 0x7)
                                {
                                    Write_Mod_Datas(&Empty_Var, sizeof(char), 1, in);
                                }
                                else
                                {
                                    Write_Mod_Datas(TmpPatterns_Notes + i, sizeof(char), 1, in);
                                }
                                break;
                            case 5:
                                // Don't save Fx 7 datas
                                if(TmpPatterns_Notes[i - 1] == 0x7)
                                {
                                    Write_Mod_Datas(&Empty_Var, sizeof(char), 1, in);
                                }
                                else
                                {
                                    Write_Mod_Datas(TmpPatterns_Notes + i, sizeof(char), 1, in);
                                }
                                break;
                            case 1:
                                // Replace the instrument order
                                if(TmpPatterns_Notes[i] < 128)
                                {
                                    TmpPatterns_Notes[i] = Get_Instr_New_Order(TmpPatterns_Notes[i]);
                                }
                                // no break on purpose
                            default:
                                Write_Mod_Datas(TmpPatterns_Notes + i, sizeof(char), 1, in);
                                break;
                        }
                    }
                }
            }
        }
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
                              Store_FX_VolumeSlideDown);

    Save_Constant("PTK_FX_NOTECUT", Store_FX_NoteCut);
    Save_Constant("PTK_FX_PITCHUP", Store_FX_PitchUp);
    Save_Constant("PTK_FX_PITCHDOWN", Store_FX_PitchDown);
    Save_Constant("PTK_FX_SETVOLUME", Store_FX_SetVolume);
    Save_Constant("PTK_FX_TRANCESLICER", Store_FX_TranceSlicer);
    Save_Constant("PTK_FX_TRANCEGLIDER", Store_FX_TranceGlider);

    Save_Constant("PTK_FX_PATTERNLOOP", Store_FX_PatternLoop);
    Save_Constant("PTK_FX_SETCUTOFF", Store_FX_SetCutOff);
    Save_Constant("PTK_FX_SETSAMPLEOFFSET", Store_FX_SetSampleOffset);
    Save_Constant("PTK_FX_SETRANDCOMCUTOFF", Store_FX_SetRandomCutOff);
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
    Save_Constant("PTK_FX_ARPEGGIO", Store_FX_Arpeggio);

    Save_Constant("PTK_FX_TICK0", Store_FX_Arpeggio | Store_FX_PatternLoop);

    // Remap the used instruments
    for(i = 0; i < 128; i++)
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

    Write_Mod_Datas(&nbr_User_Instr, sizeof(int), 1, in);

    // Writing sample data
    // TODO: check Synthprg to determine the real waves used
    for(i = 0; i < 128; i++)
    {
        // Check if it was used at pattern level
        swrite = Used_Instr2[i].old_order;
        if(swrite != -1)
        {
            Write_Mod_Datas(&Synthprg[swrite], sizeof(unsigned char), 1, in);

            if(Synthprg[swrite])
            {
                Write_Mod_Datas(&PARASynth[swrite].osc1_waveform, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].osc2_waveform, sizeof(char), 1, in);
                if(PARASynth[swrite].osc1_waveform == 5) Store_Instr_Waveform_Osc1 = TRUE;
                if(PARASynth[swrite].osc2_waveform == 5) Store_Instr_Waveform_Osc2 = TRUE;
                if(PARASynth[swrite].osc1_waveform == 3) Store_Synth_WhiteNoise = TRUE;
                if(PARASynth[swrite].osc2_waveform == 3) Store_Synth_WhiteNoise = TRUE;
                if(PARASynth[swrite].osc1_waveform == 6) Store_Synth_PinkNoise = TRUE;
                if(PARASynth[swrite].osc2_waveform == 6) Store_Synth_PinkNoise = TRUE;

                Write_Mod_Datas(&PARASynth[swrite].osc1_pw, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].osc2_pw, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].osc2_detune, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].osc2_finetune, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].vcf_resonance, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].vcf_type, sizeof(char), 1, in);
                if(PARASynth[swrite].vcf_type != 2) Store_Synth_Filter = TRUE;

                Write_Mod_Datas(&PARASynth[swrite].env1_attack, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_decay, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_sustain, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_release, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_attack, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_decay, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_sustain, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_release, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_period, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_period, sizeof(int), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].lfo1_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_osc2_pw, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_osc2_pitch, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_osc2_volume, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].lfo2_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_osc2_pw, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_osc2_pitch, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_osc2_volume, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].env1_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_osc2_pw, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_osc2_pitch, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_osc2_volume, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].env1_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env1_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].env2_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_osc2_pw, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].env2_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_osc2_pitch, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].env2_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_osc2_volume, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].env2_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].env2_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].osc3_volume, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].osc3_switch, sizeof(bool), 1, in);
                if(PARASynth[swrite].osc3_switch) Store_Synth_Osc3 = TRUE;

                Write_Mod_Datas(&PARASynth[swrite].ptc_glide, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].glb_volume, sizeof(char), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].disto, sizeof(char), 1, in);
                if(PARASynth[swrite].disto) Store_Synth_Disto = TRUE;

                Write_Mod_Datas(&PARASynth[swrite].lfo1_attack, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_decay, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_sustain, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo1_release, sizeof(int), 1, in);

                Write_Mod_Datas(&PARASynth[swrite].lfo2_attack, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_decay, sizeof(int), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_sustain, sizeof(char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite].lfo2_release, sizeof(int), 1, in);
            }

#ifdef NOCODEC
            int No_Comp = SMP_PACK_NONE;
            Write_Mod_Datas(&No_Comp, sizeof(char), 1, in);
#else
            Write_Mod_Datas(&SampleCompression[swrite], sizeof(char), 1, in);
#endif

            // Compression type

            // 16 splits / instrument
            for(int slwrite = 0; slwrite < 16; slwrite++)
            {

                Write_Mod_Datas(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite] != 0)
                {
                    int Apply_Interpolation = FALSE;

#ifndef NOCODEC
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

                    Write_Mod_Datas(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                    Write_Mod_Datas(&Loop_Start, sizeof(long), 1, in);
                    Write_Mod_Datas(&Loop_End, sizeof(long), 1, in);
                    Write_Mod_Datas(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                    Write_Mod_Datas(&Calc_Len, sizeof(long), 1, in);
                    Write_Mod_Datas(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                    Write_Mod_Datas(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                    Write_Mod_Datas(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                    if(Apply_Interpolation)
                    {
                        Calc_Len /= 2;
                        Smp_Dats = (short *) malloc(Real_Len * 2);
                        // Halve the sample
                        for(iSmp = 0; iSmp < Calc_Len; iSmp++)
                        {
                            Smp_Dats[iSmp] = *(RawSamples[swrite][0][slwrite] + (iSmp * 2));
                        }
                        Pack_Sample(in, Smp_Dats, Calc_Len, SampleCompression[swrite]);
                    }
                    else
                    {
                        Pack_Sample(in, RawSamples[swrite][0][slwrite], Calc_Len, SampleCompression[swrite]);
                    }
                    // Stereo mode ?
                    Write_Mod_Datas(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        if(Apply_Interpolation)
                        {
                            // Halve the sample
                            for(iSmp = 0; iSmp < Calc_Len; iSmp++)
                            {
                                Smp_Dats[iSmp] = *(RawSamples[swrite][1][slwrite] + (iSmp * 2));
                            }
                            Pack_Sample(in, Smp_Dats, Calc_Len, SampleCompression[swrite]);
                        }
                        else
                        {
                            Pack_Sample(in, RawSamples[swrite][1][slwrite], Calc_Len, SampleCompression[swrite]);
                        }
                    }
                    free(Smp_Dats);
                } // Exist Sample
            }
        }
    }

    Save_Constant("PTK_SYNTH_DISTO", Store_Synth_Disto);
    Save_Constant("PTK_SYNTH_OSC3", Store_Synth_Osc3);
    Save_Constant("PTK_SYNTH_FILTER", Store_Synth_Filter);
    Save_Constant("PTK_SYNTH_WHITENOISE", Store_Synth_WhiteNoise);
    Save_Constant("PTK_SYNTH_PINKNOISE", Store_Synth_PinkNoise);

    //Save_Constant("PTK_WAVEFORM", Store_Instr_Waveform_Osc1 | Store_Instr_Waveform_Osc2);
    //Save_Constant("PTK_WAVEFORM_OSC1", Store_Instr_Waveform_Osc1);
    //Save_Constant("PTK_WAVEFORM_OSC2", Store_Instr_Waveform_Osc2);

    Save_Constant("PTK_GSM", Store_Gsm);
    Save_Constant("PTK_MP3", Store_Mp3);
    Save_Constant("PTK_TRUESPEECH", Store_TrueSpeech);
    Save_Constant("PTK_AT3", Store_At3);

    for(twrite = 0; twrite < Songtracks; twrite++)
    {
        if(!Track_Is_Muted(twrite))
        {
            if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
            if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;
            Write_Mod_Datas(&TCut[twrite], sizeof(float), 1, in);
            Write_Mod_Datas(&ICut[twrite], sizeof(float), 1, in);
            Write_Mod_Datas(&TPan[twrite], sizeof(float), 1, in);
            Write_Mod_Datas(&FType[twrite], sizeof(int), 1, in);
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
                Write_Mod_Datas(&FRez[twrite], sizeof(int), 1, in);
                Write_Mod_Datas(&DThreshold[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&DClamp[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&DSend[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&CSend[twrite], sizeof(int), 1, in);
            }
    }

    // Writing mod properties
    Write_Mod_Datas(&compressor, sizeof(int), 1, in);
    Write_Mod_Datas(&c_threshold, sizeof(int), 1, in);
    Write_Mod_Datas(&BeatsPerMin, sizeof(int ), 1, in);
    Write_Mod_Datas(&TicksPerBeat, sizeof(int ), 1, in);
    if(mas_vol < 0.01f) mas_vol = 0.01f;
    if(mas_vol > 1.0f) mas_vol = 1.0f;
    Write_Mod_Datas(&mas_vol, sizeof(float), 1, in);

    float threshold = mas_comp_threshold;
    float ratio = mas_comp_ratio;
    int Comp_Flag;
    if(threshold < 0.01f) threshold = 0.01f;
    if(threshold > 100.0f) threshold = 100.0f;
    if(ratio < 0.01f) ratio = 0.01f;
    if(ratio > 100.0f) ratio = 100.0f;

    if(ratio > 0.01f)
    {
        threshold *= 0.001f;
        ratio *= 0.01f;
        Comp_Flag = 1;
        Write_Mod_Datas(&Comp_Flag, sizeof(char), 1, in);
        Write_Mod_Datas(&threshold, sizeof(float), 1, in);
        Write_Mod_Datas(&ratio, sizeof(float), 1, in);
        Save_Constant("PTK_LIMITER", TRUE);
    }
    else
    {
        Comp_Flag = 0;
        Write_Mod_Datas(&Comp_Flag, sizeof(char), 1, in);
        Save_Constant("PTK_LIMITER", FALSE);
    }

    Write_Mod_Datas(&delay_time, sizeof(int), 1, in);
    Write_Mod_Datas(&Feedback, sizeof(float), 1, in);
    Write_Mod_Datas(&DelayType, sizeof(int), 1, in);
    Write_Mod_Datas(&lchorus_delay, sizeof(int), 1, in);
    Write_Mod_Datas(&rchorus_delay, sizeof(int), 1, in);
    Write_Mod_Datas(&lchorus_feedback, sizeof(float), 1, in);
    Write_Mod_Datas(&rchorus_feedback, sizeof(float), 1, in);
    Write_Mod_Datas(&shuffle, sizeof(int), 1, in);

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
                Write_Mod_Datas(&SACTIVE[tps_pos][tps_trk], sizeof(bool), 1, in);
            }
        }
    }

    for(twrite = 0; twrite < Songtracks; twrite++)
    {
        if(!Track_Is_Muted(twrite))
        {
            Write_Mod_Datas(&LFO_ON[twrite], sizeof(char), 1, in);
            if(LFO_ON[twrite])
            {
                Store_LFO = TRUE;
                Write_Mod_Datas(&LFO_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&LFO_AMPL[twrite], sizeof(float), 1, in);
            }
        }
    }
    Save_Constant("PTK_LFO", Store_LFO);

    for(twrite = 0; twrite < Songtracks; twrite++)
    {
        if(!Track_Is_Muted(twrite))
        {
            Write_Mod_Datas(&FLANGER_ON[twrite], sizeof(char), 1, in);
            if(FLANGER_ON[twrite])
            {
                Store_Flanger = TRUE;
                Write_Mod_Datas(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
            }
        }
    }
    Save_Constant("PTK_FLANGER", Store_Flanger);

    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
    {
        if(!Track_Is_Muted(tps_trk))
        {
            Write_Mod_Datas(&Disclap[tps_trk], sizeof(bool), 1, in);
        }
    }

    Write_Mod_Datas(beatsync, sizeof(bool), 128, in);
    Write_Mod_Datas(beatlines, sizeof(short), 128, in);
    Write_Mod_Datas(&REVERBFILTER, sizeof(float), 1, in);
    Write_Mod_Datas(CustomVol, sizeof(float), 128, in);

    Write_Mod_Datas(&Store_303_1, sizeof(unsigned char), 1, in);
    if(Store_303_1)
    {
        Write_Mod_Datas(&tb303[0].selectedpattern, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].tune, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].cutoff, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].resonance, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].envmod, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].decay, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].accent, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].waveform, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[0].patternlength, sizeof(unsigned char), 32, in);
        Write_Mod_Datas(&tb303[0].tone, sizeof(unsigned char), 32 * 16, in);
        Write_Mod_Datas(&tb303[0].flag, sizeof(struct flag303), 32 * 16, in);
    }

    Write_Mod_Datas(&Store_303_2, sizeof(unsigned char), 1, in);
    if(Store_303_2)
    {
        Write_Mod_Datas(&tb303[1].selectedpattern, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].tune, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].cutoff, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].resonance, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].envmod, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].decay, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].accent, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].waveform, sizeof(unsigned char), 1, in);
        Write_Mod_Datas(&tb303[1].patternlength, sizeof(unsigned char), 32, in);
        Write_Mod_Datas(&tb303[1].tone, sizeof(unsigned char), 32 * 16, in);
        Write_Mod_Datas(&tb303[1].flag, sizeof(struct flag303), 32 * 16, in);
    }
    if(Store_303_1) Write_Mod_Datas(&tb303engine[0].tbVolume, sizeof(float), 1, in);
    if(Store_303_2) Write_Mod_Datas(&tb303engine[1].tbVolume, sizeof(float), 1, in);
    if(Store_303_1) Write_Mod_Datas(&tb303engine[0].hpf, sizeof(bool), 1, in);
    if(Store_303_2) Write_Mod_Datas(&tb303engine[1].hpf, sizeof(bool), 1, in);

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

int SaveMod(char *FileName, int NewFormat, int Simulate, Uint8 *Memory)
{
    FILE *in;
    int i;
    int j;
    int k;
    char Temph[96];
    int Old_Phony_Value = 0;
    int Sample_Pack_Value = 0;
    int Ok_Memory = TRUE;
    int fake_value;
    int Comp_Flag = TRUE;
    unsigned char *cur_pattern_col;
    unsigned char *cur_pattern;

    int twrite;
    int tps_trk;
    Mod_Length = 0;
    Mod_Mem_Pos = 0;
    Mod_Simulate = SAVE_WRITE;
    Mod_Memory = Memory;

    if(Simulate)
    {
        Mod_Simulate = Simulate;
    }

    if(!Simulate)
    {
        if(NewFormat)
        {
            sprintf(Temph, "Saving '%s.ptp' song on current directory...", FileName);
            mess_box(Temph);
            sprintf(Temph, "%s.ptp", FileName);
        }
        else
        {
            sprintf(Temph, "Saving '%s.ptk' song on current directory...", FileName);
            mess_box(Temph);
            sprintf(Temph, "%s.ptk", FileName);
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
            Ok_Memory = SaveMod_Ptp(in, Simulate, FileName);
        }
        else
        {

            if(strlen(FileName)) rtrim_string(FileName, 20);
            Write_Mod_Datas(FileName, sizeof(char), 20, in);

            Write_Mod_Datas(&nPatterns, sizeof(unsigned char), 1, in);
            Write_Mod_Datas(&sLength, sizeof(unsigned char), 1, in);
            Write_Mod_Datas(pSequence, sizeof(unsigned char), 256, in);
            Write_Mod_Datas(patternLines, sizeof(short), 128, in);

            // Clean the unused patterns garbage (doesn't seem to do much)
            for(i = Songtracks; i < MAX_TRACKS; i++)
            {
                // Next column
                cur_pattern_col = RawPatterns + (i * 6);
                for(j = 0; j < nPatterns; j++)
                {
                    // Next pattern
                    cur_pattern = cur_pattern_col + (j * 12288);
                    for(k = 0; k < patternLines[j]; k++)
                    {
                        cur_pattern[0] = 0x79;
                        cur_pattern[1] = 0xff;
                        cur_pattern[2] = 0xff;
                        cur_pattern[3] = 0xff;
                        cur_pattern[4] = 0x00;
                        cur_pattern[5] = 0x00;
                        // Next line
                        cur_pattern += (6 * MAX_TRACKS);
                    }
                }
            }

            for(int pwrite = 0; pwrite < nPatterns; pwrite++)
            {
                Write_Mod_Datas(RawPatterns + (pwrite * 12288), 1, 12288, in);
            }

            // Writing sample data
            for(int swrite = 0; swrite < 128; swrite++)
            {
                rtrim_string(nameins[swrite], 20);
                Write_Mod_Datas(&nameins[swrite], sizeof(char), 20, in);
                Write_Mod_Datas(&Midiprg[swrite], sizeof(char), 1, in);
                Write_Mod_Datas(&Synthprg[swrite], sizeof(unsigned char), 1, in);
                Write_Mod_Datas(&PARASynth[swrite], sizeof(SynthParameters), 1, in);

                // Compression type
                Write_Mod_Datas(&SampleCompression[swrite], sizeof(char), 1, in);

                // 16 splits / instrument
                for(int slwrite = 0; slwrite < 16; slwrite++)
                {

                    Write_Mod_Datas(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleType[swrite][slwrite] != 0)
                    {

                        rtrim_string(SampleName[swrite][slwrite], 64);
                        Write_Mod_Datas(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                        Write_Mod_Datas(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                        Write_Mod_Datas(&LoopStart[swrite][slwrite], sizeof(long), 1, in);
                        Write_Mod_Datas(&LoopEnd[swrite][slwrite], sizeof(long), 1, in);
                        Write_Mod_Datas(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                        Write_Mod_Datas(&SampleNumSamples[swrite][slwrite], sizeof(long), 1, in);
                        Write_Mod_Datas(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                        Write_Mod_Datas(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                        Write_Mod_Datas(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                        // All samples are converted into 16 bits
                        Write_Mod_Datas(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                        // Stereo mode ?
                        Write_Mod_Datas(&SampleChannels[swrite][slwrite], sizeof(char ), 1, in);
                        if(SampleChannels[swrite][slwrite] == 2)
                        {
                            Write_Mod_Datas(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                        }
                    }// Exist Sample
                }
            }

            // Writing Track Propertiers
            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Datas(&TCut[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&ICut[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&TPan[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FType[twrite], sizeof(int), 1, in);
                Write_Mod_Datas(&FRez[twrite], sizeof(int), 1, in);
                Write_Mod_Datas(&DThreshold[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&DClamp[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&DSend[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&CSend[twrite], sizeof(int), 1, in);
            }

            // Writing mod properties
            Write_Mod_Datas(&compressor, sizeof(int), 1, in);
            Write_Mod_Datas(&c_threshold, sizeof(int), 1, in);
            Write_Mod_Datas(&BeatsPerMin, sizeof(int), 1, in);
            Write_Mod_Datas(&TicksPerBeat, sizeof(int), 1, in);
            Write_Mod_Datas(&mas_vol, sizeof(float), 1, in);
            Write_Mod_Datas(&Comp_Flag, sizeof(char), 1, in);
            Write_Mod_Datas(&mas_comp_threshold, sizeof(float), 1, in);
            Write_Mod_Datas(&mas_comp_ratio, sizeof(float), 1, in);
            Write_Mod_Datas(&delay_time, sizeof(int), 1, in);
            Write_Mod_Datas(&Feedback, sizeof(float), 1, in);
            Write_Mod_Datas(&DelayType, sizeof(int), 1, in);
            Write_Mod_Datas(&lchorus_delay, sizeof(int), 1, in);
            Write_Mod_Datas(&rchorus_delay, sizeof(int), 1, in);
            Write_Mod_Datas(&lchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Datas(&rchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Datas(&shuffle, sizeof(int), 1, in);

            // Writing part sequence data
            for(int tps_pos = 0; tps_pos < 256; tps_pos++)
            {
                for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
                {
                    Write_Mod_Datas(&SACTIVE[tps_pos][tps_trk], sizeof(bool), 1, in);
                }
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Datas(&TRACKMIDICHANNEL[twrite], sizeof(int), 1, in);
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Datas(&LFO_ON[twrite], sizeof(char), 1, in);
                Write_Mod_Datas(&LFO_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&LFO_AMPL[twrite], sizeof(float), 1, in);
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Datas(&FLANGER_ON[twrite], sizeof(char), 1, in);
                Write_Mod_Datas(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                Write_Mod_Datas(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
            }  

            // Was a bug
            Write_Mod_Datas(&FLANGER_DEPHASE, sizeof(float), 1, in);

            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Write_Mod_Datas(&TRACKSTATE[tps_trk], sizeof(int), 1, in);
            }
            Write_Mod_Datas(&Songtracks, sizeof(char), 1, in);

            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Write_Mod_Datas(&Disclap[tps_trk], sizeof(bool), 1, in);
                // Was dispan
                Write_Mod_Datas(&fake_value, sizeof(bool), 1, in);
            }

            rtrim_string(artist, 20);
            Write_Mod_Datas(artist, sizeof(char), 20, in);
            rtrim_string(style, 20);
            Write_Mod_Datas(style, sizeof(char), 20, in);

            Write_Mod_Datas(&Old_Phony_Value, sizeof(char), 1, in);

            Write_Mod_Datas(beatsync, sizeof(bool), 128, in);
            Write_Mod_Datas(beatlines, sizeof(short), 128, in);
            Write_Mod_Datas(&REVERBFILTER, sizeof(float), 1, in);
            Write_Mod_Datas(CustomVol, sizeof(float), 128, in);
            Write_Mod_Datas(&Old_Phony_Value, sizeof(char), 1, in);

            // Include the patterns names
            for(i = 0; i < 32; i++)
            {
                rtrim_string(tb303[0].pattern_name[i], 20);
                rtrim_string(tb303[1].pattern_name[i], 20);
            }
            Write_Mod_Datas(&tb303[0], sizeof(para303), 1, in);
            Write_Mod_Datas(&tb303[1], sizeof(para303), 1, in);

            Write_Mod_Datas(&tb303engine[0].tbVolume, sizeof(float), 1, in);
            Write_Mod_Datas(&tb303engine[1].tbVolume, sizeof(float), 1, in);
            Write_Mod_Datas(&tb303engine[0].hpf, sizeof(bool), 1, in);
            Write_Mod_Datas(&tb303engine[1].hpf, sizeof(bool), 1, in);
        }

        if(!Simulate)
        {
            fclose(in);
            last_index = -1;
            Read_SMPT();
            ltActualize(0);

            if(Ok_Memory)
            {
                char name[128];
                if(NewFormat)
                {
                    sprintf(name, "Module '%s.ptp' saved succesfully...", FileName);
                }
                else
                {
                    sprintf(name, "Module '%s.ptk' saved succesfully...", FileName);
                }
                mess_box(name);
            }
        }
        if(!Ok_Memory) mess_box("Not enough memory...");
    }
    else
    {
        if(!Simulate) mess_box("Module save failed...");   
    }

    if(!Simulate)
    {
        if(snamesel == 1 || snamesel == 4 || snamesel == 5)
        {
            snamesel = 0;
            Actualize_DiskIO_Ed();
        }
    }
    return(Mod_Length);
}

// ------------------------------------------------------
// Load a synth preset
void LoadSynth(char *FileName)
{
    FILE *in;
    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        if(strcmp(extension, "TWNNSYN0") == 0)
        {
            /* Ok, extension matched! */
            mess_box("Loading Synthesizer -> structure.");  
            ResetSynthParameters(&PARASynth[ped_patsam]);

            PARASynth[ped_patsam].disto = 0;

            PARASynth[ped_patsam].lfo1_attack = 0;
            PARASynth[ped_patsam].lfo1_decay = 0;
            PARASynth[ped_patsam].lfo1_sustain = 128;
            PARASynth[ped_patsam].lfo1_release = 0x10000;

            PARASynth[ped_patsam].lfo2_attack = 0;
            PARASynth[ped_patsam].lfo2_decay = 0;
            PARASynth[ped_patsam].lfo2_sustain = 128;
            PARASynth[ped_patsam].lfo2_release = 0x10000;

            fread(&PARASynth[ped_patsam], sizeof(SynthParameters), 1, in);

            if(PARASynth[ped_patsam].ptc_glide < 1) PARASynth[ped_patsam].ptc_glide = 64;
            if(PARASynth[ped_patsam].glb_volume < 1) PARASynth[ped_patsam].glb_volume = 64;

            Synthprg[ped_patsam] = SYNTH_WAVE_CURRENT;
            sprintf(nameins[ped_patsam],PARASynth[ped_patsam].presetname);
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);

            Actualize_Instrument_Ed(0, 0);
            Actualize_DiskIO_Ed();
            Actualize_Patterned();
            mess_box("Synthesizer program loaded ok.");
        }
        else
        {
            mess_box("That file is not a "TITLE" synthesizer program file...");
        }
        fclose(in);
    }
    else
    {
        mess_box("Synthesizer program loading failed. (Probably: file not found)");
    }
}

// ------------------------------------------------------
// Save the current synth preset
void SaveSynth(void)
{
    FILE *in;
    char Temph[96];
    char extension[10];

    sprintf(extension, "TWNNSYN0");
    sprintf (Temph, "Saving '%s.pts' synthesizer program on current directory...", PARASynth[ped_patsam].presetname);
    mess_box(Temph);
    sprintf(Temph, "%s.pts", PARASynth[ped_patsam].presetname);
    in = fopen(Temph, "wb");

    if(in != NULL)
    {
        fwrite(extension, sizeof(char), 9, in);
        fwrite(&PARASynth[ped_patsam].presetname, sizeof(SynthParameters), 1, in);
        fclose(in);
        Read_SMPT();
        last_index = -1;
        ltActualize(0);
        mess_box("Synthesizer program saved succesfully..."); 
    }
    else
    {
        mess_box("Synthesizer program save failed...");
    }

    if(snamesel == 3)
    {
        Gui_Draw_Button_Box(432, 352, 164, 16, PARASynth[ped_patsam].presetname, BUTTON_NORMAL);
        snamesel = 0;
    }
}

// ------------------------------------------------------
// Load an instrument
void LoadInst(char *FileName)
{
    int old_bug;
    int Pack_Scheme = FALSE;
    int new_adsr = FALSE;

    mess_box("Attempting to load an instrument file...");
    //Sleep(1000);
    FILE *in;
    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);
        old_bug = FALSE;
        if(strcmp(extension, "TWNNINS0") == 0)
        {
            old_bug = TRUE;
            goto Read_Inst;
        }
        if(strcmp(extension, "TWNNINS2") == 0)
        {
            Pack_Scheme = TRUE;
            goto Read_Inst;
        }
        if(strcmp(extension, "TWNNINS3") == 0)
        {
            Pack_Scheme = TRUE;
            new_adsr = TRUE;
            goto Read_Inst;
        }
        if(strcmp(extension, "TWNNINS1") == 0)
        {
            Read_Inst:
            KillInst(ped_patsam);
            mess_box("Loading Instrument -> Header"); 
            fread(&nameins[ped_patsam], sizeof(char), 20, in);

            // Reading sample data
            mess_box("Loading Instrument -> Sample data");

            int swrite = ped_patsam;

            fread(&Midiprg[swrite], sizeof(char), 1, in);
            fread(&Synthprg[swrite], sizeof(unsigned char), 1, in);

            PARASynth[swrite].disto = 0;

            PARASynth[swrite].lfo1_attack = 0;
            PARASynth[swrite].lfo1_decay = 0;
            PARASynth[swrite].lfo1_sustain = 128;
            PARASynth[swrite].lfo1_release = 128;

            PARASynth[swrite].lfo2_attack = 0;
            PARASynth[swrite].lfo2_decay = 0;
            PARASynth[swrite].lfo2_sustain = 128;
            PARASynth[swrite].lfo2_release = 128;

            // BEWARE: struct is aligned on 4 bytes !!!
            if(old_bug)
            {
                fread(&PARASynth[swrite], sizeof(SynthParameters) - 4 - 32, 1, in);
            }
            else
            {
                if(new_adsr)
                {
                    fread(&PARASynth[swrite], sizeof(SynthParameters), 1, in);
                }
                else
                {
                    fread(&PARASynth[swrite], sizeof(SynthParameters) - 34, 1, in);
                }
            }

            // Gsm by default
            if(Pack_Scheme) fread(&SampleCompression[swrite], sizeof(char), 1, in);
            else SampleCompression[swrite] = SMP_PACK_GSM;

            for(int slwrite = 0; slwrite < 16; slwrite++)
            {
                fread(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite] != 0)
                {
                    if(old_bug) fread(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                    else fread(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                    fread(&Basenote[swrite][slwrite],sizeof(char), 1, in);
                    fread(&LoopStart[swrite][slwrite], sizeof(long), 1, in);
                    fread(&LoopEnd[swrite][slwrite], sizeof(long), 1, in);
                    fread(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                    fread(&SampleNumSamples[swrite][slwrite], sizeof(long), 1, in);
                    fread(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                    fread(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                    fread(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                    RawSamples[swrite][0][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * 2);

                    fread(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                    fread(&SampleChannels[swrite][slwrite], sizeof(char ), 1, in);

                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * 2);
                        fread(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                    }
                } // Exist Sample
            }
            fclose(in);
            Actualize_Patterned();
            Actualize_Instrument_Ed(2, 0);
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
            mess_box("Instrument loaded ok.");
        }
        else
        {
            mess_box("That file is not a "TITLE" instrument-file...");
        }
    } else {
        mess_box("Instrument loading failed. (Probably: file not found)");
    }
    
    if(snamesel == 1 || snamesel == 4 || snamesel == 5)
    {
        snamesel = 0;
        Actualize_DiskIO_Ed();
    }
}

// ------------------------------------------------------
// Save the current instrument
void SaveInst(void)
{

    FILE *in;
    char Temph[96];
    char extension[10];
    int synth_prg;
    int synth_save;

    sprintf(extension, "TWNNINS3");
    sprintf (Temph, "Saving '%s.pti' instrument on current directory...", nameins[ped_patsam]);
    mess_box(Temph);
    sprintf(Temph, "%s.pti", nameins[ped_patsam]);

    in = fopen(Temph,"wb");
    if(in != NULL)
    {
        // Writing header & name...
        fwrite(extension, sizeof(char), 9, in);
        fwrite(&nameins[ped_patsam], sizeof(char), 20, in);

        // Writing sample data
        int swrite = ped_patsam;
        fwrite(&Midiprg[swrite], sizeof(char), 1, in);
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

        fwrite(&synth_prg, sizeof(unsigned char), 1, in);

        fwrite(&PARASynth[swrite], sizeof(SynthParameters), 1, in);

        fwrite(&SampleCompression[swrite], sizeof(char), 1, in);

        swrite = synth_save;
        for(int slwrite = 0; slwrite < 16; slwrite++)
        {
            fwrite(&SampleType[swrite][slwrite], sizeof(char), 1, in);
            if(SampleType[swrite][slwrite] != 0)
            {
                fwrite(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                fwrite(&Basenote[swrite][slwrite],sizeof(char), 1, in);
                fwrite(&LoopStart[swrite][slwrite], sizeof(long), 1, in);
                fwrite(&LoopEnd[swrite][slwrite], sizeof(long), 1, in);
                fwrite(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                fwrite(&SampleNumSamples[swrite][slwrite], sizeof(long), 1, in);
                fwrite(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                fwrite(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                fwrite(&FDecay[swrite][slwrite], sizeof(float), 1, in);
                fwrite(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                fwrite(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                if(SampleChannels[swrite][slwrite] == 2)
                {
                    fwrite(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                }
            } // Exist Sample
        }
        fclose(in);

        Read_SMPT();
        last_index = -1;
        ltActualize(0);
        Actualize_Patterned();
        mess_box("Instrument saved succesfully..."); 
    }
    else
    {
        mess_box("Instrument save failed...");
    }

    if(snamesel == 1 || snamesel == 4 || snamesel == 5) {
        snamesel = 0;
        Actualize_DiskIO_Ed();
    }
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

        if(strcmp(extension, "TWNN3030") == 0)
        {
            // Ok, extension matched!
            mess_box("Loading 303 pattern.");
            fread(&tb303[sl3].patternlength[tb303[sl3].selectedpattern], sizeof(unsigned char), 1, in);
            fread(&tb303[sl3].tone[tb303[sl3].selectedpattern], sizeof(unsigned char), 16, in);
            fread(&tb303[sl3].flag[tb303[sl3].selectedpattern], sizeof(struct flag303), 16, in);
            fread(&tb303[sl3].pattern_name[tb303[sl3].selectedpattern], sizeof(char), 20, in);
            Actualize_303_Ed(0);
            mess_box("303 pattern loaded ok.");
        }
        else
        {
            mess_box("That file is not a "TITLE" 303 pattern file...");
        }
        fclose(in);
    }
    else
    {
        mess_box("303 pattern loading failed. (Probably: file not found)");
    }
}

// ------------------------------------------------------
// Save a 303 pattern 
void Save303(void)
{
    FILE *in;
    char Temph[96];
    char extension[10];

    sprintf(extension, "TWNN3030");
    sprintf (Temph, "Saving '%s.303' pattern on current directory...", tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
    mess_box(Temph);
    sprintf(Temph, "%s.303", tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
    in = fopen(Temph, "wb");

    if(in != NULL)
    {
        fwrite(extension, sizeof(char), 9, in);
        fwrite(&tb303[sl3].patternlength[tb303[sl3].selectedpattern], sizeof(unsigned char), 1, in);
        fwrite(&tb303[sl3].tone[tb303[sl3].selectedpattern], sizeof(unsigned char), 16, in);
        fwrite(&tb303[sl3].flag[tb303[sl3].selectedpattern], sizeof(struct flag303), 16, in);
        fwrite(&tb303[sl3].pattern_name[tb303[sl3].selectedpattern], sizeof(char), 20, in);
        fclose(in);
        Read_SMPT();
        last_index = -1;
        ltActualize(0);
        mess_box("303 pattern saved succesfully...");   
    }
    else
    {
        mess_box("303 pattern save failed...");
    }

    if(snamesel == 6)
    {
        Gui_Draw_Button_Box(600, 472, 164, 16, tb303[sl3].pattern_name[tb303[sl3].selectedpattern], BUTTON_NORMAL);
        snamesel = 0;
    }
}

// ------------------------------------------------------
// Pack a module
Uint8 *Pack_Data(Uint8 *Memory, int *Size)
{
    z_stream c_stream; // compression stream
    int Comp_Len;
    Uint8 *Final_Mem_Out = (Uint8 *) malloc(*Size);

    memset(&c_stream, 0, sizeof(c_stream));
    deflateInit(&c_stream, Z_BEST_COMPRESSION);
    c_stream.next_in  = (Bytef*) Memory;
    c_stream.next_out = Final_Mem_Out;
    c_stream.avail_in = *Size;
    c_stream.avail_out = (uInt) &Comp_Len;
    deflate(&c_stream, Z_FULL_FLUSH);
    deflate(&c_stream, Z_FINISH);
    deflateEnd(&c_stream);
    *Size = c_stream.total_out;
    return(Final_Mem_Out);
}

// ------------------------------------------------------
// Depack a compressed module
Uint8 *Depack_Data(Uint8 *Memory, int Size)
{
    z_stream d_stream;

    Uint8 *Test_Mem = (Uint8 *) malloc(Size * 10);
    if(Test_Mem)
    {
        memset(&d_stream, 0, sizeof(d_stream));
        d_stream.next_in = (Uint8 *) Memory;
        d_stream.avail_in = Size;
        d_stream.next_out = Test_Mem;
        d_stream.avail_out = 0xffffffff;
        inflateInit(&d_stream);
        inflate(&d_stream, Z_FULL_FLUSH);
        inflateEnd(&d_stream);
    }
    return(Test_Mem);
}

// ------------------------------------------------------
// Save a packed .ptk module
int Pack_Module(char *FileName)
{
    FILE *output;
    char name[128];
    char extension[10];
    char Temph[96];
    Uint8 *Final_Mem_Out;

    sprintf(Temph, "%s.ptk", FileName);

    int Len = SaveMod("", FALSE, SAVE_CALCLEN, NULL);

    Uint8 *Final_Mem = (Uint8 *) malloc(Len);
    SaveMod(FileName, FALSE, SAVE_WRITEMEM, Final_Mem);

    Final_Mem_Out = Pack_Data(Final_Mem, &Len);

    output = fopen(Temph, "wb");
    if(output)
    {
        sprintf(extension, "TWNNSNG9");
        fwrite(extension, sizeof(unsigned char), 9, output);
        fwrite(Final_Mem_Out, sizeof(unsigned char), Len, output);
        fclose(output);
        sprintf(name, "Module '%s.ptk' saved succesfully...", FileName);
    }
    else
    {
        sprintf(name, "Module save failed...");
    }
    if(Final_Mem_Out) free(Final_Mem_Out);
    if(Final_Mem) free(Final_Mem);

    if(snamesel == 1 || snamesel == 4 || snamesel == 5)
    {
        snamesel = 0;
        Actualize_DiskIO_Ed();
    }
    mess_box(name);
    Read_SMPT();
    last_index = -1;
    ltActualize(0);
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
    FILE *in;
    char extension[10];
    char FileName[MAX_PATH];
    int i;
    int Real_Palette_Idx;
    char KeyboardName[MAX_PATH];

    sprintf(extension, "TWNNCFG1");
    mess_box("Saving 'ptk.cfg' on current directory...");

#ifdef __WIN32__
    sprintf(FileName, "%s\\ptk.cfg", ExePath);
#else
    sprintf(FileName, "%s/ptk.cfg", ExePath);
#endif

    memset(KeyboardName, 0, sizeof(KeyboardName));
    sprintf(KeyboardName, "%s", Keyboard_Name);

    in = fopen(FileName, "wb");
    if(in != NULL)
    {
        fwrite(extension, sizeof(char), 9, in);
        fwrite(&ped_pattad, sizeof(ped_pattad), 1, in);
        fwrite(&patt_highlight, sizeof(patt_highlight), 1, in);
        fwrite(&AUDIO_Milliseconds, sizeof(AUDIO_Milliseconds), 1, in);
        fwrite(&c_midiin, sizeof(c_midiin), 1, in);
        fwrite(&c_midiout, sizeof(c_midiout), 1, in);
        fwrite(&MouseWheel_Multiplier, sizeof(MouseWheel_Multiplier), 1, in);
        fwrite(&Rows_Decimal, sizeof(Rows_Decimal), 1, in);
        fwrite(&FullScreen, sizeof(FullScreen), 1, in);

        for(i = 0; i < NUMBER_COLORS; i++)
        {
            Real_Palette_Idx = Idx_Palette[i];
            fwrite(&Palette[Real_Palette_Idx].r, sizeof(char), 1, in);
            fwrite(&Palette[Real_Palette_Idx].g, sizeof(char), 1, in);
            fwrite(&Palette[Real_Palette_Idx].b, sizeof(char), 1, in);
        }
        fwrite(&See_Prev_Next_Pattern, sizeof(See_Prev_Next_Pattern), 1, in);
        fwrite(&Beveled, sizeof(Beveled), 1, in);
        fwrite(&Continuous_Scroll, sizeof(Continuous_Scroll), 1, in);
        fwrite(&AutoSave, sizeof(AutoSave), 1, in);
        fwrite(&Dir_Mods, sizeof(Dir_Mods), 1, in);
        fwrite(&Dir_Instrs, sizeof(Dir_Instrs), 1, in);
        fwrite(&Dir_Presets, sizeof(Dir_Presets), 1, in);
        fwrite(KeyboardName, MAX_PATH, 1, in);
        fclose(in);

        Read_SMPT();
        last_index = -1;
        ltActualize(0);
        mess_box("Configuration file saved succesfully...");  
    }
    else
    {
        mess_box("Configuration file save failed...");
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

#ifdef __WIN32__
    sprintf(FileName, "%s\\ptk.cfg", ExePath);
#else
    sprintf(FileName, "%s/ptk.cfg", ExePath);
#endif

    memset(KeyboardName, 0, sizeof(KeyboardName));

    in = fopen(FileName, "rb");
    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];

        fread(extension, sizeof(char), 9, in);
        if(strcmp(extension, "TWNNCFG1") == 0)
        {
            fread(&ped_pattad, sizeof(ped_pattad), 1, in);
            fread(&patt_highlight, sizeof(patt_highlight), 1, in);
            fread(&AUDIO_Milliseconds, sizeof(AUDIO_Milliseconds), 1, in);
            fread(&c_midiin, sizeof(c_midiin), 1, in);
            fread(&c_midiout, sizeof(c_midiout), 1, in);
            fread(&MouseWheel_Multiplier, sizeof(MouseWheel_Multiplier), 1, in);
            fread(&Rows_Decimal, sizeof(Rows_Decimal), 1, in);
            fread(&FullScreen, sizeof(FullScreen), 1, in);

            for(i = 0; i < NUMBER_COLORS; i++)
            {
                Real_Palette_Idx = Idx_Palette[i];
                fread(&Palette[Real_Palette_Idx].r, sizeof(char), 1, in);
                fread(&Palette[Real_Palette_Idx].g, sizeof(char), 1, in);
                fread(&Palette[Real_Palette_Idx].b, sizeof(char), 1, in);
                Palette[Real_Palette_Idx].unused = 0;
            }
            fread(&See_Prev_Next_Pattern, sizeof(See_Prev_Next_Pattern), 1, in);
            fread(&Beveled, sizeof(Beveled), 1, in);
            fread(&Continuous_Scroll, sizeof(Continuous_Scroll), 1, in);
            fread(&AutoSave, sizeof(AutoSave), 1, in);
            fread(&Dir_Mods, sizeof(Dir_Mods), 1, in);
            fread(&Dir_Instrs, sizeof(Dir_Instrs), 1, in);
            fread(&Dir_Presets, sizeof(Dir_Presets), 1, in);
            fread(KeyboardName, MAX_PATH, 1, in);
        }
        fclose(in);
    }
    sprintf(Keyboard_Name, "%s", KeyboardName);

    // Set default dirs if nothing
    if(!strlen(Dir_Mods))
    {
        GETCWD(Dir_Mods, MAX_PATH);

#ifdef __WIN32__
        strcat(Dir_Mods, "\\modules");
#else
        strcat(Dir_Mods, "/modules");
#endif

    }
    if(!strlen(Dir_Instrs))
    {
        GETCWD(Dir_Instrs, MAX_PATH);

#ifdef __WIN32__
        strcat(Dir_Instrs, "\\instruments");
#else
        strcat(Dir_Instrs, "/instruments");
#endif

    }
    if(!strlen(Dir_Presets))
    {
        GETCWD(Dir_Presets, MAX_PATH);

#ifdef __WIN32__
        strcat(Dir_Presets, "\\presets");
#else
        strcat(Dir_Presets, "/presets");
#endif

    }

    cur_dir = Dir_Mods;
}
