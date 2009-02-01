// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/replay.h"

#if !defined(__STAND_ALONE__)
#include "../../../../src/midi/include/capture_midi.h"
#include "../../../../src/include/variables.h"
#endif

// ------------------------------------------------------
// Variables
long SamplesPerTick;

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

CSynth Synthesizer[MAX_TRACKS];//[MAX_POLYPHONY];
float Player_FD[MAX_TRACKS];
char sp_channelsample[MAX_TRACKS];
char sp_split[MAX_TRACKS];
int TRACKSTATE[MAX_TRACKS];   // 0->Normal 1->Muted
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
    float FLANGER_GR[MAX_TRACKS];//[MAX_POLYPHONY];
    float FLANGER_FEEDBACK[MAX_TRACKS];
    int FLANGER_DELAY[MAX_TRACKS];
    int FLANGER_OFFSET[MAX_TRACKS];
    float foff2[MAX_TRACKS];
    float foff1[MAX_TRACKS];
    float FLANGE_LEFTBUFFER[MAX_TRACKS][16400];
    float FLANGE_RIGHTBUFFER[MAX_TRACKS][16400];
#endif

float sp_Cvol[MAX_TRACKS];
float sp_Tvol[MAX_TRACKS];
float DSend[MAX_TRACKS];   
int CSend[MAX_TRACKS];
int64 Vstep1[MAX_TRACKS];

#if defined(PTK_FX_TRANCEGLIDER)
    int64 glidestep[MAX_TRACKS];
#endif

float TPan[MAX_TRACKS];
int old_note[MAX_TRACKS];

s_access sp_Position[MAX_TRACKS];
s_access sp_Position_osc1[MAX_TRACKS];
s_access sp_Position_osc2[MAX_TRACKS];

#if defined(PTK_SYNTH_OSC3)
    s_access sp_Position_osc3[MAX_TRACKS];
#endif

float CCut[MAX_TRACKS];
float TCut[MAX_TRACKS];
float ICut[MAX_TRACKS];
float LVol[MAX_TRACKS];
float RVol[MAX_TRACKS];
int FType[MAX_TRACKS];
int FRez[MAX_TRACKS];
float DThreshold[MAX_TRACKS];
bool Disclap[MAX_TRACKS];
float DClamp[MAX_TRACKS];
float CCoef[MAX_TRACKS];
float lbuff_chorus[131072];
float rbuff_chorus[131072];
float coef[5];
float coeftab[5][128][128][4];

int Songplaying;

#if defined(PTK_303)
    gear303 tb303engine[2];
    para303 tb303[2];
    unsigned char track3031;
    unsigned char track3032;
#endif

float left_float;
float right_float;
float left_chorus;
float right_chorus;
float delay_left_final;
float delay_right_final;
int PosInTick;
bool rawrender;

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    float mas_vol = 1.0f;
#else
    float mas_vol;
#endif

int left_value;
int right_value;

#if !defined(__WINAMP__)
    int Songplaying_Pattern;
#else
    extern int done;
#endif

int Subicounter;

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

int ped_line;
unsigned char pSequence[256];
int cPosition;
int cPosition_delay;
int ped_line_delay;
int pl_eff_row2;
int pl_dat_row2;
int pl_note;
int arp_pl_note;
int pl_sample;
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

#if defined(PTK_FX_ARPEGGIO)
    int64 Vstep_arp[MAX_TRACKS];
    int Arpeggio_Switch[MAX_TRACKS];
    float Arpeggio_BaseNote[MAX_TRACKS];
#endif

int repeat_loop_pos;

int repeat_loop_counter;

short patternLines[128];
bool grown;
float currsygnal;
float currsygnal2;
unsigned int res_dec;
unsigned int Currentpointer;

char Synth_Was[MAX_TRACKS];//[MAX_POLYPHONY];
short *Player_WL[MAX_TRACKS];//[MAX_POLYPHONY];
short *Player_WR[MAX_TRACKS];//[MAX_POLYPHONY];
unsigned int Rns[MAX_TRACKS];//[MAX_POLYPHONY];
float Player_SV[MAX_TRACKS];//[MAX_POLYPHONY];
char Player_SC[MAX_TRACKS];//[MAX_POLYPHONY];
char Player_LT[MAX_TRACKS];//[MAX_POLYPHONY];
char Player_LW[MAX_TRACKS];//[MAX_POLYPHONY];
unsigned int Player_LS[MAX_TRACKS];//[MAX_POLYPHONY];
unsigned int Player_LE[MAX_TRACKS];//[MAX_POLYPHONY];
unsigned int Player_LL[MAX_TRACKS];//[MAX_POLYPHONY];
unsigned int Player_NS[MAX_TRACKS];//[MAX_POLYPHONY];

#if defined(PTK_LIMITER)
    #define MAS_COMPRESSOR_SECONDS 0.1f
    #define MAS_COMPRESSOR_SIZE (int) (MAS_COMPRESSOR_SECONDS * SAMPLESPERSEC)
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

bool SACTIVE[256][MAX_TRACKS];

#if !defined(__STAND_ALONE__)
    bool SHISTORY[256][MAX_TRACKS];
#endif

int gco;
float ramper[MAX_TRACKS];//[MAX_POLYPHONY];
char Basenote[128][16];
char SampleType[128][16];
char Finetune[128][16];
unsigned char Synthprg[128];
SynthParameters PARASynth[128];
char LoopType[128][16];
Uint32 LoopStart[128][16];
Uint32 LoopEnd[128][16];
Uint32 SampleNumSamples[128][16];
bool beatsync[128];
short beatlines[128];
int64 sp_Step[MAX_TRACKS];//[MAX_POLYPHONY];
float SampleVol[128][16];
char SampleChannels[128][16];
float FDecay[128][16];
short *RawSamples[128][2][16];

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
    float decays[MAX_COMB_FILTERS][2];
    int counters[MAX_COMB_FILTERS];
    int rev_counter;
    rFilter LFP_L;
    int delays[MAX_COMB_FILTERS];       // delays for the comb filters
#endif

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

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    float lchorus_feedback = 0.6f;
    float rchorus_feedback = 0.5f;
    int lchorus_delay = 10584;
    int rchorus_delay = 15876;
    unsigned int lchorus_counter = SAMPLESPERSEC;
    unsigned int rchorus_counter = SAMPLESPERSEC;
    unsigned int lchorus_counter2 = SAMPLESPERSEC - lchorus_delay;
    unsigned int rchorus_counter2 = SAMPLESPERSEC - rchorus_delay;
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
    char num_echoes = 1;
#else
    char num_echoes;
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    float REVERBFILTER = 0.3f;
#else
    float REVERBFILTER;
#endif

int sp_Stage[MAX_TRACKS];//[MAX_POLYPHONY];
int sp_Stage2[MAX_TRACKS];//[MAX_POLYPHONY];
int sp_Stage3[MAX_TRACKS];//[MAX_POLYPHONY];
char SampleCompression[128];
int delay_time;
int DelayType;

#if defined(PTK_FLANGER)
    float Flanger_sbuf0L;
    float Flanger_sbuf1L;
    float Flanger_sbuf0R;
    float Flanger_sbuf1R;
#endif

int new_instrument[MAX_TRACKS];//[MAX_POLYPHONY];
int Pos_Segue[MAX_TRACKS];
float segue_volume[MAX_TRACKS];
float Segue_SamplesL[MAX_TRACKS];
float Segue_SamplesR[MAX_TRACKS];

#if !defined(__STAND_ALONE__)
    int L_MaxLevel;
    int R_MaxLevel;
    extern int TRACKMIDICHANNEL[MAX_TRACKS];
    float Scope_Dats[MAX_TRACKS][128];
    float Scope_Dats_LeftRight[2][128];
    int Cscope[MAX_TRACKS];
    int pos_scope;
    extern signed char c_midiin;
    extern signed char c_midiout;
    void MidiCommand(int nbr_track, int eff_dat, int row_dat);
    void MidiNoteOff(int track);
    int plx;
    char Midiprg[128];
    int LastProgram[MAX_TRACKS];
    int wait_level;
    extern int Midi_Track_Notes[MAX_TRACKS];
    char nameins[128][20];
    char SampleName[128][16][64];
    unsigned char nPatterns = 1;
    void Actualize_303_Ed(char gode);
    extern char sr_isrecording;
    extern Uint32 sed_range_start;
    extern Uint32 sed_range_end;
#else
    unsigned char nPatterns;
#endif

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

// ------------------------------------------------------
// Functions
void allPassInit(float miliSecs);
float ApplyLfo(float cy, char trcy);
void ComputeCoefs(int freq, int r, int t);

#if defined(PTK_FX_TICK0)
    void DoEffects_tick0(void);
#endif

void DoEffects(void);
float Filter(float x, char i);
float filter2p(char ch, float input, float f, float q);
float filter2px(char ch, float input, float f, float q);
float filter2p24d(char ch, float input, float f, float q);
float filterRingMod(char ch, float input, float f, float q);
float filterRingModStereo(char ch, float input);
float filterWater(char ch, float input, float f, float q);
float filterWaterStereo(char ch, float input, float f, float q);
float filterBellShaped(char ch, float input, float f, float q, float g);
float filterDelta(char ch, float input, float f, float q);
float int_filter2p(char ch, float input, float f, float q, float q2);
float filterhp(char ch, float input, float f, float q);
float filterhp2(char ch, float input, float f, float q);
int f2i(float d);

#if defined(PTK_303)
    void live303(int pltr_eff_row, int pltr_dat_row);
    void Fire303(unsigned char number,char unit);
    void Go303(void);
#endif

float Kutoff( int v);
float Resonance(float v);
float Bandwidth(int v);
void Compressor_work(void);

#if defined(PTK_FLANGER)
    float Filter_FlangerL(float input);
    float Filter_FlangerR(float input);
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    void Mas_Compressor_Set_Variables(float treshold, float ratio);
#endif

float Mas_Compressor(float input, float *rms_sum, float *Buffer, float *Env);
float Do_RMS(float input, float *rms_sum, float *buffer);

