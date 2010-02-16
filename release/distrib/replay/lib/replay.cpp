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
#include "include/replay.h"
#include "include/endianness.h"

#if defined(__PSP__)
#include <stdlib.h>
#include <string.h>
#endif

#if defined(__AROS__)
#define int32 int32_t
#endif

#if !defined(__STAND_ALONE__)
#include "../../../../src/midi/include/midi.h"
#include "../../../../src/include/variables.h"
#endif

// ------------------------------------------------------
// Variables
int SamplesPerTick;

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    int BeatsPerMin = 125;
    int TicksPerBeat = 4;
    float Feedback = 0.6f;
    int c_threshold = 32;
#else
    int BeatsPerMin;
    int TicksPerBeat;
    float Feedback;
    int c_threshold;
#endif

#if defined(PTK_FX_AUTOFADEMODE)
    char FADEMODE[MAX_TRACKS];    // 0 - Off, 1- In, 2 - Out;
    float FADECOEF[MAX_TRACKS];
#endif

#if defined(PTK_SYNTH)
CSynth Synthesizer[MAX_TRACKS][MAX_POLYPHONY];
#endif

float Player_FD[MAX_TRACKS];
char sp_channelsample[MAX_TRACKS][MAX_POLYPHONY];
char sp_split[MAX_TRACKS][MAX_POLYPHONY];
int CHAN_MUTE_STATE[MAX_TRACKS];   // 0->Normal 1->Muted
float oldspawn[MAX_TRACKS];
float roldspawn[MAX_TRACKS];
char LFO_ON[MAX_TRACKS];

#if defined(PTK_LFO)
    float LFO_RATE[MAX_TRACKS];
    float LFO_AMPL[MAX_TRACKS];
    float LFOGR[MAX_TRACKS];
#endif

char FLANGER_ON[MAX_TRACKS];

#if defined(PTK_FLANGER)
    float FLANGER_AMOUNT[MAX_TRACKS];
    float FLANGER_DEPHASE[MAX_TRACKS];
    float FLANGER_RATE[MAX_TRACKS];
    float FLANGER_AMPL[MAX_TRACKS];
    float FLANGER_GR[MAX_TRACKS];
    float FLANGER_FEEDBACK[MAX_TRACKS];
    int FLANGER_DELAY[MAX_TRACKS];
    int FLANGER_OFFSET[MAX_TRACKS];
    float foff2[MAX_TRACKS];
    float foff1[MAX_TRACKS];
    float FLANGE_LEFTBUFFER[MAX_TRACKS][16400];
    float FLANGE_RIGHTBUFFER[MAX_TRACKS][16400];
#endif

int Done_CVol[MAX_TRACKS][MAX_POLYPHONY];
float sp_Cvol[MAX_TRACKS][MAX_POLYPHONY];
float sp_Tvol[MAX_TRACKS][MAX_POLYPHONY];
float DSend[MAX_TRACKS];   
int CSend[MAX_TRACKS];
int64 Vstep1[MAX_TRACKS][MAX_POLYPHONY];

#if defined(PTK_FX_TRANCEGLIDER)
    int64 glidestep[MAX_TRACKS];
#endif

float TPan[MAX_TRACKS];
int old_note[MAX_TRACKS][MAX_POLYPHONY];

s_access sp_Position[MAX_TRACKS][MAX_POLYPHONY];

#if defined(PTK_SYNTH)
s_access sp_Position_osc1[MAX_TRACKS][MAX_POLYPHONY];
s_access sp_Position_osc2[MAX_TRACKS][MAX_POLYPHONY];

#if defined(PTK_SYNTH_OSC3)
    s_access sp_Position_osc3[MAX_TRACKS][MAX_POLYPHONY];
#endif
#endif

#if defined(PTK_TRACKFILTERS)
float CCut[MAX_TRACKS];
#endif

char Use_Cubic = TRUE;

float TCut[MAX_TRACKS];
float ICut[MAX_TRACKS];
float LVol[MAX_TRACKS];
float RVol[MAX_TRACKS];
int FType[MAX_TRACKS];
int FRez[MAX_TRACKS];
float DThreshold[MAX_TRACKS];
char Disclap[MAX_TRACKS];
float DClamp[MAX_TRACKS];
float CCoef[MAX_TRACKS];
float lbuff_chorus[131072];
float rbuff_chorus[131072];
float coef[5];
float coeftab[5][128][128][4];

#if defined(__PSP__)
volatile int Songplaying;
#else
int Songplaying;
#endif

int New_Instrument[MAX_TRACKS];
int Pos_Segue[MAX_TRACKS];
float Segue_Volume[MAX_TRACKS];
float Segue_SamplesL[MAX_TRACKS];
float Segue_SamplesR[MAX_TRACKS];

#if defined(PTK_303)
    gear303 tb303engine[2];
    para303 tb303[2];
    unsigned char track3031;
    unsigned char track3032;
#endif

float left_float;
float right_float;
float left_float_render;
float right_float_render;
float left_chorus;
float right_chorus;
float delay_left_final;
float delay_right_final;
int PosInTick;

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    char rawrender;
    char rawrender_32float;
    int rawrender_range;
    int rawrender_from;
    int rawrender_to;
    float mas_vol = 1.0f;
#else
    float mas_vol;
#endif

float local_mas_vol;
float local_curr_mas_vol;
volatile float local_curr_ramp_vol;
volatile float local_ramp_vol;

int left_value;
int right_value;

#if !defined(__WINAMP__)
    int Songplaying_Pattern;

#if !defined(__STAND_ALONE__) 
    int done;
#endif

#else
    extern int done;
#endif

int Subicounter;

#if defined(PTK_MP3)
char Mp3_BitRate[MAX_INSTRS];
int Type_Mp3_BitRate[] =
{
    64, 88, 96, 128, 160, 192
};
#endif

#if defined(PTK_AT3)
char At3_BitRate[MAX_INSTRS];
int Type_At3_BitRate[] =
{
    66, 105, 132
};
#endif

#if defined(PTK_FX_PATTERNBREAK)
// 255 when no jump or yes on patbreak < 128 = line to jump.
#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    int Patbreak = 255;
    int Patbreak2 = 255;
#else
    int Patbreak;
    int Patbreak2;
#endif
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    char Songtracks = 6;
#else
    char Songtracks;
#endif

char Channels_Polyphony[MAX_TRACKS];
char Channels_MultiNotes[MAX_TRACKS];

int ped_line;
unsigned char pSequence[256];
int cPosition;
int cPosition_delay;
int ped_line_delay;
int pl_eff_row2;
int pl_dat_row2;
int pl_note[MAX_POLYPHONY];
int pl_sample[MAX_POLYPHONY];
int pl_vol_row;
int pl_pan_row;
unsigned char *RawPatterns;
int pl_eff_row;
int pl_dat_row;
int glide;       
float CustomVol[128];
unsigned int SubCounter;
unsigned int SamplesPerSub;
int shuffle;

#if defined(PTK_SHUFFLE)
int shufflestep;
int shuffleswitch;
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    unsigned char sLength = 1;
#else
    unsigned char sLength;
#endif

#if defined(PTK_FX_REVERSE)
    int Reverse_Switch[MAX_TRACKS];
#endif

#if defined(PTK_FX_VIBRATO)
    int64 Vstep_vib[MAX_TRACKS][MAX_POLYPHONY];
    int Vibrato_Switch[MAX_TRACKS];
    float Vibrato_BaseNote[MAX_TRACKS][MAX_POLYPHONY];
    int Vibcounter[MAX_TRACKS];
#endif

#if defined(PTK_FX_ARPEGGIO)
    int64 Vstep_arp[MAX_TRACKS][MAX_POLYPHONY];
    int Arpeggio_Switch[MAX_TRACKS];
    float Arpeggio_BaseNote[MAX_TRACKS][MAX_POLYPHONY];
#endif

#if defined(PTK_FX_PATTERNLOOP)
int repeat_loop_pos;
int repeat_loop_counter;
int repeat_loop_counter_in;
#endif

short patternLines[MAX_ROWS];
char grown;
float Curr_Signal_L[MAX_POLYPHONY];
float Curr_Signal_R[MAX_POLYPHONY];
float All_Signal_L;
float All_Signal_R;
unsigned int Current_Pointer;

#if defined(PTK_SYNTH)
char Synth_Was[MAX_TRACKS][MAX_POLYPHONY];
#endif

short *Player_WL[MAX_TRACKS][MAX_POLYPHONY];
short *Player_WR[MAX_TRACKS][MAX_POLYPHONY];
unsigned int Rns[MAX_TRACKS][MAX_POLYPHONY];
float Player_SV[MAX_TRACKS][MAX_POLYPHONY];
char Player_SC[MAX_TRACKS][MAX_POLYPHONY];
char Player_LT[MAX_TRACKS][MAX_POLYPHONY];
char Player_LW[MAX_TRACKS][MAX_POLYPHONY];
unsigned int Player_LS[MAX_TRACKS][MAX_POLYPHONY];
unsigned int Player_LE[MAX_TRACKS][MAX_POLYPHONY];
unsigned int Player_LL[MAX_TRACKS][MAX_POLYPHONY];
unsigned int Player_NS[MAX_TRACKS][MAX_POLYPHONY];

#if defined(PTK_LIMITER)
    #define MAS_COMPRESSOR_SECONDS 0.1f
    #define MAS_COMPRESSOR_SIZE (int) (MAS_COMPRESSOR_SECONDS * MIX_RATE)
    float mas_comp_bufferL[MAS_COMPRESSOR_SIZE];
    float mas_comp_bufferR[MAS_COMPRESSOR_SIZE];
    int mas_comp_pos_rms_buffer;
    float rms_sumL;
    float rms_sumR;
    float mas_comp_ratio;
    float mas_comp_threshold;
    float mas_envL;
    float mas_envR;
    float mas_threshold;
    float mas_ratio;
#endif

char CHAN_ACTIVE_STATE[256][MAX_TRACKS];

#if !defined(__STAND_ALONE__)
    char CHAN_HISTORY_STATE[256][MAX_TRACKS];
#endif

int gco;
float ramper[MAX_TRACKS];
char Basenote[MAX_INSTRS][16];
char SampleType[MAX_INSTRS][16];
char Finetune[MAX_INSTRS][16];

#if !defined(__NO_CODEC__)
#if !defined(__STAND_ALONE__) && !(__WINAMP__)
char SamplesSwap[MAX_INSTRS];
short *RawSamples_Swap[MAX_INSTRS][2][16];
#endif
#endif

unsigned char Synthprg[128];

#if defined(PTK_SYNTH)
#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
SynthParameters PARASynth[128];
#else
SYNTH_DATA PARASynth[128];
#endif
#endif

char LoopType[MAX_INSTRS][16];
Uint32 LoopStart[MAX_INSTRS][16];
Uint32 LoopEnd[MAX_INSTRS][16];
Uint32 SampleNumSamples[MAX_INSTRS][16];
char beatsync[MAX_INSTRS];
short beatlines[MAX_INSTRS];
int64 sp_Step[MAX_TRACKS][MAX_POLYPHONY];
float SampleVol[MAX_INSTRS][16];
char SampleChannels[MAX_INSTRS][16];
float FDecay[MAX_INSTRS][16];
short *RawSamples[MAX_INSTRS][2][16];

#if defined(PTK_COMPRESSOR)
    int currentCounter;
    int delayedCounter;
    int delayedCounter2;
    int delayedCounter3;
    int delayedCounter4;
    int delayedCounter5;
    int delayedCounter6;
    float allBuffer_L[5760];
    float allBuffer_L2[5760];
    float allBuffer_L3[5760];
    float allBuffer_L4[5760];
    float allBuffer_L5[5760];
    float allBuffer_L6[5760];
    float delay_left_buffer[MAX_COMB_FILTERS][100000];
    float delay_right_buffer[MAX_COMB_FILTERS][100000];
    int counters[MAX_COMB_FILTERS];
    int rev_counter;
    rFilter LFP_L;
    char num_echoes;
    int delays[MAX_COMB_FILTERS];       // delays for the comb filters
    float decays[MAX_COMB_FILTERS];

#endif // PTK_COMPRESSOR

float buf024[MAX_TRACKS];
float buf124[MAX_TRACKS];
float buf0[MAX_TRACKS];
float buf1[MAX_TRACKS];
float fx1[MAX_TRACKS];
float fx2[MAX_TRACKS];
float fy1[MAX_TRACKS];
float fy2[MAX_TRACKS];
float xi0[MAX_TRACKS];
float xi1[MAX_TRACKS];
float xi2[MAX_TRACKS];

#if !defined(__STAND_ALONE__) && !defined(__WINAMP__)
extern int gui_thread_action;
extern int gui_bpm_action;
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    float lchorus_feedback = 0.6f;
    float rchorus_feedback = 0.5f;
    int lchorus_delay = 10584;
    int rchorus_delay = 15876;
    unsigned int lchorus_counter = MIX_RATE;
    unsigned int rchorus_counter = MIX_RATE;
    unsigned int lchorus_counter2 = MIX_RATE - lchorus_delay;
    unsigned int rchorus_counter2 = MIX_RATE - rchorus_delay;
#else
    float lchorus_feedback;
    float rchorus_feedback;
    int lchorus_delay;
    int rchorus_delay;
    unsigned int lchorus_counter;
    unsigned int rchorus_counter;
    unsigned int lchorus_counter2;
    unsigned int rchorus_counter2;
#endif

// 0-->Off 1-->On
char compressor;

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    float REVERBFILTER = 0.3f;
#else
    float REVERBFILTER;
#endif

int Reserved_Sub_Channels[MAX_TRACKS][MAX_POLYPHONY];
int Note_Sub_Channels[MAX_TRACKS][MAX_POLYPHONY];

#if defined(PTK_INSTRUMENTS)
int sp_Stage[MAX_TRACKS][MAX_POLYPHONY];
#endif

#if defined(PTK_SYNTH)
int sp_Stage2[MAX_TRACKS][MAX_POLYPHONY];
int sp_Stage3[MAX_TRACKS][MAX_POLYPHONY];
#endif

char SampleCompression[MAX_INSTRS];
int delay_time;

#if defined(PTK_FLANGER)
    float Flanger_sbuf0L;
    float Flanger_sbuf1L;
    float Flanger_sbuf0R;
    float Flanger_sbuf1R;
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    int DelayType = 1;
#endif

#if !defined(__STAND_ALONE__)
    int L_MaxLevel;
    int R_MaxLevel;
    extern int CHAN_MIDI_PRG[MAX_TRACKS];
    float Scope_Dats[MAX_TRACKS][128];
    float Scope_Dats_LeftRight[2][128];
    int Cscope[MAX_TRACKS];
    int pos_scope;
    extern signed char c_midiin;
    extern signed char c_midiout;
    int plx;
    char Midiprg[128];
    int LastProgram[MAX_TRACKS];
    int wait_level;
    char nameins[128][20];
    char SampleName[128][16][64];
    unsigned char nPatterns = 1;
    void Actualize_303_Ed(char gode);
    extern char sr_isrecording;
    extern int32 sed_range_start;
    extern int32 sed_range_end;
#else
    unsigned char nPatterns;
#endif

#if defined(PTK_SYNTH_PINK)
unsigned int dice[7];
static unsigned long ctz[64] =
{
    6, 0, 1, 0, 2, 0, 1, 0,
    3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0,
    3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0,
    3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0,
    3, 0, 1, 0, 2, 0, 1, 0,
};
#endif

extern int Midi_Notes_History[MAX_TRACKS][256];
extern int Midi_Current_Notes[MAX_TRACKS][MAX_POLYPHONY];

#if !defined(__STAND_ALONE__)
void Clear_Midi_Channels_Pool(void);
#endif

// ------------------------------------------------------
// Functions
float ApplyLfo(float cy, int trcy);
void ComputeCoefs(int freq, int r, int t);

#if defined(PTK_FX_TICK0)
    void DoEffects_tick0(void);
#endif
#if defined(PTK_FX_PATTERNLOOP)
    void DoEffects_tick0_b(void);
#endif

void DoEffects(void);
float Filter(float x, char i);
float filter2p(int ch, float input, float f, float q);
float filter2px(int ch, float input, float f, float q);
float filter2p24d(int ch, float input, float f, float q);
float filterRingMod(int ch, float input, float f, float q);
float filterRingModStereo(int ch, float input);
float filterWater(int ch, float input, float f);
float filterWaterStereo(int ch, float input, float f);
float filterBellShaped(int ch, float input, float f, float q, float g);
float filterDelta(int ch, float input, float f, float q);
float int_filter2p(int ch, float input, float f, float q, float q2);
float filterhp(int ch, float input, float f, float q);
float filterhp2(int ch, float input, float f, float q);

#if defined(PTK_303)
    void live303(int pltr_eff_row, int pltr_dat_row);
    void Fire303(unsigned char number, int unit);
    void Go303(void);
#endif

float Kutoff(int v);
float Resonance(float v);
float Bandwidth(int v);
void Compressor_work(void);

void Initreverb();

#if defined(PTK_FLANGER)
    float Filter_FlangerL(float input);
    float Filter_FlangerR(float input);
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    void Mas_Compressor_Set_Variables(float treshold, float ratio);
#endif

float Mas_Compressor(float input, float *rms_sum, float *Buffer, float *Env);
float Do_RMS(float input, float *rms_sum, float *buffer);

volatile int Done_Reset;
void Reset_Values(void);

// ------------------------------------------------------
// Audio mixer
void STDCALL Mixer(Uint8 *Buffer, Uint32 Len)
{
#if defined(__MACOSX__)
    float *pSamples = (float *) Buffer;
#else
    short *pSamples = (short *) Buffer;
#endif
    int i;

#if !defined(__STAND_ALONE__)
    float clamp_left_value;
    float clamp_right_value;
#endif

#if !defined(__STAND_ALONE__)
    if(!rawrender)
    {
#endif

#if defined(__MACOSX__)
        for(i = Len - 1; i >= 0; i -= 8)
#else
        for(i = Len - 1; i >= 0; i -= 4)
#endif
        {
            GetPlayerValues();

#if defined(__MACOSX__)
            // Send them to the driver first
            *pSamples++ = left_float;
            *pSamples++ = right_float;

            // Prepare them for display now
            left_float *= 32767.0f;
            right_float *= 32767.0f;

            left_value = (int) (left_float);
            right_value = (int) (right_float);
#else

            *pSamples++ = left_value;
            *pSamples++ = right_value;

#endif

#if !defined(__STAND_ALONE__)
            // Gather datas for the scopes and the vumeters

#if !defined(__WINAMP__)
            if(Songplaying_Pattern)
#endif
            {
                clamp_left_value = (float) left_value;
                clamp_right_value = (float) right_value;

#if defined(__LINUX__)
                clamp_left_value /= 8192.0f;
                clamp_right_value /= 8192.0f;
                clamp_left_value *= 32767.0f;
                clamp_right_value *= 32767.0f;
#endif

                if(clamp_left_value > L_MaxLevel) L_MaxLevel = (int) clamp_left_value;
                if(clamp_right_value > R_MaxLevel) R_MaxLevel = (int) clamp_right_value;

                wait_level++;
                if(wait_level > 127)
                {
                    wait_level = 0;
                    L_MaxLevel -= 128;
                    R_MaxLevel -= 128;
                    if(L_MaxLevel < 0) L_MaxLevel = 0;
                    if(R_MaxLevel < 0) R_MaxLevel = 0;
                }

                Scope_Dats_LeftRight[0][pos_scope] = ((float) clamp_left_value) * 0.25f;
                Scope_Dats_LeftRight[1][pos_scope] = ((float) clamp_right_value) * 0.25f;
                pos_scope++;
                pos_scope &= 127;
            }
#endif
        }

        if(local_curr_ramp_vol <= 0.0f)
        {
            Reset_Values();
        }

#if !defined(__WINAMP__)
        if(AUDIO_GetSamples() > ((AUDIO_Latency * ((AUDIO_DBUF_RESOLUTION * AUDIO_DBUF_CHANNELS) >> 3)) / 4))
        {
            Songplaying_Pattern = TRUE;
        }
#endif

#if !defined(__STAND_ALONE__)
    } //RawRender
#endif
}

