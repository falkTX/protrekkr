// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "../extralibs/zlib-1.2.3/zlib.h"

#include "../include/variables.h"
#include "../include/ptk.h"
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
    {   75, 89 },
    {   71, 90 },
    {   67, 91 },
    {   63, 92 },
    {   60, 93 },
    {   56, 94 },
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
int Read_Mod_Data(void *Datas, int Unit, int Length, FILE *Handle);
int Read_Mod_Data_Swap(void *Datas, int Unit, int Length, FILE *Handle);
int Write_Mod_Data(void *Datas, int Unit, int Length, FILE *Handle);
int Write_Mod_Data_Swap(void *Datas, int Unit, int Length, FILE *Handle);
void Pack_Sample(FILE *FileHandle, short *Sample, int Size, char Pack_Type);
short *Unpack_Sample(FILE *FileHandle, int Dest_Length, char Pack_Type);
void Read_Synth_Params(int (*Read_Function)(void *, int ,int, FILE *),
                       int (*Read_Function_Swap)(void *, int ,int, FILE *),
                       FILE *in,
                       int idx,
                       int read_disto,
                       int read_lfo_adsr,
                       int new_version);
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
    //Set_Track_Slider(gui_track);
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
// Scale an amiga protracker .mod effect data
float Convert_AmigaMod_Value(int value, float scale1, float scale2)
{
    float newvalue = (float) value;
    newvalue /= scale1;
    newvalue *= scale2;
    if(newvalue > scale2) newvalue = scale2;
    return(newvalue);
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
    float volume;
    Uint32 x;
    int ramp;
    float ramp_vol;

    SongStop();

    in = fopen(FileName, "rb");
    if(in != NULL) {
        Free_Samples();
        Clear_Patterns_Pool();

#if !defined(__NO_MIDI__)
        Midi_Reset();
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

            SampleNumSamples[swrite][0] = (int) (fgetc(in) << 8) + (int) fgetc(in);
            SampleNumSamples[swrite][0] *= 2;
            fread(&Finetune[swrite][0], sizeof(char), 1, in);
            Finetune[swrite][0] *= 16;
            CustomVol[swrite] = Convert_AmigaMod_Value(fgetc(in), 64.0f, 1.0f);
            SampleVol[swrite][0] = 1.0f;
            Basenote[swrite][0] = 48 - 5 + 12 + 12 + 12 + 12 - 2;

            SampleType[swrite][0] = 0;    // NO SAMPLE

            // Calculate/Adapt AMIGA loop points to ptk LoopPoints:

            LoopStart[swrite][0] = (int) (fgetc(in) << 8) + (int) fgetc(in);
            LoopStart[swrite][0] *= 2;
            long Replen = ((int) (fgetc(in) << 8) + (int) fgetc(in));
            Replen *= 2;

            if(Replen > 2)
            {
                LoopEnd[swrite][0] = LoopStart[swrite][0] + Replen - 2;
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
                    int tmo = Get_Pattern_Offset(pwrite, pw2, li2);
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

                    *(RawPatterns + tmo + PATTERN_NOTE) = t_note;
                    *(RawPatterns + tmo + PATTERN_INSTR) = t_sample;

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
                                    case 1: t_argu = 10; break;
                                    case 2: t_argu = 9; break;
                                    case 3: t_argu = 8; break;
                                    case 4: t_argu = 6; break;
                                    case 5: t_argu = 5; break;
                                    case 6: t_argu = 4; break;
                                    case 7: t_argu = 3; break;
                                    case 8: t_argu = 3; break;
                                    case 9: t_argu = 2; break;
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
                            if(t_argu) t_command = 0x1b;
                            else t_command = 0;
                            break;

                        // PITCH UP
                        case 1:
                            t_argu >>= 1;
                            break;

                        // PITCH UP
                        case 2:
                            t_argu >>= 1;
                            break;

                        // GLIDE
                        case 5:
                            t_argu = 0;
                        case 3:
                            t_command = 5;
                            break;

                        // VIBRATO [Not supported by PTK, ptk wasn't made to sound like a violin :P ].
                        case 4:
                            t_command = 0;
                            t_argu = 0;
                            break;


                        // DEMOSYNCHRO SIGNAL???
                        case 8:
                            t_command = 7;
                            break;

                        // (Only handle the volume slide part of the command)
                        case 6:
                        // Volume Sliders
                        case 0xa:
                            if(t_argu >= 16)
                            {
                                t_command = 25; // Vol SlideUp
                                t_argu = (int) Convert_AmigaMod_Value(t_argu >> 4, 16.0f, 45.0f);
                            }
                            else
                            {
                                t_command = 26; // Vol Slide Down
                                t_argu = (int) Convert_AmigaMod_Value(t_argu & 0xf, 16.0f, 45.0f);
                            }
                            break;

                        // VOLUME
                        case 0xc:
                            t_command = 3;
                            t_argu = (int) Convert_AmigaMod_Value(t_argu, 64.0f, 255.0f);
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
                                *(RawPatterns + tmo + PATTERN_VOLUME) = (t_argu & 0xf) | 0xf0;
                                t_command = 0;
                                t_argu = 0;
                            }

                            if(t_argu > 0xd0 && t_argu < 0xf0)
                            {
                                t_command = 0;
                                t_argu = 0;
                            }
                            break;

                    } // Pattern FX adapter end.

                    *(RawPatterns + tmo + PATTERN_FX) = t_command;
                    *(RawPatterns + tmo + PATTERN_FXDATA) = t_argu;
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
                RawSamples[swrite][0][0] = (short *) malloc((SampleNumSamples[swrite][0] * sizeof(short)));
                for(x = 0; x < SampleNumSamples[swrite][0]; x++)
                {
                    *(RawSamples[swrite][0][0] + x) = (short) fgetc(in) << 8;
                }
                *(RawSamples[swrite][0][0]) = 0;

#define MOD_RAMP_SIZE 40

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
                }
                else
                {
                    if(LoopStart[swrite][0] >= MOD_RAMP_SIZE)
                    {
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
                    }
                }
            }
            else
            {
                // NO SAMPLE
                SampleType[swrite][0] = 0;
            }
        }

        TPan[0] = 0.58f;
        TPan[1] = 0.42f;
        TPan[2] = 0.34f;
        TPan[3] = 0.66f;
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
// TODO: split this one in 2
void LoadMod(char *FileName)
{
    int Ye_Old_Phony_Value;
    int Ptk_Format = FALSE;
    int New_adsr = FALSE;
    int New_Comp = FALSE;
    int Portable = FALSE;
    int Poly = FALSE;
    int Multi = FALSE;

    char Comp_Flag;

    int i;
    int j;
    int k;
    int Old_Ntk;
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
    int UnPacked_Size;
    unsigned char *Packed_Module = NULL;

    Mod_Simulate = LOAD_READ;
    Mod_Mem_Pos = 0;
    Mod_Memory = NULL;

    SongStop();
    mess_box("Attempting to load the song file...");

    FILE *in;
    in = fopen(FileName, "rb");
    Old_Ntk = FALSE;

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        Read_Data(extension, sizeof(char), 9, in);

        switch(extension[7])
        {
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
                Ptk_Format = TRUE;
                goto Read_Mod_File;

            // Old noisetrekker
            case '2':
                Old_Ntk = TRUE;
                Ptk_Format = TRUE;
                goto Read_Mod_File;
        }

        // Read a possible packed module ID
        unsigned int New_Extension;
        fseek(in, 0, SEEK_SET);
        // (No swapping here to make sure the modules are endianness dependant)
        Read_Data(&New_Extension, sizeof(int), 1, in);

        if(New_Extension == 'KTRP')
        {
            Ptk_Format = FALSE;
            New_Comp = TRUE;

Read_Mod_File:

            mess_box("Loading song -> Header");
            Free_Samples();

            mas_comp_threshold = 100.0f;
            mas_comp_ratio = 0.0f;

            allow_save = Ptk_Format;

            Clear_Patterns_Pool();

#if !defined(__NO_MIDI__)
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

            if(Ptk_Format) Read_Mod_Data(FileName, sizeof(char), 20, in);
            else memset(FileName, 0, 20);
            Read_Mod_Data(&nPatterns, sizeof(char), 1, in);

            Songtracks = MAX_TRACKS;
            if(!Ptk_Format) Read_Mod_Data(&Songtracks, sizeof(char), 1, in);
            Read_Mod_Data(&sLength, sizeof(char), 1, in);

            if(Ptk_Format)
            {
                Read_Mod_Data(pSequence, sizeof(char), 256, in);
            }
            else
            {
                Read_Mod_Data(pSequence, sizeof(char), sLength, in);
            }

            Clear_Patterns_Pool();

            // Load the patterns rows infos
            if(Ptk_Format)
            {
                for(i = 0; i < PATTERN_MAX_ROWS; i++)
                {
                    Read_Mod_Data_Swap(&patternLines[i], sizeof(short), 1, in);
                }

                // Multi notes
                if(Multi)
                {
                    Read_Mod_Data(Channels_MultiNotes, sizeof(char), MAX_TRACKS, in);
                }
            }
            else
            {
                for(i = 0; i < PATTERN_MAX_ROWS; i++)
                {
                    patternLines[i] = 64;
                }
                for(i = 0; i < nPatterns; i++)
                {
                    patternLines[i] = 0;
                    Read_Mod_Data(&patternLines[i], sizeof(char), 1, in);
                    patternLines[i] = Swap_16(patternLines[i]);
                }

                // Multi notes
                Read_Mod_Data(Channels_MultiNotes, sizeof(char), Songtracks, in);
            }

            // Load the patterns data
            if(Ptk_Format)
            {
                for(int pwrite = 0; pwrite < nPatterns; pwrite++)
                {
                    Read_Mod_Data(RawPatterns + pwrite * PATTERN_LEN, sizeof(char), PATTERN_LEN, in);
                }
            }
            else
            {
                TmpPatterns = RawPatterns;
                for(int pwrite = 0; pwrite < nPatterns; pwrite++)
                {
                    TmpPatterns_Rows = TmpPatterns + (pwrite * PATTERN_LEN);
                    for(i = 0; i < PATTERN_BYTES; i++)
                    {
                        // Bytes / track
                        for(k = 0; k < Songtracks; k++)
                        {
                            // Tracks
                            TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                            for(j = 0; j < patternLines[pwrite]; j++)
                            {
                                // Rows
                                TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                                Read_Mod_Data(TmpPatterns_Notes + i, sizeof(char), 1, in);
                            }
                        }
                    }
                }
            }

            mess_box("Loading song -> Sample data");
            if(Ptk_Format)
            {
                for(int swrite = 0; swrite < 128; swrite++)
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

                    Read_Synth_Params(Read_Mod_Data, Read_Mod_Data_Swap, in, swrite, new_disto, New_adsr, Portable);

                    // Fix some very old Ntk bugs
                    if(PARASynth[swrite].lfo1_period > 128) PARASynth[swrite].lfo1_period = 128;
                    if(PARASynth[swrite].lfo2_period > 128) PARASynth[swrite].lfo2_period = 128;
                    if(Old_Ntk)
                    {
                        if(PARASynth[swrite].ptc_glide < 1) PARASynth[swrite].ptc_glide = 64;
                        if(PARASynth[swrite].glb_volume < 1) PARASynth[swrite].glb_volume = 64;
                    }

                    // Compression type
                    if(Pack_Scheme) Read_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);

                    for(int slwrite = 0; slwrite < 16; slwrite++)
                    {
                        Read_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                        if(SampleType[swrite][slwrite] != 0)
                        {
                            if(Old_Bug) Read_Mod_Data(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                            else Read_Mod_Data(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                            Read_Mod_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);

                            Read_Mod_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                            Read_Mod_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                            Read_Mod_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);

                            Read_Mod_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                            Read_Mod_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Data_Swap(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                            Read_Mod_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                            RawSamples[swrite][0][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short));
                            Read_Mod_Data(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                            Swap_Sample(RawSamples[swrite][0][slwrite], swrite, slwrite);
                            *RawSamples[swrite][0][slwrite] = 0;

                            // Stereo flag
                            Read_Mod_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                            if(SampleChannels[swrite][slwrite] == 2)
                            {
                                RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short));
                                Read_Mod_Data(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                                Swap_Sample(RawSamples[swrite][1][slwrite], swrite, slwrite);
                                *RawSamples[swrite][1][slwrite] = 0;
                            }
                        } // Exist Sample
                    }
                }
            }
            else
            {
                // Packed format
                Read_Mod_Data(&nbr_instr, sizeof(int), 1, in);

                for(int swrite = 0; swrite < nbr_instr; swrite++)
                {
                    memset(nameins[swrite], 0, 20);
                    Read_Mod_Data(&Synthprg[swrite], sizeof(char), 1, in);

                    if(Synthprg[swrite])
                    {
                        Read_Mod_Data(&PARASynth[swrite].osc1_waveform, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].osc2_waveform, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].osc1_pw, sizeof(int), 1, in);
                        
                        Read_Mod_Data(&PARASynth[swrite].osc2_pw, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].osc2_detune, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].osc2_finetune, sizeof(char), 1, in);
                        
                        Read_Mod_Data(&PARASynth[swrite].vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].vcf_resonance, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].vcf_type, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].env1_attack, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_decay, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_sustain, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_release, sizeof(int), 1, in);
                        
                        Read_Mod_Data(&PARASynth[swrite].env2_attack, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_decay, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_sustain, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_release, sizeof(int), 1, in);
                        
                        Read_Mod_Data(&PARASynth[swrite].lfo1_period, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_period, sizeof(int), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].lfo1_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_osc2_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_osc2_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_osc2_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].lfo2_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_osc2_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_osc2_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_osc2_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].env1_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_osc2_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_osc2_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_osc2_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env1_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].env2_osc1_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_osc2_pw, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_osc1_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_osc2_pitch, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_osc1_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_osc2_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_vcf_cutoff, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].env2_vcf_resonance, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].osc3_volume, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].osc3_switch, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].ptc_glide, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].glb_volume, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].disto, sizeof(char), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].lfo1_attack, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_decay, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_sustain, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo1_release, sizeof(int), 1, in);

                        Read_Mod_Data(&PARASynth[swrite].lfo2_attack, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_decay, sizeof(int), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_sustain, sizeof(char), 1, in);
                        Read_Mod_Data(&PARASynth[swrite].lfo2_release, sizeof(int), 1, in);
                    }
                    else
                    {
                        memset(&PARASynth[swrite], 0, sizeof(SynthParameters));
                    }

                    // Compression type
                    Read_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);

                    for(int slwrite = 0; slwrite < 16; slwrite++)
                    {
                        Read_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);

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
                            
                            Read_Mod_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                            Read_Mod_Data(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                            Read_Mod_Data(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                            Read_Mod_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                            
                            Read_Mod_Data(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                            Read_Mod_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                            
                            Read_Mod_Data(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                            Read_Mod_Data(&FDecay[swrite][slwrite], sizeof(float), 1, in);
                            
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
                            Read_Mod_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
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

            Set_Default_Channels_Polyphony();

            if(Ptk_Format)
            {
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

                Read_Mod_Data_Swap(&delay_time, sizeof(int), 1, in);
                Read_Mod_Data_Swap(&Feedback, sizeof(float), 1, in);
                Read_Mod_Data_Swap(&DelayType, sizeof(int), 1, in);
                Read_Mod_Data_Swap(&lchorus_delay, sizeof(int), 1, in);
                Read_Mod_Data_Swap(&rchorus_delay, sizeof(int), 1, in);
                Read_Mod_Data_Swap(&lchorus_feedback, sizeof(float), 1, in);
                Read_Mod_Data_Swap(&rchorus_feedback, sizeof(float), 1, in);
                Read_Mod_Data_Swap(&shuffle, sizeof(int), 1, in);

            }
            else
            {
                // Reading Track Properties
                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Data(&TCut[twrite], sizeof(float), 1, in);
                    Read_Mod_Data(&ICut[twrite], sizeof(float), 1, in);
                    if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
                    if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;

                    Read_Mod_Data(&TPan[twrite], sizeof(float), 1, in);
                    ComputeStereo(twrite);
                    Read_Mod_Data(&FType[twrite], sizeof(int), 1, in);
                    Read_Mod_Data(&FRez[twrite], sizeof(int), 1, in);
                    Read_Mod_Data(&DThreshold[twrite], sizeof(float), 1, in);
                    Read_Mod_Data(&DClamp[twrite], sizeof(float), 1, in);
                    Read_Mod_Data(&DSend[twrite], sizeof(float), 1, in);
                    Read_Mod_Data(&CSend[twrite], sizeof(int), 1, in);
                    Read_Mod_Data(&Channels_Polyphony[twrite], sizeof(char), 1, in);
                }

                // Reading mod properties
                Read_Mod_Data(&compressor, sizeof(char), 1, in);
                Read_Mod_Data(&c_threshold, sizeof(int), 1, in);
                Read_Mod_Data(&BeatsPerMin, sizeof(int), 1, in);
                Read_Mod_Data(&TicksPerBeat, sizeof(int), 1, in);
                Read_Mod_Data(&mas_vol, sizeof(float), 1, in);
                if(mas_vol < 0.01f) mas_vol = 0.01f;
                if(mas_vol > 1.0f) mas_vol = 1.0f;

                if(New_Comp)
                {
                    Comp_Flag = 0;
                    Read_Mod_Data(&Comp_Flag, sizeof(char), 1, in);
                    if(Comp_Flag)
                    {
                        Read_Mod_Data(&mas_comp_threshold, sizeof(float), 1, in);
                        if(mas_comp_threshold < 0.01f) mas_comp_threshold = 0.01f;
                        if(mas_comp_threshold > 100.0f) mas_comp_threshold = 100.0f;
                    
                        Read_Mod_Data(&mas_comp_ratio, sizeof(float), 1, in);
                        if(mas_comp_ratio < 0.01f) mas_comp_ratio = 0.01f;
                        if(mas_comp_ratio > 100.0f) mas_comp_ratio = 100.0f;
                    }
                }

                Read_Mod_Data(&delay_time, sizeof(int), 1, in);
                Read_Mod_Data(&Feedback, sizeof(float), 1, in);
                Read_Mod_Data(&DelayType, sizeof(int), 1, in);
                Read_Mod_Data(&lchorus_delay, sizeof(int), 1, in);
                Read_Mod_Data(&rchorus_delay, sizeof(int), 1, in);
                Read_Mod_Data(&lchorus_feedback, sizeof(float), 1, in);
                Read_Mod_Data(&rchorus_feedback, sizeof(float), 1, in);
                Read_Mod_Data(&shuffle, sizeof(int), 1, in);
            }

            // Reading track part sequence

            if(Ptk_Format)
            {
                for(tps_pos = 0; tps_pos < 256; tps_pos++)
                {
                    for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
                    {
                        Read_Mod_Data(&CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char), 1, in);
                        CHAN_HISTORY_STATE[tps_pos][tps_trk] = FALSE;
                    }
                }
            }
            else
            {
                // Reset'em first
                for(tps_pos = 0; tps_pos < 256; tps_pos++)
                {
                    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                    {
                        CHAN_ACTIVE_STATE[tps_pos][tps_trk] = TRUE;
                        CHAN_HISTORY_STATE[tps_pos][tps_trk] = FALSE;
                    }
                }
                for(tps_pos = 0; tps_pos < sLength; tps_pos++)
                {
                    for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                    {
                        Read_Mod_Data(&CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char), 1, in);
                        CHAN_HISTORY_STATE[tps_pos][tps_trk] = FALSE;
                    }
                }
            }

            for(int spl = 0; spl < Songtracks; spl++)
            {
                CCoef[spl] = float((float) CSend[spl] / 127.0f);
            }

            if(Ptk_Format)
            {
                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Data_Swap(&CHAN_MIDI_PRG[twrite], sizeof(int), 1, in);
                }
            }

            if(Ptk_Format)
            {
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
                    foff2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                    foff1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                }
            }
            else
            {
                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Data(&LFO_ON[twrite], sizeof(char), 1, in);
                    if(LFO_ON[twrite])
                    {
                        Read_Mod_Data(&LFO_RATE[twrite], sizeof(float), 1, in);
                        Read_Mod_Data(&LFO_AMPL[twrite], sizeof(float), 1, in);
                    }
                }

                for(twrite = 0; twrite < Songtracks; twrite++)
                {
                    Read_Mod_Data(&FLANGER_ON[twrite], sizeof(char), 1, in);
                    if(FLANGER_ON[twrite])
                    {
                        Read_Mod_Data(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                        Read_Mod_Data(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                        Read_Mod_Data(&FLANGER_RATE[twrite], sizeof(float), 1, in);
                        Read_Mod_Data(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
                        Read_Mod_Data(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                        Read_Mod_Data(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
                        FLANGER_OFFSET[twrite] = 8192;
                        foff2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                        foff1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                    }
                }
            }

            if(Ptk_Format)
            {
                Read_Mod_Data_Swap(&FLANGER_DEPHASE, sizeof(float), 1, in);

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
            }
            else
            {
                for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
                {
                    Read_Mod_Data(&Disclap[tps_trk], sizeof(char), 1, in);
                }
            }

            if(Ptk_Format)
            {
                Read_Mod_Data(artist, sizeof(char), 20, in);
                Read_Mod_Data(style, sizeof(char), 20, in);
            }
            else
            {
                memset(artist, 0, 20);
                memset(style, 0, 20);
            }

            if(!Portable) if(Ptk_Format) Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);
            
            Read_Mod_Data(beatsync, sizeof(char), 128, in);
            
            if(Ptk_Format)
            {
                for(i = 0; i < 128; i++)
                {
                    Read_Mod_Data_Swap(&beatlines[i], sizeof(short), 1, in);
                }

                Read_Mod_Data_Swap(&REVERBFILTER, sizeof(float), 1, in);

                for(i = 0; i < 128; i++)
                {
                    Read_Mod_Data_Swap(&CustomVol[i], sizeof(float), 1, in);
                }
            }
            else
            {
                for(i = 0; i < 128; i++)
                {
                    Read_Mod_Data(&beatlines[i], sizeof(short), 1, in);
                }

                Read_Mod_Data(&REVERBFILTER, sizeof(float), 1, in);

                for(i = 0; i < 128; i++)
                {
                    Read_Mod_Data(&CustomVol[i], sizeof(float), 1, in);
                }
            }

            if(!Portable) if(Ptk_Format) Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);

            // Read the 303 datas
            if(Ptk_Format)
            {
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
            else
            {
                Reset_303_Parameters(&tb303[0]);
                Reset_303_Parameters(&tb303[1]);

                Read_Mod_Data(&tb303[0].enabled, sizeof(char), 1, in);
                if(tb303[0].enabled)
                {
                    Read_Mod_Data(&tb303[0].selectedpattern, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].tune, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].cutoff, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].resonance, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].envmod, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].decay, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].accent, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].waveform, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[0].patternlength, sizeof(char), 32, in);
                    Read_Mod_Data(&tb303[0].tone, sizeof(char), 32 * 16, in);
                    Read_Mod_Data(&tb303[0].flag, sizeof(struct flag303), 32 * 16, in);
                }

                Read_Mod_Data(&tb303[1].enabled, sizeof(char), 1, in);
                if(tb303[1].enabled)
                {
                    Read_Mod_Data(&tb303[1].selectedpattern, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].tune, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].cutoff, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].resonance, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].envmod, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].decay, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].accent, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].waveform, sizeof(char), 1, in);
                    Read_Mod_Data(&tb303[1].patternlength, sizeof(char), 32, in);
                    Read_Mod_Data(&tb303[1].tone, sizeof(char), 32 * 16, in);
                    Read_Mod_Data(&tb303[1].flag, sizeof(struct flag303), 32 * 16, in);
                }
                if(tb303[0].enabled) Read_Mod_Data(&tb303engine[0].tbVolume, sizeof(float), 1, in);
                if(tb303[1].enabled) Read_Mod_Data(&tb303engine[1].tbVolume, sizeof(float), 1, in);
                if(tb303[0].enabled) Read_Mod_Data(&tb303engine[0].hpf, sizeof(char), 1, in);
                if(tb303[1].enabled) Read_Mod_Data(&tb303engine[1].hpf, sizeof(char), 1, in);
            }

            fclose(in);

            // Init the tracker context
            Init_Tracker_Context_After_ModLoad();

            mess_box("Module loaded sucessfully...");
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