// ------------------------------------------------------
// Audio mixer
void STDCALL Mixer(Uint8 *Buffer, Uint32 Len)
{
    short *pSamples = (short *) Buffer;
    int i;

#if !defined(__STAND_ALONE__)
    float clamp_left_value;
    float clamp_right_value;
#endif

#if !defined(__STAND_ALONE__)
    if(!rawrender)
    {
#endif
        for(i = Len - 1; i >= 0; i -= 4)
        {
            GetPlayerValues(mas_vol);

            *pSamples++ = left_value;
            *pSamples++ = right_value;

#if !defined(__STAND_ALONE__)
            // Gather datas for the scopes and the vumeters
            if(Songplaying_Pattern)
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
                    if(L_MaxLevel > 128) L_MaxLevel -= 128;
                    if(R_MaxLevel > 128) R_MaxLevel -= 128;
                }

                Scope_Dats_LeftRight[0][pos_scope] = ((float) clamp_left_value) * 0.25f;
                Scope_Dats_LeftRight[1][pos_scope] = ((float) clamp_right_value) * 0.25f;
                pos_scope++;
                pos_scope &= 127;
            }
#endif
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

    Cubic_Init();

    // Create the stock waveforms
    int i;
    float incr = 1.0f / 44100.0f;
    float stop = 2.0f;
    float x;
    short *wav_sin = STOCK_SIN;
    short *wav_saw = STOCK_SAW;
    short *wav_pul = STOCK_PUL;
    short *wav_wit = STOCK_WIT;

#if defined(PTK_SYNTH_PINKNOISE)
    short *wav_pin = STOCK_PIN;
    unsigned int newrand;
    unsigned int prevrand;
    unsigned int k;
#endif

    unsigned int seed = 0x12345678;
    unsigned int total = 0;

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    Mas_Compressor_Set_Variables(100.0f, 0.0f);
#endif

    SIZE_WAVEFORMS = 0;
    for(x = 0; x < (stop - incr / 2); x += incr)
    {
        float value = (float) ((PI * 2.0f) * x);
        //      float value2 = (float) ((PI * 2.0f) * (x * 2.0f));
        //    value2 = sinf(value + sinf(value2));

        if(sinf(value) < 0.0f) *wav_pul++ = 32767;
        else *wav_pul++ = -32767;

        *wav_saw++ = (unsigned short) (fmodf(x * 2.0f, 64.0f) * 32767.0f);

        //*wav_pul++ = (short) (value2 * 16384.0f);
        //value = (float) ((PI * 2.0) * x);
        //value2 = sinf(value + sinf(value));
        //*wav_saw++ = (short) (value2 * 16384.0f);

        *wav_sin++ = (unsigned short) (sinf(value) * 32767.0f);

        *wav_wit++ = (short) (rand() - 16384);

#if defined(PTK_SYNTH_PINKNOISE)
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

    /* Initializing SINETABLE */
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

short *Unpack_Sample(int Dest_Length, char Pack_Type)
{
    int Packed_Length;
    short *Dest_Buffer;
    Uint8 *Packed_Read_Buffer;

    Mod_Dat_Read(&Packed_Length, sizeof(char) * 4);
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
            case SMP_PACK_GSM:

#if defined(PTK_GSM)
                UnpackGSM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
#endif

                break;
            case SMP_PACK_MP3:

#if defined(PTK_MP3)
                UnpackMP3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
#endif
                break;

            case SMP_PACK_TRUESPEECH:

#if defined(PTK_TRUESPEECH)
                UnpackTrueSpeech(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
#endif
                break;
        }

        free(Packed_Read_Buffer);
        return(Dest_Buffer);
    }
}

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

    Ptk_Stop();

    if(dwModule[0] == 'KTRP')
    {
        Cur_Module += 4;

        // Allocate the patterns
        if(RawPatterns) free(RawPatterns);
        RawPatterns = (unsigned char *) malloc(PBLEN);
        if(!RawPatterns) return(FALSE);

        Pre_Song_Init();

        Mod_Dat_Read(&nPatterns, sizeof(unsigned char));
        Mod_Dat_Read(&Songtracks, sizeof(char));
        Mod_Dat_Read(&sLength, sizeof(unsigned char));
        Mod_Dat_Read(pSequence, sizeof(unsigned char) * sLength);

        for(i = 0; i < nPatterns; i++)
        {
            Mod_Dat_Read(&patternLines[i], sizeof(char));
        }
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
                        Mod_Dat_Read(TmpPatterns_Notes + i, 1);
                    }
                }
            }
        }

        Mod_Dat_Read(&nbr_instr, sizeof(int));

        for(int swrite = 0; swrite < nbr_instr; swrite++)
        {
            Mod_Dat_Read(&Synthprg[swrite], sizeof(bool));

            if(Synthprg[swrite])
            {
                Mod_Dat_Read(&PARASynth[swrite], sizeof(SynthParameters));
            } 

            // Compression type
            Mod_Dat_Read(&SampleCompression[swrite], sizeof(char));

            for(int slwrite = 0; slwrite < 16; slwrite++)
            {
                Mod_Dat_Read(&SampleType[swrite][slwrite], sizeof(char));
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
                    Mod_Dat_Read(&Basenote[swrite][slwrite], sizeof(char));
                    Mod_Dat_Read(&LoopStart[swrite][slwrite], sizeof(long));
                    Mod_Dat_Read(&LoopEnd[swrite][slwrite], sizeof(long));
                    Mod_Dat_Read(&LoopType[swrite][slwrite], sizeof(char));
                    Mod_Dat_Read(&SampleNumSamples[swrite][slwrite], sizeof(long));
                    Mod_Dat_Read(&Finetune[swrite][slwrite], sizeof(char));
                    Mod_Dat_Read(&SampleVol[swrite][slwrite], sizeof(float));
                    Mod_Dat_Read(&FDecay[swrite][slwrite], sizeof(float));
                    Save_Len = SampleNumSamples[swrite][slwrite];

                    Apply_Interpolation = SampleCompression[swrite] == SMP_PACK_NONE ? FALSE : TRUE;

                    if(Apply_Interpolation)
                    {
                        Save_Len /= 2;
                        Sample_Buffer = Unpack_Sample(Save_Len, SampleCompression[swrite]);
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
                        RawSamples[swrite][0][slwrite] = Unpack_Sample(Save_Len, SampleCompression[swrite]);
                    }
                    *(RawSamples[swrite][0][slwrite]) = 0;

                    // Stereo flag
                    Mod_Dat_Read(&SampleChannels[swrite][slwrite], sizeof(char));
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        if(Apply_Interpolation)
                        {
                            Sample_Buffer = Unpack_Sample(Save_Len, SampleCompression[swrite]);
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
                            RawSamples[swrite][1][slwrite] = Unpack_Sample(Save_Len, SampleCompression[swrite]);
                        }
                        *RawSamples[swrite][1][slwrite] = 0;
                    }
                    if(Sample_Buffer) free(Sample_Buffer);

                }// Exist Sample
            }
        }

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
            Mod_Dat_Read(&DSend[twrite], sizeof(float));
            Mod_Dat_Read(&CSend[twrite], sizeof(int));
        }

        // Reading mod properties
        Mod_Dat_Read(&compressor, sizeof(int));
        Mod_Dat_Read(&c_threshold, sizeof(int));
        Mod_Dat_Read(&BeatsPerMin, sizeof(int));
        Mod_Dat_Read(&TicksPerBeat, sizeof(int));
        Mod_Dat_Read(&mas_vol, sizeof(float));

        int Comp_Flag;
        Mod_Dat_Read(&Comp_Flag, sizeof(char));

#if defined(PTK_LIMITER)
        Mod_Dat_Read(&mas_threshold, sizeof(float));
        Mod_Dat_Read(&mas_ratio, sizeof(float));
#endif

        Mod_Dat_Read(&delay_time, sizeof(int));
        Mod_Dat_Read(&Feedback, sizeof(float));
        Mod_Dat_Read(&DelayType, sizeof(int));
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
                Mod_Dat_Read(&SACTIVE[tps_pos][tps_trk], sizeof(bool));
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
            Mod_Dat_Read(&Disclap[tps_trk], sizeof(bool));
        }

        Mod_Dat_Read(beatsync, sizeof(bool) * 128);
        Mod_Dat_Read(beatlines, sizeof(short) * 128);
        Mod_Dat_Read(&REVERBFILTER, sizeof(float));

        Mod_Dat_Read(CustomVol, sizeof(float) * 128);

        unsigned char tb303_1_enabled;
        unsigned char tb303_2_enabled;
        // Read the 303 datas
        Mod_Dat_Read(&tb303_1_enabled, sizeof(unsigned char));

#if defined(PTK_303)
        if(tb303_1_enabled)
        {
            Mod_Dat_Read(&tb303[0], sizeof(para303));
        }
#endif

        Mod_Dat_Read(&tb303_2_enabled, sizeof(unsigned char));

#if defined(PTK_303)
        if(tb303_2_enabled)
        {
            Mod_Dat_Read(&tb303[1], sizeof(para303));
        }
        if(tb303_1_enabled) Mod_Dat_Read(&tb303engine[0].tbVolume, sizeof(float));
        if(tb303_2_enabled) Mod_Dat_Read(&tb303engine[1].tbVolume, sizeof(float));
        if(tb303_1_enabled) Mod_Dat_Read(&tb303engine[0].hpf, sizeof(bool));
        if(tb303_2_enabled) Mod_Dat_Read(&tb303engine[1].hpf, sizeof(bool));
#endif

        cPosition = start_position;

#if defined(PTK_COMPRESSOR)
        Initreverb();
#endif

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
    repeat_loop_pos = 0;       // No repeat loop
    repeat_loop_counter = -1;
    ped_line = 0;
    ped_line_delay = 0;
    Subicounter = 0;
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
            SHISTORY[j][i] = false;
        }
    }
#endif

    Songplaying = 1;
}

// ------------------------------------------------------
// Stop replaying
void PTKEXPORT Ptk_Stop(void)
{
    Songplaying = 0;

#if defined(PTK_303)
    tb303engine[0].tbPattern = 255;
    tb303engine[0].tbLine = 255;
    tb303engine[1].tbPattern = 255;
    tb303engine[1].tbLine = 255;
    track3031 = 255;
    track3032 = 255;
#endif

    for(int stopper = 0; stopper < MAX_TRACKS; stopper++)
    {
    //    for(int stopper_poly = 0; stopper_poly < MAX_POLYPHONY; stopper_poly++) {
        sp_Stage[stopper]/*[stopper_poly]*/ = PLAYING_NOSAMPLE;
        sp_Stage2[stopper]/*[stopper_poly]*/ = PLAYING_NOSAMPLE;
        sp_Stage3[stopper]/*[stopper_poly]*/ = PLAYING_NOSAMPLE;
        Synthesizer[stopper]/*[stopper_poly]*/.NoteOff();
        sp_channelsample[stopper] = -1;
    //}
    }
}