// ------------------------------------------------------
// Init the replayer driver
#if !defined(__WINAMP__)
#if defined(__WIN32__)
int STDCALL Ptk_InitDriver(HWND hWnd, int milliseconds)
{
    AUDIO_Milliseconds = milliseconds;
#else
int STDCALL Ptk_InitDriver(int milliseconds)
{
    AUDIO_Milliseconds = milliseconds;
#endif
#else
int STDCALL Ptk_InitDriver(void)
{
#endif

    int i;

#if defined(PTK_SYNTH)
    // Create the stock waveforms
    float incr = 1.0f / fMIX_RATE;
    float stop = 2.0f;
    float x;
    unsigned short temp_saw;

#if defined(PTK_SYNTH_SIN)
    short *wav_sin = STOCK_SIN;
#endif

#if defined(PTK_SYNTH_SAW)
    short *wav_saw = STOCK_SAW;
#endif

#if defined(PTK_SYNTH_PULSE)
    short *wav_pul = STOCK_PULSE;
#endif

#if defined(PTK_SYNTH_WHITE)
    short *wav_wit = STOCK_WHITE;
#endif

#if defined(PTK_SYNTH_PINK)
    short *wav_pin = STOCK_PINK;
    unsigned int newrand;
    unsigned int prevrand;
    unsigned int k;
    unsigned int seed = 0x12345678;
    unsigned int total = 0;
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    Mas_Compressor_Set_Variables(100.0f, 0.0f);
#endif

    SIZE_WAVEFORMS = 0;
    for(x = 0; x < (stop - incr / 2); x += incr)
    {
        float value = (float) ((PI * 2.0f) * x);
        //      float value2 = (float) ((PI * 2.0f) * (x * 2.0f));
        //    value2 = sinf(value + sinf(value2));

#if defined(PTK_SYNTH_PULSE)
        if(sinf(value) < 0.0f) *wav_pul++ = 32767;
        else *wav_pul++ = -32767;
#endif

#if defined(PTK_SYNTH_SAW)
        // There's a problem with fmodf->signed short in mingw here
        temp_saw = (unsigned short) (fmodf(x * 2.0f, 64.0f) * 32767.0f);
        *wav_saw++ = (short) (((float) (short) temp_saw));
#endif

        //*wav_pul++ = (short) (value2 * 16384.0f);
        //value = (float) ((PI * 2.0) * x);
        //value2 = sinf(value + sinf(value));
        //*wav_saw++ = (short) (value2 * 16384.0f);

#if defined(PTK_SYNTH_SIN)
        *wav_sin++ = (unsigned short) (sinf(value) * 32767.0f);
#endif

#if defined(PTK_SYNTH_WHITE)
        *wav_wit++ = (short) (rand() - 16384);
#endif

#if defined(PTK_SYNTH_PINK)
        // McCartney pink noise generator
        k = ctz[SIZE_WAVEFORMS & 63];
        prevrand = dice[k];
        seed = 1664525 * seed + 1013904223;
        newrand = seed >> 3;
        dice[k] = newrand;
        total += (newrand - prevrand);
        seed = 1103515245 * seed + 12345;
        newrand = seed >> 3;
        *wav_pin++ = (short) ((((total + newrand) * (1.0f / (3 << 29)) - 1) - .25f) * 16384.0f);
#endif

        SIZE_WAVEFORMS++;
    }
#endif // PTK_SYNTH

    // Initializing working SINETABLE
    for(i = 0; i < 360; i++)
    {
        SIN[i] = (float) sinf(i * 0.0174532f);
    }

#if !defined(__WINAMP__)

#if defined(__WIN32__)
    if(!AUDIO_Init_Driver(hWnd, &Mixer))
#else
    if(!AUDIO_Init_Driver(&Mixer))
#endif
    {
        return(FALSE);
    }
    AUDIO_Play();

#else  // __WINAMP__

    Pre_Song_Init();

#endif // __WINAMP__

    return(TRUE);
}

// ------------------------------------------------------
// Load a module
#if defined(__STAND_ALONE__)
Uint8 *Cur_Module;

void Mod_Dat_Read(void *Dest, int size)
{
    memcpy(Dest, Cur_Module, size);
    Cur_Module += size;
}

#if defined(PTK_INSTRUMENTS)
short *Unpack_Sample(int Dest_Length, char Pack_Type, int BitRate)
{
    int Packed_Length;
    short *Dest_Buffer;
    Uint8 *Packed_Read_Buffer;

    Mod_Dat_Read(&Packed_Length, sizeof(int));
    if(Packed_Length == -1)
    {
        // Sample wasn't packed
        Packed_Read_Buffer = (Uint8 *) malloc(Dest_Length * 2);
        Mod_Dat_Read(Packed_Read_Buffer, sizeof(char) * (Dest_Length * 2));
        return((short *) Packed_Read_Buffer);
    }
    else
    {
        Packed_Read_Buffer = (Uint8 *) malloc(Packed_Length);
        // Read the packer buffer
        Mod_Dat_Read(Packed_Read_Buffer, sizeof(char) * Packed_Length);
        Dest_Buffer = (short *) malloc(Dest_Length * 2);

        switch(Pack_Type)
        {

#if defined(__PSP__)
            case SMP_PACK_AT3:

#if !defined(__NO_CODEC__)
#if defined(PTK_AT3)
                UnpackAT3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length, BitRate);
#endif
#endif
#endif
                break;

            case SMP_PACK_GSM:

#if !defined(__NO_CODEC__)
#if defined(PTK_GSM)
                UnpackGSM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
#endif
#endif

                break;

            case SMP_PACK_MP3:

#if !defined(__NO_CODEC__)
#if defined(PTK_MP3)
                UnpackMP3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length, BitRate);
#endif
#endif
                break;

            case SMP_PACK_TRUESPEECH:

#if !defined(__NO_CODEC__)
#if defined(PTK_TRUESPEECH)
                UnpackTrueSpeech(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
#endif
#endif
                break;

            case SMP_PACK_ADPCM:

#if !defined(__NO_CODEC__)
#if defined(PTK_ADPCM)
                UnpackADPCM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
#endif
#endif

            case SMP_PACK_8BIT:

#if !defined(__NO_CODEC__)
#if defined(PTK_8BIT)
                Unpack8Bit(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
#endif
#endif
                break;
        }

        free(Packed_Read_Buffer);
        return(Dest_Buffer);
    }
}
#endif // PTK_INSTRUMENTS

int PTKEXPORT Ptk_InitModule(Uint8 *Module, int start_position)
{
    Uint32 *dwModule = (Uint32 *) Module;
    Cur_Module = Module;

    int i;
    int j;
    int k;
    unsigned char *TmpPatterns;
    unsigned char *TmpPatterns_Tracks;
    unsigned char *TmpPatterns_Notes;
    unsigned char *TmpPatterns_Rows;
    int tps_trk;
    int nbr_instr;
    int twrite;

    // .ptp modules aren't portable from big endian platform to little endian ones
    // (so that header will be saved as PRTK on little endian platforms and as KTRP on the other ones)
    if(dwModule[0] == 'KTRP')
    {
        Cur_Module += 4;

        Pre_Song_Init();

        Mod_Dat_Read(&nPatterns, sizeof(char));
        Mod_Dat_Read(&Songtracks, sizeof(char));
        Mod_Dat_Read(&sLength, sizeof(char));

        Mod_Dat_Read(&Use_Cubic, sizeof(char));

        Mod_Dat_Read(pSequence, sizeof(char) * sLength);

        // Patterns lines
        for(i = 0; i < nPatterns; i++)
        {
            patternLines[i] = 0;
            Mod_Dat_Read(&patternLines[i], sizeof(char));
            patternLines[i] = Swap_16(patternLines[i]);
        }

        // Allocated the necessary room for the patterns
        int max_lines = (PATTERN_BYTES * Songtracks * PATTERN_ROW_LEN * nPatterns);

        // Free the patterns block
        if(RawPatterns) free(RawPatterns);

        RawPatterns = (unsigned char *) malloc(max_lines);
        if(!RawPatterns) return(FALSE);

        // Multi notes
        Mod_Dat_Read(Channels_MultiNotes, sizeof(char) * Songtracks);

        TmpPatterns = RawPatterns;
        for(int pwrite = 0; pwrite < nPatterns; pwrite++)
        {
            TmpPatterns_Rows = TmpPatterns + (pwrite * PATTERN_LEN);
            for(i = 0; i < PATTERN_BYTES; i++)
            {   // Bytes / track
                for(k = 0; k < Songtracks; k++)
                {   // Tracks
                    TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                    for(j = 0; j < patternLines[pwrite]; j++)
                    {   // Rows
                        TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                        Mod_Dat_Read(TmpPatterns_Notes + i, sizeof(char));
                    }
                }
            }
        }

        Mod_Dat_Read(&nbr_instr, sizeof(int));

        for(int swrite = 0; swrite < nbr_instr; swrite++)
        {
            Mod_Dat_Read(&Synthprg[swrite], sizeof(char));

#if defined(PTK_SYNTH)
            if(Synthprg[swrite])
            {
                Mod_Dat_Read(&PARASynth[swrite], sizeof(SYNTH_DATA));
            }
#endif

            // Compression type
            Mod_Dat_Read(&SampleCompression[swrite], sizeof(char));

#if defined(PTK_MP3) || defined(PTK_AT3)
            switch(SampleCompression[swrite])
            {

#if defined(PTK_MP3)
                case SMP_PACK_MP3:
                    Mod_Dat_Read(&Mp3_BitRate[swrite], sizeof(char));
                    break;
#endif

#if defined(PTK_AT3)
                case SMP_PACK_AT3:
                    Mod_Dat_Read(&At3_BitRate[swrite], sizeof(char));
                    break;
#endif

            }
#endif

            for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
            {
                Mod_Dat_Read(&SampleType[swrite][slwrite], sizeof(char));

#if defined(PTK_INSTRUMENTS)
                if(SampleType[swrite][slwrite])
                {
                    int Apply_Interpolation;
                    Uint32 Save_Len;
                    Uint32 iSmp;
                    short Sample1;
                    short Sample2;
                    short *Sample_Buffer = NULL;
                    short *Sample_Dest_Buffer;

                    // No samples names in packed modules
                    Mod_Dat_Read(&Basenote[swrite][slwrite], sizeof(char));
                    Mod_Dat_Read(&LoopStart[swrite][slwrite], sizeof(int));
                    Mod_Dat_Read(&LoopEnd[swrite][slwrite], sizeof(int));
                    Mod_Dat_Read(&LoopType[swrite][slwrite], sizeof(char));
                    Mod_Dat_Read(&SampleNumSamples[swrite][slwrite], sizeof(int));
                    Mod_Dat_Read(&Finetune[swrite][slwrite], sizeof(char));
                    Mod_Dat_Read(&SampleVol[swrite][slwrite], sizeof(float));
                    Mod_Dat_Read(&FDecay[swrite][slwrite], sizeof(float));
                    Save_Len = SampleNumSamples[swrite][slwrite];

                    Apply_Interpolation = SampleCompression[swrite] == SMP_PACK_NONE ? FALSE : TRUE;

                    if(Apply_Interpolation)
                    {
                        Save_Len /= 2;
                        Sample_Buffer = Unpack_Sample(Save_Len,
                                                      SampleCompression[swrite],
                                                      SampleCompression[swrite] == SMP_PACK_MP3 ?
#if defined(PTK_MP3)
                                                      Type_Mp3_BitRate[Mp3_BitRate[swrite]]
#else
                                                      0
#endif
                                                      :

#if defined(PTK_AT3)
                                                      Type_At3_BitRate[At3_BitRate[swrite]]
#else
                                                      0
#endif
                                                     );

                        Sample_Dest_Buffer = (short *) malloc((Save_Len * 2 * sizeof(short)) + 2);
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
                        RawSamples[swrite][0][slwrite] = Sample_Dest_Buffer;
                    }
                    else
                    {
                        RawSamples[swrite][0][slwrite] = Unpack_Sample(Save_Len,
                                                                       SampleCompression[swrite],
                                                                       SampleCompression[swrite] == SMP_PACK_MP3 ?
#if defined(PTK_MP3)
                                                                            Type_Mp3_BitRate[Mp3_BitRate[swrite]]
#else
                                                                            0
#endif
                                                                            :
#if defined(PTK_AT3)
                                                                            Type_At3_BitRate[At3_BitRate[swrite]]
#else
                                                                            0
#endif
                                                                      );
                    }
                    *(RawSamples[swrite][0][slwrite]) = 0;

                    // Stereo flag
                    Mod_Dat_Read(&SampleChannels[swrite][slwrite], sizeof(char));
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        if(Apply_Interpolation)
                        {
                            Sample_Buffer = Unpack_Sample(Save_Len,
                                                          SampleCompression[swrite],
                                                          SampleCompression[swrite] == SMP_PACK_MP3 ?
#if defined(PTK_MP3)
                                                                Type_Mp3_BitRate[Mp3_BitRate[swrite]]
#else
                                                                0
#endif
                                                                :
#if defined(PTK_AT3)
                                                                Type_At3_BitRate[At3_BitRate[swrite]]
#else
                                                                0
#endif
                                                         );

                            Sample_Dest_Buffer = (short *) malloc((Save_Len * 2 * sizeof(short)) + 2);
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
                            RawSamples[swrite][1][slwrite] = Sample_Dest_Buffer;
                        }
                        else
                        {
                            RawSamples[swrite][1][slwrite] = Unpack_Sample(Save_Len,
                                                                           SampleCompression[swrite],
                                                                           SampleCompression[swrite] == SMP_PACK_MP3 ?
#if defined(PTK_MP3)
                                                                                Type_Mp3_BitRate[Mp3_BitRate[swrite]]
#else
                                                                                0
#endif
                                                                                :
#if defined(PTK_AT3)
                                                                                Type_At3_BitRate[At3_BitRate[swrite]]
#else
                                                                                0
#endif
                                                                          );
                        }
                        *RawSamples[swrite][1][slwrite] = 0;
                    }
                    if(Sample_Buffer) free(Sample_Buffer);

                }// Exist Sample
#endif // PTK_INSTRUMENTS

            }
        }

        // Reading mod properties
        Mod_Dat_Read(&compressor, sizeof(char));

        // Reading Track Properties
        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Mod_Dat_Read(&TCut[twrite], sizeof(float));
            Mod_Dat_Read(&ICut[twrite], sizeof(float));

            Mod_Dat_Read(&TPan[twrite], sizeof(float));
            ComputeStereo(twrite);
            Mod_Dat_Read(&FType[twrite], sizeof(int));
            Mod_Dat_Read(&FRez[twrite], sizeof(int));
            Mod_Dat_Read(&DThreshold[twrite], sizeof(float));
            Mod_Dat_Read(&DClamp[twrite], sizeof(float));

#if defined(PTK_COMPRESSOR)
            if(compressor)
            {
                Mod_Dat_Read(&DSend[twrite], sizeof(float));
            }
#endif

            Mod_Dat_Read(&CSend[twrite], sizeof(int));
            Mod_Dat_Read(&Channels_Polyphony[twrite], sizeof(char));
        }

        Mod_Dat_Read(&c_threshold, sizeof(int));
        Mod_Dat_Read(&BeatsPerMin, sizeof(int));
        Mod_Dat_Read(&TicksPerBeat, sizeof(int));
        Mod_Dat_Read(&mas_vol, sizeof(float));

        char Comp_Flag;
        Mod_Dat_Read(&Comp_Flag, sizeof(char));

#if defined(PTK_LIMITER)
        if(Comp_Flag)
        {
            Mod_Dat_Read(&mas_threshold, sizeof(float));
            Mod_Dat_Read(&mas_ratio, sizeof(float));
        }
#endif

        Mod_Dat_Read(&Feedback, sizeof(float));

#if defined(PTK_COMPRESSOR)
        if(compressor)
        {
            Mod_Dat_Read(&num_echoes, sizeof(char));

            for(i = 0; i < num_echoes; i++)
            {
                Mod_Dat_Read(&delays[i], sizeof(int));
            }
            for(i = 0; i < num_echoes; i++)
            {
                Mod_Dat_Read(&decays[i], sizeof(float));
            }
        }
#endif

        Mod_Dat_Read(&lchorus_delay, sizeof(int));
        Mod_Dat_Read(&rchorus_delay, sizeof(int));
        Mod_Dat_Read(&lchorus_feedback, sizeof(float));
        Mod_Dat_Read(&rchorus_feedback, sizeof(float));
        Mod_Dat_Read(&shuffle, sizeof(int));

        // Reading track part sequence
        for(int tps_pos = 0; tps_pos < sLength; tps_pos++)
        {
            for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
            {
                Mod_Dat_Read(&CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char));
            }
        }

        for(int spl = 0; spl < Songtracks; spl++)
        {
            CCoef[spl] = float((float) CSend[spl] / 127.0f);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Mod_Dat_Read(&LFO_ON[twrite], sizeof(char));

#if defined(PTK_LFO)
            if(LFO_ON[twrite])
            {
                Mod_Dat_Read(&LFO_RATE[twrite], sizeof(float));
                Mod_Dat_Read(&LFO_AMPL[twrite], sizeof(float));
            }
#endif
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Mod_Dat_Read(&FLANGER_ON[twrite], sizeof(char));

#if defined(PTK_FLANGER)
            if(FLANGER_ON[twrite])
            {
                Mod_Dat_Read(&FLANGER_AMOUNT[twrite], sizeof(float));
                Mod_Dat_Read(&FLANGER_DEPHASE[twrite], sizeof(float));
                Mod_Dat_Read(&FLANGER_RATE[twrite], sizeof(float));
                Mod_Dat_Read(&FLANGER_AMPL[twrite], sizeof(float));
                Mod_Dat_Read(&FLANGER_FEEDBACK[twrite], sizeof(float));
                Mod_Dat_Read(&FLANGER_DELAY[twrite], sizeof(int));
                FLANGER_OFFSET[twrite] = 8192;
                foff2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
                foff1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
            }
#endif

        }

        for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
        {
            Mod_Dat_Read(&Disclap[tps_trk], sizeof(char));
        }

        char Instrs;

        Mod_Dat_Read(&Instrs, sizeof(char));

        if(Instrs)
        {
            Mod_Dat_Read(beatsync, sizeof(char) * MAX_INSTRS);
            Mod_Dat_Read(beatlines, sizeof(short) * MAX_INSTRS);
        }
        Mod_Dat_Read(&REVERBFILTER, sizeof(float));

        if(Instrs)
        {
            Mod_Dat_Read(CustomVol, sizeof(float) * MAX_INSTRS);
        }

        char tb303_1_enabled;
        char tb303_2_enabled;
        // Read the 303 datas
        Mod_Dat_Read(&tb303_1_enabled, sizeof(char));

#if defined(PTK_303)
        tb303[0].enabled = tb303_1_enabled;
        if(tb303_1_enabled)
        {
            Mod_Dat_Read(&tb303[0].selectedpattern, sizeof(para303) - sizeof(char));
        }
#endif

        Mod_Dat_Read(&tb303_2_enabled, sizeof(char));

#if defined(PTK_303)
        tb303[1].enabled = tb303_2_enabled;
        if(tb303_2_enabled)
        {
            Mod_Dat_Read(&tb303[1].selectedpattern, sizeof(para303) - sizeof(char));
        }

        if(tb303_1_enabled) Mod_Dat_Read(&tb303engine[0].tbVolume, sizeof(float));
        if(tb303_2_enabled) Mod_Dat_Read(&tb303engine[1].tbVolume, sizeof(float));
        if(tb303_1_enabled) Mod_Dat_Read(&tb303engine[0].hpf, sizeof(char));
        if(tb303_2_enabled) Mod_Dat_Read(&tb303engine[1].hpf, sizeof(char));
#endif

        cPosition = start_position;

        Post_Song_Init();

        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}
#endif  // __STAND_ALONE__

// ------------------------------------------------------
// Release the replayer driver
void PTKEXPORT Ptk_ReleaseDriver(void)
{

#if !defined(__WINAMP__)
    AUDIO_Stop_Driver();
#endif

}

// ------------------------------------------------------
// Retrieve the current position in the song
int PTKEXPORT Ptk_GetRow(void)
{
    return(ped_line_delay);
}
int PTKEXPORT Ptk_GetPosition(void)
{
    return(cPosition_delay);
}

// ------------------------------------------------------
// Set the current position in the song
void PTKEXPORT Ptk_SetPosition(int new_position)
{
    if(new_position >= sLength) new_position = sLength - 1;
    if(new_position < 0) new_position = 0;

#if !defined(__WINAMP__)
    Songplaying_Pattern = FALSE;
#endif

    cPosition = new_position;
    cPosition_delay = new_position;
    PosInTick = 0;
    SubCounter = 0;

#if defined(PTK_FX_PATTERNLOOP)
    repeat_loop_pos = -1;       // No repeat loop
    repeat_loop_counter = 0;
    repeat_loop_counter_in = 0;
#endif

    ped_line = 0;
    ped_line_delay = 0;
    Subicounter = 0;
}