#if !defined(__NO_CODEC__)
    short *Dest_Buffer;
#endif

    Uint8 *Packed_Read_Buffer;

    Read_Mod_Data(&Packed_Length, sizeof(int), 1, FileHandle);
    if(Packed_Length == -1)
    {
        // Sample wasn't packed
        Packed_Read_Buffer = (Uint8 *) malloc(Dest_Length * 2);
        Read_Mod_Data(Packed_Read_Buffer, sizeof(char), Dest_Length * 2, FileHandle);
        return((short *) Packed_Read_Buffer);
    }
    else
    {

#if !defined(__NO_CODEC__)
        Packed_Read_Buffer = (Uint8 *) malloc(Packed_Length);
        // Read the packer buffer
        Read_Mod_Data(Packed_Read_Buffer, sizeof(char), Packed_Length, FileHandle);
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
    int PackedLen = 0;
    short *PackedSample = NULL;

#if !defined(__NO_CODEC__)
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
        Write_Mod_Data(&PackedLen, sizeof(char), 4, FileHandle);
        // Write the encoded datas
        Write_Mod_Data(PackedSample, sizeof(char), PackedLen, FileHandle);
    }
    else
    {
#endif
        // Couldn't pack (too small or user do not want that to happens)
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

// ------------------------------------------------------
// module saving related functions
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

    New_RawPatterns = (unsigned char *) malloc(PATTERN_FULL_SIZE);
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
                                if(i == PATTERN_FX)
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
                                    if(i == PATTERN_FX)
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

                Write_Data(&real_fx_nbr, 2, 1, Out_FX);
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
                            }
                        }

                        // Check the effects column
                        if(i == PATTERN_FX)
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
                            case PATTERN_FX:
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
                            case PATTERN_INSTR:
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

    Write_Mod_Data(&nbr_User_Instr, sizeof(int), 1, in);

    // Writing sample data
    for(i = 0; i < 128; i++)
    {
        // Check if it was used at pattern level
        swrite = Used_Instr2[i].old_order;
        if(swrite != -1)
        {
            Write_Mod_Data(&Synthprg[swrite], sizeof(unsigned char), 1, in);

            if(Synthprg[swrite])
            {
                Write_Mod_Data(&PARASynth[swrite].osc1_waveform, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].osc2_waveform, sizeof(char), 1, in);
                if(PARASynth[swrite].osc1_waveform == 5) Store_Instr_Waveform_Osc1 = TRUE;
                if(PARASynth[swrite].osc2_waveform == 5) Store_Instr_Waveform_Osc2 = TRUE;
                if(PARASynth[swrite].osc1_waveform == 3) Store_Synth_WhiteNoise = TRUE;
                if(PARASynth[swrite].osc2_waveform == 3) Store_Synth_WhiteNoise = TRUE;
                if(PARASynth[swrite].osc1_waveform == 6) Store_Synth_PinkNoise = TRUE;
                if(PARASynth[swrite].osc2_waveform == 6) Store_Synth_PinkNoise = TRUE;

                Write_Mod_Data(&PARASynth[swrite].osc1_pw, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].osc2_pw, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].osc2_detune, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].osc2_finetune, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].vcf_resonance, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].vcf_type, sizeof(char), 1, in);
                if(PARASynth[swrite].vcf_type != 2) Store_Synth_Filter = TRUE;

                Write_Mod_Data(&PARASynth[swrite].env1_attack, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_decay, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_sustain, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_release, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_attack, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_decay, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_sustain, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_release, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_period, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_period, sizeof(int), 1, in);

                Write_Mod_Data(&PARASynth[swrite].lfo1_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_osc2_pw, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_osc2_pitch, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_osc2_volume, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].lfo2_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_osc2_pw, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_osc2_pitch, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_osc2_volume, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].env1_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_osc2_pw, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_osc2_pitch, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_osc2_volume, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].env1_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env1_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].env2_osc1_pw, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_osc2_pw, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].env2_osc1_pitch, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_osc2_pitch, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].env2_osc1_volume, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_osc2_volume, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].env2_vcf_cutoff, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].env2_vcf_resonance, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].osc3_volume, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].osc3_switch, sizeof(char), 1, in);
                if(PARASynth[swrite].osc3_switch) Store_Synth_Osc3 = TRUE;

                Write_Mod_Data(&PARASynth[swrite].ptc_glide, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].glb_volume, sizeof(char), 1, in);

                Write_Mod_Data(&PARASynth[swrite].disto, sizeof(char), 1, in);
                if(PARASynth[swrite].disto) Store_Synth_Disto = TRUE;

                Write_Mod_Data(&PARASynth[swrite].lfo1_attack, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_decay, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_sustain, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo1_release, sizeof(int), 1, in);

                Write_Mod_Data(&PARASynth[swrite].lfo2_attack, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_decay, sizeof(int), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_sustain, sizeof(char), 1, in);
                Write_Mod_Data(&PARASynth[swrite].lfo2_release, sizeof(int), 1, in);
            }