// ------------------------------------------------------
// Init replay variables before loading a module
void Pre_Song_Init(void)
{
    int i;

#if defined(PTK_FLANGER)
    int j;
#endif

#if !defined(__STAND_ALONE__)
    Initreverb();
#endif

    glide = 0;

    for(int ini = 0; ini < MAX_TRACKS; ini++)
    {

#if defined(PTK_FX_ARPEGGIO)
        Arpeggio_Switch[ini] = 0;
        Arpeggio_BaseNote[ini] = 0;
        Vstep_arp[ini] = 0;
#endif

        Synth_Was[ini] = 0;
        Player_WL[ini] = 0;
        Player_WR[ini] = 0;
        Rns[ini] = 0;
        Player_SV[ini] = 0;
        Player_SC[ini] = 0;
        Player_LT[ini] = 0;
        Player_LW[ini] = 0;
        Player_LS[ini] = 0;
        Player_LE[ini] = 0;
        Player_LL[ini] = 0;
        Player_NS[ini] = 0;

#if defined(PTK_FX_AUTOFADEMODE)
        FADEMODE[ini] = 0;
        FADECOEF[ini] = 0.0f;
#endif

        old_note[ini] = 0;
        Synthesizer[ini]/*[poly]*/.Reset();
        
        sp_Position[ini].absolu = 0;
        sp_Position_osc1[ini].absolu = 0;
        sp_Position_osc2[ini].absolu = 0;

#if defined(PTK_SYNTH_OSC3)
        sp_Position_osc3[ini].absolu = 0;
#endif
        
        Player_FD[ini] = 0.0f;

        ResetFilters(ini);

        sp_channelsample[ini] = -1;
        sp_split[ini] = 0;

#if !defined(__STAND_ALONE__)
        TRACKMIDICHANNEL[ini] = ini;
        TRACKSTATE[ini] = 0;
#endif

        oldspawn[ini] = 0;
        roldspawn[ini] = 0;
        
        LFO_ON[ini] = 0;
        LFO_RATE[ini] = 0.0001f;
        LFO_AMPL[ini] = 0;

#if defined(PTK_LFO)
        LFOGR[ini] = 0;
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
        DThreshold[ini] = 65535;
        DClamp[ini] = 65535;
        Disclap[ini] = 0;
#endif

        sp_Step[ini] = 0;
        sp_Stage[ini] = 0;
        sp_Stage2[ini] = 0;
        sp_Stage3[ini] = 0;

        new_instrument[ini] = 0;
        Pos_Segue[ini] = 0;
        segue_volume[ini] = 0;
        Segue_SamplesL[ini] = 0;
        Segue_SamplesR[ini] = 0;

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

        sp_Cvol[ini] = 0.0f;
        sp_Tvol[ini] = 0.0f;
        
#if !defined(__STAND_ALONE__)
        DSend[ini] = 0;
        CSend[ini] = 0;
#endif

        Vstep1[ini] = 0;

#if defined(PTK_FX_TRANCEGLIDER)
        glidestep[ini] = 0;
#endif

        CCut[ini] = 126.0f;
    }

#if defined(PTK_FLANGER)
    Flanger_sbuf0L = 0;
    Flanger_sbuf1L = 0;
    Flanger_sbuf0R = 0;
    Flanger_sbuf1R = 0;

    for(i = 0; i < MAX_COMB_FILTERS; i++)
    {
        for(j = 0 ; j < 100000; j++)
        {
            delay_left_buffer[i][j] = 0;
            delay_right_buffer[i][j] = 0;
        }
    }
#endif

    for(i = 0; i < 128; i++)
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

#if defined(__STAND_ALONE__)
    for(int freer = 0; freer < 128; freer++)
    {
        for(char pedsplit = 0; pedsplit < 16; pedsplit++)
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
        }
    }
#endif

}

// ------------------------------------------------------
// Init the replayer datas
void Post_Song_Init(void)
{
    allPassInit((float) c_threshold);

#if defined(PTK_SHUFFLE)
    shufflestep = shuffle;
    shuffleswitch = -1;
#endif

    SubCounter = 0;
    repeat_loop_pos = 0;       // No repeat loop
    repeat_loop_counter = -1;

#if defined(PTK_FX_ARPEGGIO)
    for(int i = 0; i < MAX_TRACKS; i++)
    {
        Arpeggio_Switch[i] = 0;
        Arpeggio_BaseNote[i] = 0;
        Vstep_arp[i] = 0;
    }
#endif

    lchorus_counter = SAMPLESPERSEC;
    rchorus_counter = SAMPLESPERSEC;
    lchorus_counter2 = SAMPLESPERSEC - lchorus_delay;
    rchorus_counter2 = SAMPLESPERSEC - rchorus_delay;
    SamplesPerTick = (int) ((60 * SAMPLESPERSEC) / (BeatsPerMin * TicksPerBeat));
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

#if !defined(__STAND_ALONE__)
    int i;
#endif

    left_float = 0;
    right_float = 0;
    delay_left_final = 1.0f;
    delay_right_final = 1.0f;

    if(Songplaying)
    {
        if(PosInTick == 0)
        {

#if defined(PTK_FX_TICK0)
            DoEffects_tick0();
#endif

            Subicounter = 0;

#if defined(PTK_FX_PATTERNBREAK)
            Patbreak = 255;
            Patbreak2 = 255;
#endif

            for(int ct = 0; ct < Songtracks; ct++)
            {
                int efactor = ct * 6 + ped_line * 96 + pSequence[cPosition] * 12288;
                int efactor2 = ct * 6 + ped_line_delay * 96 + pSequence[cPosition_delay] * 12288;
                pl_note = *(RawPatterns + efactor);
                arp_pl_note = *(RawPatterns + efactor);
                pl_sample = *(RawPatterns + efactor + 1);
                pl_vol_row = *(RawPatterns + efactor + 2);
                pl_pan_row = *(RawPatterns + efactor + 3);
                pl_eff_row = *(RawPatterns + efactor + 4);
                pl_dat_row = *(RawPatterns + efactor + 5);
                pl_eff_row2 = *(RawPatterns + efactor2 + 4);
                pl_dat_row2 = *(RawPatterns + efactor2 + 5);

                if(pl_vol_row <= 64) sp_Tvol[ct] = (float) pl_vol_row * 0.015625f; // Setting volume.

#if defined(PTK_FX_SETVOLUME)
                if(pl_eff_row == 3) sp_Tvol[ct] = (float) pl_dat_row * 0.0039062f; // Setting volume.
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
#if !defined(__NOMIDI__)
                /* MidiController commands */
                if(pl_pan_row == 144 && c_midiout != -1 && pl_eff_row < 128)
                {
                    MidiCommand(176 + TRACKMIDICHANNEL[ct], pl_eff_row, pl_dat_row);
                }

                if(pl_eff_row == 128 && c_midiout != -1 && pl_dat_row < 128)
                {
                    MidiCommand(176 + TRACKMIDICHANNEL[ct], 0, pl_dat_row);
                }
#endif
#endif

#if defined(PTK_FX_PATTERNBREAK)
                if(pl_eff_row == 0xd && pl_dat_row < 128) Patbreak = pl_dat_row;
                if(pl_eff_row2 == 0xd && pl_dat_row2 < 128) Patbreak2 = pl_dat_row2;
#endif

                if(pl_note < 120)
                {

                    int toffset = 0;
                    glide = 0;
                    if(pl_eff_row == 9) toffset = pl_dat_row;
                    else if(pl_eff_row == 5) glide = 1;

                    if(pl_vol_row <= 64 || pl_eff_row == 3)
                    {
                        Sp_Playwave(ct, (float) pl_note, pl_sample, sp_Tvol[ct], toffset, glide, FALSE);
                    }
                    else
                    {
                        Sp_Playwave(ct, (float) pl_note, pl_sample, CustomVol[pl_sample], toffset, glide, FALSE);
                    }
                }

                if(pl_note == 120)
                {
                    // Note OFF on track ct
                    //for(i = 0; i < MAX_POLYPHONY; i++) {
                    if(sp_Stage[ct]/*[i]*/ == PLAYING_SAMPLE) sp_Stage[ct]/*[i]*/ = PLAYING_SAMPLE_NOTEOFF;
                    Synthesizer[ct]/*[i]*/.NoteOff();
                    //}

#if defined(PTK_303)
                    noteoff303(ct); // 303 Note Off...
#endif

#if !defined(__STAND_ALONE__)
#if !defined(__NOMIDI__)
                    MidiNoteOff(ct);
#endif
#endif
                }
            } // For loop

#if defined(PTK_303)
            Go303();
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
                // Pattern break
                ped_line = Patbreak;

#if !defined(__STAND_ALONE__)
                if(!plx) 
#endif
                {
                    cPosition++;
                }

                if(cPosition >= sLength)
                {
                    cPosition = 0;
#if defined(__WINAMP__)
                    done = 1;
#endif
                }

#if !defined(__STAND_ALONE__)
                for(i = 0; i < MAX_TRACKS; i++)
                {
                    SHISTORY[cPosition][i] = false;
                }
#endif

                repeat_loop_counter = -1;
                repeat_loop_pos = 0;
            }
#else
            ped_line++;
#endif  // PTK_FX_PATTERNBREAK

            if(ped_line == patternLines[pSequence[cPosition]])
            {
                // Normal end of pattern
                ped_line = 0;

#if !defined(__STAND_ALONE__)
                if(!plx)
#endif
                {
                    cPosition++;
                }

                if(cPosition >= sLength)
                {
                    cPosition = 0;
#if defined(__WINAMP__)
                    done = 1;
#endif
                }

#if !defined(__STAND_ALONE__)
                for(i = 0; i < MAX_TRACKS; i++)
                {
                    SHISTORY[cPosition][i] = false;
                }
#endif

				repeat_loop_counter = -1;
                repeat_loop_pos = 0;
            }

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
                    ped_line_delay = Patbreak2;
#if !defined(__STAND_ALONE__)
                    if(!plx)
#endif
                    {
                        cPosition_delay++;
                    }
                    if(cPosition_delay >= sLength) cPosition_delay = 0;
                }
#else
                ped_line_delay++;
#endif // PTK_FX_PATTERNBREAK

				if(ped_line_delay == patternLines[pSequence[cPosition_delay]])
                {
                    ped_line_delay = 0;
#if !defined(__STAND_ALONE__)
                    if(!plx)
#endif
                    {
                        cPosition_delay++;
                    }
                    if(cPosition_delay >= sLength) cPosition_delay = 0;
                }
            }