// ------------------------------------------------------
// Reset some values before starting playing
void Reset_Values(void)
{
    int i;

    if(!Done_Reset)
    {
        lchorus_counter = MIX_RATE;
        rchorus_counter = MIX_RATE;
        lchorus_counter2 = MIX_RATE - lchorus_delay;
        rchorus_counter2 = MIX_RATE - rchorus_delay;

        for(i = 0; i < 131072; i++)
        {
            lbuff_chorus[i] = 0.0f;
            rbuff_chorus[i] = 0.0f;
        }

        for(i = 0; i < MAX_TRACKS; i++)
        {
            ResetFilters(i);

#if defined(PTK_TRACKFILTERS)
            CCut[i] = 0.0f;
#endif

            ramper[i] = 0;

#if defined(PTK_FLANGER)
            foff2[i] = 0.0f;
            foff1[i] = 0.0f;
            for(int ini2 = 0; ini2 < 16400; ini2++)
            {
                FLANGE_LEFTBUFFER[i][ini2] = 0.0f;
                FLANGE_RIGHTBUFFER[i][ini2] = 0.0f;
            }

#endif

            New_Instrument[i] = 0;
            Pos_Segue[i] = 0;
            Segue_Volume[i] = 0;
            Segue_SamplesL[i] = 0;
            Segue_SamplesR[i] = 0;
        }

#if defined(PTK_FLANGER)
        Flanger_sbuf0L = 0;
        Flanger_sbuf1L = 0;
        Flanger_sbuf0R = 0;
        Flanger_sbuf1R = 0;
#endif

#if defined(PTK_COMPRESSOR)
        Initreverb();
#endif

        for(int stopper = 0; stopper < MAX_TRACKS; stopper++)
        {
            for(int stopper_poly = 0; stopper_poly < MAX_POLYPHONY; stopper_poly++)
            {

#if defined(PTK_SYNTH)
                Synthesizer[stopper][stopper_poly].Reset();
#endif

#if defined(PTK_INSTRUMENTS)
                sp_Stage[stopper][stopper_poly] = PLAYING_NOSAMPLE;
#endif

#if defined(PTK_SYNTH)
                sp_Stage2[stopper][stopper_poly] = PLAYING_NOSAMPLE;
                sp_Stage3[stopper][stopper_poly] = PLAYING_NOSAMPLE;
#endif
                Reserved_Sub_Channels[stopper][stopper_poly] = -1;
                Note_Sub_Channels[stopper][stopper_poly] = -1;
                sp_channelsample[stopper][stopper_poly] = -1;
            }
        }

#if defined(PTK_303)
        
        tb303engine[0].reset();
        tb303engine[1].reset();
        
        track3031 = 255;
        track3032 = 255;
#endif

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
        Midi_AllNotesOff();
#endif
        // Clear all midi channels
        Clear_Midi_Channels_Pool();
#endif

        // Store the visual position (not the replayed one)
        if(Songplaying)
        {
            cPosition = cPosition_delay;
            ped_line = ped_line_delay;
        }

#if defined(__PSP__)
        volatile int *ptr_Done_Reset = (int *) (((int) &Done_Reset) | 0x40000000);
        *ptr_Done_Reset = TRUE;
#else
        Done_Reset = TRUE;
#endif

#if !defined(__STAND_ALONE__) && !defined(__WINAMP__)
        gui_thread_action = TRUE;
#endif

        Songplaying = FALSE;
    }
}

// ------------------------------------------------------
// Start replaying
void PTKEXPORT Ptk_Play(void)
{

#if !defined(__STAND_ALONE__)
    int i;
    int j;

    for(j = 0; j < 256; j++)
    {
        for(i = 0; i < MAX_TRACKS; i++)
        {
            CHAN_HISTORY_STATE[j][i] = FALSE;
        }
    }
#endif

    Done_Reset = FALSE;

    local_mas_vol = 1.0f;
    local_curr_mas_vol = 0.0f;

    local_ramp_vol = 1.0f;
    local_curr_ramp_vol = 0.0f;

    Reset_Values();
    Done_Reset = FALSE;

    Songplaying = TRUE;

#if defined(__PSP__)
    sceKernelDcacheWritebackInvalidateAll();	
#endif
}

#if defined(__PSP__)
extern "C"
{
    void me_sceKernelDcacheWritebackInvalidateAll(void);
}
#endif

// ------------------------------------------------------
// Stop replaying
extern int AUDIO_Play_Flag;

void PTKEXPORT Ptk_Stop(void)
{
#if defined(__PSP__)
    // Thanks to MIPS, that machine really sucks
    volatile int *ptr_Done_Reset = (int *) (((int) &Done_Reset) | 0x40000000);
    *ptr_Done_Reset = FALSE;
    volatile float *ptr_local_ramp_vol = (float *) (((int) &local_ramp_vol) | 0x40000000);
    volatile float *ptr_local_curr_ramp_vol = (float *) (((int) &local_curr_ramp_vol) | 0x40000000);
    me_sceKernelDcacheWritebackInvalidateAll();
    sceKernelDcacheWritebackInvalidateAll();	
    while(*ptr_Done_Reset == FALSE && AUDIO_Play_Flag && *ptr_local_curr_ramp_vol != 0.0f)
    {
        *ptr_local_ramp_vol = 0.0f;
    }
#else
    Done_Reset = FALSE;
    while(Done_Reset == FALSE && AUDIO_Play_Flag && local_curr_ramp_vol != 0.0f)
    {
        local_ramp_vol = 0.0f;

#if defined(__MACOSX__)
        usleep(10);
#endif
#if defined(__WIN32__)
        Sleep(10);
#endif
#if defined(__LINUX__) 
        usleep(10);
#endif

    }
#endif

#if defined(__STAND_ALONE__) && !defined(__WINAMP__)
    // Free the patterns block
    if(RawPatterns) free(RawPatterns);
    RawPatterns = NULL;
#endif

}

// ------------------------------------------------------
// Init replay variables before loading a module
void Pre_Song_Init(void)
{
    int i;

    glide = 0;

    for(int ini = 0; ini < MAX_TRACKS; ini++)
    {
        for(i = 0; i < MAX_POLYPHONY; i++)
        {

#if defined(PTK_FX_ARPEGGIO)
            Arpeggio_BaseNote[ini][i] = 0;
            Vstep_arp[ini][i] = 0;
#endif

#if defined(PTK_FX_VIBRATO)
            Vibrato_BaseNote[ini][i] = 0;
            Vstep_vib[ini][i] = 0;
#endif

#if defined(PTK_SYNTH)
            Synth_Was[ini][i] = 0;
#endif

            Player_WL[ini][i] = 0;
            Player_WR[ini][i] = 0;
            Rns[ini][i] = 0;
            Player_SV[ini][i] = 0;
            Player_SC[ini][i] = 0;
            Player_LT[ini][i] = 0;
            Player_LW[ini][i] = 0;
            Player_LS[ini][i] = 0;
            Player_LE[ini][i] = 0;
            Player_LL[ini][i] = 0;
            Player_NS[ini][i] = 0;

#if defined(PTK_SYNTH)
            Synthesizer[ini][i].Reset();
#endif

            sp_Step[ini][i] = 0;

#if defined(PTK_INSTRUMENTS)
            sp_Stage[ini][i] = PLAYING_NOSAMPLE;
#endif

#if defined(PTK_SYNTH)
            sp_Stage2[ini][i] = PLAYING_NOSAMPLE;
            sp_Stage3[ini][i] = PLAYING_NOSAMPLE;
#endif

            sp_Position[ini][i].absolu = 0;

#if defined(PTK_SYNTH)
            sp_Position_osc1[ini][i].absolu = 0;
            sp_Position_osc2[ini][i].absolu = 0;

#if defined(PTK_SYNTH_OSC3)
            sp_Position_osc3[ini][i].absolu = 0;
#endif
#endif

            CHAN_ACTIVE_STATE[ini][i] = TRUE;

            old_note[ini][i] = 0;

            Vstep1[ini][i] = 0;

            sp_Cvol[ini][i] = 0.0f;

            sp_channelsample[ini][i] = -1;
            sp_split[ini][i] = 0;

            sp_Tvol[ini][i] = 0.0f;
        }

        Channels_MultiNotes[ini] = 1;

        Player_FD[ini] = 0.0f;

#if defined(PTK_FX_AUTOFADEMODE)
        FADEMODE[ini] = 0;
        FADECOEF[ini] = 0.0f;
#endif

        ResetFilters(ini);

#if !defined(__STAND_ALONE__)
        CHAN_MIDI_PRG[ini] = ini;
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
        CHAN_MUTE_STATE[ini] = 0;
#endif

        oldspawn[ini] = 0;
        roldspawn[ini] = 0;
        
#if defined(PTK_LFO)
        LFO_ON[ini] = 0;
        LFO_RATE[ini] = 0.0001f;
        LFO_AMPL[ini] = 0;
#endif

#if !defined(__STAND_ALONE__)
        FLANGER_AMOUNT[ini] = -0.8f;
        FLANGER_DEPHASE[ini] = 0.0174532f;
        FLANGER_ON[ini] = 0;
        FLANGER_RATE[ini] = 0.0068125f / 57.29578f;
        FLANGER_AMPL[ini] = 0.0f;
        FLANGER_GR[ini] = 0;
        FLANGER_FEEDBACK[ini] = -0.51f;
        FLANGER_DELAY[ini] = 176;
        FLANGER_OFFSET[ini] = 176;

        TPan[ini] = 0.5f;
        TCut[ini] = 126.0f;
        ICut[ini] = 0.0039062f;
        FType[ini] = 4;
        FRez[ini] = 64;
        DThreshold[ini] = 32767;
        DClamp[ini] = 32767;
        Disclap[ini] = 0;
#endif

        ramper[ini] = 0;

#if defined(PTK_FLANGER)
        foff2[ini] = 0.0f;
        foff1[ini] = 0.0f;
        for(int ini2 = 0; ini2 < 16400; ini2++)
        {
            FLANGE_LEFTBUFFER[ini][ini2] = 0.0f;
            FLANGE_RIGHTBUFFER[ini][ini2] = 0.0f;
        }
#endif

#if !defined(__STAND_ALONE__)
        DSend[ini] = 0;
        CSend[ini] = 0;
#endif

#if defined(PTK_FX_TRANCEGLIDER)
        glidestep[ini] = 0;
#endif

#if defined(PTK_TRACKFILTERS)
        CCut[ini] = 0.0f;
#endif

    }

#if defined(PTK_303)
    tb303engine[0].reset();
    tb303engine[1].reset();
#endif

#if defined(PTK_FLANGER)
    Flanger_sbuf0L = 0;
    Flanger_sbuf1L = 0;
    Flanger_sbuf0R = 0;
    Flanger_sbuf1R = 0;
#endif

    for(i = 0; i < MAX_INSTRS; i++)
    {
        CustomVol[i] = 1.0f;
    }

#if defined(PTK_LIMITER)
    mas_comp_pos_rms_buffer = 0;
    rms_sumL = 0;
    rms_sumR = 0;
    mas_comp_ratio = 0;
    mas_comp_threshold = 0;
    mas_envL = 0;
    mas_envR = 0;

    for(i = 0; i < MAS_COMPRESSOR_SIZE; i++)
    {
        mas_comp_bufferL[i] = 0;
        mas_comp_bufferR[i] = 0;
    }
#endif

    for(int dini = 0; dini < 131072; dini++)
    {
        lbuff_chorus[dini] = 0.0f;
        rbuff_chorus[dini] = 0.0f;
    }

    for(int cutt = 0; cutt < 128; cutt++)
    {
        for(int rezz = 0; rezz < 128; rezz++)
        {
            for(int typp = 0; typp < 4; typp++)
            {
                ComputeCoefs(cutt, rezz, typp);
                coeftab[0][cutt][rezz][typp] = coef[0];      
                coeftab[1][cutt][rezz][typp] = coef[1];
                coeftab[2][cutt][rezz][typp] = coef[2];
                coeftab[3][cutt][rezz][typp] = coef[3];
                coeftab[4][cutt][rezz][typp] = coef[4];
            }
        }
    }

#if defined(PTK_INSTRUMENTS)
    Free_Samples();
#endif

}

// ------------------------------------------------------
// Init the replayer datas
void Post_Song_Init(void)
{
    int i;
    int j;

#if defined(PTK_SHUFFLE)
    shufflestep = shuffle;
    shuffleswitch = -1;
#endif

    for(i = 0; i < MAX_TRACKS; i++)
    {
        for(j = 0; j < MAX_POLYPHONY; j++)
        {
            Reserved_Sub_Channels[i][j] = -1;
            Note_Sub_Channels[i][j] = -1;
        }
    }

    SubCounter = 0;

#if defined(PTK_FX_PATTERNLOOP)
    repeat_loop_pos = -1;       // No repeat loop
    repeat_loop_counter = 0;
    repeat_loop_counter_in = 0;
#endif

    for(i = 0; i < MAX_TRACKS; i++)
    {

#if defined(PTK_FX_ARPEGGIO)
        Arpeggio_Switch[i] = 0;
#endif

#if defined(PTK_FX_VIBRATO)
        Vibrato_Switch[i] = 0;
#endif
        
        for(j = 0; j < MAX_POLYPHONY; j++)
        {

#if defined(PTK_FX_ARPEGGIO)
            Arpeggio_BaseNote[i][j] = 0;
            Vstep_arp[i][j] = 0;
#endif

#if defined(PTK_FX_VIBRATO)
            Vibrato_BaseNote[i][j] = 0;
            Vstep_vib[i][j] = 0;
#endif

        }
       
#if defined(PTK_FX_REVERSE)
        Reverse_Switch[i] = 0;
#endif

    }

    lchorus_counter = MIX_RATE;
    rchorus_counter = MIX_RATE;
    lchorus_counter2 = MIX_RATE - lchorus_delay;
    rchorus_counter2 = MIX_RATE - rchorus_delay;
    SamplesPerTick = (int) ((60 * MIX_RATE) / (BeatsPerMin * TicksPerBeat));
    PosInTick = 0;
    SamplesPerSub = SamplesPerTick / 6;
    ped_line = 0;
    ped_line_delay = 0;

#if !defined(__WINAMP__)
    Songplaying_Pattern = FALSE;
#endif

#if defined(PTK_SHUFFLE)
    if(shuffleswitch == 1) shufflestep = -((SamplesPerTick * shuffle) / 200);
    else shufflestep = (SamplesPerTick * shuffle) / 200;
#endif

    local_ramp_vol = 1.0f;
    local_curr_ramp_vol = 0.0f;
    
    local_mas_vol = 1.0f;
    local_curr_mas_vol = 0.0f;

    cPosition_delay = cPosition;

    // Start as the last known position
    for(int spl = 0; spl < MAX_TRACKS; spl++)
    {
        CCoef[spl] = float((float) CSend[spl] / 127.0f);
        ComputeStereo(spl);
    }

#if !defined(__WINAMP__)
    AUDIO_ResetTimer();
#endif

}