#if defined(__NO_CODEC__)
            char No_Comp = SMP_PACK_NONE;
            Write_Mod_Data(&No_Comp, sizeof(char), 1, in);
#else
            Write_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);
#endif

            // Compression type

            // 16 splits / instrument
            for(int slwrite = 0; slwrite < 16; slwrite++)
            {

                Write_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite] != 0)
                {
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
                    Write_Mod_Data(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                    Write_Mod_Data(&FDecay[swrite][slwrite], sizeof(float), 1, in);

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
                    Write_Mod_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
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
                }   // Exist Sample
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
            Write_Mod_Data(&DSend[twrite], sizeof(float), 1, in);
            Write_Mod_Data(&CSend[twrite], sizeof(int), 1, in);
            Write_Mod_Data(&Channels_Polyphony[twrite], sizeof(char), 1, in);
        }
    }

    // Writing mod properties
    Write_Mod_Data(&compressor, sizeof(char), 1, in);
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

    Write_Mod_Data(&delay_time, sizeof(int), 1, in);
    Write_Mod_Data(&Feedback, sizeof(float), 1, in);
    Write_Mod_Data(&DelayType, sizeof(int), 1, in);
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
            Write_Mod_Data(&Disclap[tps_trk], sizeof(char), 1, in);
        }
    }

    Write_Mod_Data(beatsync, sizeof(char), 128, in);
    Write_Mod_Data(beatlines, sizeof(short), 128, in);
    Write_Mod_Data(&REVERBFILTER, sizeof(float), 1, in);
    Write_Mod_Data(CustomVol, sizeof(float), 128, in);

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
        Write_Mod_Data(&tb303[0].patternlength, sizeof(char), 32, in);
        Write_Mod_Data(&tb303[0].tone, sizeof(char), 32 * 16, in);
        Write_Mod_Data(&tb303[0].flag, sizeof(struct flag303), 32 * 16, in);
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
        Write_Mod_Data(&tb303[1].patternlength, sizeof(char), 32, in);
        Write_Mod_Data(&tb303[1].tone, sizeof(char), 32 * 16, in);
        Write_Mod_Data(&tb303[1].flag, sizeof(struct flag303), 32 * 16, in);
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
    char Temph[96];
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
            // .ptp
            Ok_Memory = SaveMod_Ptp(in, Simulate, FileName);
        }
        else
        {
            // .ptk
            if(strlen(FileName)) rtrim_string(FileName, 20);
            Write_Mod_Data(FileName, sizeof(char), 20, in);

            Write_Mod_Data(&nPatterns, sizeof(char), 1, in);
            Write_Mod_Data(&sLength, sizeof(char), 1, in);
            Write_Mod_Data(pSequence, sizeof(char), MAX_SEQUENCES, in);

            Swap_Short_Buffer(patternLines, PATTERN_MAX_ROWS);
            Write_Mod_Data(patternLines, sizeof(short), PATTERN_MAX_ROWS, in);
            Swap_Short_Buffer(patternLines, PATTERN_MAX_ROWS);

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
                        cur_pattern[PATTERN_NOTE] = 121;
                        cur_pattern[PATTERN_INSTR] = 255;
                        cur_pattern[PATTERN_VOLUME] = 255;
                        cur_pattern[PATTERN_PANNING] = 255;
                        cur_pattern[PATTERN_FX] = 0;
                        cur_pattern[PATTERN_FXDATA] = 0;
                        // Next line
                        cur_pattern += PATTERN_ROW_LEN;
                    }
                }
            }

            for(int pwrite = 0; pwrite < nPatterns; pwrite++)
            {
                Write_Mod_Data(RawPatterns + (pwrite * PATTERN_LEN), sizeof(char), PATTERN_LEN, in);
            }

            // Writing sample data
            for(int swrite = 0; swrite < MAX_INSTRS; swrite++)
            {
                rtrim_string(nameins[swrite], 20);
                Write_Mod_Data(&nameins[swrite], sizeof(char), 20, in);
                Write_Mod_Data(&Midiprg[swrite], sizeof(char), 1, in);
                Write_Mod_Data(&Synthprg[swrite], sizeof(char), 1, in);

                Write_Synth_Params(&Write_Mod_Data, &Write_Mod_Data_Swap, in, swrite);

                // Compression type
                Write_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);

                // 16 splits / instrument
                for(int slwrite = 0; slwrite < 16; slwrite++)
                {
                    Write_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleType[swrite][slwrite] != 0)
                    {
                        rtrim_string(SampleName[swrite][slwrite], 64);
                        Write_Mod_Data(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                        Write_Mod_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                        
                        Write_Mod_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                        
                        Write_Mod_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                        Write_Mod_Data_Swap(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                        Write_Mod_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                        // All samples are converted into 16 bits
                        Write_Unpacked_Sample(Write_Mod_Data, in, swrite, slwrite);

                    }// Exist Sample
                }
            }

            // Writing Track Propertiers
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

            Write_Mod_Data_Swap(&delay_time, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&Feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&DelayType, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&lchorus_delay, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&rchorus_delay, sizeof(int), 1, in);
            Write_Mod_Data_Swap(&lchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&rchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(&shuffle, sizeof(int), 1, in);

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
            Write_Mod_Data_Swap(&FLANGER_DEPHASE, sizeof(float), 1, in);

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
                Write_Mod_Data_Swap(&CustomVol[i], sizeof(float), 1, in);
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
            ltActualize(0);

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
                mess_box(name);
            }
        }
        if(!Ok_Memory) mess_box("Not enough memory.");
    }
    else
    {
        if(!Simulate) mess_box("Module save failed.");   
    }

    if(!Simulate)
    {
        if(snamesel == 1 || snamesel == 4 || snamesel == 5)
        {
            snamesel = 0;
            Actualize_DiskIO_Ed(0);
        }
    }
    return(Mod_Length);
}