#endif // __WINAMP__
        }
    }

    for(char c = 0; c < Songtracks; c++)
    {
        grown = false;
        currsygnal = 0;
        currsygnal2 = 0;
        bool gotsome = false;

        // If wav is selected in the synth we don't play it directly but through the synth.

        // -------------------------------------------

        // Interpolation algorithm
        //    for(i = 0; i < MAX_POLYPHONY; i++) {
        res_dec = sp_Position[c].half.last;

        if(sp_Stage[c] == PLAYING_SAMPLE || sp_Stage[c] == PLAYING_SAMPLE_NOTEOFF)
        {

            if(!Synth_Was[c]) goto ByPass_Wav;
            if((Synthesizer[c].OSC1_WAVEFORM != 5 && Synthesizer[c].OSC2_WAVEFORM != 5))
            {
ByPass_Wav:
                if(sp_Stage[c] == PLAYING_SAMPLE_NOTEOFF)
                {
                    // Note Stop
                    sp_Tvol[c] = 0.0f;
                    if(sp_Cvol[c] < 0.01f) sp_Stage[c] = PLAYING_NOSAMPLE;
                }

                if(sp_Cvol[c] > sp_Tvol[c]) sp_Cvol[c] -= 0.005f;
                else sp_Cvol[c] += 0.005f;

                gotsome = true;

                Currentpointer = sp_Position[c].half.first;

                currsygnal = Cubic_Work(
                            *(Player_WL[c] + Currentpointer - 1),
                            *(Player_WL[c] + Currentpointer),
                            *(Player_WL[c] + Currentpointer + 1),
                            *(Player_WL[c] + Currentpointer + 2),
                            res_dec, Currentpointer,
                            Rns[c]) * sp_Cvol[c] * Player_SV[c];

                // Stereo sample
                if(Player_SC[c] == 2)
                {
                    grown = true;
                    currsygnal2 = Cubic_Work(*(Player_WR[c]+ Currentpointer - 1),
                                             *(Player_WR[c] + Currentpointer),
                                             *(Player_WR[c] + Currentpointer + 1),
                                             *(Player_WR[c] + Currentpointer + 2),
                                             res_dec, Currentpointer,
                                             Rns[c]) * sp_Cvol[c] * Player_SV[c];
                }

                // End of Interpolation algo
                if(Player_LW[c] == SMP_LOOPING_BACKWARD)
                {
                    sp_Position[c].absolu -= Vstep1[c];
                }
                else
                {
                    sp_Position[c].absolu += Vstep1[c];
                }

                if(Player_LT[c] == SMP_LOOP_PINGPONG)
                {
                    if(Player_LW[c] == SMP_LOOPING_FORWARD)
                    {
                        if(sp_Position[c].half.first >= Player_LE[c])
                        {
                            Player_LW[c] = SMP_LOOPING_BACKWARD;
                        }
                    }
                    else
                    {
                        if(sp_Position[c].half.first <= Player_LS[c])
                        {
                            Player_LW[c] = SMP_LOOPING_FORWARD;
                        }
                    }
                }
                if(Player_LT[c] == SMP_LOOP_FORWARD &&
                   sp_Position[c].half.first >= Player_LE[c])
                {
                    sp_Position[c].half.first -= Player_LL[c];
                }
                if(Player_LT[c] == SMP_LOOP_NONE &&
                   sp_Position[c].half.first >= Player_NS[c])
                {
                    sp_Stage[c] = PLAYING_NOSAMPLE;
                }
            }

            // We have no adsr for those
            if(!SACTIVE[cPosition][c])
            {
                currsygnal = 0;
                currsygnal2 = 0;
            }
        } // sp!!0

#if defined(PTK_303)
        if(track3031 == c)
        {
            currsygnal += tb303engine[0].tbGetSample();
            gotsome = true;
        }
        if(track3032 == c)
        {
            currsygnal += tb303engine[1].tbGetSample();
            gotsome = true;
        }
#endif

        if(Synthesizer[c].ENV1_STAGE || Synthesizer[c].ENV2_STAGE)
        {
            currsygnal += Synthesizer[c].GetSample(Player_WL[c], Player_WR[c],
                                                   Player_SC[c],
                                                   Player_LT[c],
                                                   Player_LT[c] > SMP_LOOP_NONE ? Player_LE[c]: Player_NS[c],
                                                   Player_LT[c] > SMP_LOOP_NONE ? Player_LL[c]: 0,
                                                   &currsygnal2,
                                                   Rns[c], sp_Cvol[c] * Player_SV[c], &sp_Stage2[c], &sp_Stage3[c],
                                                   (Uint64 *) &sp_Position_osc1[c],
                                                   (Uint64 *) &sp_Position_osc2[c],
#if defined(PTK_SYNTH_OSC3)
                                                   (Uint64 *) &sp_Position_osc3[c],
#endif
                                                   Vstep1[c]);

            if(sp_Cvol[c] > sp_Tvol[c]) sp_Cvol[c] -= 0.005f;
            else sp_Cvol[c] += 0.005f;

            if((Synthesizer[c].OSC1_WAVEFORM == 5 || Synthesizer[c].OSC2_WAVEFORM == 5))
            {
                if(Player_SC[c] == 2) grown = true;
            }
            gotsome = true;
        }

        // Send a note off if the channel is being turned off
        if(!SACTIVE[cPosition][c]

#if !defined(__STAND_ALONE__)
           && !SHISTORY[cPosition][c]
#endif

          )
        {

#if !defined(__STAND_ALONE__)
            SHISTORY[cPosition][c] = true;
#endif

            Synthesizer[c].NoteOff();

#if !defined(__STAND_ALONE__)
#if !defined(__NOMIDI__)
            MidiNoteOff(c);
#endif
#endif

#if defined(PTK_303)
            noteoff303(c);
#endif

        }

        // -----------------------------------------------

        if(gotsome)
        {

#if defined(PTK_TRACKFILTERS)
            if(FType[c] != 4)
            { /* Track filter actived */
                float const dfi = TCut[c] - CCut[c];

                if(dfi < -1.0 || dfi > 1.0f) CCut[c] += dfi * ICut[c];

                if(FType[c] < 4)
                {

#if defined(PTK_FILTER_LOHIBAND)
                    gco = f2i(ApplyLfo(CCut[c] - ramper[c], c));

                    ramper[c] += Player_FD[c] * gco * 0.015625f;

                    coef[0] = coeftab[0][gco][FRez[c]][FType[c]];
                    coef[1] = coeftab[1][gco][FRez[c]][FType[c]];
                    coef[2] = coeftab[2][gco][FRez[c]][FType[c]];
                    coef[3] = coeftab[3][gco][FRez[c]][FType[c]];
                    coef[4] = coeftab[4][gco][FRez[c]][FType[c]];
                    currsygnal = Filter(currsygnal + 1, c);
                    if(grown) currsygnal2 = Filter(currsygnal2 + 1, c);
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
                            currsygnal = filter2p(c, currsygnal + 1, realcut, (float) FRez[c]);
                            if(grown) currsygnal2 = filter2p(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_LO48)
                        case 6:
                            currsygnal = filter2p(c, currsygnal + 1, realcut, (float) FRez[c]);
                            currsygnal = filter2p24d(c, currsygnal + 1, realcut, (float) FRez[c]);
                            if(grown)
                            {
                                currsygnal2 = filter2p(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                                currsygnal2 = filter2p24d(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                            }
                            break;
#endif

#if defined(PTK_FILTER_LP24)
                        case 7:
                            currsygnal = filter2p(c, currsygnal + 1, realcut, (float) FRez[c]);
                            currsygnal2 = filter2p24d(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_AMODM)
                        case 8:
                            currsygnal = filterRingMod(c, currsygnal, realcut, (float) FRez[c]);
                            if(grown) currsygnal2 = filterRingMod(c, currsygnal2, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_AMODS)
                        case 9:
                            currsygnal = filterRingMod(c, currsygnal, realcut, (float) FRez[c]);
                            currsygnal2 = filterRingModStereo(c, currsygnal2);
                            break;
#endif

#if defined(PTK_FILTER_SINGLEM)
                        case 10:
                            currsygnal = filterWater(c, currsygnal, realcut, (float) FRez[c]);
                            if(grown) currsygnal2 = filterWater(c, currsygnal2, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_SINGLES)
                        case 11:
                            currsygnal = filterWater(c, currsygnal, realcut, (float) FRez[c]);
                            currsygnal2 = filterWaterStereo(c, currsygnal2, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_EQM15)
                        case 12:
                            currsygnal = filterBellShaped(c, currsygnal, realcut, (float) FRez[c], -15);
                            if(grown) currsygnal2 = filterBellShaped(c, currsygnal2, realcut, (float) FRez[c], -15);
                            break;
#endif

#if defined(PTK_FILTER_EQM6)
                        case 13:
                            currsygnal = filterBellShaped(c, currsygnal, realcut, (float) FRez[c], -6);
                            if(grown) currsygnal2 = filterBellShaped(c, currsygnal2, realcut, (float) FRez[c], -6);
                            break;
#endif

#if defined(PTK_FILTER_EQP6)
                        case 14:
                            currsygnal = filterBellShaped(c, currsygnal, realcut, (float) FRez[c], 6);
                            if(grown) currsygnal2 = filterBellShaped(c, currsygnal2, realcut, (float) FRez[c], 6);
                            break;
#endif

#if defined(PTK_FILTER_EQP15)
                        case 15:
                            currsygnal = filterBellShaped(c, currsygnal, realcut, (float) FRez[c], 15);
                            if(grown) currsygnal2 = filterBellShaped(c, currsygnal2, realcut, (float) FRez[c], 15);
                            break;
#endif

#if defined(PTK_FILTER_DELTA)
                        case 16:
                            currsygnal = filterDelta(c, currsygnal, realcut, (float) FRez[c]);
                            if(grown) currsygnal2 = filterDelta(c, currsygnal2, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_DISTL)
                        case 17:
                            currsygnal = int_filter2p(c, currsygnal, realcut, (float) FRez[c], 0.25f);
                            if(grown) currsygnal2 = int_filter2p(c, currsygnal2, realcut, (float) FRez[c], 0.25f);
                            break;
#endif

#if defined(PTK_FILTER_DISTM)
                        case 18:
                            currsygnal = int_filter2p(c, currsygnal, realcut, (float) FRez[c], 0.56f);
                            if(grown) currsygnal2 = int_filter2p(c, currsygnal2, realcut, (float) FRez[c], 0.56f);
                            break;
#endif

#if defined(PTK_FILTER_DISTH)
                        case 19:
                            currsygnal = int_filter2p(c, currsygnal, realcut, (float) FRez[c], 0.78f);
                            if(grown) currsygnal2 = int_filter2p(c, currsygnal2, realcut, (float) FRez[c], 0.78f);
                            break;
#endif

#if defined(PTK_FILTER_DIST)
                        case 20:
                            currsygnal = int_filter2p(c, currsygnal, realcut, (float) FRez[c], 0.96f);
                            if(grown) currsygnal2 = int_filter2p(c, currsygnal2, realcut, (float) FRez[c], 0.96f);
                            break;
#endif

#if defined(PTK_FILTER_HP12M)
                        case 21:
                            currsygnal = filterhp(c, currsygnal + 1, realcut, (float) FRez[c]);
                            if(grown) currsygnal2 = filterhp(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_HP12S)
                        case 22:
                            currsygnal = filterhp(c, currsygnal + 1, realcut, (float) FRez[c]);
                            currsygnal2 = filterhp2(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                            break;
#endif

#if defined(PTK_FILTER_HP24M)
                        case 23:
                            currsygnal = filterhp(c, currsygnal + 1, realcut, (float) FRez[c]);
                            currsygnal = filterhp2(c, currsygnal + 1, realcut, (float) FRez[c]);
                            if(grown)
                            {
                                currsygnal2 = filterhp(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                                currsygnal2 = filterhp2(c, currsygnal2 + 1, realcut, (float) FRez[c]);
                            }
                            break;
#endif

                    } //SWITCHCASE [FILTERS]
                }
            } /* Filter end */
#endif // PTK_TRACKFILTERS

            if(!grown) currsygnal2 = currsygnal;

            if(new_instrument[c])
            {
                currsygnal = (currsygnal * (1.0f - segue_volume[c])) + (Segue_SamplesL[c] * segue_volume[c]);
                currsygnal2 = (currsygnal2 * (1.0f - segue_volume[c])) + (Segue_SamplesR[c] * segue_volume[c]);
                Pos_Segue[c]++;
                segue_volume[c] -= 1.0f / 127.0f;
                if(Pos_Segue[c] >= 128)
                {
                    new_instrument[c] = FALSE;
                }
            }
            else
            {
                // Store the transition
                Segue_SamplesL[c] = currsygnal;
                Segue_SamplesR[c] = currsygnal2;
            }

            if(Disclap[c])
            { /* Distortion */
                if(currsygnal > DThreshold[c]) currsygnal = DClamp[c];
                else if(currsygnal < -DThreshold[c]) currsygnal = -DClamp[c];

                if(currsygnal2 > DThreshold[c]) currsygnal2 = DClamp[c];
                else if(currsygnal2 < -DThreshold[c]) currsygnal2 = -DClamp[c];
            }

            currsygnal *= LVol[c];
            currsygnal2 *= RVol[c];

            // 32-Bit HQ Interpolated System Flanger

#if defined(PTK_FLANGER)
            if(FLANGER_ON[c])
            {
                FLANGE_LEFTBUFFER[c][FLANGER_OFFSET[c]] = currsygnal * FLANGER_AMOUNT[c] + oldspawn[c] * FLANGER_FEEDBACK[c];
                FLANGE_RIGHTBUFFER[c][FLANGER_OFFSET[c]] = currsygnal2 * FLANGER_AMOUNT[c] + roldspawn[c] * FLANGER_FEEDBACK[c];

                float fstep1 = powf(2.0f, sinf(FLANGER_GR[c]) * FLANGER_AMPL[c]);
                float fstep2 = powf(2.0f, sinf(FLANGER_GR[c] + FLANGER_DEPHASE[c]) * FLANGER_AMPL[c]);

                foff2[c] += fstep1;
                foff1[c] += fstep2;  

                if(foff2[c] >= 16384.0f) foff2[c] = 0.0f;
                if(foff1[c] >= 16384.0f) foff1[c] = 0.0f;
                if(foff2[c] < 0.0f) foff2[c] = 0.0f;
                if(foff1[c] < 0.0f) foff1[c] = 0.0f;

                oldspawn[c] = FLANGE_LEFTBUFFER[c][f2i(foff2[c])];
                roldspawn[c] = FLANGE_RIGHTBUFFER[c][f2i(foff1[c])];
                currsygnal += Filter_FlangerL(oldspawn[c]);
                currsygnal2 += Filter_FlangerR(roldspawn[c]);

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

#if !defined(__STAND_ALONE__)
            if(!TRACKSTATE[c])
#endif
            {
                // Dry Send

                left_float += currsygnal;
                right_float += currsygnal2;

                // Sending to delay...

                float const DS = DSend[c];

                if(DS > 0.008f)
                {
                    delay_left_final += currsygnal * DS;
                    delay_right_final += currsygnal2 * DS;
                }

                // Sending to chorus

                float const DC = CCoef[c];

                if(DC > 0.008f)
                {
                    left_chorus += currsygnal * DC;
                    right_chorus += currsygnal2 * DC;
                }

            } // Close trackstate
        }

#if !defined(__STAND_ALONE__)
        if(Songplaying_Pattern)
        {
            if(!TRACKSTATE[c])
            {
                Scope_Dats[c][Cscope[c]] = (currsygnal + currsygnal2) * 0.15f;
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
// Play a waveform
void Sp_Playwave(int channel, float note, int sample, float vol,
                 unsigned int offset, int glide, int Play_Entire)
{

#if defined(PTK_FX_AUTOFADEMODE)
    FADEMODE[channel] = 0;
#endif

    int Cur_Position;
    float note2;
    int associated_sample;
    int no_retrig_adsr = FALSE;

    // Empty row ? Take the current one.
    if(sample == 255)
    {
        sample = sp_channelsample[channel];
        vol = sp_Tvol[channel];
        if(glide) no_retrig_adsr = TRUE;
    }

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

    Cur_Position = cPosition;

    // sp_Stage2[channel] = PLAYING_STOCK;
    if(Synthesizer[channel].OSC1_WAVEFORM == 5)
    {
        sp_Position[channel].absolu = 0;
        sp_Position_osc1[channel].absolu = 0;

#if defined(PTK_SYNTH_OSC3)
        sp_Position_osc3[channel].absolu = 0;
#endif

    }
    if(Synthesizer[channel].OSC2_WAVEFORM == 5)
    {
        sp_Position[channel].absolu = 0;
        sp_Position_osc2[channel].absolu = 0;
    }

    // Check if the channel have to be played
    if(SACTIVE[Cur_Position][channel])
    {
        char split = 0;
        int mnote = int(note);

        for(char revo = 0; revo < 16; revo++)
        {
            if(note >= Basenote[associated_sample][revo] &&
               SampleType[associated_sample][revo] != 0)
            {
                split = revo;
            }
        }
        if(sample != sp_channelsample[channel])
        {
            glide = 0;
        }

        note2 = note - 48;

        note -= Basenote[associated_sample][split];
        note += float((float) Finetune[associated_sample][split] * 0.0078125f);

        if(!no_retrig_adsr)
        {
            Synthesizer[channel].OSC1_WAVEFORM = 4;
            Synthesizer[channel].OSC2_WAVEFORM = 4;
            if(Synthprg[sample])
            {
                Synthesizer[channel].ChangeParameters(PARASynth[sample]);
                Synthesizer[channel].NoteOn(note2, vol, LoopType[associated_sample][split],
                                            LoopType[associated_sample][split] > SMP_LOOP_NONE ? LoopEnd[associated_sample][split]: (SampleNumSamples[associated_sample][split] - 2), LoopEnd[associated_sample][split] - LoopStart[associated_sample][split],
                                            note);
            }
        }

        // Fix a bug as this can also be used for synth
        // which isn't correct
        Player_SC[channel] = 0;

        sp_Tvol[channel] = vol;
        double spreadnote = (double) powf(2.0f, note2 / 12.0f);
        spreadnote *= 4294967296.0f;

#if defined(PTK_FX_ARPEGGIO)
        Vstep_arp[channel] = (int64) spreadnote;
        Arpeggio_BaseNote[channel] = note2;
#endif

        if(glide)
        {
            sp_Step[channel] = (int64) spreadnote;
        }
        else
        {
            Vstep1[channel] = (int64) spreadnote;
            sp_Step[channel] = (int64) spreadnote;
        }

        // Only synth
        sp_Stage[channel] = PLAYING_NOSAMPLE;
        sp_Stage2[channel] = PLAYING_STOCK;
        sp_Stage3[channel] = PLAYING_STOCK;

        sp_channelsample[channel] = sample;

        if(SampleType[associated_sample][split])
        {
            if(Synthprg[sample])
            {
                // Synth + sample if both are != wav
                if(Synthesizer[channel].OSC1_WAVEFORM != 5 &&
                   Synthesizer[channel].OSC2_WAVEFORM != 5)
                {
                    sp_Stage[channel] = PLAYING_SAMPLE;
                }
            }
            else
            {
                // Only sample
                sp_Stage[channel] = PLAYING_SAMPLE;
                sp_Stage2[channel] = PLAYING_NOSAMPLE;
                sp_Stage3[channel] = PLAYING_NOSAMPLE;
            }

            if(!offset) sp_Cvol[channel] = vol;
            else sp_Cvol[channel] = 0;

#if defined(PTK_FX_ARPEGGIO)
            Arpeggio_BaseNote[channel] = note;
#endif

            if(beatsync[associated_sample])
            {
                double spreadnote = (double) (SampleNumSamples[associated_sample][split]) / ((double) beatlines[associated_sample] * (double) SamplesPerTick);
                spreadnote *= 4294967296.0f;

#if defined(PTK_FX_ARPEGGIO)
                Vstep_arp[channel] = (int64) spreadnote;
#endif

                Vstep1[channel] = (int64) spreadnote;
                sp_Step[channel] = (int64) spreadnote;
            }
            else
            {
                double spreadnote = (double) powf(2.0f, note / 12.0f);
                spreadnote *= 4294967296.0f;

#if defined(PTK_FX_ARPEGGIO)
                Vstep_arp[channel] = (int64) spreadnote;
#endif

                if(glide)
                {
                    sp_Step[channel] = (int64) spreadnote;
                }
                else
                {
                    Vstep1[channel] = (int64) spreadnote;
                    sp_Step[channel] = (int64) spreadnote;
                }
            }

            Synth_Was[channel] = Synthprg[sample];

            sp_split[channel] = split;

            // Player Pointer Assignment

#if !defined(__STAND_ALONE__)
            Uint32 Sel_Start;
            Uint32 Sel_End;
            
            // Only play the selection
            if(userscreen == USER_SCREEN_SAMPLE_EDIT &&
               sed_range_start != sed_range_end && Play_Entire)
            {
                Sel_Start = sed_range_start;
                Sel_End = sed_range_end;
                if(sed_range_start > sed_range_end)
                {
                    Sel_End = sed_range_start;
                    Sel_Start = sed_range_end;
                }
                Player_LS[channel] = Sel_Start;
                Player_LE[channel] = Sel_End;
                sp_Position[channel].half.first = Sel_Start;
                Player_NS[channel] = Sel_End;
                if(!glide) sp_Position[channel].half.first += offset << 8;
            }
            else
            {
                Player_LS[channel] = LoopStart[associated_sample][split];
                Player_LE[channel] = LoopEnd[associated_sample][split];
                Player_NS[channel] = SampleNumSamples[associated_sample][split] - 2;
                if(!glide) sp_Position[channel].half.first = offset << 8;
            }
#else
            Player_LS[channel] = LoopStart[associated_sample][split];
            Player_LE[channel] = LoopEnd[associated_sample][split];
            Player_NS[channel] = SampleNumSamples[associated_sample][split] - 2;
            if(!glide) sp_Position[channel].half.first = offset << 8;
#endif
            Player_LL[channel] = Player_LE[channel] - Player_LS[channel];

            Rns[channel] = SampleNumSamples[associated_sample][split];
            Player_SV[channel] = SampleVol[associated_sample][split];
            Player_LT[channel] = LoopType[associated_sample][split];
            Player_LW[channel] = SMP_LOOPING_FORWARD;
            Player_SC[channel] = SampleChannels[associated_sample][split];
            Player_FD[channel] = FDecay[associated_sample][split];
            Player_WL[channel] = RawSamples[associated_sample][0][split];

            if(SampleChannels[associated_sample][split] == 2)
            {
                Player_WR[channel] = RawSamples[associated_sample][1][split];
            }
            if(!glide) ramper[channel] = 0;

            // Sample is out of range
            if((int) sp_Position[channel].half.first >= SampleNumSamples[associated_sample][split])
            {
                sp_Stage[channel] = PLAYING_NOSAMPLE;
                sp_Stage2[channel] = PLAYING_NOSAMPLE;
            }
        }
        else
        {
            sp_Cvol[channel] = 1.0f;
            Player_SV[channel] = 1.0f;
            if(!glide)
            {
                ramper[channel] = 0;
                sp_Position[channel].half.first = offset << 8;
            }
        }
        if(!no_retrig_adsr)
        {
            sp_Position_osc1[channel] = sp_Position[channel];
            sp_Position_osc2[channel] = sp_Position[channel];

#if defined(PTK_SYNTH_OSC3)
            sp_Position_osc3[channel] = sp_Position[channel];
#endif

        }
        Pos_Segue[channel] = 0;
        segue_volume[channel] = 1.0f;
        new_instrument[channel] = TRUE;

#if !defined(__STAND_ALONE__)
        if(TRACKSTATE[channel] == 0 &&
           c_midiout != -1 &&
           Midiprg[associated_sample] != -1)
        {

#if !defined(__NOMIDI__)
            MidiNoteOff(channel);
#endif

            /* Set the midi program if it was modified */
            if(LastProgram[TRACKMIDICHANNEL[channel]] != Midiprg[associated_sample])
            {

#if !defined(__NOMIDI__)
                MidiCommand(192 + TRACKMIDICHANNEL[channel], Midiprg[associated_sample], 127);
#endif

                //midiOutShortMsg(midiout_handle, (192 + TRACKMIDICHANNEL[channel]) | (Midiprg[associated_sample] << 8) | (127 << 16));
                LastProgram[TRACKMIDICHANNEL[channel]] = Midiprg[associated_sample];
            }
            /* Send the note to the midi device */

#if !defined(__NOMIDI__)
            float veloc = vol * mas_vol;

            Midi_Track_Notes[TRACKMIDICHANNEL[channel]] = mnote;
            MidiCommand(144 + TRACKMIDICHANNEL[channel], mnote, f2i(veloc * 127));
#endif

            //midiOutShortMsg(midiout_handle, (144 + TRACKMIDICHANNEL[channel]) | (mnote << 8) | (f2i(veloc * 127) << 16));
        }
#endif
    }
}

// ------------------------------------------------------
// All pass filter initialization
#if defined(PTK_COMPRESSOR)
void allPassInit(float miliSecs)
{
    currentCounter = 5759;
    delayedCounter = 5759 - int(miliSecs * 44.1f);
    delayedCounter2 = 5759 - int(miliSecs * 50.1f);
    delayedCounter3 = 5759 - int(miliSecs * 60.1f);
    delayedCounter4 = 5759 - int(miliSecs * 70.1f);
    delayedCounter5 = 5759 - int(miliSecs * 73.1f);
    delayedCounter6 = 5759 - int(miliSecs * 79.1f);

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
#endif

// ------------------------------------------------------
// Handle patterns effects
#if defined(PTK_FX_TICK0)
void DoEffects_tick0(void)
{
    int temp_ped_line = ped_line;

    for(int trackef = 0; trackef < Songtracks; trackef++)
    {
        int tefactor = trackef * 6 + temp_ped_line * 96 + pSequence[cPosition] * 12288;
        int pltr_eff_row = *(RawPatterns + tefactor + 4);
        int pltr_dat_row = *(RawPatterns + tefactor + 5);

        switch(pltr_eff_row)
        {

#if defined(PTK_FX_PATTERNLOOP)
            // $06 Pattern loop
            case 0x6:
                if(!pltr_dat_row)
                {
                    // Set the repeat position
                    repeat_loop_counter = -1;
                    repeat_loop_pos = ped_line;
                }
                else
                {
                    if(repeat_loop_counter == -1)
                    {
                        repeat_loop_counter = (int) pltr_dat_row;
                        ped_line = repeat_loop_pos;
                        ped_line_delay = repeat_loop_pos;
                    }
                    else
                    {
                        // count
                        repeat_loop_counter--;
                        if(repeat_loop_counter)
                        {
                            ped_line = repeat_loop_pos;
                            ped_line_delay = repeat_loop_pos;
                        }
                        else
                        {
                            repeat_loop_counter = -1;
                            repeat_loop_pos = 0;
                        }
                    }
                } 
                break;
#endif

#if defined(PTK_FX_ARPEGGIO)
            // $1b arpeggio switch on/off
            case 0x1b:

                Arpeggio_Switch[trackef] = pltr_dat_row;
                if(!pltr_dat_row)
                {
                    Vstep1[trackef] = Vstep_arp[trackef];
                }
                break;
#endif

        }
    }      
}
#endif

// ------------------------------------------------------
// Process ticks X effects
void DoEffects(void)
{
    for(int trackef = 0; trackef < Songtracks; trackef++)
    {
        int tefactor = trackef * 6 + ped_line * 96 + pSequence[cPosition] * 12288;
        int pltr_note = *(RawPatterns + tefactor);
        int pltr_sample = *(RawPatterns + tefactor + 1);
        unsigned char pltr_vol_row = *(RawPatterns + tefactor + 2);
        int pltr_pan_row = *(RawPatterns + tefactor + 3);
        int64 pltr_eff_row = *(RawPatterns + tefactor + 4);
        int64 pltr_dat_row = *(RawPatterns + tefactor + 5);

        if(Subicounter == 0)
        {
            if(pltr_note == 121 && pltr_sample != 255) sp_Tvol[trackef] = CustomVol[pltr_sample];
        }

#if defined(PTK_FX_AUTOFADEMODE)
        // Autofade routine
        switch(FADEMODE[trackef])
        {
            case 1:
                sp_Tvol[trackef] += FADECOEF[trackef];

                if(sp_Tvol[trackef] > 1.0f)
                {
                    sp_Tvol[trackef] = 1.0f;
                    FADEMODE[trackef] = 0;
                }
                break;

            case 2:
                sp_Tvol[trackef] -= FADECOEF[trackef];

                if(sp_Tvol[trackef] < 0.0f)
                {
                    sp_Tvol[trackef] = 0.0f;
                    FADEMODE[trackef] = 0;
                }
                break;
        }
#endif

        // Effects

#if defined(PTK_FX_NOTECUT)
        if((pltr_vol_row & 0xf0) == 0xf0)
        { /* Note Cut: Fx */
            unsigned char kinder = pltr_vol_row & 0xf;
            if(Subicounter == kinder)
            {
                if(sp_Stage[trackef] == PLAYING_SAMPLE)
                {
                    if(FType[trackef] == 4) sp_Stage[trackef] = PLAYING_SAMPLE_NOTEOFF;
                    else sp_Tvol[trackef] = 0.001f;
                }
                Synthesizer[trackef].NoteOff();

#if !defined(__STAND_ALONE__)
#if !defined(__NOMIDI__)
                MidiNoteOff(trackef);
#endif
#endif
            }
        }
#endif

#if defined(PTK_FX_0)
        switch(pltr_eff_row)
        {

#if defined(PTK_FX_PITCHUP)
            // $01 Pitch Up 
            case 0x1:
#if defined(__GCC__)
                if(Vstep1[trackef] < 137438953472ll) Vstep1[trackef] += pltr_dat_row << 21;
#else
                if(Vstep1[trackef] < 137438953472) Vstep1[trackef] += pltr_dat_row << 21;
#endif
                break;
#endif  // PTK_FX_PITCHUP

#if defined(PTK_FX_PITCHDOWN)
            // $02 Pitch Down 
            case 0x2:
                Vstep1[trackef] -= pltr_dat_row << 21;
                if(Vstep1[trackef] < 16) Vstep1[trackef] = 16;
                break;
#endif

#if defined(PTK_FX_SETVOLUME)
            // $03 Set volume
            case 0x3:
                sp_Tvol[trackef] = pltr_dat_row * 0.0039062f;
                break;
#endif

#if defined(PTK_FX_TRANCESLICER)
            // $04 Trance slicer
            case 0x4:
                if(Subicounter == 0) sp_Tvol[trackef] = 1.0f;

                if(Subicounter >= pltr_dat_row) sp_Tvol[trackef] = 0;
                break;
#endif

#if defined(PTK_FX_TRANCEGLIDER)
            // $05 Glider
            case 0x5:
                if(pltr_dat_row) glidestep[trackef] = pltr_dat_row << 21;

                if(Vstep1[trackef] < sp_Step[trackef])
                {
                    Vstep1[trackef] += glidestep[trackef];
                    if(Vstep1[trackef] > sp_Step[trackef])
                    {
                        Vstep1[trackef] = sp_Step[trackef];
                    }
                }
                else
                {
                    if(Vstep1[trackef] > sp_Step[trackef])
                    {
                        Vstep1[trackef] -= glidestep[trackef];
                        if(Vstep1[trackef] < sp_Step[trackef]) Vstep1[trackef] = sp_Step[trackef];
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
                    TCut[trackef] = float((float) (rand() / 256) / ((float) pltr_dat_row + 1.0f));

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
                if(pltr_note == 121) pltr_note = old_note[trackef];
                old_note[trackef] = pltr_note;
                if(pltr_dat_row > 0 && (Subicounter % pltr_dat_row) == 0)
                {
                    if(pltr_vol_row <= 64)
                    {
                        Sp_Playwave(trackef, (float) pltr_note, pltr_sample, pltr_vol_row * 0.015625f, 0, 0, FALSE);
                    }
                    else
                    {
                        Sp_Playwave(trackef, (float) pltr_note, pltr_sample, CustomVol[pltr_sample], 0, 0, FALSE);
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
                    SamplesPerTick = (int) ((60 * SAMPLESPERSEC) / (BeatsPerMin * TicksPerBeat));  
                    SamplesPerSub = SamplesPerTick / 6;
                }
#if !defined(__STAND_ALONE__)
                gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
                teac = 2;
#endif
                break;
#endif

#if defined(PTK_FX_SETSPEED)
            // $0f Set speed
            case 0x0f:
                TicksPerBeat = (int) pltr_dat_row;
                if(TicksPerBeat < 1) TicksPerBeat = 1;
                if(TicksPerBeat > 16) TicksPerBeat = 16;
                SamplesPerTick = (int) ((60 * SAMPLESPERSEC) / (BeatsPerMin * TicksPerBeat));  
                SamplesPerSub = SamplesPerTick / 6;

#if defined(PTK_SHUFFLE)
                if(shuffleswitch == 1) shufflestep = -((SamplesPerTick * shuffle) / 200);
                else shufflestep = (SamplesPerTick * shuffle) / 200;
#endif

#if !defined(__STAND_ALONE__)
                gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
                teac = 2;
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
                sp_Tvol[trackef] += pltr_dat_row * 0.0039062f;
                if(sp_Tvol[trackef] > 1.0f) sp_Tvol[trackef] = 1.0f;
                break;
#endif

#if defined(PTK_FX_VOLUMESLIDEDOWN)
            // $1a Volume slide down
            case 0x1a: 
                sp_Tvol[trackef] -= pltr_dat_row * 0.0039062f;
                if(sp_Tvol[trackef] < 0.0f) sp_Tvol[trackef] = 0.0f;
                break;
#endif

        }

#endif  // PTK_FX_X


#if defined(PTK_FX_ARPEGGIO)
        // Let's do the arpeggio
        double arpnote;

        if(Arpeggio_Switch[trackef])
        {
            switch((Subicounter % 3))
            {
                case 0:
                    Vstep1[trackef] = Vstep_arp[trackef];
                    break;
                case 1:
                    arpnote = (double) powf(2.0f, ((Arpeggio_BaseNote[trackef] + (Arpeggio_Switch[trackef] >> 4))) / 12.0f);
                    arpnote *= 4294967296.0f;
                    Vstep1[trackef] = (int64) arpnote;
                    break;
                case 2:
                    arpnote = (double) powf(2.0f, ((Arpeggio_BaseNote[trackef] + (Arpeggio_Switch[trackef] & 0xf))) / 12.0f);
                    arpnote *= 4294967296.0f;
                    Vstep1[trackef] = (int64) arpnote;
                    break;
            }
        }
#endif

    }// FOR LOOP ON TRACKS
}

// ------------------------------------------------------
// Reset the tracks filters
void ResetFilters(char tr)
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
float ApplyLfo(float cy, char trcy)
{

#if defined(PTK_LFO)
    if(LFO_ON[trcy] == 1)
    {
        cy += SIN[f2i(LFOGR[trcy])] * LFO_AMPL[trcy];
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
void ComputeStereo(char channel)
{
    LVol[channel] = 1.0f - TPan[channel];
    RVol[channel] = 1.0f - LVol[channel];
    if(LVol[channel] > 0.5f) LVol[channel] = 0.5f;
    if(RVol[channel] > 0.5f) RVol[channel] = 0.5f;
}

// ------------------------------------------------------
// Main mixing routine
void GetPlayerValues(float master_coef)
{
    left_chorus = 2.0f;
    right_chorus = 2.0f;
    Sp_Player();

    if(++lchorus_counter > 88200) lchorus_counter = 44100;
    if(++rchorus_counter > 88200) rchorus_counter = 44100;
    lbuff_chorus[lchorus_counter] = left_chorus + lbuff_chorus[lchorus_counter2] * lchorus_feedback;
    rbuff_chorus[rchorus_counter] = right_chorus + rbuff_chorus[rchorus_counter2] * rchorus_feedback;
    if(++lchorus_counter2 > 88200) lchorus_counter2 = 44100;
    if(++rchorus_counter2 > 88200) rchorus_counter2 = 44100;
    float rchore = lbuff_chorus[lchorus_counter2];
    float lchore = rbuff_chorus[rchorus_counter2];
    left_float = lchore + left_float;
    right_float = rchore + right_float;

#if defined(PTK_COMPRESSOR)
    Compressor_work();
#endif

#if defined(PTK_LIMITER)
    mas_comp_pos_rms_buffer++;
    if(mas_comp_pos_rms_buffer > MAS_COMPRESSOR_SIZE - 1) mas_comp_pos_rms_buffer = 0;
    if(mas_comp_ratio != 0.0f)
    {
        left_float = Mas_Compressor(left_float / 32767.0f, &rms_sumL, mas_comp_bufferL, &mas_envL) * 32767.0f;
        right_float = Mas_Compressor(right_float / 32767.0f, &rms_sumR, mas_comp_bufferR, &mas_envR) * 32767.0f;
    }
#endif

#if defined(__LINUX__)
    // It looks like the maximum range for OSS are -8192 +8192
    // (but i'm not sure about that)
    left_float /= 32767.0f;
    right_float /= 32767.0f;
    left_float *= 8192.0f;
    right_float *= 8192.0f;
#endif

    left_value = f2i(left_float * master_coef);
    right_value = f2i(right_float * master_coef);

    if(left_value > 32767) left_value = 32767;
    if(left_value < -32767) left_value = -32767;
    if(right_value > 32767) right_value = 32767;
    if(right_value < -32767) right_value = -32767;
}

// ------------------------------------------------------
// Precalc filters coefficients
void ComputeCoefs(int freq, int r, int t)
{
    float omega = float(2 * PI * Kutoff(freq) / 44100.0f);
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

float Kutoff( int v)
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

int f2i(float d)
{
    return((int) d);
}

#if defined(PTK_PROC_FILTER2P)
float filter2p(char ch, float input, float f, float q)
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
float filterhp2(char ch, float input, float f, float q)
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
float filterhp(char ch, float input, float f, float q)
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
float filter2p24d(char ch, float input, float f, float q)
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
float filterRingMod(char ch, float input, float f, float q)
{
    q++;

    f = float(f * 0.0078125f);
    buf0[ch] += f * (q * 0.125f);
    if(buf0[ch] >= 360.0f) buf0[ch] -= 360.0f;

    return input * SIN[f2i(buf0[ch])];
}
#endif

#if defined(PTK_PROC_FILTERRINGMODSTEREO)
float filterRingModStereo(char ch, float input)
{
    return float(input * cosf(buf0[ch] * 0.0174532f));
}
#endif

#if defined(PTK_PROC_FILTERWATER)
float filterWater(char ch, float input, float f, float q)
{
    f = 127.0f - f;
    float ad = input - buf0[ch];

    if(ad > 1.0f || ad < -1.0f) buf0[ch] += ad / f;
    return buf0[ch];
}
#endif

#if defined(PTK_PROC_FILTERWATERSTEREO)
float filterWaterStereo(char ch, float input, float f, float q)
{
    f = 127.0f - f;
    float ad = input - buf1[ch];

    if(ad > 1.0f || ad < -1.0f) buf1[ch] += ad / f;
    return buf1[ch];
}
#endif

#if defined(PTK_PROC_FILTERDELTA)
float filterDelta(char ch, float input, float f, float q)
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

/*float filterDeltaStereo(char ch, float input, float f, float q)
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
float filterBellShaped(char ch, float input, float f, float q, float g)
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
float int_filter2p(char ch, float input, float f, float q, float q2)
{
    q *= 0.0787401f;
    input = filter2px(ch, input, f, q2);
    return float(32767.0f * powf(fabsf(input) / 32767.0f, 1.0f - q / 11.0f));
}

float filter2px(char ch, float input, float f, float q)
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
    }

#if !defined(__STAND_ALONE__)
    if(!sr_isrecording) Actualize_303_Ed(0);
#endif

}

void Fire303(unsigned char number,char unit)
{
    tb303engine[unit].tbLine = 0;
   
    switch(number)
    {
        case 0x00: tb303engine[unit].tbPattern = tb303[unit].selectedpattern; break;
        case 0xa1: tb303engine[unit].tbPattern = 0; break;
        case 0xa2: tb303engine[unit].tbPattern = 1; break;
        case 0xa3: tb303engine[unit].tbPattern = 2; break;
        case 0xa4: tb303engine[unit].tbPattern = 3; break;
        case 0xa5: tb303engine[unit].tbPattern = 4; break;
        case 0xa6: tb303engine[unit].tbPattern = 5; break;
        case 0xa7: tb303engine[unit].tbPattern = 6; break;
        case 0xa8: tb303engine[unit].tbPattern = 7; break;
        case 0xb1: tb303engine[unit].tbPattern = 8; break;
        case 0xb2: tb303engine[unit].tbPattern = 9; break;
        case 0xb3: tb303engine[unit].tbPattern = 10; break;
        case 0xb4: tb303engine[unit].tbPattern = 11; break;
        case 0xb5: tb303engine[unit].tbPattern = 12; break;
        case 0xb6: tb303engine[unit].tbPattern = 13; break;
        case 0xb7: tb303engine[unit].tbPattern = 14; break;
        case 0xb8: tb303engine[unit].tbPattern = 15; break;
        case 0xc1: tb303engine[unit].tbPattern = 16; break;
        case 0xc2: tb303engine[unit].tbPattern = 17; break;
        case 0xc3: tb303engine[unit].tbPattern = 18; break;
        case 0xc4: tb303engine[unit].tbPattern = 19; break;
        case 0xc5: tb303engine[unit].tbPattern = 20; break;
        case 0xc6: tb303engine[unit].tbPattern = 21; break;
        case 0xc7: tb303engine[unit].tbPattern = 22; break;
        case 0xc8: tb303engine[unit].tbPattern = 23; break;
        case 0xd1: tb303engine[unit].tbPattern = 24; break;
        case 0xd2: tb303engine[unit].tbPattern = 25; break;
        case 0xd3: tb303engine[unit].tbPattern = 26; break;
        case 0xd4: tb303engine[unit].tbPattern = 27; break;
        case 0xd5: tb303engine[unit].tbPattern = 28; break;
        case 0xd6: tb303engine[unit].tbPattern = 29; break;
        case 0xd7: tb303engine[unit].tbPattern = 30; break;
        case 0xd8: tb303engine[unit].tbPattern = 31; break;

        default: /* No Fire */
            tb303engine[unit].tbLine = 255;
            break;
    }

#if !defined(__STAND_ALONE__)
    if(!sr_isrecording) Actualize_303_Ed(0);
#endif

}

void noteoff303(char strack)
{
    if(strack == track3031)
    {
        tb303engine[0].tbLine = 255;
        tb303engine[0].tbPattern = 255;
        track3031 = 255;
        tb303engine[0].tbBuf0 = 0.0f;
        tb303engine[0].tbBuf1 = 0.0f;
    }

    if(strack == track3032)
    {
        tb303engine[1].tbLine = 255;
        tb303engine[1].tbPattern = 255;
        track3032 = 255;
        tb303engine[1].tbBuf0 = 0.0f;
        tb303engine[1].tbBuf1 = 0.0f;
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
            SACTIVE[inico][inico2] = true;

#if !defined(__STAND_ALONE__)
            SHISTORY[inico][inico2] = false;
#endif

        }
        pSequence[inico] = 0;

        if(inico < 128)
        {
            beatsync[inico] = false;
            beatlines[inico] = 16;

#if !defined(__STAND_ALONE__)
            sprintf(nameins[inico], "Untitled");
#endif

            ResetSynthParameters(&PARASynth[inico]);
            KillInst(inico);
        }
    }
}

void KillInst(int inst_nbr)
{

#if !defined(__NOCODEC__)
    // Gsm is the default packing scheme
    SampleCompression[inst_nbr] = SMP_PACK_GSM;
#else
    SampleCompression[inst_nbr] = SMP_PACK_NONE;
#endif

    for(int z = 0; z < 16; z++)
    {
        if(RawSamples[inst_nbr][0][z])
        {
            if(RawSamples[inst_nbr][0][z]) free(RawSamples[inst_nbr][0][z]);
            RawSamples[inst_nbr][0][z] = NULL;
        }
        RawSamples[inst_nbr][0][z] = NULL;
        if(SampleChannels[inst_nbr][z] == 2)
        {
            if(RawSamples[inst_nbr][1][z])
            {
                if(RawSamples[inst_nbr][1][z]) free(RawSamples[inst_nbr][1][z]);
                RawSamples[inst_nbr][1][z] = NULL;
            }
            RawSamples[inst_nbr][1][z] = NULL;
        }

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

        Synthprg[inst_nbr] = SYNTH_WAVE_OFF;
        CustomVol[inst_nbr] = 1.0f;
    }
}

// ------------------------------------------------------
/* Next Function: used to reset synthparameters Structure */
/* Well, I think the default preset is not very cool, but nah! */
void ResetSynthParameters(SynthParameters *TSP)
{

#if !defined(__STAND_ALONE__)
    sprintf(TSP->presetname, "Untitled");
#endif

    TSP->osc1_waveform = 1;
    TSP->osc2_waveform = 2;
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
    TSP->env1_osc1_volume = 64;
    TSP->env1_osc2_volume = 64;   
    TSP->env1_vcf_cutoff = 64;
    TSP->env1_vcf_resonance = 64; 
    TSP->env2_osc1_pw = 64;
    TSP->env2_osc2_pw = 64;
    TSP->env2_osc1_pitch = 64;
    TSP->env2_osc2_pitch = 64;
    TSP->env2_osc1_volume = 64;
    TSP->env2_osc2_volume = 64;
    TSP->env2_vcf_cutoff = 64;
    TSP->env2_vcf_resonance = 64; 
    TSP->osc3_volume = 128;
    TSP->osc3_switch = false;
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

// ------------------------------------------------------
// Free all allocated Samples
void Free_Samples(void)
{
    for(int freer = 0; freer < 128; freer++)
    {
        for(char pedsplit = 0; pedsplit < 16; pedsplit++)
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
        }
    }
}
#endif // __STAND_ALONE__

// ------------------------------------------------------
#if defined(PTK_COMPRESSOR)
void Initreverb()
{
    LFP_L.Reset();

    switch(DelayType)
    {
        case 0: // Deep Night II
            decays[0][0] = 20; decays[0][1] =  0;
            decays[1][0] =  0; decays[1][1] = 15;
            decays[2][0] = 15; decays[2][1] =  0;
            decays[3][0] =  0; decays[3][1] = 10;
            decays[4][0] =  9; decays[4][1] =  0;
            decays[5][0] =  1; decays[5][1] =  8;
            decays[6][0] =  8; decays[6][1] =  1;
            decays[7][0] =  1; decays[7][1] =  4;
            decays[8][0] =  4; decays[8][1] =  0;
            decays[9][0] =  1; decays[9][1] =  2;

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

        case 1: // Deep Night III
            decays[0][0] =  7; decays[0][1] =  7;
            decays[1][0] =-13; decays[1][1] =-15;
            decays[2][0] = 25; decays[2][1] = 32;
            decays[3][0] = 31; decays[3][1] = 26;
            decays[4][0] = 20; decays[4][1] =-30;
            decays[5][0] = 28; decays[5][1] = 24;
            decays[6][0] =-21; decays[6][1] =-18;
            decays[7][0] = 18; decays[7][1] = 14;
            decays[8][0] =-13; decays[8][1] =-12;
            decays[9][0] =  9; decays[9][1] =  7;

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

        case 2: // Deep Night III
            decays[0][0] =  1; decays[0][1] =  2;
            decays[1][0] =  1; decays[1][1] = -4;
            decays[2][0] =  9; decays[2][1] =  1;
            decays[3][0] = 12; decays[3][1] = 11;
            decays[4][0] = 22; decays[4][1] = -1;
            decays[5][0] =  1; decays[5][1] = 19;
            decays[6][0] = 15; decays[6][1] = -1;
            decays[7][0] =  1; decays[7][1] = 12;
            decays[8][0] =  7; decays[8][1] =- 1;
            decays[9][0] =  2; decays[9][1] =  3;

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

        case 3: // Deep Night II
            decays[0][0] = 22; decays[0][1] =  3;
            decays[1][0] =  5; decays[1][1] = 12;
            decays[2][0] = 12; decays[2][1] =  1;
            decays[3][0] =  3; decays[3][1] =  5;

            delays[0] = 2000;
            delays[1] = 4400; 
            delays[2] = 5000;
            delays[3] = 6200;

            num_echoes = 4;
            break;

        case 4: // Deep Night II
            decays[0][0] = 11; decays[0][1] = 0;
            decays[1][0] = 0 ; decays[1][1] = 21;
            decays[2][0] = 31; decays[2][1] = 0;
            decays[3][0] = 0 ; decays[3][1] = 41;

            delays[0] = 3012;
            delays[1] = 4012;
            delays[2] = 4022;
            delays[3] = 5232;

            num_echoes = 4;
            break;

        case 5: // Deep Night III
            decays[0][0] =  7; decays[0][1] =  7;
            decays[1][0] =-13; decays[1][1] =-15;
            decays[2][0] = 25; decays[2][1] = 32;
            decays[3][0] = 31; decays[3][1] = 26;
            decays[4][0] = 20; decays[4][1] =-30;
            decays[5][0] = 28; decays[5][1] = 24;
            decays[6][0] =-21; decays[6][1] =-18;
            decays[7][0] = 18; decays[7][1] = 14;
            decays[8][0] =-13; decays[8][1] =-12;
            decays[9][0] =  9; decays[9][1] =  7;

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

        case 6: // Deep Night III
            decays[0][0] =  7; decays[0][1] =  7;
            decays[1][0] =-13; decays[1][1] =-15;
            decays[2][0] = 25; decays[2][1] = 32;
            decays[3][0] = 31; decays[3][1] = 26;
            decays[4][0] = 20; decays[4][1] =-30;
            decays[5][0] = 28; decays[5][1] = 24;
            decays[6][0] =-21; decays[6][1] =-18;
            decays[7][0] = 18; decays[7][1] = 14;
            decays[8][0] =-13; decays[8][1] =-12;
            decays[9][0] = 12; decays[9][1] =  8;

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

    for(int i = 0; i < num_echoes; i++)
    {
        int mlrw = 99999 - (delay_time + delays[i]) * 4;

        if(mlrw < 0) mlrw = 0;

        decays[i][0] = decays[i][0] * 0.015625f;
        decays[i][1] = decays[i][1] * 0.015625f;

        counters[i] = mlrw;
    }
    rev_counter = 99999;
}

// ------------------------------------------------------
void Compressor_work(void)
{
    if(compressor)
    {
        float l_rout = 0;
        float r_rout = 0;

        // Comb filter
        for(int i = 0; i < num_echoes; i++)
        {
            delay_left_buffer[i][rev_counter] = (delay_left_final + delay_right_final + delay_left_buffer[i][counters[i]]) * decays[i][0];
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

        // Stereo diffussion
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
            tbpars->flag[c][d].reserved2_flag = 0;
            tbpars->flag[c][d].reserved3_flag = 0;
            tbpars->flag[c][d].reserved4_flag = 0;
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