// ------------------------------------------------------
// Main Player Routine
void Sp_Player(void)
{
    unsigned int Old_Pointer;
    unsigned int res_dec;

#if defined(PTK_303)
    float Signal_303;
#endif

    char gotsome;
    int c;
    int i;
    int j;
    int trigger_note_off;

    left_float = 0;
    right_float = 0;

#if defined(PTK_COMPRESSOR)
    delay_left_final = 1.0f;
    delay_right_final = 1.0f;
#endif

    if(Songplaying)
    {
        if(PosInTick == 0)
        {

#if defined(PTK_FX_TICK0)
            DoEffects_tick0();
#endif

#if defined(PTK_FX_PATTERNLOOP)
            if(repeat_loop_counter_in)
            {
                ped_line -= repeat_loop_pos + 1;
                ped_line_delay -= repeat_loop_pos + 1;
                if(ped_line < 0) ped_line = 0;
                if(ped_line_delay < 0) ped_line_delay = 0;
                repeat_loop_counter_in = 0;
            }
#endif

            Subicounter = 0;

#if defined(PTK_FX_PATTERNBREAK)
            Patbreak = 255;
            Patbreak2 = 255;
#endif

            for(int ct = 0; ct < Songtracks; ct++)
            {
                int efactor = Get_Pattern_Offset(pSequence[cPosition], ct, ped_line);
                int efactor2 = Get_Pattern_Offset(pSequence[cPosition_delay], ct, ped_line_delay);
                
                for(i = 0; i < Channels_MultiNotes[ct]; i++)
                {
                    pl_note[i] = *(RawPatterns + efactor + PATTERN_NOTE1 + (i * 2));
                    pl_sample[i] = *(RawPatterns + efactor + PATTERN_INSTR1 + (i * 2));
                }
                
                pl_vol_row = *(RawPatterns + efactor + PATTERN_VOLUME);
                pl_pan_row = *(RawPatterns + efactor + PATTERN_PANNING);
                pl_eff_row = *(RawPatterns + efactor + PATTERN_FX);
                pl_dat_row = *(RawPatterns + efactor + PATTERN_FXDATA);
                pl_eff_row2 = *(RawPatterns + efactor2 + PATTERN_FX);
                pl_dat_row2 = *(RawPatterns + efactor2 + PATTERN_FXDATA);

#if defined(PTK_VOLUME_COLUMN)
                if(pl_vol_row <= 64)
                {
                    for(i = 0; i < Channels_Polyphony[ct]; i++)
                    {
                        sp_Tvol[ct][i] = (float) pl_vol_row * 0.015625f; // Setting volume.
                    }
                }
#endif

#if defined(PTK_FX_SETVOLUME)
                if(pl_eff_row == 3)
                {
                    for(i = 0; i < Channels_Polyphony[ct]; i++)
                    {
                        sp_Tvol[ct][i] = (float) pl_dat_row * 0.0039062f; // Setting volume.
                    }
                }
#endif

                if(pl_pan_row <= 128)
                {
                    TPan[ct] = (float) pl_pan_row * 0.0078125f; 
                    ComputeStereo(ct);
                }

#if !defined(__STAND_ALONE__)
                if(!sr_isrecording)
#endif
#if defined(PTK_303)
                {
                    live303(pl_eff_row, pl_dat_row);
                }
#endif

#if defined(PTK_303)
                if(pl_eff_row == 0x31)
                {
                    track3031 = ct;
                    Fire303(pl_dat_row, 0);
                }
                if(pl_eff_row == 0x32)
                {
                    track3032 = ct;
                    Fire303(pl_dat_row, 1);
                }
#endif

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
                /* MidiController commands */
                if(pl_pan_row == 0x90 && c_midiout != -1 && pl_eff_row < 128)
                {
                    Midi_Send(176 + CHAN_MIDI_PRG[ct], pl_eff_row, pl_dat_row);
                }

                if(pl_eff_row == 0x80 && c_midiout != -1 && pl_dat_row < 128)
                {
                    Midi_Send(176 + CHAN_MIDI_PRG[ct], 0, pl_dat_row);
                }
#endif
#endif

#if defined(PTK_FX_PATTERNBREAK)
                if(pl_eff_row == 0xd && pl_dat_row < MAX_ROWS) Patbreak = pl_dat_row;
                if(pl_eff_row2 == 0xd && pl_dat_row2 < MAX_ROWS) Patbreak2 = pl_dat_row2;
#endif

                int toffset;
                int free_sub_channel;

                toffset = 0;
                glide = 0;
                if(pl_eff_row == 9) toffset = pl_dat_row;
                else if(pl_eff_row == 5) glide = 1;

                for(i = 0; i < Channels_MultiNotes[ct]; i++)
                {
                    if(pl_note[i] < 120 && Note_Sub_Channels[ct][i] != -1)
                    {
                        j = Reserved_Sub_Channels[ct][i];

#if defined(PTK_INSTRUMENTS)
                        // Get the virtual channel it was playing on and remove it
                        if(sp_Stage[ct][j] == PLAYING_SAMPLE)
                        {
                            sp_Stage[ct][j] = PLAYING_SAMPLE_NOTEOFF;
                        }
#endif
#if defined(PTK_SYNTH)
                        Synthesizer[ct][j].NoteOff();
#endif

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
                        if(Midi_Current_Notes[CHAN_MIDI_PRG[ct]][j])
                        {
                            Midi_NoteOff(ct, Midi_Current_Notes[CHAN_MIDI_PRG[ct]][j]);
                            Midi_Current_Notes[CHAN_MIDI_PRG[ct]][j] = 0;
                        }
#endif
#endif
                    }
                }

                for(i = 0; i < Channels_MultiNotes[ct]; i++)
                {
                    if(pl_note[i] < 120)
                    {
                        free_sub_channel = Get_Free_Sub_Channel(ct, Channels_Polyphony[ct]);
                        if(free_sub_channel == -1) free_sub_channel = i;

                        // Mark it as playing
                        Note_Sub_Channels[ct][i] = i;
                        Reserved_Sub_Channels[ct][i] = free_sub_channel;

#if defined(PTK_VOLUME_COLUMN) || defined(PTK_FX_SETVOLUME)
                        if(pl_vol_row <= 64 || pl_eff_row == 3)
                        {
                            Play_Instrument(ct,
                                            free_sub_channel,
                                            pl_note[i],
                                            pl_sample[i],
                                            sp_Tvol[ct][free_sub_channel],
                                            toffset,
                                            glide,
                                            FALSE, i + 1);
                        }
                        else
#endif
                        {
                            // Use the default sample volume if there's nothing
                            // in the volume column of no 0x3 fx
                            Play_Instrument(ct,
                                            free_sub_channel,
                                            pl_note[i],
                                            pl_sample[i],
                                            CustomVol[pl_sample[i]],
                                            toffset,
                                            glide,
                                            FALSE, i + 1);
                        }
                    }
                }

                // Trigger a note off
                trigger_note_off = FALSE;
                for(i = 0; i < Channels_MultiNotes[ct]; i++)
                {
                    if(pl_note[i] == 120)
                    {
                        trigger_note_off = TRUE;
                        if(Note_Sub_Channels[ct][i] != -1)
                        {
                            j = Reserved_Sub_Channels[ct][i];

#if defined(PTK_INSTRUMENTS)
                            if(sp_Stage[ct][j] == PLAYING_SAMPLE) sp_Stage[ct][j] = PLAYING_SAMPLE_NOTEOFF;
#endif

#if defined(PTK_SYNTH)
                            Synthesizer[ct][j].NoteOff();
#endif

                            Reserved_Sub_Channels[ct][i] = -1;
                            Note_Sub_Channels[ct][i] = -1;

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
                            if(Midi_Current_Notes[CHAN_MIDI_PRG[ct]][i])
                            {
                                Midi_NoteOff(ct, Midi_Current_Notes[CHAN_MIDI_PRG[ct]][i]);
                                Midi_Current_Notes[CHAN_MIDI_PRG[ct]][i] = 0;
                            }
#endif
#endif
                        }


                    }
                }

#if defined(PTK_303)
                if(trigger_note_off)
                {
                    noteoff303(ct); // 303 Note Off...
                }
#endif

            } // Channels loop

#if defined(PTK_303)
            Go303();
#endif

#if defined(PTK_FX_PATTERNLOOP)
            DoEffects_tick0_b();
#endif

        }// Pos in tick == 0

        if(!SubCounter) DoEffects();

        SubCounter++;

        if(SubCounter > SamplesPerSub - 1)
        {
            SubCounter = 0;
            Subicounter++;
        }

        PosInTick++;

#if defined(PTK_SHUFFLE)
        if(PosInTick > SamplesPerTick + shufflestep)
        {
            shuffleswitch = -shuffleswitch;

            if(shuffleswitch == 1)
            {
                shufflestep = -((SamplesPerTick * shuffle) / 200);
            }
            else
            {
                shufflestep = (SamplesPerTick * shuffle) / 200;
            }
#else
        if(PosInTick > SamplesPerTick)
        {
#endif

            SubCounter = 0;
            PosInTick = 0;

#if defined(PTK_FX_PATTERNBREAK)
            if(Patbreak > 127)
            {
                ped_line++;
            }
            else
            {

#if !defined(__STAND_ALONE__)
                if(is_recording_2)
                {
                    Next_Line_Pattern_Auto(&cPosition, Patbreak, &ped_line);
                }
                else
#endif
                {

#if !defined(__STAND_ALONE__)
                    if(!plx)
#endif
                    {
                        cPosition++;
                    }
                }

                // Pattern break
                ped_line = Patbreak;

#if !defined(__STAND_ALONE__)
                if(!is_recording_2)
#endif
                {
                    if(cPosition >= sLength)
                    {
                        cPosition = 0;
#if defined(__WINAMP__)
                        done = 1;
#endif
                    }
                }

#if !defined(__STAND_ALONE__)
                for(i = 0; i < MAX_TRACKS; i++)
                {
                    CHAN_HISTORY_STATE[cPosition][i] = FALSE;
                }
#endif

#if defined(PTK_FX_PATTERNLOOP)
                repeat_loop_pos = -1;
                repeat_loop_counter = 0;
                repeat_loop_counter_in = 0;
#endif

            }
#else
            ped_line++;
#endif  // PTK_FX_PATTERNBREAK

            if(ped_line == patternLines[pSequence[cPosition]])
            {

#if !defined(__STAND_ALONE__)
                if(is_recording_2)
                {
                    Next_Line_Pattern_Auto(&cPosition, patternLines[pSequence[cPosition]], &ped_line);
                }
                else
#endif
                {

#if !defined(__STAND_ALONE__)
                    if(!plx)
#endif
                    {
                        cPosition++;
                    }
                }

                // Normal end of pattern
                ped_line = 0;

#if !defined(__STAND_ALONE__)
                if(!is_recording_2)
#endif
                {
                    if(cPosition >= sLength)
                    {
                        cPosition = 0;
#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
                        done = 1;
#endif
                    }
                }

#if !defined(__STAND_ALONE__)
                for(i = 0; i < MAX_TRACKS; i++)
                {
                    CHAN_HISTORY_STATE[cPosition][i] = FALSE;
                }
#endif

#if defined(PTK_FX_PATTERNLOOP)
                repeat_loop_pos = -1;
				repeat_loop_counter = 0;
                repeat_loop_counter_in = 0;
#endif

            }

            // Delayed pattern

#if !defined(__WINAMP__)
            if(Songplaying_Pattern)
            {

#if defined(PTK_FX_PATTERNBREAK)
                if(Patbreak2 > 127)
                {
                    ped_line_delay++;
                }
                else
                {

#if !defined(__STAND_ALONE__)
                    if(!plx)
#endif
                    {
                        cPosition_delay++;
                    }
                    if(cPosition_delay >= sLength) cPosition_delay = 0;

                    ped_line_delay = Patbreak2;
                }
#else
                ped_line_delay++;
#endif // PTK_FX_PATTERNBREAK

				if(ped_line_delay == patternLines[pSequence[cPosition_delay]])
                {

#if !defined(__STAND_ALONE__)
                    if(!plx)
#endif

                    {
                        cPosition_delay++;
                    }
                    if(cPosition_delay >= sLength) cPosition_delay = 0;

                    ped_line_delay = 0;
                }
            }

#endif // __WINAMP__

        }
    }

    for(c = 0; c < Songtracks; c++)
    {

#if defined(PTK_FX_VIBRATO)
        Vibcounter[c]++;
#endif

        grown = FALSE;
        gotsome = FALSE;

        All_Signal_L = 0;
        All_Signal_R = 0;

        // -------------------------------------------

        for(i = 0; i < Channels_Polyphony[c]; i++)
        {
            Curr_Signal_L[i] = 0;
            Curr_Signal_R[i] = 0;
            Done_CVol[c][i] = FALSE;

#if defined(PTK_INSTRUMENTS)

            // Play a sample
            if(sp_Stage[c][i] == PLAYING_SAMPLE || sp_Stage[c][i] == PLAYING_SAMPLE_NOTEOFF)
            {

#if defined(PTK_SYNTH)

                if(!Synth_Was[c][i]) goto ByPass_Wav;

                if((Synthesizer[c][i].Data.OSC1_WAVEFORM != WAVEFORM_WAV &&
                    Synthesizer[c][i].Data.OSC2_WAVEFORM != WAVEFORM_WAV))
                {
ByPass_Wav:
#endif
                    if(sp_Stage[c][i] == PLAYING_SAMPLE_NOTEOFF)
                    {
                        // Note Stop
                        sp_Tvol[c][i] = 0.0f;
                        if(sp_Cvol[c][i] < 0.01f)
                        {
                            sp_Stage[c][i] = PLAYING_NOSAMPLE;
                        }
                    }
                    if(sp_Cvol[c][i] > sp_Tvol[c][i])
                    {
                        sp_Cvol[c][i] -= 0.004f;
                        Done_CVol[c][i] = TRUE;
                    }
                    else
                    {
                        sp_Cvol[c][i] += 0.004f;
                        Done_CVol[c][i] = TRUE;
                    }
                    if(sp_Cvol[c][i] > 1.0f) sp_Cvol[c][i] = 1.0f;
                    if(sp_Cvol[c][i] < 0.0f) sp_Cvol[c][i] = 0.0f;

                    res_dec = sp_Position[c][i].half.last;

                    // We had some signal (on any channel)
                    gotsome = TRUE;

                    Current_Pointer = sp_Position[c][i].half.first;

#if !defined(__STAND_ALONE__)
                    // Fix a bug that occurs when loading samples while playing
                    if(Current_Pointer >= Player_NS[c][i] - 1) Current_Pointer = Player_NS[c][i] - 1;
#endif

                    if(Current_Pointer) Old_Pointer = Current_Pointer - 1;
                    else Old_Pointer = 0;

                    if(Player_WL[c][i])
                    {
#if defined(__STAND_ALONE__) && !defined(__WINAMP__)
#if defined(PTK_USE_CUBIC)
                        Curr_Signal_L[i] = Cubic_Work(*(Player_WL[c][i] + Old_Pointer),
                                                      *(Player_WL[c][i] + Current_Pointer),
                                                      *(Player_WL[c][i] + Current_Pointer + 1),
                                                      *(Player_WL[c][i] + Current_Pointer + 2),
                                                      res_dec, Current_Pointer,
                                                      Rns[c][i]) * sp_Cvol[c][i] * Player_SV[c][i];
#else
                        Curr_Signal_L[i] = (*(Player_WL[c][i] + Current_Pointer)
                                            * sp_Cvol[c][i] * Player_SV[c][i]);
#endif

#else
                        if(Use_Cubic)
                        {
                            Curr_Signal_L[i] = Cubic_Work(*(Player_WL[c][i] + Old_Pointer),
                                                          *(Player_WL[c][i] + Current_Pointer),
                                                          *(Player_WL[c][i] + Current_Pointer + 1),
                                                          *(Player_WL[c][i] + Current_Pointer + 2),
                                                          res_dec, Current_Pointer,
                                                          Rns[c][i]) * sp_Cvol[c][i] * Player_SV[c][i];
                        }
                        else
                        {
                            Curr_Signal_L[i] = (*(Player_WL[c][i] + Current_Pointer)
                                                * sp_Cvol[c][i] * Player_SV[c][i]);
                        }
#endif
                    }

                    // Stereo sample
                    if(Player_SC[c][i] == 2)
                    {
                        grown = TRUE;

#if defined(__STAND_ALONE__) && !defined(__WINAMP__)
#if defined(PTK_USE_CUBIC)
                        Curr_Signal_R[i] = Cubic_Work(*(Player_WR[c][i] + Old_Pointer),
                                                      *(Player_WR[c][i] + Current_Pointer),
                                                      *(Player_WR[c][i] + Current_Pointer + 1),
                                                      *(Player_WR[c][i] + Current_Pointer + 2),
                                                      res_dec, Current_Pointer,
                                                      Rns[c][i]) * sp_Cvol[c][i] * Player_SV[c][i];
#else
                        Curr_Signal_R[i] = (*(Player_WR[c][i] + Current_Pointer)
                                            * sp_Cvol[c][i] * Player_SV[c][i]);
#endif

#else
                        if(Use_Cubic)
                        {
                            Curr_Signal_R[i] = Cubic_Work(*(Player_WR[c][i] + Old_Pointer),
                                                          *(Player_WR[c][i] + Current_Pointer),
                                                          *(Player_WR[c][i] + Current_Pointer + 1),
                                                          *(Player_WR[c][i] + Current_Pointer + 2),
                                                          res_dec, Current_Pointer,
                                                          Rns[c][i]) * sp_Cvol[c][i] * Player_SV[c][i];
                        }
                        else
                        {
                            Curr_Signal_R[i] = (*(Player_WR[c][i] + Current_Pointer)
                                                * sp_Cvol[c][i] * Player_SV[c][i]);
                        }
#endif
                    }

                    // End of Interpolation algo
                    if(Player_LW[c][i] == SMP_LOOPING_BACKWARD)
                    {
                        if((int) sp_Position[c][i].half.first > 0)
                        {
                            sp_Position[c][i].absolu -= Vstep1[c][i];
                        }
                    }
                    else
                    {
                        sp_Position[c][i].absolu += Vstep1[c][i];
                    }

                    switch(Player_LT[c][i])
                    {
                        case SMP_LOOP_FORWARD:
                            if(Player_LW[c][i] == SMP_LOOPING_FORWARD)
                            {
                                if(sp_Position[c][i].half.first >= Player_LE[c][i])
                                {
                                    sp_Position[c][i].half.first = Player_LS[c][i];
                                }
                            }
                            else
                            {
                                if((int) sp_Position[c][i].half.first <= (int) Player_LS[c][i])
                                {
                                    sp_Position[c][i].half.first = Player_LE[c][i];
                                }
                            }
                            break;

                        case SMP_LOOP_PINGPONG:
                            if(Player_LW[c][i] == SMP_LOOPING_FORWARD)
                            {
                                if(sp_Position[c][i].half.first >= Player_LE[c][i])
                                {
                                    sp_Position[c][i].half.first = Player_LE[c][i];
                                    Player_LW[c][i] = SMP_LOOPING_BACKWARD;
                                }
                            }
                            else
                            {
                                if((int) sp_Position[c][i].half.first <= (int) Player_LS[c][i])
                                {
                                    Player_LW[c][i] = SMP_LOOPING_FORWARD;
                                    sp_Position[c][i].half.first = Player_LS[c][i];
                                }
                            }
                            break;

                        case SMP_LOOP_NONE:
                            if(Player_LW[c][i] == SMP_LOOPING_FORWARD)
                            {
                                if(sp_Position[c][i].half.first >= Player_NS[c][i])
                                {
                                    sp_Position[c][i].half.first = Player_NS[c][i];
                                    sp_Stage[c][i] = PLAYING_NOSAMPLE;
                                }
                            }
                            else
                            {
                                if((int) sp_Position[c][i].half.first <= 0)
                                {
                                    sp_Position[c][i].half.first = 0;
                                    sp_Stage[c][i] = PLAYING_NOSAMPLE;
                                }
                            }
                            break;
                    }

#if defined(PTK_SYNTH)
                }
#endif

            }
#endif

#if defined(PTK_SYNTH)
            // Synth
            if(Synthesizer[c][i].ENV1_STAGE || Synthesizer[c][i].ENV2_STAGE)
            {
                if(!Done_CVol[c][i])
                {
                    if(sp_Cvol[c][i] > sp_Tvol[c][i]) sp_Cvol[c][i] -= 0.004f;
                    else sp_Cvol[c][i] += 0.004f;
                    if(sp_Cvol[c][i] > 1.0f) sp_Cvol[c][i] = 1.0f;
                    if(sp_Cvol[c][i] < 0.0f) sp_Cvol[c][i] = 0.0f;
                }

                Curr_Signal_L[i] += Synthesizer[c][i].GetSample(Player_WL[c][i],
                                                                Player_WR[c][i],
                                                                Player_SC[c][i],
                                                                Player_LT[c][i],
                                                                Player_LT[c][i] > SMP_LOOP_NONE ? Player_LE[c][i]: Player_NS[c][i],
                                                                Player_LT[c][i] > SMP_LOOP_NONE ? Player_LL[c][i]: 0,
                                                                &Curr_Signal_R[i],
                                                                Rns[c][i],
                                                                sp_Cvol[c][i] * Player_SV[c][i],
                                                                &sp_Stage2[c][i],
                                                                &sp_Stage3[c][i],
                                                                (Uint64 *) &sp_Position_osc1[c][i],
                                                                (Uint64 *) &sp_Position_osc2[c][i],
#if defined(PTK_SYNTH_OSC3)
                                                                (Uint64 *) &sp_Position_osc3[c][i],
#endif
                                                                Vstep1[c][i]
                                                               );

                if((Synthesizer[c][i].Data.OSC1_WAVEFORM == WAVEFORM_WAV ||
                    Synthesizer[c][i].Data.OSC2_WAVEFORM == WAVEFORM_WAV))
                {
                    if(Player_SC[c][i] == 2) grown = TRUE;
                }
                gotsome = TRUE;
            }
#endif // PTK_SYNTH

            // Gather the signals of all the sub channels
            All_Signal_L += Curr_Signal_L[i];
            All_Signal_R += Curr_Signal_R[i];
        }

#if defined(PTK_303)
        if(track3031 == c && CHAN_ACTIVE_STATE[cPosition][c])
        {
            Signal_303 = tb303engine[0].tbGetSample();
            All_Signal_L += Signal_303;
            if(grown) All_Signal_R += Signal_303;
            gotsome = TRUE;
        }
        if(track3032 == c && CHAN_ACTIVE_STATE[cPosition][c])
        {
            Signal_303 += tb303engine[1].tbGetSample();
            All_Signal_L += Signal_303;
            if(grown) All_Signal_R += Signal_303;
            gotsome = TRUE;
        }
#endif

        // Send a note off if the channel is being turned off
        if(!CHAN_ACTIVE_STATE[cPosition][c]

#if !defined(__STAND_ALONE__)
           && !CHAN_HISTORY_STATE[cPosition][c]
#endif

          )
        {

#if !defined(__STAND_ALONE__)
            CHAN_HISTORY_STATE[cPosition][c] = TRUE;
#endif

            // We send a note off to all sub channels
            for(i = 0; i < Channels_Polyphony[c]; i++)
            {

#if defined(PTK_INSTRUMENTS)
                if(sp_Stage[c][i] == PLAYING_SAMPLE)
                {
                    sp_Stage[c][i] = PLAYING_SAMPLE_NOTEOFF;
                }
#endif

#if defined(PTK_SYNTH)
                Synthesizer[c][i].NoteOff();
#endif

            }

#if defined(PTK_303)
            noteoff303(c);
#endif

#if !defined(__STAND_ALONE__)
    #if !defined(__NO_MIDI__)
            Midi_NoteOff(c, -1);
    #endif
#endif

        }

        // A rather clumsy cross fading to avoid the most outrageous clicks
        // (i also tried with splines but didn't hear any difference)
        if(New_Instrument[c] && Channels_Polyphony[c] == 1)
        {
            All_Signal_L = (All_Signal_L * (1.0f - Segue_Volume[c])) + (Segue_SamplesL[c] * Segue_Volume[c]);
            All_Signal_R = (All_Signal_R * (1.0f - Segue_Volume[c])) + (Segue_SamplesR[c] * Segue_Volume[c]);
            Pos_Segue[c]++;
            Segue_Volume[c] -= 1.0f / 127.0f;
            if(Pos_Segue[c] >= 128)
            {
                New_Instrument[c] = FALSE;
            }
        }
        else
        {
            // Store the transition
            Segue_SamplesL[c] = All_Signal_L;
            Segue_SamplesR[c] = All_Signal_R;
        }

        // -----------------------------------------------
        if(gotsome)
        {

#if defined(PTK_TRACKFILTERS)
            if(FType[c] != 4)
            { // Track filter actived
                float const dfi = TCut[c] - CCut[c];

                if(dfi < -1.0f || dfi > 1.0f) CCut[c] += dfi * ICut[c];

                if(FType[c] < 4)
                {

#if defined(PTK_FILTER_LOHIBAND)
                    gco = (int) (ApplyLfo(CCut[c] - ramper[c], c));

                    ramper[c] += Player_FD[c] * gco * 0.015625f;

                    coef[0] = coeftab[0][gco][FRez[c]][FType[c]];
                    coef[1] = coeftab[1][gco][FRez[c]][FType[c]];
                    coef[2] = coeftab[2][gco][FRez[c]][FType[c]];
                    coef[3] = coeftab[3][gco][FRez[c]][FType[c]];
                    coef[4] = coeftab[4][gco][FRez[c]][FType[c]];

                    All_Signal_L = Filter(All_Signal_L + 1.0f, c);
                    if(grown) All_Signal_R = Filter(All_Signal_R + 1.0f, c);
#endif

                }
                else
                {
                    float const realcut = ApplyLfo(CCut[c] - ramper[c], c);

                    ramper[c] += Player_FD[c] * realcut * 0.015625f;

                    switch(FType[c])
                    {

#if defined(PTK_FILTER_LO24)
                        case 5:
                            All_Signal_L = filter2p(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            if(grown) All_Signal_R = filter2p(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_LO48)
                        case 6:
                            All_Signal_L = filter2p(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            All_Signal_L = filter2p24d(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            if(grown)
                            {
                                All_Signal_R = filter2p(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                                All_Signal_R = filter2p24d(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                            }
                            break;
#endif

#if defined(PTK_FILTER_LP24)
                        case 7:
                            All_Signal_L = filter2p(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            if(grown) All_Signal_R = filter2p24d(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_AMODM)
                        case 8:
                            All_Signal_L = filterRingMod(c, All_Signal_L, realcut, (float) FRez[c]);
                            if(grown) All_Signal_R = filterRingMod(c, All_Signal_R, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_AMODS)
                        case 9:
                            if(grown)
                            {
                                All_Signal_L = filterRingMod(c, All_Signal_L, realcut, (float) FRez[c]);
                                All_Signal_R = filterRingModStereo(c, All_Signal_R);
                            }
                            else
                            {
                                All_Signal_R = All_Signal_L;
                                All_Signal_L = filterRingMod(c, All_Signal_L, realcut, (float) FRez[c]);
                                All_Signal_R = filterRingModStereo(c, All_Signal_R);
                                grown = TRUE;
                            }
                            break;
#endif

#if defined(PTK_FILTER_SINGLEM)
                        case 10:
                            All_Signal_L = filterWater(c, All_Signal_L, realcut);
                            if(grown) All_Signal_R = filterWater(c, All_Signal_R, realcut);
                            break;
#endif

#if defined(PTK_FILTER_SINGLES)
                        case 11:
                            All_Signal_L = filterWater(c, All_Signal_L, realcut);
                            if(grown) All_Signal_R = filterWaterStereo(c, All_Signal_R, realcut);
                            break;
#endif

#if defined(PTK_FILTER_EQM15)
                        case 12:
                            All_Signal_L = filterBellShaped(c, All_Signal_L, realcut, (float) FRez[c], -15);
                            if(grown) All_Signal_R = filterBellShaped(c, All_Signal_R, realcut, (float) FRez[c], -15);
                            break;
#endif

#if defined(PTK_FILTER_EQM6)
                        case 13:
                            All_Signal_L = filterBellShaped(c, All_Signal_L, realcut, (float) FRez[c], -6);
                            if(grown) All_Signal_R = filterBellShaped(c, All_Signal_R, realcut, (float) FRez[c], -6);
                            break;
#endif

#if defined(PTK_FILTER_EQP6)
                        case 14:
                            All_Signal_L = filterBellShaped(c, All_Signal_L, realcut, (float) FRez[c], 6);
                            if(grown) All_Signal_R = filterBellShaped(c, All_Signal_R, realcut, (float) FRez[c], 6);
                            break;
#endif

#if defined(PTK_FILTER_EQP15)
                        case 15:
                            All_Signal_L = filterBellShaped(c, All_Signal_L, realcut, (float) FRez[c], 15);
                            if(grown) All_Signal_R = filterBellShaped(c, All_Signal_R, realcut, (float) FRez[c], 15);
                            break;
#endif

#if defined(PTK_FILTER_DELTA)
                        case 16:
                            All_Signal_L = filterDelta(c, All_Signal_L, realcut, (float) FRez[c]);
                            if(grown) All_Signal_R = filterDelta(c, All_Signal_R, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_DISTL)
                        case 17:
                            All_Signal_L = int_filter2p(c, All_Signal_L, realcut, (float) FRez[c], 0.25f);
                            if(grown) All_Signal_R = int_filter2p(c, All_Signal_R, realcut, (float) FRez[c], 0.25f);
                            break;
#endif

#if defined(PTK_FILTER_DISTM)
                        case 18:
                            All_Signal_L = int_filter2p(c, All_Signal_L, realcut, (float) FRez[c], 0.56f);
                            if(grown) All_Signal_R = int_filter2p(c, All_Signal_R, realcut, (float) FRez[c], 0.56f);
                            break;
#endif

#if defined(PTK_FILTER_DISTH)
                        case 19:
                            All_Signal_L = int_filter2p(c, All_Signal_L, realcut, (float) FRez[c], 0.78f);
                            if(grown) All_Signal_R = int_filter2p(c, All_Signal_R, realcut, (float) FRez[c], 0.78f);
                            break;
#endif

#if defined(PTK_FILTER_DIST)
                        case 20:
                            All_Signal_L = int_filter2p(c, All_Signal_L, realcut, (float) FRez[c], 0.96f);
                            if(grown) All_Signal_R = int_filter2p(c, All_Signal_R, realcut, (float) FRez[c], 0.96f);
                            break;
#endif

#if defined(PTK_FILTER_HP12M)
                        case 21:
                            All_Signal_L = filterhp(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            if(grown) All_Signal_R = filterhp(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_HP12S)
                        case 22:
                            All_Signal_L = filterhp(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            if(grown) All_Signal_R = filterhp2(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_HP24M)
                        case 23:
                            All_Signal_L = filterhp(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            All_Signal_L = filterhp2(c, All_Signal_L + 1.0f, realcut, (float) FRez[c]);
                            if(grown)
                            {
                                All_Signal_R = filterhp(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                                All_Signal_R = filterhp2(c, All_Signal_R + 1.0f, realcut, (float) FRez[c]);
                            }
                            break;
#endif

                    } //SWITCHCASE [FILTERS]
                }

            } // Filter end
#endif // PTK_TRACKFILTERS

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
            if(CHAN_MUTE_STATE[c])
            {
                All_Signal_L = 0;
            }
#endif
            // Duplicate the mono signal if necessary
            if(!grown) All_Signal_R = All_Signal_L;

            // Dry Send
#if defined(PTK_DISCLAP)
            if(Disclap[c])
            {   // Distortion
                if(All_Signal_L > DThreshold[c]) All_Signal_L = DClamp[c];
                else if(All_Signal_L < -DThreshold[c]) All_Signal_L = -DClamp[c];

                if(All_Signal_R > DThreshold[c]) All_Signal_R = DClamp[c];
                else if(All_Signal_R < -DThreshold[c]) All_Signal_R = -DClamp[c];
            }
#endif

            All_Signal_L *= LVol[c];
            All_Signal_R *= RVol[c];

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
            if(CHAN_MUTE_STATE[c])
            {
                All_Signal_L = 0;
                All_Signal_R = 0;
            }
#endif
        }

#if defined(PTK_FLANGER)
        // 32-Bit HQ Interpolated System Flanger
        if(FLANGER_ON[c])
        {
            FLANGE_LEFTBUFFER[c][FLANGER_OFFSET[c]] = All_Signal_L * FLANGER_AMOUNT[c] + oldspawn[c] * FLANGER_FEEDBACK[c];
            FLANGE_RIGHTBUFFER[c][FLANGER_OFFSET[c]] = All_Signal_R * FLANGER_AMOUNT[c] + roldspawn[c] * FLANGER_FEEDBACK[c];

#if defined(__PSP__)
            // May be a bit faster
            float fstep1 = powf(2.0f,
                                SIN[(int) ((FLANGER_GR[c] / 6.283185f) * 360.0f)] * FLANGER_AMPL[c]);
            float fstep2 = powf(2.0f,
                                SIN[(int) (((FLANGER_GR[c] / 6.283185f) +
                                            (FLANGER_DEPHASE[c] / 3.1415927f)
                                           ) * 360.0f
                                          )] * FLANGER_AMPL[c]);
#else
            float fstep1 = powf(2.0f, sinf(FLANGER_GR[c]) * FLANGER_AMPL[c]);
            float fstep2 = powf(2.0f, sinf(FLANGER_GR[c] + FLANGER_DEPHASE[c]) * FLANGER_AMPL[c]);
#endif

            foff2[c] += fstep1;
            foff1[c] += fstep2;  

            if(foff2[c] >= 16384.0f) foff2[c] = 0.0f;
            if(foff1[c] >= 16384.0f) foff1[c] = 0.0f;
            if(foff2[c] < 0.0f) foff2[c] = 0.0f;
            if(foff1[c] < 0.0f) foff1[c] = 0.0f;

            oldspawn[c] = FLANGE_LEFTBUFFER[c][(int) (foff2[c])];
            roldspawn[c] = FLANGE_RIGHTBUFFER[c][(int) (foff1[c])];

            All_Signal_L += Filter_FlangerL(oldspawn[c]);
            All_Signal_R += Filter_FlangerR(roldspawn[c]);

            if(++FLANGER_OFFSET[c] > 16383) FLANGER_OFFSET[c] = 0;
            FLANGER_GR[c] += FLANGER_RATE[c];

            if(FLANGER_GR[c] >= 6.283185f)
            {
                FLANGER_GR[c] -= 6.283185f;
                foff2[c] = float(FLANGER_OFFSET[c] - FLANGER_DELAY[c]);
                foff1[c] = float(FLANGER_OFFSET[c] - FLANGER_DELAY[c]);
            }
        }
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
        if(CHAN_MUTE_STATE[c])
        {
            All_Signal_L = 0;
            All_Signal_R = 0;
        }
#endif

        left_float += All_Signal_L;
        right_float += All_Signal_R;

#if defined(PTK_COMPRESSOR)
        // Sending to delay...
        float const DS = DSend[c];

        if(DS > 0.008f)
        {
            delay_left_final += All_Signal_L * DS;
            delay_right_final += All_Signal_R * DS;
        }
#endif

        // Sending to chorus
        float const DC = CCoef[c];

        if(DC > 0.008f)
        {
            left_chorus += All_Signal_L * DC;
            right_chorus += All_Signal_R * DC;
        }

#if !defined(__STAND_ALONE__)
#if !defined(__WINAMP__)
        if(Songplaying_Pattern)
#endif
        {
            if(!CHAN_MUTE_STATE[c])
            {
                Scope_Dats[c][Cscope[c]] = (All_Signal_L + All_Signal_R) * 0.15f;
            }
            else
            {
                Scope_Dats[c][Cscope[c]] = 0.0f;
            }
            Cscope[c]++;
            Cscope[c] &= 127;
        }
#endif
    }

}

// ------------------------------------------------------
// Look for a free sub channel
int Get_Free_Sub_Channel(int channel, int polyphony)
{
    int i;

    for(i = 0; i < polyphony; i++)
    {
        if(
#if defined(PTK_INSTRUMENTS)
           sp_Stage[channel][i] == PLAYING_NOSAMPLE 
#else
           TRUE
#endif
#if defined(PTK_SYNTH)
           && sp_Stage2[channel][i] == PLAYING_NOSAMPLE 
           && sp_Stage3[channel][i] == PLAYING_NOSAMPLE
#endif
          )
        {
            return(i);
        }
    }

#if defined(PTK_INSTRUMENTS)
    for(i = 0; i < polyphony; i++)
    {
        if(sp_Stage[channel][i] == PLAYING_SAMPLE_NOTEOFF)
        {
            return(i);
        }
    }
#endif
    
    // None found
    return(-1);
}

// ------------------------------------------------------
// Play a waveform
void Play_Instrument(int channel, int sub_channel,
                     int inote, int sample,
                     float vol, unsigned int offset,
                     int glide, int Play_Selection, int midi_sub_channel)
{

#if defined(PTK_FX_AUTOFADEMODE)
    FADEMODE[channel] = 0;
#endif

    int Cur_Position;
    int note2;
    float note;
    int associated_sample;
    int no_retrig_adsr = FALSE;

    // Empty row ? Take the current one.
    if(sample == 255)
    {
        sample = (unsigned char) sp_channelsample[channel][sub_channel];
        vol = sp_Tvol[channel][sub_channel];
        if(glide) no_retrig_adsr = TRUE;
    }

#if defined(PTK_SYNTH)
    switch(Synthprg[sample])
    {
        case SYNTH_WAVE_OFF:          // synth off
        case SYNTH_WAVE_CURRENT:      // Current waveform selected
            associated_sample = sample;
            break;
        default:                      // Any other waveform selected
            associated_sample = Synthprg[sample] - 2;
            break;
    }
#else
    associated_sample = sample;
#endif

    Cur_Position = cPosition;

#if defined(PTK_SYNTH)
    if(Synthesizer[channel][sub_channel].Data.OSC1_WAVEFORM == WAVEFORM_WAV)
    {
        sp_Position[channel][sub_channel].absolu = 0;
        sp_Position_osc1[channel][sub_channel].absolu = 0;

#if defined(PTK_SYNTH_OSC3)
        sp_Position_osc3[channel][sub_channel].absolu = 0;
#endif

    }
    if(Synthesizer[channel][sub_channel].Data.OSC2_WAVEFORM == WAVEFORM_WAV)
    {
        sp_Position[channel][sub_channel].absolu = 0;
        sp_Position_osc2[channel][sub_channel].absolu = 0;
    }
#endif

    // Check if the channel have to be played
    if(CHAN_ACTIVE_STATE[Cur_Position][channel])
    {
        int split = 0;

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
        int mnote = inote;
#endif
#endif

#if defined(PTK_INSTRUMENTS)
        for(int revo = 0; revo < 16; revo++)
        {
            if(inote >= Basenote[associated_sample][revo] &&
               SampleType[associated_sample][revo] != 0)
            {
                split = revo;
            }
        }
#endif

        if(sample != sp_channelsample[channel][sub_channel])
        {
            glide = 0;
        }

        note2 = inote - 48;

#if defined(PTK_INSTRUMENTS)
        note = (float) inote;
        note -= Basenote[associated_sample][split];
        note += float((float) Finetune[associated_sample][split] * 0.0078125f);
#endif

#if defined(PTK_SYNTH)
        if(!no_retrig_adsr)
        {
            Synthesizer[channel][sub_channel].Data.OSC1_WAVEFORM = WAVEFORM_NONE;
            Synthesizer[channel][sub_channel].Data.OSC2_WAVEFORM = WAVEFORM_NONE;
            if(Synthprg[sample])
            {
#if defined(__STAND_ALONE__) && !defined(__WINAMP__)
                Synthesizer[channel][sub_channel].ChangeParameters(&PARASynth[sample]);
#else
                Synthesizer[channel][sub_channel].ChangeParameters(PARASynth[sample]);
#endif
                Synthesizer[channel][sub_channel].NoteOn(note2,
                                                         vol,
                                                         LoopType[associated_sample][split],
                                                         LoopType[associated_sample][split] > SMP_LOOP_NONE ? LoopEnd[associated_sample][split]: (SampleNumSamples[associated_sample][split] - 2),
                                                         LoopEnd[associated_sample][split] - LoopStart[associated_sample][split]
#if defined(PTK_INSTRUMENTS)
                                                         ,note
#endif
                                                         );
            }
        }
#endif

        // Fix a bug as this can also be used for synth
        // which isn't correct
        Player_SC[channel][sub_channel] = 0;

        sp_Tvol[channel][sub_channel] = vol;
        double spreadnote = (double) powf(2.0f, note2 / 12.0f);
        spreadnote *= 4294967296.0f;

#if defined(PTK_FX_ARPEGGIO)
        Vstep_arp[channel][sub_channel] = (int64) spreadnote;
        Arpeggio_BaseNote[channel][sub_channel] = (float) note2;
#endif

#if defined(PTK_FX_VIBRATO)
        Vstep_vib[channel][sub_channel] = (int64) spreadnote;
        Vibrato_BaseNote[channel][sub_channel] = (float) note2;
#endif

        if(glide)
        {
            sp_Step[channel][sub_channel] = (int64) spreadnote;
        }
        else
        {
            Vstep1[channel][sub_channel] = (int64) spreadnote;
            sp_Step[channel][sub_channel] = (int64) spreadnote;
        }

        // Only synth
#if defined(PTK_INSTRUMENTS)
        sp_Stage[channel][sub_channel] = PLAYING_NOSAMPLE;
#endif

#if defined(PTK_SYNTH)
        if(Synthprg[sample] == SYNTH_WAVE_OFF)
        {
            sp_Stage2[channel][sub_channel] = PLAYING_NOSAMPLE;
            sp_Stage3[channel][sub_channel] = PLAYING_NOSAMPLE;
        }
        else
        {
            sp_Stage2[channel][sub_channel] = PLAYING_STOCK;
            sp_Stage3[channel][sub_channel] = PLAYING_STOCK;
        }
#endif

        sp_channelsample[channel][sub_channel] = sample;

#if defined(PTK_INSTRUMENTS)
        if(SampleType[associated_sample][split])
        {

#if defined(PTK_SYNTH)
            if(Synthprg[sample])
            {
                // Synth + sample if both are != wav
                if(Synthesizer[channel][sub_channel].Data.OSC1_WAVEFORM != WAVEFORM_WAV &&
                   Synthesizer[channel][sub_channel].Data.OSC2_WAVEFORM != WAVEFORM_WAV)
                {
                    sp_Stage[channel][sub_channel] = PLAYING_SAMPLE;
                }
            }
            else
            {
#endif

                // Only sample
                sp_Stage[channel][sub_channel] = PLAYING_SAMPLE;

#if defined(PTK_SYNTH)
                sp_Stage2[channel][sub_channel] = PLAYING_NOSAMPLE;
                sp_Stage3[channel][sub_channel] = PLAYING_NOSAMPLE;
#endif

#if defined(PTK_SYNTH)
            }
#endif

            if(!offset) 
            {
            //sp_Cvol[channel][sub_channel] = 0;
            }
            else sp_Cvol[channel][sub_channel] = vol;

#if defined(PTK_FX_ARPEGGIO)
            Arpeggio_BaseNote[channel][sub_channel] = note;
#endif

#if defined(PTK_FX_VIBRATO)
            Vibrato_BaseNote[channel][sub_channel] = note;
#endif

            if(beatsync[associated_sample])
            {
                double spreadnote = (double) (SampleNumSamples[associated_sample][split]) / ((double) beatlines[associated_sample] * (double) SamplesPerTick);
                spreadnote *= 4294967296.0f;

#if defined(PTK_FX_ARPEGGIO)
                Vstep_arp[channel][sub_channel] = (int64) spreadnote;
#endif

#if defined(PTK_FX_VIBRATO)
                Vstep_vib[channel][sub_channel] = (int64) spreadnote;
#endif

                Vstep1[channel][sub_channel] = (int64) spreadnote;
                sp_Step[channel][sub_channel] = (int64) spreadnote;
            }
            else
            {
                double spreadnote = (double) powf(2.0f, note / 12.0f);
                spreadnote *= 4294967296.0f;

#if defined(PTK_FX_ARPEGGIO)
                Vstep_arp[channel][sub_channel] = (int64) spreadnote;
#endif

#if defined(PTK_FX_VIBRATO)
                Vstep_vib[channel][sub_channel] = (int64) spreadnote;
#endif

                if(glide)
                {
                    sp_Step[channel][sub_channel] = (int64) spreadnote;
                }
                else
                {
                    Vstep1[channel][sub_channel] = (int64) spreadnote;
                    sp_Step[channel][sub_channel] = (int64) spreadnote;
                }
            }

#if defined(PTK_SYNTH)
            Synth_Was[channel][sub_channel] = Synthprg[sample];
#endif

            sp_split[channel][sub_channel] = split;

            // Player Pointer Assignment

#if !defined(__STAND_ALONE__)
            Uint32 Sel_Start;
            Uint32 Sel_End;
            
            // Only play the selection
            if(userscreen == USER_SCREEN_SAMPLE_EDIT &&
               sed_range_start != sed_range_end && Play_Selection)
            {
                Sel_Start = sed_range_start;
                Sel_End = sed_range_end;
                if(sed_range_start > sed_range_end)
                {
                    Sel_End = sed_range_start;
                    Sel_Start = sed_range_end;
                }
                Player_LS[channel][sub_channel] = Sel_Start;
                Player_LE[channel][sub_channel] = Sel_End;
                sp_Position[channel][sub_channel].half.first = Sel_Start;
                Player_NS[channel][sub_channel] = Sel_End;
                if(!glide) sp_Position[channel][sub_channel].half.first += offset << 8;
            }
            else
            {
                Player_LS[channel][sub_channel] = LoopStart[associated_sample][split];
                Player_LE[channel][sub_channel] = LoopEnd[associated_sample][split];
                Player_NS[channel][sub_channel] = SampleNumSamples[associated_sample][split] - 2;
                if(!glide) sp_Position[channel][sub_channel].half.first = offset << 8;
            }
#else
            Player_LS[channel][sub_channel] = LoopStart[associated_sample][split];
            Player_LE[channel][sub_channel] = LoopEnd[associated_sample][split];
            Player_NS[channel][sub_channel] = SampleNumSamples[associated_sample][split] - 2;
            if(!glide) sp_Position[channel][sub_channel].half.first = offset << 8;
#endif
            Player_LL[channel][sub_channel] = Player_LE[channel][sub_channel] - Player_LS[channel][sub_channel];

            Rns[channel][sub_channel] = SampleNumSamples[associated_sample][split];
            Player_SV[channel][sub_channel] = SampleVol[associated_sample][split];
            Player_LT[channel][sub_channel] = LoopType[associated_sample][split];
            Player_LW[channel][sub_channel] = SMP_LOOPING_FORWARD;
            Player_SC[channel][sub_channel] = SampleChannels[associated_sample][split];

            // I know this isn't exactly correct but using a sub channel for this
            // would mean that we'd have to maintain 1 filters state / sub channel which would be insane.
            Player_FD[channel] = FDecay[associated_sample][split];

            Player_WL[channel][sub_channel] = RawSamples[associated_sample][0][split];

            if(SampleChannels[associated_sample][split] == 2)
            {
                Player_WR[channel][sub_channel] = RawSamples[associated_sample][1][split];
            }
            if(!glide) ramper[channel] = 0;

            // Sample is out of range
            if((int) sp_Position[channel][sub_channel].half.first >= (int) SampleNumSamples[associated_sample][split])
            {

#if defined(PTK_INSTRUMENTS)
                sp_Stage[channel][sub_channel] = PLAYING_NOSAMPLE;
#endif

#if defined(PTK_SYNTH)
                sp_Stage2[channel][sub_channel] = PLAYING_NOSAMPLE;
                sp_Stage3[channel][sub_channel] = PLAYING_NOSAMPLE;
#endif

            }
        }
        else
#endif // PTK_INSTRUMENTS

        {
            sp_Cvol[channel][sub_channel] = 1.0f;
            Player_SV[channel][sub_channel] = 1.0f;
            if(!glide)
            {
                ramper[channel] = 0;
                sp_Position[channel][sub_channel].half.first = offset << 8;
            }
        }
        if(!no_retrig_adsr)
        {

#if defined(PTK_SYNTH)
            sp_Position_osc1[channel][sub_channel] = sp_Position[channel][sub_channel];
            sp_Position_osc2[channel][sub_channel] = sp_Position[channel][sub_channel];

#if defined(PTK_SYNTH_OSC3)
            sp_Position_osc3[channel][sub_channel] = sp_Position[channel][sub_channel];
#endif
#endif

        }
        Pos_Segue[channel] = 0;
        Segue_Volume[channel] = 1.0f;
        New_Instrument[channel] = TRUE;

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
        if(CHAN_MUTE_STATE[channel] == 0 &&
           c_midiout != -1 &&
           Midiprg[associated_sample] != -1)
        {
            // Remove the previous note
            if(midi_sub_channel >= 1 && Midi_Current_Notes[CHAN_MIDI_PRG[channel]][midi_sub_channel - 1])
            {
                Midi_NoteOff(channel, Midi_Current_Notes[CHAN_MIDI_PRG[channel]][midi_sub_channel - 1]);
                Midi_Current_Notes[CHAN_MIDI_PRG[channel]][midi_sub_channel - 1] = 0;
            }

            // Set the midi program if it was modified
            if(LastProgram[CHAN_MIDI_PRG[channel]] != Midiprg[associated_sample])
            {
                Midi_Send(192 + CHAN_MIDI_PRG[channel], Midiprg[associated_sample], 127);
                LastProgram[CHAN_MIDI_PRG[channel]] = Midiprg[associated_sample];
            }

            // Send the note to the midi device
            float veloc = vol * mas_vol * local_mas_vol * local_ramp_vol;

            Midi_Send(144 + CHAN_MIDI_PRG[channel], mnote, (int) (veloc * 127));
            if(midi_sub_channel < 0) Midi_Current_Notes[CHAN_MIDI_PRG[channel]][(-midi_sub_channel) - 1] = mnote;
            else Midi_Current_Notes[CHAN_MIDI_PRG[channel]][midi_sub_channel - 1] = mnote;
        }
#endif // __NO_MIDI
#endif // __STAND_ALONE__

    }
}

// ------------------------------------------------------
// Handle patterns effects
#if defined(PTK_FX_TICK0)
void DoEffects_tick0(void)
{
    int temp_ped_line = ped_line;
    int i;

    for(int trackef = 0; trackef < Songtracks; trackef++)
    {
        int tefactor = Get_Pattern_Offset(pSequence[cPosition], trackef, temp_ped_line);
        int pltr_eff_row = *(RawPatterns + tefactor + PATTERN_FX);
        int pltr_dat_row = *(RawPatterns + tefactor + PATTERN_FXDATA);

        switch(pltr_eff_row)
        {

#if defined(PTK_FX_ARPEGGIO)
            // $1b arpeggio switch on/off
            case 0x1b:

                Arpeggio_Switch[trackef] = pltr_dat_row;
                if(!pltr_dat_row)
                {
                    for(i = 0; i < Channels_Polyphony[trackef]; i++)
                    {
                        Vstep1[trackef][i] = Vstep_arp[trackef][i];
                    }
                }
                break;
#endif

#if defined(PTK_FX_VIBRATO)
            // $1c vibrato switch on/off
            case 0x1d:

                Vibcounter[trackef] = 0;
                Vibrato_Switch[trackef] = pltr_dat_row;
                if(!pltr_dat_row)
                {
                    for(i = 0; i < Channels_Polyphony[trackef]; i++)
                    {
                        Vstep1[trackef][i] = Vstep_vib[trackef][i];
                    }
                }
                break;
#endif

#if defined(PTK_FX_REVERSE)
            case 0x1e:

                Reverse_Switch[trackef] = TRUE;
                break;
#endif
        }
    }      
}

#if defined(PTK_FX_PATTERNLOOP)
void DoEffects_tick0_b(void)
{
    int temp_ped_line = ped_line;

    for(int trackef = 0; trackef < Songtracks; trackef++)
    {
        int tefactor = Get_Pattern_Offset(pSequence[cPosition], trackef, temp_ped_line);
        int pltr_eff_row = *(RawPatterns + tefactor + PATTERN_FX);
        int pltr_dat_row = *(RawPatterns + tefactor + PATTERN_FXDATA);

        switch(pltr_eff_row)
        {
            // $06 Pattern loop
            case 0x6:
                if(!pltr_dat_row)
                {
                    if(repeat_loop_counter == 0)
                    {
                        repeat_loop_pos = ped_line;
                    }
                }
                else
                {
                    if(repeat_loop_pos != -1)
                    {
                        if(repeat_loop_counter == 0)
                        {
                            repeat_loop_counter = (int) pltr_dat_row;
                            repeat_loop_pos = (ped_line - repeat_loop_pos);
                            repeat_loop_counter_in = 1;
                        }
                        else
                        {
                            // count down
                            repeat_loop_counter--;
                            if(!repeat_loop_counter)
                            {
                                repeat_loop_pos = -1;
                                repeat_loop_counter_in = 0;
                            }
                            else
                            {
                                repeat_loop_counter_in = 1;
                            }
                        }
                    }
                } 
                break;

        }
    }      
}
#endif

#endif

// ------------------------------------------------------
// Process ticks X effects
void DoEffects(void)
{
    int i;
    int j;
    int pltr_note[MAX_POLYPHONY];
    int pltr_sample[MAX_POLYPHONY];

    for(int trackef = 0; trackef < Songtracks; trackef++)
    {
        int tefactor = Get_Pattern_Offset(pSequence[cPosition], trackef, ped_line);

        for(i = 0; i < Channels_MultiNotes[trackef]; i++)
        {
            pltr_note[i] = *(RawPatterns + tefactor + PATTERN_NOTE1 + (i * 2));
            pltr_sample[i] = *(RawPatterns + tefactor + PATTERN_INSTR1 + (i * 2));
        }

#if defined(PTK_FX_NOTECUT) || defined(PTK_FX_NOTERETRIGGER)
        unsigned char pltr_vol_row = *(RawPatterns + tefactor + PATTERN_VOLUME);
#endif

#if defined(PTK_FX_0) || defined(PTK_FX_X)
        int64 pltr_eff_row = *(RawPatterns + tefactor + PATTERN_FX);
#endif

        int64 pltr_dat_row = *(RawPatterns + tefactor + PATTERN_FXDATA);

        // Sample only: only set the default volume
        if(Subicounter == 0)
        {
            for(i = 0; i < Channels_MultiNotes[trackef]; i++)
            {
                if(pltr_note[i] == 121 && pltr_sample[i] != 255)
                {
                    sp_Tvol[trackef][Reserved_Sub_Channels[trackef][i]] = CustomVol[pltr_sample[i]];
                }
            }
        }

#if defined(PTK_FX_AUTOFADEMODE)
        // Autofade routine
        switch(FADEMODE[trackef])
        {
            case 1:
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    sp_Tvol[trackef][i] += FADECOEF[trackef];

                    if(sp_Tvol[trackef][i] > 1.0f)
                    {
                        sp_Tvol[trackef][i] = 1.0f;
                        FADEMODE[trackef] = 0;
                    }
                }
                break;

            case 2:
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    sp_Tvol[trackef][i] -= FADECOEF[trackef];

                    if(sp_Tvol[trackef][i] < 0.0f)
                    {   
                        sp_Tvol[trackef][i] = 0.0f;
                        FADEMODE[trackef] = 0;
                    }
                }
                break;
        }
#endif

        // Effects

#if defined(PTK_FX_NOTECUT)
        if((pltr_vol_row & 0xf0) == 0xf0)
        { // Note Cut: Fx
            unsigned char kinder = pltr_vol_row & 0xf;
            if(Subicounter == kinder)
            {
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {

#if defined(PTK_INSTRUMENTS)
                    if(sp_Stage[trackef][i] == PLAYING_SAMPLE)
                    {
                        if(FType[trackef] == 4) sp_Stage[trackef][i] = PLAYING_SAMPLE_NOTEOFF;
                        else sp_Tvol[trackef][i] = 0.001f;
                    }
#endif

#if defined(PTK_SYNTH)
                    Synthesizer[trackef][i].NoteOff();
#endif

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
                    if(Midi_Current_Notes[CHAN_MIDI_PRG[trackef]][i])
                    {
                        Midi_NoteOff(trackef, Midi_Current_Notes[CHAN_MIDI_PRG[trackef]][i]);
                        Midi_Current_Notes[CHAN_MIDI_PRG[trackef]][i] = 0;
                    }
#endif
#endif
                }
            }
        }
#endif

#if defined(PTK_FX_0)
        switch(pltr_eff_row)
        {

#if defined(PTK_FX_PITCHUP)
            // $01 Pitch Up
            case 0x1:

                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
#if defined(__GCC__)
                    if(Vstep1[trackef][i] < 137438953472ll) Vstep1[trackef][i] += pltr_dat_row << 21;
#else
                    if(Vstep1[trackef][i] < 137438953472) Vstep1[trackef][i] += pltr_dat_row << 21;
#endif
                }
            
                break;
#endif  // PTK_FX_PITCHUP

#if defined(PTK_FX_PITCHDOWN)
            // $02 Pitch Down
            case 0x2:

                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    Vstep1[trackef][i] -= pltr_dat_row << 21;
                    if(Vstep1[trackef][i] < 16) Vstep1[trackef][i] = 16;
                }
                break;
#endif

#if defined(PTK_FX_SETVOLUME)
            // $03 Set volume
            case 0x3:

                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    sp_Tvol[trackef][i] = pltr_dat_row * 0.0039062f;
                }
                break;
#endif

#if defined(PTK_FX_TRANCESLICER)
            // $04 Trance slicer
            case 0x4:
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    if(Subicounter == 0) sp_Tvol[trackef][i] = 1.0f;

                    if(Subicounter >= pltr_dat_row) sp_Tvol[trackef][i] = 0;
                }
                break;
#endif

#if defined(PTK_FX_TRANCEGLIDER)
            // $05 Glider
            case 0x5:
                if(pltr_dat_row) glidestep[trackef] = pltr_dat_row << 21;

                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    if(Vstep1[trackef][i] < sp_Step[trackef][i])
                    {
                        Vstep1[trackef][i] += glidestep[trackef];
                        if(Vstep1[trackef][i] > sp_Step[trackef][i])
                        {
                            Vstep1[trackef][i] = sp_Step[trackef][i];
                        }
                    }
                    else
                    {
                        if(Vstep1[trackef][i] > sp_Step[trackef][i])
                        {
                            Vstep1[trackef][i] -= glidestep[trackef];
                            if(Vstep1[trackef][i] < sp_Step[trackef][i]) Vstep1[trackef][i] = sp_Step[trackef][i];
                        }
                    }
                }
                break;
#endif

        } // SWITCH CASES

#endif  // PTK_FX_0

#if defined(PTK_FX_X)
        switch(pltr_eff_row)
        {

#if !defined(__STAND_ALONE__)
            // $07 FX
            case 0x7:
                // ... nothing ...
                break;
#endif

#if defined(PTK_FX_SETCUTOFF)
            // $08 SetCutOff
            case 0x8:
                TCut[trackef] = (float) pltr_dat_row / 2.0f;
                break;
#endif

#if defined(PTK_FX_SETRANDOMCUTOFF)
            // $0a SetRandomCutOff
            case 0xa:
                if(Subicounter == 0)
                {
                    TCut[trackef] = float((float) (rand() & 0xff) * ((float) (pltr_dat_row / 255.0f)));

                    if(TCut[trackef] < 1) TCut[trackef] = 1;
                    if(TCut[trackef] > 127) TCut[trackef] = 127;
                }
                break;
#endif

#if defined(PTK_FX_SLIDEUPCUTOFF)
            // $0b SlideUpCutOff 
            case 0xb:
                if(Subicounter == 0)
                {
                    TCut[trackef] += pltr_dat_row;
                    if(TCut[trackef] > 127) TCut[trackef] = 127;
                }
                break;
#endif

#if defined(PTK_FX_SLIDEDOWNCUTOFF)
            // $0c SlideDownCutOff 
            case 0xc:
                if(Subicounter == 0)
                {
                    TCut[trackef] -= pltr_dat_row;
                    if(TCut[trackef] < 1) TCut[trackef] = 1;
                }
                break;
#endif

#if defined(PTK_FX_NOTERETRIGGER)
            // $0e Note retrigger
            case 0xe: 

                int free_sub_channel;
                
                // No new note
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    if(pltr_note[i] == 121) pltr_note[i] = old_note[trackef][i];
                    old_note[trackef][i] = pltr_note[i];
                }

                if(pltr_dat_row > 0 && (Subicounter % pltr_dat_row) == 0)
                {
                    for(i = 0; i < Channels_MultiNotes[trackef]; i++)
                    {
                        if(pl_note[i] < 120 && Note_Sub_Channels[trackef][i] != -1)
                        {
                            j = Reserved_Sub_Channels[trackef][i];

#if defined(PTK_INSTRUMENTS)
                            // Get the virtual channel it was playing on and remove it
                            if(sp_Stage[trackef][j] == PLAYING_SAMPLE)
                            {
                                sp_Stage[trackef][j] = PLAYING_SAMPLE_NOTEOFF;
                            }
#endif
#if defined(PTK_SYNTH)
                            Synthesizer[trackef][j].NoteOff();
#endif

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
                            if(Midi_Current_Notes[CHAN_MIDI_PRG[trackef]][j])
                            {
                                Midi_NoteOff(trackef, Midi_Current_Notes[CHAN_MIDI_PRG[trackef]][j]);
                                Midi_Current_Notes[CHAN_MIDI_PRG[trackef]][j] = 0;
                            }
#endif
#endif
                        }
                    }

                    for(i = 0; i < Channels_MultiNotes[trackef]; i++)
                    {
                        free_sub_channel = Get_Free_Sub_Channel(trackef, Channels_Polyphony[trackef]);
                        if(free_sub_channel == -1) free_sub_channel = i;

                        // Mark it as playing
                        Note_Sub_Channels[trackef][i] = i;
                        Reserved_Sub_Channels[trackef][i] = free_sub_channel;

                        // Retrigger all playing sub channels
                        if(pltr_vol_row <= 64)
                        {
                            Play_Instrument(trackef,
                                            free_sub_channel,
                                            pltr_note[i],
                                            pltr_sample[i],
                                            pltr_vol_row * 0.015625f,
                                            0, 0,
                                            FALSE, i + 1);
                        }
                        else
                        {
                            Play_Instrument(trackef,
                                            free_sub_channel,
                                            pltr_note[i],
                                            pltr_sample[i],
                                            CustomVol[pltr_sample[i]],
                                            0, 0,
                                            FALSE, i + 1);
                        }
                    }
                }
                break;
#endif

#if defined(PTK_FX_SETBPM)
            // $f0 Set BPM
            case 0xf0:
                if(pltr_dat_row > 32)
                {
                    BeatsPerMin = (int) pltr_dat_row;
                    SamplesPerTick = (int) ((60 * MIX_RATE) / (BeatsPerMin * TicksPerBeat));  
                    SamplesPerSub = SamplesPerTick / 6;
                }

#if !defined(__STAND_ALONE__) && !defined(__WINAMP__)
                gui_bpm_action = TRUE;
#endif

                break;
#endif

#if defined(PTK_FX_SETSPEED)
            // $0f Set speed
            case 0x0f:
                TicksPerBeat = (int) pltr_dat_row;
                if(TicksPerBeat < 1) TicksPerBeat = 1;
                if(TicksPerBeat > 16) TicksPerBeat = 16;
                SamplesPerTick = (int) ((60 * MIX_RATE) / (BeatsPerMin * TicksPerBeat));
                SamplesPerSub = SamplesPerTick / 6;

#if defined(PTK_SHUFFLE)
                if(shuffleswitch == 1) shufflestep = -((SamplesPerTick * shuffle) / 200);
                else shufflestep = (SamplesPerTick * shuffle) / 200;
#endif

#if !defined(__STAND_ALONE__) && !defined(__WINAMP__)
                gui_bpm_action = TRUE;
#endif
                break;
#endif

#if defined(PTK_FX_SENDTODELAYCOMMAND)
            // $10 Send to delay Command
            case 0x10:
                CCoef[trackef] = (float) pltr_dat_row / 255.0f;
                break;
#endif

#if defined(PTK_FX_SENDTOREVERBCOMMAND)
            // $11 Send to reverb Command
            case 0x11:
                DSend[trackef] = (float) pltr_dat_row / 255.0f;
                break;
#endif

#if defined(PTK_FX_SETDISTORTIONTHRESHOLD)
            // $12 Set distortion Threshold
            case 0x12:
                DThreshold[trackef] = (float) pltr_dat_row * 128.0f;
                break;
#endif

#if defined(PTK_FX_SETDISTORTIONCLAMP)
            // $13 Set distortion clamp
            case 0x13: 
                DClamp[trackef] = (float) pltr_dat_row * 128.0f;
                break;
#endif

#if defined(PTK_FX_SETFILTERRESONANCE)
            // $14 Set filter resonance
            case 0x14:
                FRez[trackef] = (int) (pltr_dat_row / 2);
                break;
#endif

#if defined(PTK_FX_SETFILTERTYPE)
            // $15 Set filter Type
            case 0x15:
                if(pltr_dat_row <= MAX_FILTER) FType[trackef] = (int) pltr_dat_row;
                break;
#endif

#if defined(PTK_LFO)
#if defined(PTK_FX_RESETFILTERLFO)
            // $16 Reset filter lfo
            case 0x16:
                LFOGR[trackef] = 0.0f;
                break;
#endif
#endif

#if defined(PTK_FX_AUTOFADEIN)
            // $17 Auto fade in xx ticks
            case 0x17:
                if(pltr_dat_row > 0)
                {
                    FADECOEF[trackef] = 0.1666667f / (float) pltr_dat_row;
                    FADEMODE[trackef] = 1;
                }
                break;
#endif

#if defined(PTK_FX_AUTOFADEOUT)
            // $18 Auto fade out xx ticks
            case 0x18:
                if(pltr_dat_row > 0)
                {
                    FADECOEF[trackef] = 0.1666667f / (float) pltr_dat_row;
                    FADEMODE[trackef] = 2;
                }
                break;
#endif

#if defined(PTK_FX_VOLUMESLIDEUP)
            // $19 Volume slide up
            case 0x19:
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    sp_Tvol[trackef][i] += pltr_dat_row * 0.0039062f;
                    if(sp_Tvol[trackef][i] > 1.0f) sp_Tvol[trackef][i] = 1.0f;
                }
                break;
#endif

#if defined(PTK_FX_VOLUMESLIDEDOWN)
            // $1a Volume slide down
            case 0x1a: 
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    sp_Tvol[trackef][i] -= pltr_dat_row * 0.0039062f;
                    if(sp_Tvol[trackef][i] < 0.0f) sp_Tvol[trackef][i] = 0.0f;
                }
                break;
#endif

#if defined(PTK_FX_SETGLOBALVOLUME)
            // $1a Set global volume
            case 0x1c:
                local_mas_vol = pltr_dat_row / 255.0f;
                break;
#endif
        }

#endif  // PTK_FX_X

#if defined(PTK_FX_ARPEGGIO)
        // Let's do the arpeggio
        double arpnote;

        if(Arpeggio_Switch[trackef])
        {
            for(i = 0; i < Channels_Polyphony[trackef]; i++)
            {
                switch((Subicounter % 3))
                {
                    case 0:
                        Vstep1[trackef][i] = Vstep_arp[trackef][i];
                        break;
                    case 1:
                        arpnote = (double) powf(2.0f, ((Arpeggio_BaseNote[trackef][i] + (Arpeggio_Switch[trackef] >> 4))) / 12.0f);
                        arpnote *= 4294967296.0f;
                        Vstep1[trackef][i] = (int64) arpnote;
                        break;
                    case 2:
                        arpnote = (double) powf(2.0f, ((Arpeggio_BaseNote[trackef][i] + (Arpeggio_Switch[trackef] & 0xf))) / 12.0f);
                        arpnote *= 4294967296.0f;
                        Vstep1[trackef][i] = (int64) arpnote;
                        break;
                }
            }
        }
#endif

#if defined(PTK_FX_VIBRATO)
        // Let's do the vibrato
        double vibnote;
        float vib_speed;
        float vib_amp;

        if(Vibrato_Switch[trackef])
        {
            vib_speed = ((float) (Vibrato_Switch[trackef] >> 4)) * (float) Vibcounter[trackef];
            vib_speed = sinf((vib_speed * 0.00045f) / (3.14159f * 2.0f));
            vib_amp = (float) (Vibrato_Switch[trackef] & 0xf) * 0.09f;
            vib_speed *= vib_amp;

            for(i = 0; i < Channels_Polyphony[trackef]; i++)
            {
                vibnote = (double) powf(2.0f, ((Vibrato_BaseNote[trackef][i] + vib_speed)) / 12.0f);
                vibnote *= 4294967296.0f;
                Vstep1[trackef][i] = (int64) vibnote;
            }
        }
#endif

#if defined(PTK_FX_REVERSE)
        if(Reverse_Switch[trackef])
        {
            Reverse_Switch[trackef] = FALSE;
            if(pltr_dat_row)
            {
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    if(sp_Stage[trackef][i] != PLAYING_NOSAMPLE)
                    {
                        Player_LW[trackef][i] = SMP_LOOPING_BACKWARD;
                    }
                    if(sp_Stage2[trackef][i] != PLAYING_NOSAMPLE)
                    {
                        Synthesizer[trackef][i].ENV1_LOOP_BACKWARD = TRUE;
                        Synthesizer[trackef][i].ENV3_LOOP_BACKWARD = TRUE;
                    }
                    if(sp_Stage3[trackef][i] != PLAYING_NOSAMPLE)
                    {
                        Synthesizer[trackef][i].ENV2_LOOP_BACKWARD = TRUE;
                    }

                    if(Player_LT[trackef][i] == SMP_LOOP_NONE || Player_LT[trackef][i] == SMP_LOOP_PINGPONG)
                    {
                        int Max_Loop = Player_NS[trackef][i];
                        if(Player_LT[trackef][i] != SMP_LOOP_NONE) if((int) Player_LE[trackef][i] < Max_Loop) Max_Loop = Player_LE[trackef][i];
                        if(Player_LW[trackef][i] == SMP_LOOPING_BACKWARD)
                        {
                            if(sp_Position[trackef][i].half.first == 0)
                            {
                                sp_Position[trackef][i].half.first = Max_Loop;
                            }
                        }

                        if(Synthesizer[trackef][i].ENV1_LOOP_BACKWARD)
                        {
                            if(Synthesizer[trackef][i].Data.OSC1_WAVEFORM == WAVEFORM_WAV)
                            {
                                sp_Position_osc1[trackef][i].half.first = Max_Loop;
                                sp_Position_osc3[trackef][i].half.first = Max_Loop;
                            }
                        }
                        if(Synthesizer[trackef][i].ENV2_LOOP_BACKWARD)
                        {
                            if(Synthesizer[trackef][i].Data.OSC2_WAVEFORM == WAVEFORM_WAV)
                            {
                                sp_Position_osc2[trackef][i].half.first = Max_Loop;
                            }
                        }
                    }
                }
            }
            else
            {
                for(i = 0; i < Channels_Polyphony[trackef]; i++)
                {
                    if(sp_Stage[trackef][i] != PLAYING_NOSAMPLE)
                    {
                        Player_LW[trackef][i] = SMP_LOOPING_FORWARD;
                    }
                    if(sp_Stage2[trackef][i] != PLAYING_NOSAMPLE) 
                    {
                        Synthesizer[trackef][i].ENV1_LOOP_BACKWARD = FALSE;
                        Synthesizer[trackef][i].ENV3_LOOP_BACKWARD = FALSE;
                    }
                    if(sp_Stage3[trackef][i] != PLAYING_NOSAMPLE) 
                    {
                        Synthesizer[trackef][i].ENV2_LOOP_BACKWARD = FALSE;
                    }
                }
            }
        }
#endif

    }// FOR LOOP ON TRACKS
}