// ------------------------------------------------------
// Load a synth preset
void LoadSynth(char *FileName)
{
    FILE *in;
    int new_version = FALSE;

    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        if(strcmp(extension, "TWNNSYN1") == 0)
        {
            new_version = TRUE;
            goto Read_Synth;
        }

        if(strcmp(extension, "TWNNSYN0") == 0)
        {
Read_Synth:
            // Ok, extension matched!
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

            Read_Synth_Params(Read_Data, Read_Data_Swap, in, ped_patsam, TRUE, TRUE, new_version);

            // Fix some old Ntk bugs
            if(PARASynth[ped_patsam].lfo1_period > 128) PARASynth[ped_patsam].lfo1_period = 128;
            if(PARASynth[ped_patsam].lfo2_period > 128) PARASynth[ped_patsam].lfo2_period = 128;

            Synthprg[ped_patsam] = SYNTH_WAVE_CURRENT;
            sprintf(nameins[ped_patsam],PARASynth[ped_patsam].presetname);
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);

            Actualize_Instrument_Ed(0, 0);
            Actualize_DiskIO_Ed(0);
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

    sprintf(extension, "TWNNSYN1");
    sprintf (Temph, "Saving '%s.pts' synthesizer program on current directory...", PARASynth[ped_patsam].presetname);
    mess_box(Temph);
    sprintf(Temph, "%s.pts", PARASynth[ped_patsam].presetname);
    in = fopen(Temph, "wb");

    if(in != NULL)
    {
        Write_Data(extension, sizeof(char), 9, in);
        Write_Synth_Params(&Write_Data, &Write_Data_Swap, in, ped_patsam);
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
    int old_bug = FALSE;
    int Pack_Scheme = FALSE;
    int new_adsr = FALSE;
    int tight = FALSE;

    mess_box("Attempting to load an instrument file...");
    //Sleep(1000);
    FILE *in;
    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        Read_Data(extension, sizeof(char), 9, in);
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
        if(strcmp(extension, "TWNNINS4") == 0)
        {
            Pack_Scheme = TRUE;
            new_adsr = TRUE;
            tight = TRUE;
            goto Read_Inst;
        }
        if(strcmp(extension, "TWNNINS1") == 0)
        {
Read_Inst:
            KillInst(ped_patsam);
            mess_box("Loading Instrument -> Header"); 
            Read_Data(&nameins[ped_patsam], sizeof(char), 20, in);

            // Reading sample data
            mess_box("Loading Instrument -> Sample data");

            int swrite = ped_patsam;

            Read_Data(&Midiprg[swrite], sizeof(char), 1, in);
            Read_Data(&Synthprg[swrite], sizeof(char), 1, in);

            PARASynth[swrite].disto = 0;

            PARASynth[swrite].lfo1_attack = 0;
            PARASynth[swrite].lfo1_decay = 0;
            PARASynth[swrite].lfo1_sustain = 128;
            PARASynth[swrite].lfo1_release = 128;

            PARASynth[swrite].lfo2_attack = 0;
            PARASynth[swrite].lfo2_decay = 0;
            PARASynth[swrite].lfo2_sustain = 128;
            PARASynth[swrite].lfo2_release = 128;

            Read_Synth_Params(Read_Data, Read_Data_Swap, in, swrite, !old_bug, new_adsr, tight);

            // Gsm by default
            if(Pack_Scheme) Read_Data(&SampleCompression[swrite], sizeof(char), 1, in);
            else SampleCompression[swrite] = SMP_PACK_GSM;

            for(int slwrite = 0; slwrite < 16; slwrite++)
            {
                Read_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite] != 0)
                {
                    if(old_bug) Read_Data(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                    else Read_Data(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                    Read_Data(&Basenote[swrite][slwrite],sizeof(char), 1, in);
                    
                    Read_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                    Read_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                    Read_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                    
                    Read_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                    Read_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                    Read_Data_Swap(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                    Read_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                    RawSamples[swrite][0][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short));

                    // Samples data
                    Read_Data(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                    Swap_Sample(RawSamples[swrite][0][slwrite], swrite, slwrite);
                    *RawSamples[swrite][0][slwrite] = 0;

                    // Number of channel(s)
                    Read_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short));
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
        Actualize_DiskIO_Ed(0);
    }
}

// ------------------------------------------------------
// Save the current instrument
void SaveInst(void)
{
    FILE *in;
    char Temph[96];
    char extension[10];
    char synth_prg;
    int synth_save;

    sprintf(extension, "TWNNINS4");
    sprintf (Temph, "Saving '%s.pti' instrument on current directory...", nameins[ped_patsam]);
    mess_box(Temph);
    sprintf(Temph, "%s.pti", nameins[ped_patsam]);

    in = fopen(Temph,"wb");
    if(in != NULL)
    {
        // Writing header & name...
        Write_Data(extension, sizeof(char), 9, in);
        Write_Data(&nameins[ped_patsam], sizeof(char), 20, in);

        // Writing sample data
        int swrite = ped_patsam;

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

        Write_Synth_Params(&Write_Data, &Write_Data_Swap, in, swrite);

        Write_Data(&SampleCompression[swrite], sizeof(char), 1, in);

        swrite = synth_save;
        for(int slwrite = 0; slwrite < 16; slwrite++)
        {
            Write_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
            if(SampleType[swrite][slwrite] != 0)
            {
                Write_Data(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                Write_Data(&Basenote[swrite][slwrite],sizeof(char), 1, in);
                Write_Data_Swap(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                Write_Data_Swap(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                Write_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                Write_Data_Swap(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                Write_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                Write_Data_Swap(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                Write_Data_Swap(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                Write_Unpacked_Sample(Write_Data, in, swrite, slwrite);

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
        Actualize_DiskIO_Ed(0);
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

        if(strcmp(extension, "TWNN3031") == 0)
        {
            // Ok, extension matched!
            mess_box("Loading 303 pattern.");

            Load_303_Data(Read_Data, Read_Data_Swap, in, sl3, tb303[sl3].selectedpattern);

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

    sprintf(extension, "TWNN3031");
    sprintf (Temph, "Saving '%s.303' pattern on current directory...", tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
    mess_box(Temph);
    sprintf(Temph, "%s.303", tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
    in = fopen(Temph, "wb");

    if(in != NULL)
    {
        Write_Data(extension, sizeof(char), 9, in);

        Save_303_Data(Write_Data, Write_Data_Swap, in, sl3, tb303[sl3].selectedpattern);

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
Uint8 *Depack_Data(Uint8 *Memory, int Sizen, int Size_Out)
{
    z_stream d_stream;

    Uint8 *Test_Mem = (Uint8 *) malloc(Size_Out);
    if(Test_Mem)
    {
        memset(&d_stream, 0, sizeof(d_stream));
        d_stream.next_in = (Uint8 *) Memory;
        d_stream.avail_in = Size_Out;
        d_stream.next_out = Test_Mem;
        d_stream.avail_out = -1;
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
    int Depack_Size;

    sprintf(Temph, "%s.ptk", FileName);

    int Len = SaveMod("", FALSE, SAVE_CALCLEN, NULL);

    Depack_Size = Len;

    Uint8 *Final_Mem = (Uint8 *) malloc(Len);
    SaveMod(FileName, FALSE, SAVE_WRITEMEM, Final_Mem);

    Final_Mem_Out = Pack_Data(Final_Mem, &Len);

    output = fopen(Temph, "wb");
    if(output)
    {
        sprintf(extension, "TWNNSNGC");
        Write_Data(extension, sizeof(char), 9, output);
        Write_Data_Swap(&Depack_Size, sizeof(int), 1, output);
        Write_Data(Final_Mem_Out, sizeof(char), Len, output);
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
        Actualize_DiskIO_Ed(0);
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
    signed char phony = -1;

    sprintf(extension, "TWNNCFG2");
    mess_box("Saving 'ptk.cfg' on current directory...");

#if defined(__WIN32__)
    sprintf(FileName, "%s\\ptk.cfg", ExePath);
#else
    sprintf(FileName, "%s/ptk.cfg", ExePath);
#endif

    memset(KeyboardName, 0, sizeof(KeyboardName));
    sprintf(KeyboardName, "%s", Keyboard_Name);

    in = fopen(FileName, "wb");
    if(in != NULL)
    {
        Write_Data(extension, sizeof(char), 9, in);
        Write_Data_Swap(&ped_pattad, sizeof(ped_pattad), 1, in);
        Write_Data_Swap(&patt_highlight, sizeof(patt_highlight), 1, in);
        Write_Data_Swap(&AUDIO_Milliseconds, sizeof(AUDIO_Milliseconds), 1, in);

#if defined(__NO_MIDI__)
        Write_Data(&phony, sizeof(phony), 1, in);
#else
        Write_Data(&c_midiin, sizeof(c_midiin), 1, in);
#endif

#if defined(__NO_MIDI__)
        Write_Data(&phony, sizeof(phony), 1, in);
#else
        Write_Data(&c_midiout, sizeof(c_midiout), 1, in);
#endif

        Write_Data_Swap(&MouseWheel_Multiplier, sizeof(MouseWheel_Multiplier), 1, in);
        Write_Data(&Rows_Decimal, sizeof(Rows_Decimal), 1, in);
        Write_Data(&FullScreen, sizeof(FullScreen), 1, in);

        for(i = 0; i < NUMBER_COLORS; i++)
        {
            Real_Palette_Idx = Idx_Palette[i];
            Write_Data(&Ptk_Palette[Real_Palette_Idx].r, sizeof(char), 1, in);
            Write_Data(&Ptk_Palette[Real_Palette_Idx].g, sizeof(char), 1, in);
            Write_Data(&Ptk_Palette[Real_Palette_Idx].b, sizeof(char), 1, in);
        }
        Write_Data(&See_Prev_Next_Pattern, sizeof(See_Prev_Next_Pattern), 1, in);
        Write_Data_Swap(&Beveled, sizeof(Beveled), 1, in);
        Write_Data_Swap(&Continuous_Scroll, sizeof(Continuous_Scroll), 1, in);
        Write_Data(&AutoSave, sizeof(AutoSave), 1, in);
        
        Write_Data(&Dir_Mods, sizeof(Dir_Mods), 1, in);
        Write_Data(&Dir_Instrs, sizeof(Dir_Instrs), 1, in);
        Write_Data(&Dir_Presets, sizeof(Dir_Presets), 1, in);
        Write_Data(KeyboardName, MAX_PATH, 1, in);

        Write_Data(&rawrender_32float, sizeof(char), 1, in);

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
    signed char phony = -1;

#if defined(__WIN32__)
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

        Read_Data(extension, sizeof(char), 9, in);
        if(strcmp(extension, "TWNNCFG2") == 0)
        {
            Read_Data_Swap(&ped_pattad, sizeof(ped_pattad), 1, in);
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
            Read_Data(KeyboardName, MAX_PATH, 1, in);

            Read_Data(&rawrender_32float, sizeof(char), 1, in);
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

    cur_dir = Dir_Mods;
}

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
    short *new_buffer = (short *) malloc(SampleNumSamples[sample][bank] * sizeof(short));
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

    swap_buffer = Swap_New_Sample(RawSamples[sample][0][bank], sample, bank);
    if(swap_buffer)
    {
        Write_Function(swap_buffer, sizeof(short), SampleNumSamples[sample][bank], in);
        free(swap_buffer);
    }
    else
    {
        Write_Function(RawSamples[sample][0][bank], sizeof(short), SampleNumSamples[sample][bank], in);
    }

    Write_Function(&SampleChannels[sample][bank], sizeof(char), 1, in);
    if(SampleChannels[sample][bank] == 2)
    {
        swap_buffer = Swap_New_Sample(RawSamples[sample][1][bank], sample, bank);
        if(swap_buffer)
        {
            Write_Function(swap_buffer, sizeof(short), SampleNumSamples[sample][bank], in);
            free(swap_buffer);
        }
        else
        {
            Write_Function(RawSamples[sample][1][bank], sizeof(short), SampleNumSamples[sample][bank], in);
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
                       int new_version)
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
                Read_Function(&PARASynth[idx], sizeof(SynthParameters) - 4 - 32, 1, in);
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