// ------------------------------------------------------
// Reset the tracks filters
void ResetFilters(int tr)
{
    buf024[tr] = 0.0f;
    buf124[tr] = 0.0f;
    buf0[tr] = 0.0f;
    buf1[tr] = 0.0f;
    fx1[tr] = 0.0f;
    fx2[tr] = 0.0f;
    fy1[tr] = 0.0f;
    fy2[tr] = 0.0f;
    xi0[tr] = 0.0f;
    xi1[tr] = 0.0f;
    xi2[tr] = 0.0f;
}

// ------------------------------------------------------
// Process LFO
float ApplyLfo(float cy, int trcy)
{

#if defined(PTK_LFO)
    if(LFO_ON[trcy] == 1)
    {
        cy += SIN[(int) (LFOGR[trcy])] * LFO_AMPL[trcy];
        LFOGR[trcy] += LFO_RATE[trcy];
        if(LFOGR[trcy] >= 360.0f) LFOGR[trcy] -= 360.0f;
    }
#endif

    if(cy < 1.0f) cy = 1.0f;
    if(cy > 126.0f) cy = 126.0f;
    return cy;
}

// ------------------------------------------------------
// Stereo volume table
void ComputeStereo(int channel)
{
    LVol[channel] = sqrtf(1.0f - TPan[channel]);
    RVol[channel] = sqrtf(TPan[channel]);
}

// ------------------------------------------------------
// Main mixing routine
void GetPlayerValues(void)
{
    left_chorus = 0.0f;
    right_chorus = 0.0f;
    Sp_Player();

    if(++lchorus_counter > (MIX_RATE * 2)) lchorus_counter = MIX_RATE;
    if(++rchorus_counter > (MIX_RATE * 2)) rchorus_counter = MIX_RATE;
    lbuff_chorus[lchorus_counter] = left_chorus + lbuff_chorus[lchorus_counter2] * lchorus_feedback;
    rbuff_chorus[rchorus_counter] = right_chorus + rbuff_chorus[rchorus_counter2] * rchorus_feedback;
    if(++lchorus_counter2 > (MIX_RATE * 2)) lchorus_counter2 = MIX_RATE;
    if(++rchorus_counter2 > (MIX_RATE * 2)) rchorus_counter2 = MIX_RATE;
    float rchore = lbuff_chorus[lchorus_counter2];
    float lchore = rbuff_chorus[rchorus_counter2];
    left_float += lchore;
    right_float += rchore;

#if defined(PTK_COMPRESSOR)
    Compressor_work();
#endif

    left_float /= 32767.0f;
    right_float /= 32767.0f;

#if defined(PTK_LIMITER)
    mas_comp_pos_rms_buffer++;
    if(mas_comp_pos_rms_buffer > MAS_COMPRESSOR_SIZE - 1) mas_comp_pos_rms_buffer = 0;

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    if(mas_comp_ratio > 0.01f)
    {
#else
    if(mas_ratio > 0.01f)
    {
#endif
        left_float = Mas_Compressor(left_float, &rms_sumL, mas_comp_bufferL, &mas_envL);
        right_float = Mas_Compressor(right_float, &rms_sumR, mas_comp_bufferR, &mas_envR);
    }
#endif

    left_float *= mas_vol;
    right_float *= mas_vol;

    if(local_curr_mas_vol >= local_mas_vol)
    {
        local_curr_mas_vol -= 0.003f;
        if(local_curr_mas_vol < 0.0f) local_curr_mas_vol = 0.0f;
    }
    else
    {
        local_curr_mas_vol += 0.003f;
        if(local_curr_mas_vol > 1.0f) local_curr_mas_vol = 1.0f;
    }

    left_float *= local_curr_mas_vol;
    right_float *= local_curr_mas_vol;

    if(local_curr_ramp_vol >= local_ramp_vol)
    {
        local_curr_ramp_vol -= 0.003f;
        if(local_curr_ramp_vol < 0.0f) local_curr_ramp_vol = 0.0f;
    }
    else
    {
        local_curr_ramp_vol += 0.003f;
        if(local_curr_ramp_vol > 1.0f) local_curr_ramp_vol = 1.0f;
    }

    left_float *= local_curr_ramp_vol;
    right_float *= local_curr_ramp_vol;

    if(left_float > 1.0f) left_float = 1.0f;
    if(left_float < -1.0f) left_float = -1.0f;
    if(right_float > 1.0f) right_float = 1.0f;
    if(right_float < -1.0f) right_float = -1.0f;

    left_float_render = left_float;
    right_float_render = right_float;

#if !defined(__MACOSX__) // Only float32 on mac

    #if defined(__LINUX__)
        // It looks like the maximum range for OSS is -8192 +8192
        // (higher than that will produce heavily satured signals)
        left_float *= 8192.0f;
        right_float *= 8192.0f;
    #else
        left_float *= 32767.0f;
        right_float *= 32767.0f;
    #endif

    left_value = (int) left_float;
    right_value = (int) right_float;

#endif // __MACOSX__
}

// ------------------------------------------------------
// Precalc filters coefficients
void ComputeCoefs(int freq, int r, int t)
{
    float omega = float(2 * PI * Kutoff(freq) / fMIX_RATE);
    float sn = (float) sinf(omega);
    float cs = (float) cosf(omega);
    float alpha;
    float a0;
    float a1;
    float a2;
    float b0;
    float b1;
    float b2;

    if(t < 2) alpha = float(sn / Resonance(r * (freq + 70.0f) / (127.0f + 70.0f)));
    else alpha = float(sn * sinhf(Bandwidth(r) * omega / sn));

    switch(t)
    {
        default:
        case 0: // LP
            b0 =  (1.0f - cs) / 2.0f;
            b1 =   1.0f - cs;
            b2 =  (1.0f - cs) / 2.0f;
            a0 =   1.0f + alpha;
            a1 =  -2.0f * cs;
            a2 =   1.0f - alpha;
            break;

        case 1: // HP
            b0 =  (1.0f + cs) / 2.0f;
            b1 = -(1.0f + cs);
            b2 =  (1.0f + cs) / 2.0f;
            a0 =   1.0f + alpha;
            a1 =  -2.0f * cs;
            a2 =   1.0f - alpha;
            break;

        case 2: // BP
            b0 =   alpha;
            b1 =   0.0f;
            b2 =  -alpha;
            a0 =   1.0f + alpha;
            a1 =  -2.0f * cs;
            a2 =   1.0f - alpha;
            break;

        case 3: // BR
            b0 =   1.0f;
            b1 =  -2.0f * cs;
            b2 =   1.0f;
            a0 =   1.0f + alpha;
            a1 =  -2.0f * cs;
            a2 =   1.0f - alpha;
            break;
    }

    coef[0] = b0 / a0;
    coef[1] = b1 / a0;
    coef[2] = b2 / a0;
    coef[3] = -a1 / a0;
    coef[4] = -a2 / a0;
}

// ------------------------------------------------------
// Filters run

#if defined(PTK_PROC_FILTER)
float Filter(float x, char i)
{
    float y;

    y = coef[0] * x + coef[1] * fx1[i] + coef[2] * fx2[i] + coef[3] * fy1[i] + coef[4] * fy2[i];
    fy2[i] = fy1[i];
    fy1[i] = y;
    fx2[i] = fx1[i];
    fx1[i] = x;
    return y;
}
#endif

float Kutoff(int v)
{
    return powf((v + 5.0f) / (127.0f + 5.0f), 1.7f) * 13000.0f + 30.0f;
}

float Resonance(float v)
{
    return powf(v / 127.0f, 4.0f) * 150.0f + 0.1f;
}

float Bandwidth(int v)
{
    return powf(v / 127.0f, 4.0f) * 4.0f + 0.1f;
}

#if defined(PTK_PROC_FILTER2P)
float filter2p(int ch, float input, float f, float q)
{
    f *= 0.0078125f;
    q *= 0.0078125f;

    float fa = float(1.0f - f); 
    float fb = float(q * (1.0f + (1.0f / fa)));
    buf0[ch] = fa * buf0[ch] + f * (input + fb * (buf0[ch] - buf1[ch])); 
    buf1[ch] = fa * buf1[ch] + f * buf0[ch];
    return buf1[ch];  
}
#endif

#if defined(PTK_PROC_FILTERHP2)
float filterhp2(int ch, float input, float f, float q)
{
    f *= 0.0078125f;
    q *= 0.0078125f;

    float fa = float(1.0f - f);
    float fb = float(q * (1.0f + (1.0f / fa)));
    buf024[ch] = fa * buf024[ch] + f * (input + fb * (buf024[ch] - buf124[ch])); 
    buf124[ch] = fa * buf124[ch] + f * buf024[ch];
    return input - buf124[ch];
}
#endif

#if defined(PTK_PROC_FILTERHP)
float filterhp(int ch, float input, float f, float q)
{
    f *= 0.0078125f;
    q *= 0.0078125f;

    float fa = float(1.0f - f); 
    float fb = float(q * (1.0f + (1.0f / fa)));
    buf0[ch] = fa * buf0[ch] + f * (input + fb * (buf0[ch] - buf1[ch])); 
    buf1[ch] = fa * buf1[ch] + f * buf0[ch];
    return input - buf1[ch];  
}
#endif

#if defined(PTK_PROC_FILTER2P24D)
float filter2p24d(int ch, float input, float f, float q)
{
    f *= 0.0078125f;
    q *= 0.0078125f;
    float fa = float(1.0f - f); 
    float fb = float(q * (1.0f + (1.0f / fa)));
    buf024[ch] = fa * buf024[ch] + f * (input + fb * (buf024[ch] - buf124[ch])); 
    buf124[ch] = fa * buf124[ch] + f * buf024[ch];
    return buf124[ch];  
}
#endif

#if defined(PTK_PROC_FILTERRINGMOD)
float filterRingMod(int ch, float input, float f, float q)
{
    q++;

    f = float(f * 0.0078125f);
    buf0[ch] += f * (q * 0.125f);
    if(buf0[ch] >= 360.0f) buf0[ch] -= 360.0f;

    return input * SIN[(int) buf0[ch]];
}
#endif

#if defined(PTK_PROC_FILTERRINGMODSTEREO)
float filterRingModStereo(int ch, float input)
{
    return input * cosf(buf0[ch] * 0.0174532f);
}
#endif

#if defined(PTK_PROC_FILTERWATER)
float filterWater(int ch, float input, float f)
{
    f = 127.0f - f;
    float ad = input - buf0[ch];

    if(ad > 1.0f || ad < -1.0f) buf0[ch] += ad / f;
    return buf0[ch];
}
#endif

#if defined(PTK_PROC_FILTERWATERSTEREO)
float filterWaterStereo(int ch, float input, float f)
{
    f = 127.0f - f;
    float ad = input - buf1[ch];

    if(ad > 1.0f || ad < -1.0f) buf1[ch] += ad / f;
    return buf1[ch];
}
#endif

#if defined(PTK_PROC_FILTERDELTA)
float filterDelta(int ch, float input, float f, float q)
{
    f = 127.0f - f;
    q *= 0.007874f;

    float output = buf1[ch];
    if(buf1[ch] > 1.0f || buf1[ch] < -1.0f) buf1[ch] *= q;

    buf0[ch]++;
    if(buf0[ch] >= f)
    {
        buf0[ch] = 0.0f;
        output = input;
        buf1[ch] = input;
    }
    return output;
}
#endif

/*float filterDeltaStereo(int ch, float input, float f, float q)
{
    f = 127.0f - f;
    q *= 0.007874f;

    float output = buf124[ch];
    if(buf124[ch] > 1.0f || buf124[ch] < -1.0f) buf124[ch] *= q;

    buf024[ch]++;
    if(buf024[ch] >= f)
    {
        buf024[ch] = 0.0f;
        output = input;
        buf124[ch] = input;
    }
    return output;
}
*/

#if defined(PTK_PROC_FILTERBELLSHAPED)
float filterBellShaped(int ch, float input, float f, float q, float g)
{
    input++;
    q *= 0.007874f;

    if(q < 0.01f) q = 0.01f;
    float freq = 320.0f + (f * 127.65625f);
    float a, b, c;
    float a0, a1, a2, b1, b2;              // filter coefficients
    float Wn, Wp;
    float gain = g / 6.6f;
    if(freq > 22100.0f) freq = 22100.0f;   // apply Nyquist frequency
    Wn = 1.0f / (6.2831853f * freq);       // freq of center
    Wp = float(Wn / tanf(Wn / 88200.0f));  // prewarped frequency
    a = (Wn * Wn * Wp * Wp);
    float t1 = Wn * Wp * q;
    b = (3.0f + gain) * t1;
    c = (3.0f - gain) * t1;
    t1 = a + c + 1.0f;
    b2 = (1.0f - c + a) / t1;
    a2 = (1.0f - b + a) / t1;
    b1 = a1 = 2.0f * (1.0f - a) / t1;
    a0 = (a + b + 1.0f) / t1;
    xi0[ch] = input - b1 * xi1[ch] - b2 * xi2[ch];
    float output = a0 * xi0[ch] + a1 * xi1[ch] + a2 * xi2[ch];
    xi2[ch] = xi1[ch];
    xi1[ch] = xi0[ch];
    return output;
}
#endif

#if defined(PTK_PROC_FILTERINT2P)
float int_filter2p(int ch, float input, float f, float q, float q2)
{
    q *= 0.0787401f;
    input = filter2px(ch, input, f, q2);
    return float(32767.0f * powf(fabsf(input) / 32767.0f, 1.0f - q / 11.0f));
}

float filter2px(int ch, float input, float f, float q)
{
    f *= 0.0078125f;
    float fa = float(1.0f - f);
    float fb = float(q * (1.0f + (1.0f / fa)));
    buf0[ch] = fa * buf0[ch] + f * (input + fb * (buf0[ch] - buf1[ch]));
    buf1[ch] = fa * buf1[ch] + f * buf0[ch];
    return buf1[ch];
}
#endif

// ------------------------------------------------------
// Record the 303 parameters changes

#if defined(PTK_303)
void live303(int pltr_eff_row, int pltr_dat_row)
{
    switch(pltr_eff_row)
    {
        case 0x33: tb303[0].cutoff = pltr_dat_row / 2; break;
        case 0x34: tb303[1].cutoff = pltr_dat_row / 2; break;
        case 0x35: tb303[0].resonance = pltr_dat_row / 2; break;
        case 0x36: tb303[1].resonance = pltr_dat_row / 2; break;
        case 0x37: tb303[0].envmod = pltr_dat_row / 2; break;
        case 0x38: tb303[1].envmod = pltr_dat_row / 2; break;
        case 0x39: tb303[0].decay = pltr_dat_row / 2; break;
        case 0x3a: tb303[1].decay = pltr_dat_row / 2; break;
        case 0x3b: tb303[0].accent = pltr_dat_row / 2; break;
        case 0x3c: tb303[1].accent = pltr_dat_row / 2; break;
        case 0x3d: tb303[0].tune = pltr_dat_row / 2; break;
        case 0x3e: tb303[1].tune = pltr_dat_row / 2; break;
        case 0x41: tb303engine[0].RampVolume = ((float) pltr_dat_row) / 255.0f; break;
        case 0x42: tb303engine[1].RampVolume = ((float) pltr_dat_row) / 255.0f; break;
    }

#if !defined(__STAND_ALONE__) && !defined(__WINAMP__)
    if(!sr_isrecording) Actualize_303_Ed(0);
#endif

}

void Fire303(unsigned char number, int unit)
{
    tb303engine[unit].tbLine = 0;
   
    switch(number)
    {
        case 0x00: tb303engine[unit].tbPattern = tb303[unit].selectedpattern; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa1: tb303engine[unit].tbPattern = 0; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa2: tb303engine[unit].tbPattern = 1; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa3: tb303engine[unit].tbPattern = 2; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa4: tb303engine[unit].tbPattern = 3; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa5: tb303engine[unit].tbPattern = 4; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa6: tb303engine[unit].tbPattern = 5; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa7: tb303engine[unit].tbPattern = 6; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xa8: tb303engine[unit].tbPattern = 7; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb1: tb303engine[unit].tbPattern = 8; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb2: tb303engine[unit].tbPattern = 9; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb3: tb303engine[unit].tbPattern = 10; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb4: tb303engine[unit].tbPattern = 11; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb5: tb303engine[unit].tbPattern = 12; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb6: tb303engine[unit].tbPattern = 13; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb7: tb303engine[unit].tbPattern = 14; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xb8: tb303engine[unit].tbPattern = 15; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc1: tb303engine[unit].tbPattern = 16; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc2: tb303engine[unit].tbPattern = 17; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc3: tb303engine[unit].tbPattern = 18; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc4: tb303engine[unit].tbPattern = 19; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc5: tb303engine[unit].tbPattern = 20; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc6: tb303engine[unit].tbPattern = 21; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc7: tb303engine[unit].tbPattern = 22; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xc8: tb303engine[unit].tbPattern = 23; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd1: tb303engine[unit].tbPattern = 24; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd2: tb303engine[unit].tbPattern = 25; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd3: tb303engine[unit].tbPattern = 26; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd4: tb303engine[unit].tbPattern = 27; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd5: tb303engine[unit].tbPattern = 28; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd6: tb303engine[unit].tbPattern = 29; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd7: tb303engine[unit].tbPattern = 30; tb303engine[unit].RampVolume = 1.0f; break;
        case 0xd8: tb303engine[unit].tbPattern = 31; tb303engine[unit].RampVolume = 1.0f; break;

        default: // No Fire
            tb303engine[unit].tbLine = 255;
            tb303engine[unit].RampVolume = 0.0f;
            break;
    }

#if !defined(__STAND_ALONE__) && !defined(__WINAMP__)
    if(!sr_isrecording) Actualize_303_Ed(0);
#endif

}

void noteoff303(char strack)
{
    if(strack == track3031)
    {
        tb303engine[0].RampVolume = 0.0f;
        track3032 = 255;
    }

    if(strack == track3032)
    {
        tb303engine[1].RampVolume = 0.0f;
        track3032 = 255;
    }
}

void Go303(void)
{
    if(tb303engine[0].tbPattern != 255)
    {
        tb303engine[0].tbNoteOn(tb303[0].tone[tb303engine[0].tbPattern][tb303engine[0].tbLine], &tb303[0]);
        tb303engine[0].tbLine++;
        if(tb303engine[0].tbLine == tb303[0].patternlength[tb303engine[0].tbPattern]) tb303engine[0].tbLine = 0;
    }

    if(tb303engine[1].tbPattern != 255)
    {
        tb303engine[1].tbNoteOn(tb303[1].tone[tb303engine[1].tbPattern][tb303engine[1].tbLine], &tb303[1]);
        tb303engine[1].tbLine++;
        if(tb303engine[1].tbLine == tb303[1].patternlength[tb303engine[1].tbPattern]) tb303engine[1].tbLine = 0; 
    }
}
#endif

// ------------------------------------------------------
// Bank initializer
#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
void init_sample_bank(void)
{
    Reset_303_Parameters(&tb303[0]);
    Reset_303_Parameters(&tb303[1]);

    for(int inico = 0; inico < 256; inico++)
    {
        // All tracks activated on entire song (1-Active 0-Inactive)
        for(int inico2 = 0; inico2 < MAX_TRACKS; inico2++)
        {
            CHAN_ACTIVE_STATE[inico][inico2] = TRUE;
            CHAN_HISTORY_STATE[inico][inico2] = FALSE;
        }
        pSequence[inico] = 0;

        if(inico < 128)
        {
            beatsync[inico] = FALSE;
            beatlines[inico] = 16;

#if !defined(__WINAMP__)
            sprintf(nameins[inico], "Untitled");
#endif

            ResetSynthParameters(&PARASynth[inico]);

            KillInst(inico);
        }
    }
}

void KillInst(int inst_nbr)
{

#if !defined(__NO_CODEC__)
    // Gsm is the default packing scheme
    SampleCompression[inst_nbr] = SMP_PACK_GSM;
#else
    SampleCompression[inst_nbr] = SMP_PACK_NONE;
#endif

#if defined(PTK_MP3)
    Mp3_BitRate[inst_nbr] = 0;
#endif

#if defined(PTK_AT3)
    At3_BitRate[inst_nbr] = 0;
#endif

#if !defined(__NO_CODEC__) && !defined(__STAND_ALONE__)
    SamplesSwap[inst_nbr] = FALSE;
#endif

    beatsync[inst_nbr] = FALSE;
    beatlines[inst_nbr] = 16;

    for(int z = 0; z < 16; z++)
    {
        if(RawSamples[inst_nbr][0][z])
        {
            if(RawSamples[inst_nbr][0][z]) free(RawSamples[inst_nbr][0][z]);
        }
        RawSamples[inst_nbr][0][z] = NULL;
        if(SampleChannels[inst_nbr][z] == 2)
        {
            if(RawSamples[inst_nbr][1][z])
            {
                if(RawSamples[inst_nbr][1][z]) free(RawSamples[inst_nbr][1][z]);
            }
            RawSamples[inst_nbr][1][z] = NULL;
        }

#if !defined(__NO_CODEC__)
#if !defined(__STAND_ALONE__) && !defined(__WINAMP__)
        if(RawSamples_Swap[inst_nbr][0][z])
        {
            if(RawSamples_Swap[inst_nbr][0][z]) free(RawSamples_Swap[inst_nbr][0][z]);
        }
        RawSamples_Swap[inst_nbr][0][z] = NULL;
        if(SampleChannels[inst_nbr][z] == 2)
        {
            if(RawSamples_Swap[inst_nbr][1][z])
            {
                if(RawSamples_Swap[inst_nbr][1][z]) free(RawSamples_Swap[inst_nbr][1][z]);
            }
            RawSamples_Swap[inst_nbr][1][z] = NULL;
        }
#endif
#endif

        SampleChannels[inst_nbr][z] = 0;
        SampleType[inst_nbr][z] = 0;
        LoopStart[inst_nbr][z] = 0;
        LoopEnd[inst_nbr][z] = 0;
        LoopType[inst_nbr][z] = SMP_LOOP_NONE;
        SampleNumSamples[inst_nbr][z] = 0;
        Finetune[inst_nbr][z] = 0;
        SampleVol[inst_nbr][z] = 0.0f;
        FDecay[inst_nbr][z] = 0.0f;
        Basenote[inst_nbr][z] = 48;

#if !defined(__STAND_ALONE__)
        sprintf(SampleName[inst_nbr][z], "Untitled");
        Midiprg[inst_nbr] = -1;
#endif

#if defined(PTK_SYNTH)
        Synthprg[inst_nbr] = SYNTH_WAVE_OFF;
#endif

        CustomVol[inst_nbr] = 1.0f;
    }
}

// ------------------------------------------------------
// Next Function: used to reset synthparameters Structure
// Well, I think the default preset is not very cool, but nah!
#if defined(PTK_SYNTH)
void ResetSynthParameters(SynthParameters *TSP)
{

#if !defined(__WINAMP__)
    sprintf(TSP->presetname, "Untitled");
#endif

    TSP->osc1_waveform = WAVEFORM_SAW;
    TSP->osc2_waveform = WAVEFORM_PULSE;
    TSP->osc1_pw = 256;
    TSP->osc2_pw = 256;
    TSP->osc2_detune = 65;
    TSP->osc2_finetune = 0;
    TSP->vcf_cutoff = 64;
    TSP->vcf_resonance = 64;
    TSP->vcf_type = 0;
    TSP->env1_attack = 0;
    TSP->env1_decay = 2560;
    TSP->env1_sustain = 20;
    TSP->env1_release = 16384;
    TSP->env2_attack = 0;
    TSP->env2_decay = 2560;
    TSP->env2_sustain = 20;
    TSP->env2_release = 16384;
    TSP->lfo1_period = 16;
    TSP->lfo2_period = 16;
    TSP->lfo1_osc1_pw = 64;
    TSP->lfo1_osc2_pw = 64;
    TSP->lfo1_osc1_pitch = 64;
    TSP->lfo1_osc2_pitch = 64;
    TSP->lfo1_osc1_volume = 64;
    TSP->lfo1_osc2_volume = 64;   
    TSP->lfo1_vcf_cutoff = 64;
    TSP->lfo1_vcf_resonance = 64; 
    TSP->lfo2_osc1_pw = 64;
    TSP->lfo2_osc2_pw = 64;
    TSP->lfo2_osc1_pitch = 64;
    TSP->lfo2_osc2_pitch = 64;
    TSP->lfo2_osc1_volume = 64;
    TSP->lfo2_osc2_volume = 64;   
    TSP->lfo2_vcf_cutoff = 64;
    TSP->lfo2_vcf_resonance = 64; 
    TSP->env1_osc1_pw = 64;
    TSP->env1_osc2_pw = 64;
    TSP->env1_osc1_pitch = 64;
    TSP->env1_osc2_pitch = 64;
    TSP->env1_osc1_volume = 127;
    TSP->env1_osc2_volume = 127;   
    TSP->env1_vcf_cutoff = 64;
    TSP->env1_vcf_resonance = 64; 
    TSP->env2_osc1_pw = 64;
    TSP->env2_osc2_pw = 64;
    TSP->env2_osc1_pitch = 64;
    TSP->env2_osc2_pitch = 64;
    TSP->env2_osc1_volume = 127;
    TSP->env2_osc2_volume = 127;
    TSP->env2_vcf_cutoff = 64;
    TSP->env2_vcf_resonance = 64; 
    TSP->osc3_volume = 128;
    TSP->osc3_switch = FALSE;
    TSP->ptc_glide = 0;
    TSP->glb_volume = 128;
    TSP->disto = 0;
    TSP->lfo1_attack = 2560;
    TSP->lfo1_decay = 2560;
    TSP->lfo1_sustain = 16;
    TSP->lfo1_release = 16384;
    TSP->lfo2_attack = 2560;
    TSP->lfo2_decay = 2560;
    TSP->lfo2_sustain = 16;
    TSP->lfo2_release = 16384;
}
#endif // PTK_SYNTH
#endif // !__STAND_ALONE__ || __WINAMP__

// ------------------------------------------------------
// Free all allocated Samples
#if defined(PTK_INSTRUMENTS)
void Free_Samples(void)
{
    for(int freer = 0; freer < MAX_INSTRS; freer++)
    {
        for(char pedsplit = 0; pedsplit < MAX_INSTRS_SPLITS; pedsplit++)
        {
            if(SampleType[freer][pedsplit] != 0)
            {
                if(RawSamples[freer][0][pedsplit]) free(RawSamples[freer][0][pedsplit]);
                RawSamples[freer][0][pedsplit] = NULL;
                if(SampleChannels[freer][pedsplit] == 2)
                {
                    if(RawSamples[freer][1][pedsplit]) free(RawSamples[freer][1][pedsplit]);
                    RawSamples[freer][1][pedsplit] = NULL;
                }
            }

#if !defined(__NO_CODEC__) && !defined(__STAND_ALONE__)
            if(SampleType[freer][pedsplit] != 0)
            {
                if(RawSamples_Swap[freer][0][pedsplit]) free(RawSamples_Swap[freer][0][pedsplit]);
                RawSamples_Swap[freer][0][pedsplit] = NULL;
                if(SampleChannels[freer][pedsplit] == 2)
                {
                    if(RawSamples_Swap[freer][1][pedsplit]) free(RawSamples_Swap[freer][1][pedsplit]);
                    RawSamples_Swap[freer][1][pedsplit] = NULL;
                }
            }
#endif

        }

#if !defined(__NO_CODEC__) && !defined(__STAND_ALONE__)
        SamplesSwap[freer] = FALSE;
#endif

    }
}
#endif

// ------------------------------------------------------
// Initialize the reverb data
#if defined(PTK_COMPRESSOR)
void Initreverb()
{
    int i;
    int j;

    LFP_L.Reset();

    for(i = 0; i < num_echoes; i++)
    {
        int mlrw = 99999 - (delays[i] * 4);

        if(mlrw < 0) mlrw = 0;

        counters[i] = mlrw;
    }
 
    for(i = 0; i < MAX_COMB_FILTERS; i++)
    {
        for(j = 0 ; j < 100000; j++)
        {
            delay_left_buffer[i][j] = 0;
            delay_right_buffer[i][j] = 0;
        }
    }

    rev_counter = 99999;
    
    InitRevervbFilter();
}

void InitRevervbFilter(void)
{
    currentCounter = 5759;
    delayedCounter = 5759 - int(c_threshold * 44.1f);
    delayedCounter2 = 5759 - int(c_threshold * 50.1f);
    delayedCounter3 = 5759 - int(c_threshold * 60.1f);
    delayedCounter4 = 5759 - int(c_threshold * 70.1f);
    delayedCounter5 = 5759 - int(c_threshold * 73.1f);
    delayedCounter6 = 5759 - int(c_threshold * 79.1f);

    if(delayedCounter < 0) delayedCounter = 0;
    if(delayedCounter2 < 0) delayedCounter2 = 0;
    if(delayedCounter3 < 0) delayedCounter3 = 0;
    if(delayedCounter4 < 0) delayedCounter4 = 0;
    if(delayedCounter5 < 0) delayedCounter5 = 0;
    if(delayedCounter6 < 0) delayedCounter6 = 0;

    for(int yb = 0; yb < 5760; yb++)
    {
        allBuffer_L[yb] = 0.0f;
        allBuffer_L2[yb] = 0.0f;
        allBuffer_L3[yb] = 0.0f;
        allBuffer_L4[yb] = 0.0f;
        allBuffer_L5[yb] = 0.0f;
        allBuffer_L6[yb] = 0.0f;
    }
}

// ------------------------------------------------------
void Compressor_work(void)
{
    if(compressor)
    {
        float l_rout = 0;

        // Comb filter
        for(int i = 0; i < num_echoes; i++)
        {
            delay_left_buffer[i][rev_counter] = (delay_left_final + delay_right_final +
                                                 delay_left_buffer[i][counters[i]]) *
                                                 decays[i];
            l_rout += delay_left_buffer[i][counters[i]];
            if(++counters[i] > 99999) counters[i] = 0;
        }

        if(++rev_counter > 99999) rev_counter = 0;

        // STEREO ALLPASS FILTER:
        float temp1;

        temp1 = LFP_L.fWork(l_rout++, REVERBFILTER);

        l_rout = (-Feedback * temp1) + allBuffer_L[delayedCounter];
        allBuffer_L[currentCounter] = (l_rout * Feedback) + temp1;

        temp1 = l_rout;
        l_rout = (-Feedback * temp1) + allBuffer_L2[delayedCounter2];
        allBuffer_L2[currentCounter] = (l_rout * Feedback) + temp1;

        temp1 = l_rout;
        l_rout = (-Feedback * temp1) + allBuffer_L3[delayedCounter3];
        allBuffer_L3[currentCounter] = (l_rout * Feedback) + temp1;

        temp1 = l_rout;
        l_rout = (-Feedback * temp1) + allBuffer_L4[delayedCounter4];
        allBuffer_L4[currentCounter] = (l_rout * Feedback) + temp1;

        temp1 = l_rout;
        l_rout = (-Feedback * temp1) + allBuffer_L5[delayedCounter5];
        allBuffer_L5[currentCounter] = (l_rout * Feedback) + temp1;

        // Stereo diffusion
        right_float += l_rout;

        temp1 = l_rout;
        l_rout = (-Feedback * temp1) + allBuffer_L6[delayedCounter6];
        allBuffer_L6[currentCounter] = (l_rout * Feedback) + temp1;

        // Updating delayed counters
        if(++delayedCounter > 5759) delayedCounter = 0;
        if(++delayedCounter2 > 5759) delayedCounter2 = 0;
        if(++delayedCounter3 > 5759) delayedCounter3 = 0;
        if(++delayedCounter4 > 5759) delayedCounter4 = 0;
        if(++delayedCounter5 > 5759) delayedCounter5 = 0;
        if(++delayedCounter6 > 5759) delayedCounter6 = 0;

        // Updating current counter
        currentCounter++;
        if(currentCounter > 5759) currentCounter = 0;

        left_float += l_rout;
    }
}
#endif

// ------------------------------------------------------
#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
void Reset_303_Parameters(para303 *tbpars)
{
    char c;
    char d;

    tbpars->enabled = 0;
    tbpars->selectedpattern = 0;
    tbpars->tune = 64;
    tbpars->cutoff = 64;
    tbpars->resonance = 64;
    tbpars->envmod = 64;
    tbpars->decay = 64;
    tbpars->accent = 64;
    tbpars->waveform = 0;

    for(c = 0; c < 32; c++)
    {
        tbpars->patternlength[c] = 16;

        for(d = 0; d < 16; d++)
        {
            tbpars->tone[c][d] = 0;
            tbpars->flag[c][d].slide_flag = 0;
            tbpars->flag[c][d].accent_flag = 0;
            tbpars->flag[c][d].transposeup_flag = 0;
            tbpars->flag[c][d].transposedown_flag = 0;
            tbpars->flag[c][d].pause = 1;
        }
#if !defined(__STAND_ALONE__)
        for(d = 0; d < 16; d++)
        {
            sprintf(tbpars->pattern_name[c], "Untitled");
        }
#endif
    }
}
#endif

// ------------------------------------------------------
// Filter flanger signal
#if defined(PTK_FLANGER)
float Filter_FlangerL(float input)
{
    float fa = 1.0f - FLANGER_LOPASS_CUTOFF; 
    float fb = float(FLANGER_LOPASS_RESONANCE * (1.0f + (1.0f / fa)));
    Flanger_sbuf0L = fa * Flanger_sbuf0L + FLANGER_LOPASS_CUTOFF * (input + fb * (Flanger_sbuf0L - Flanger_sbuf1L));
    Flanger_sbuf1L = fa * Flanger_sbuf1L + FLANGER_LOPASS_CUTOFF * Flanger_sbuf0L;
    return(Flanger_sbuf1L);
}
float Filter_FlangerR(float input)
{
    float fa = 1.0f - FLANGER_LOPASS_CUTOFF;
    float fb = float(FLANGER_LOPASS_RESONANCE * (1.0f + (1.0f / fa)));
    Flanger_sbuf0R = fa * Flanger_sbuf0R + FLANGER_LOPASS_CUTOFF * (input + fb * (Flanger_sbuf0R - Flanger_sbuf1R));
    Flanger_sbuf1R = fa * Flanger_sbuf1R + FLANGER_LOPASS_CUTOFF * Flanger_sbuf0R;
    return(Flanger_sbuf1R);
}
#endif

// ------------------------------------------------------
// Compressor / Limiter
#if defined(PTK_LIMITER)
float mas_attack = 0.977579f;
float mas_release = 0.977579f;

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
void Mas_Compressor_Set_Variables(float threshold, float ratio)
{
    if(threshold < 0.01f) threshold = 0.01f;
    if(threshold > 100.0f) threshold = 100.0f;
    if(ratio < 0.01f) ratio = 0.01f;
    if(ratio > 100.0f) ratio = 100.0f;
    mas_comp_threshold = threshold;
    mas_comp_ratio = ratio;
    mas_threshold = threshold * 0.001f;
    mas_ratio = ratio * 0.01f;
}
#endif

float Mas_Compressor(float input, float *rms_sum, float *buffer, float *env)
{
    float gain;
    float rmsf = Do_RMS(input, rms_sum, buffer);

    float theta = rmsf > *env ? mas_attack : mas_release;
    *env = (1.0f - theta) * rmsf + theta * *env;

    gain = 1.0f;
    if(*env > mas_threshold)
    {
        gain = expf((logf(mas_threshold) - logf(*env)) * mas_ratio);
    }
    return input * gain;
}

float Do_RMS(float input, float *rms_sum, float *buffer)
{
    *rms_sum -= buffer[mas_comp_pos_rms_buffer];
    buffer[mas_comp_pos_rms_buffer] = input * input;
    *rms_sum += buffer[mas_comp_pos_rms_buffer];
    if(*rms_sum < 0.0f) *rms_sum = 0.0f;
    return(sqrtf(*rms_sum / (float) MAS_COMPRESSOR_SIZE));
}
#endif

// ------------------------------------------------------
// Return an index in a pattern's module
int Get_Pattern_Offset(int pattern, int track, int row)
{
    return((track * PATTERN_BYTES) + (row * PATTERN_ROW_LEN) + (pattern * PATTERN_LEN));
}
