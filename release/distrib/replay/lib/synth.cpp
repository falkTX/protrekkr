// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/replay.h"

// ------------------------------------------------------
// Variables
int SIZE_WAVEFORMS;

float SIN[360]; // Sine float-precalculated table, in absolute degrees.
short STOCK_SIN[SIZE_WAVEFORMS_SPACE * 2];
short STOCK_SAW[SIZE_WAVEFORMS_SPACE * 2];
short STOCK_PUL[SIZE_WAVEFORMS_SPACE * 2];
short STOCK_WIT[SIZE_WAVEFORMS_SPACE * 2];

#if defined(PTK_SYNTH_PINKNOISE)
    short STOCK_PIN[SIZE_WAVEFORMS_SPACE * 2];
#endif

// ------------------------------------------------------
// This next function resets the synthesizer to default values
void CSynth::Reset(void)
{

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    // Synthesizer General Reset
    GS_VAL = 0;
    GLB_VOLUME = 1.0f;
    DISTO = 0.0f;
    OSC1_WAVEFORM = 1;      /* Sawtooth */
    OSC2_WAVEFORM = 2;      /* Pulse */

    T_OSC1_VOLUME = 0;
    T_OSC2_VOLUME = 0;

    GLOBAL_VOLUME = 0;

    OSC2_DETUNE = 0;        /* No Semitone Detune */
    OSC2_FINETUNE = 0.1f;   /* 1/10 Semitone detune */

    VCF_CUTOFF = 0.5f;      /* 10000Hz Cutoff */
    VCF_RESONANCE = 0.5f;   /* Not very weird =] */
    VCF_TYPE = 0;           /* LowPass filter */

    ENV1_ATTACK = 0.0f;     /* About 59 miliseconds */
    ENV1_DECAY = 0.1f;      /* The same here */
    ENV1_SUSTAIN = 0.3f;    /* Sustain volume at 1/3 */
    ENV1_RELEASE = 0.0f;    /* About 371 Miliseconds */ 

    ENV2_ATTACK = 0.0f;     /* About 59 miliseconds */
    ENV2_DECAY = 0.1f;      /* The same here */
    ENV2_SUSTAIN = 0.3f;    /* Sustain volume at 1/3 */
    ENV2_RELEASE = 0.0f;    /* About 371 Miliseconds */ 

    LFO1_PERIOD = 16;
    LFO2_PERIOD = 16;

    LFO1_GR = 0;
    LFO2_GR = 0;

    LFO1_SUBGRCOUNTER = 0;
    LFO2_SUBGRCOUNTER = 0;

    LFO1_SUBGRMAX = 200;
    LFO2_SUBGRMAX = 200;

    LFO1_VALUE = 0;
    LFO2_VALUE = 0;

    LFO1_OSC1_PW = 0;
    LFO1_OSC2_PW = 0;
    LFO1_OSC1_PITCH = 0;
    LFO1_OSC2_PITCH = 0;
    LFO1_OSC1_VOLUME = 0;
    LFO1_OSC2_VOLUME = 0;   
    LFO1_VCF_CUTOFF = 0;
    LFO1_VCF_RESONANCE = 0; 

    LFO2_OSC1_PW = 0;
    LFO2_OSC2_PW = 0;
    LFO2_OSC1_PITCH = 0;
    LFO2_OSC2_PITCH = 0;
    LFO2_OSC1_VOLUME = 0;
    LFO2_OSC2_VOLUME = 0;   
    LFO2_VCF_CUTOFF = 0;
    LFO2_VCF_RESONANCE = 0; 

    ENV1_OSC1_PW = 0;
    ENV1_OSC2_PW = 0;
    ENV1_OSC1_PITCH = 0;
    ENV1_OSC2_PITCH = 0;
    ENV1_OSC1_VOLUME = 1.0f;
    ENV1_OSC2_VOLUME = 1.0f;   
    ENV1_VCF_CUTOFF = 0;
    ENV1_VCF_RESONANCE = 0; 

    ENV2_OSC1_PW = 0;
    ENV2_OSC2_PW = 0;
    ENV2_OSC1_PITCH = 0;
    ENV2_OSC2_PITCH = 0;
    ENV2_OSC1_VOLUME = 0;
    ENV2_OSC2_VOLUME = 0;   
    ENV2_VCF_CUTOFF = 0;
    ENV2_VCF_RESONANCE = 0; 

    OSC3_SWITCH = FALSE;

    OSC1_STEP = 0;
    OSC2_STEP = 0;

    ENV1_STAGE = 0;
    ENV2_STAGE = 0;

    ENV1_COUNTER = 0;
    ENV2_COUNTER = 0;

    ENV1_VALUE = 0;
    ENV2_VALUE = 0;

    ENV1_VOLUME_1 = 0;
    ENV1_VOLUME_2 = 0;
    ENV2_VOLUME_1 = 0;
    ENV2_VOLUME_2 = 0;

    ENV1_MIN = 0;
    ENV2_MIN = 0;

    LFO1_STAGE = 0;
    LFO1_COUNTER = 0;
    LFO1_ADSR_VALUE = 0;

    LFO2_STAGE = 0;
    LFO2_COUNTER = 0;
    LFO2_ADSR_VALUE = 0;

    LFO1_ATTACK = 0.0f;
    LFO1_DECAY = 0.1f;
    LFO1_SUSTAIN = 0.3f;
    LFO1_RELEASE = 0.0f;

    LFO2_ATTACK = 0.0f;
    LFO2_DECAY = 0.1f;
    LFO2_SUSTAIN = 0.3f;
    LFO2_RELEASE = 0.0f;

    GS_VAL = 0;

    OSC3_VOLUME = 0;

#if defined(__GCC__)
    PTC_GLIDE64 = 4294967296ll;
#else
    PTC_GLIDE64 = 4294967296;
#endif

#endif

    OSC1_SPEED = 0;
    OSC2_SPEED = 0;
    OSC3_SPEED = 0;
}

// ------------------------------------------------------
// This function is for internal use only. Makes the LFO's run.
void CSynth::LfoAdvance(void)
{
    LFO1_SUBGRCOUNTER++;
    LFO2_SUBGRCOUNTER++;

    switch(LFO1_STAGE)
    {
        case 1:
            LFO1_ADSR_VALUE += LFO1_A_COEF;
            LFO1_COUNTER++;
            if(LFO1_COUNTER >= LFO1b_ATTACK)
            {
                if(LFO1_ADSR_VALUE > 1.0f) LFO1_ADSR_VALUE = 1.0f;
                LFO1_COUNTER = 0;
                LFO1_STAGE = 2;
            }
            break;
        case 2:
            LFO1_ADSR_VALUE -= LFO1_D_COEF;
            LFO1_COUNTER++;
            if(LFO1_COUNTER >= LFO1b_DECAY)
            {
                if(LFO1_ADSR_VALUE < 0.0f) LFO1_ADSR_VALUE = 0.0f;
                LFO1_ADSR_VALUE = LFO1_SUSTAIN;
                LFO1_COUNTER = 0;
                LFO1_STAGE = 3;
            }
            break;
        case 3:
            LFO1_ADSR_VALUE = LFO1_SUSTAIN;
            break;
        case 4:
            LFO1_ADSR_VALUE -= LFO1_R_COEF;
            LFO1_COUNTER++;
            if(LFO1_COUNTER > LFO1b_RELEASE)
            {
                LFO1_ADSR_VALUE = 0.0f;
                LFO1_COUNTER = 0;
                LFO1_STAGE = 0;
            }
            break;
    }

    if(LFO1_SUBGRCOUNTER > LFO1_SUBGRMAX)
    {
        LFO1_SUBGRCOUNTER = 0;
        LFO1_GR++;
        if(LFO1_GR > 359) LFO1_GR = 0;
    }

    LFO1_VALUE = SIN[LFO1_GR] * LFO1_ADSR_VALUE;

    switch(LFO2_STAGE)
    {
        case 1:
            LFO2_ADSR_VALUE += LFO2_A_COEF;
            LFO2_COUNTER++;
            if(LFO2_COUNTER >= LFO2b_ATTACK)
            {
                if(LFO2_ADSR_VALUE > 1.0f) LFO2_ADSR_VALUE = 1.0f;
                LFO2_COUNTER = 0;
                LFO2_STAGE = 2;
            }
            break;
        case 2:
            LFO2_ADSR_VALUE -= LFO2_D_COEF;
            LFO2_COUNTER++;
            if(LFO2_COUNTER >= LFO2b_DECAY)
            {
                if(LFO2_ADSR_VALUE < 0.0f) LFO2_ADSR_VALUE = 0.0f;
                LFO2_ADSR_VALUE = LFO2_SUSTAIN;
                LFO2_COUNTER = 0;
                LFO2_STAGE = 3;
            }
            break;
        case 3:
            LFO2_ADSR_VALUE = LFO2_SUSTAIN;
            break;
        case 4:
            LFO2_ADSR_VALUE -= LFO2_R_COEF;
            LFO2_COUNTER++;
            if(LFO2_COUNTER > LFO2b_RELEASE)
            {
                LFO2_ADSR_VALUE = 0.0f;
                LFO2_COUNTER = 0;
                LFO2_STAGE = 0;
            }
            break;
    }

    if(LFO2_SUBGRCOUNTER > LFO2_SUBGRMAX)
    {
        LFO2_SUBGRCOUNTER = 0;
        LFO2_GR++;
        if(LFO2_GR > 359) LFO2_GR = 0;
    }

    LFO2_VALUE = SIN[LFO2_GR] * LFO2_ADSR_VALUE;
}

// ------------------------------------------------------
// The cooler NoteOn typical message for this Class CSynth Objects =].
// (used at tick 0)
void CSynth::NoteOn(float note, float speed, int Looping, unsigned int Length,
                    unsigned int Loop_Length, float note_smp)
{
    float note_1 = OSC1_WAVEFORM == 5 ? note_smp: note;
    float note_2 = OSC2_WAVEFORM == 5 ? note_smp: note;

    float smp_freq;
    float adsr_ratio;

    ENV1_STAGE = 1; /* '0' is off, '1' starts the attack */
    ENV2_STAGE = 1;

    ENV1_COUNTER = 0; /* Envelope stage counter, in samples */
    ENV2_COUNTER = 0;

    OSC1_STEP = powf(2.0f, note_1 / 12.0f);
    OSC2_STEP = powf(2.0f, (note_2 + OSC2_FINETUNE + OSC2_DETUNE) / 12.0f);

    ENV1_VALUE = 0;
    ENV2_VALUE = 0;

    ENV1_MIN = 0;
    ENV2_MIN = 0;

    ENV1_VOLUME_1 = 0;
    ENV2_VOLUME_1 = 0;
    ENV1_VOLUME_2 = 0;
    ENV2_VOLUME_2 = 0;

    if(OSC1_WAVEFORM != 5)
    {
        Length = SIZE_WAVEFORMS;
        Loop_Length = SIZE_WAVEFORMS;
    }
    smp_freq = ((float) Length) / OSC1_STEP;
    adsr_ratio = smp_freq / 1024.0f;
    ENV1b_ATTACK = (ENV1_ATTACK / adsr_ratio) * smp_freq;
    ENV1b_DECAY = (ENV1_DECAY / adsr_ratio) * smp_freq;
    LFO1b_ATTACK = (LFO1_ATTACK / adsr_ratio) * smp_freq;
    LFO1b_DECAY = (LFO1_DECAY / adsr_ratio) * smp_freq;
    if(Looping)
    {
        smp_freq = ((float) Loop_Length) / OSC1_STEP;
        adsr_ratio = smp_freq / 1024.0f;
    }
    ENV1b_RELEASE = (ENV1_RELEASE / adsr_ratio) * smp_freq;
    LFO1b_RELEASE = (LFO1_RELEASE / adsr_ratio) * smp_freq;

    if(OSC2_WAVEFORM != 5)
    {
        Length = SIZE_WAVEFORMS;
        Loop_Length = SIZE_WAVEFORMS;
    }
    smp_freq = ((float) Length) / OSC2_STEP;
    adsr_ratio = smp_freq / 1024.0f;
    ENV2b_ATTACK = (ENV2_ATTACK / adsr_ratio) * smp_freq;
    ENV2b_DECAY = (ENV2_DECAY / adsr_ratio) * smp_freq;
    LFO2b_ATTACK = (LFO2_ATTACK / adsr_ratio) * smp_freq;
    LFO2b_DECAY = (LFO2_DECAY / adsr_ratio) * smp_freq;
    if(Looping)
    {
        smp_freq = ((float) Loop_Length) / OSC2_STEP;
        adsr_ratio = smp_freq / 1024.0f;
    }
    ENV2b_RELEASE = (ENV2_RELEASE / adsr_ratio) * smp_freq;
    LFO2b_RELEASE = (LFO2_RELEASE / adsr_ratio) * smp_freq;

    if(ENV1b_ATTACK < 1.0f) ENV1b_ATTACK = 1.0f;
    if(ENV1b_DECAY < 1.0f) ENV1b_DECAY = 1.0f;
    if(ENV1b_RELEASE < 1.0f) ENV1b_RELEASE = 1.0f;
    if(ENV2b_ATTACK < 1.0f) ENV2b_ATTACK = 1.0f;
    if(ENV2b_DECAY < 1.0f) ENV2b_DECAY = 1.0f;
    if(ENV2b_RELEASE < 1.0f) ENV2b_RELEASE = 1.0f;

    /* Update ENV1 */
    ENV1_D_COEF = (1.0f - ENV1_SUSTAIN) / (float) ENV1b_DECAY;
    ENV1_R_COEF = ENV1_SUSTAIN / (float) ENV1b_RELEASE;

    /* Update ENV2 */
    ENV2_D_COEF = (1.0f - ENV2_SUSTAIN) / (float) ENV2b_DECAY;
    ENV2_R_COEF = ENV2_SUSTAIN / (float) ENV2b_RELEASE;

    ENV1_A_COEF = (1.0f - ENV1_VALUE) / ENV1b_ATTACK;
    ENV2_A_COEF = (1.0f - ENV2_VALUE) / ENV2b_ATTACK;

    ENV1_LOOP_BACKWARD = FALSE;
    ENV2_LOOP_BACKWARD = FALSE;
    ENV3_LOOP_BACKWARD = FALSE;

    LFO1_STAGE = 1;
    LFO2_STAGE = 1;
    LFO1_GR = 0;
    LFO2_GR = 0;
    LFO1_VALUE = 0;
    LFO2_VALUE = 0;
    LFO1_ADSR_VALUE = 0;
    LFO2_ADSR_VALUE = 0;
    LFO1_COUNTER = 0;
    LFO2_COUNTER = 0;
    LFO1_SUBGRCOUNTER = 0;
    LFO2_SUBGRCOUNTER = 0;

    if(LFO1b_ATTACK < 1.0f) LFO1b_ATTACK = 1.0f;
    if(LFO1b_DECAY < 1.0f) LFO1b_DECAY = 1.0f;
    if(LFO1b_RELEASE < 1.0f) LFO1b_RELEASE = 1.0f;
    if(LFO2b_ATTACK < 1.0f) LFO2b_ATTACK = 1.0f;
    if(LFO2b_DECAY < 1.0f) LFO2b_DECAY = 1.0f;
    if(LFO2b_RELEASE < 1.0f) LFO2b_RELEASE = 1.0f;

    LFO1_D_COEF = (1.0f - LFO1_SUSTAIN) / (float) LFO1b_DECAY;
    LFO1_R_COEF = LFO1_SUSTAIN / (float) LFO1b_RELEASE;

    LFO2_D_COEF = (1.0f - LFO2_SUSTAIN) / (float) LFO2b_DECAY;
    LFO2_R_COEF = LFO2_SUSTAIN / (float) LFO2b_RELEASE;

    LFO1_A_COEF = (1.0f - LFO1_ADSR_VALUE) / LFO1b_ATTACK;
    LFO2_A_COEF = (1.0f - LFO2_ADSR_VALUE) / LFO2b_ATTACK;

    GLOBAL_VOLUME = speed;
}

// ------------------------------------------------------
// Envelopes run function
void CSynth::EnvRun(int *track, int *track2)
{
    /* ENV1 */
    switch(ENV1_STAGE)
    {
        /* Attack */
        case 1:
            ENV1_VALUE += ENV1_A_COEF;
            ENV1_COUNTER++;
            if(ENV1_COUNTER >= ENV1b_ATTACK)
            {
                if(ENV1_VALUE > 1.0f) ENV1_VALUE = 1.0f;
                ENV1_COUNTER = 0;
                ENV1_STAGE = 2;
            }
            break;

        /* Decay */
        case 2:
            ENV1_VALUE -= ENV1_D_COEF;
            ENV1_COUNTER++;
            if(ENV1_COUNTER >= ENV1b_DECAY)
            {
                if(ENV1_VALUE < 0.0f) ENV1_VALUE = 0.0f;
                ENV1_VALUE = ENV1_SUSTAIN;
                ENV1_COUNTER = 0;
                ENV1_STAGE = 3;
            }
            break;

        /* Sustain */
        case 3:
            ENV1_VALUE = ENV1_SUSTAIN;
            break;

        /* Release */
        case 4:
            ENV1_VALUE -= ENV1_R_COEF;
            ENV1_COUNTER++;
            if(ENV1_COUNTER > ENV1b_RELEASE)
            {
                ENV1_VALUE = 0.0f;
                ENV1_COUNTER = 0;
                ENV1_STAGE = 0; /* Stop the rock ENV1 */
                OSC1_SPEED = 0;
                OSC3_SPEED = 0;
                *track = 0;
            }
            break;
    }

    /* ENV2 */
    switch(ENV2_STAGE)
    {
        /* Attack */
        case 1:
            ENV2_VALUE += ENV2_A_COEF;
            ENV2_COUNTER++;
            if(ENV2_COUNTER >= ENV2b_ATTACK)
            {
                if(ENV2_VALUE > 1.0f) ENV2_VALUE = 1.0f;
                ENV2_COUNTER = 0;
                ENV2_STAGE = 2;
            }
            break;

        /* Decay */
        case 2:
            ENV2_VALUE -= ENV2_D_COEF;
            ENV2_COUNTER++;
            if(ENV2_COUNTER >= ENV2b_DECAY)
            {
                if(ENV2_VALUE < 0.0f) ENV2_VALUE = 0.0f;
                ENV2_VALUE = ENV2_SUSTAIN;
                ENV2_COUNTER = 0;
                ENV2_STAGE = 3;
            }
            break;

        /* Sustain */
        case 3:
            ENV2_VALUE = ENV2_SUSTAIN;
            break;

        /* Release */
        case 4:
            ENV2_VALUE -= ENV2_R_COEF;
            ENV2_COUNTER++;
            if(ENV2_COUNTER > ENV2b_RELEASE)
            {
                ENV2_VALUE = 0.0f;
                ENV2_COUNTER = 0;
                ENV2_STAGE = 0; /* Stop the rock ENV2 */
                OSC2_SPEED = 0;
                *track2 = 0;
            }
            break;
    }

    if(ENV1_OSC1_VOLUME == 0.0f) ENV1_VOLUME_1 = ENV1_VALUE;
    else ENV1_VOLUME_1 = (ENV1_VALUE * ENV1_OSC1_VOLUME);

    if(ENV1_OSC2_VOLUME == 0.0f) ENV1_VOLUME_2 = ENV1_VALUE;
    else ENV1_VOLUME_2 = (ENV1_VALUE * ENV1_OSC2_VOLUME);

    if(ENV2_OSC1_VOLUME == 0.0f) ENV2_VOLUME_1 = ENV2_VALUE;
    else ENV2_VOLUME_1 = (ENV2_VALUE * ENV2_OSC1_VOLUME);

    if(ENV2_OSC2_VOLUME == 0.0f) ENV2_VOLUME_2 = ENV2_VALUE;
    else ENV2_VOLUME_2 = (ENV2_VALUE * ENV2_OSC2_VOLUME);

    ENV1_MIN = 0.0f;
    if(LFO1_OSC1_VOLUME == 0.0f && LFO2_OSC1_VOLUME == 0.0f)
    {
        ENV1_MIN = 1.0f;
    }
    ENV2_MIN = 0.0f;
    if(LFO1_OSC2_VOLUME == 0.0f && LFO2_OSC2_VOLUME == 0.0f)
    {
        ENV2_MIN = 1.0f;
    }
}

// ------------------------------------------------------
// 'Note Off' message for CSynth class objects
void CSynth::NoteOff(void)
{
    if(ENV1_STAGE > 0 && ENV1_STAGE < 4)
    {
        ENV1_R_COEF = ENV1_VALUE / (float) ENV1b_RELEASE;
        ENV1_COUNTER = 0;
        ENV1_STAGE = 4;
    }

    if(ENV2_STAGE > 0 && ENV2_STAGE < 4)
    {
        ENV2_R_COEF = ENV2_VALUE / (float) ENV2b_RELEASE;
        ENV2_COUNTER = 0;
        ENV2_STAGE = 4;
    }

    if(LFO1_STAGE > 0 && LFO1_STAGE < 4)
    {
        LFO1_R_COEF = LFO1_ADSR_VALUE / (float) LFO1b_RELEASE;
        LFO1_COUNTER = 0;
        LFO1_STAGE = 4;
    }

    if(LFO2_STAGE > 0 && LFO2_STAGE < 4)
    {
        LFO2_R_COEF = LFO2_ADSR_VALUE / (float) LFO2b_RELEASE;
        LFO2_COUNTER = 0;
        LFO2_STAGE = 4;
    }
}

// ------------------------------------------------------
// The cool/render function, gets the next synth sample.
// (Used at all ticks)
float CSynth::GetSample(short *Left_Samples,
                        short *Right_Samples,
                        char Stereo,
                        char Loop_Type,
                        unsigned int Length,
                        unsigned int Loop_Sub,
                        float *Right_Signal,
                        unsigned int Rns, float vol,
                        int *track, 
                        int *track2,
                        Uint64 *position_osc1,
                        Uint64 *position_osc2,
#if defined(PTK_SYNTH_OSC3)
                        Uint64 *position_osc3,
#endif
                        int64 osc_speed)
    {

    s_access *pos_osc1 = (s_access *) position_osc1;
    s_access *pos_osc2 = (s_access *) position_osc2;

#if defined(PTK_SYNTH_OSC3)
    s_access *pos_osc3 = (s_access *) position_osc3;
#endif

    s_access *pos_osc1_disto;
    short *Left_Samples1;
    unsigned int i_POSITION;
    unsigned int res_dec;

    int64 osc_speed1;
    int64 osc_speed1b;
    int64 osc_speed2;
    int64 osc_speed_tune;
    float mul_datL;
    float mul_datR;

    GS_VAL = 0;
    GS_VAL2 = 0;

    if(ENV1_STAGE)
    {
        /* Oscillator1 On */
        if(OSC1_WAVEFORM != 4)
        {
            T_OSC1_VOLUME = ((LFO1_VALUE * LFO1_OSC1_VOLUME + LFO2_VALUE * LFO2_OSC1_VOLUME) + ENV1_MIN)
                             * ENV1_VOLUME_1 * ENV1_VOLUME_2;

            if(*track)
            {
                if(PTC_GLIDE64 != 0 && OSC1_SPEED != 0)
                {
                    if(osc_speed > OSC1_SPEED)
                    {
                        OSC1_SPEED += PTC_GLIDE64;
                        if(OSC1_SPEED > osc_speed) OSC1_SPEED = osc_speed;
                    }
                    else
                    {
                        OSC1_SPEED -= PTC_GLIDE64;
                        if(OSC1_SPEED < osc_speed) OSC1_SPEED = osc_speed;
                    }
                }
                else
                {
                    OSC1_SPEED = osc_speed;
                }
                osc_speed1 = (int64) ((double) (
                            + LFO1_VALUE * LFO1_OSC1_PITCH
                            + LFO2_VALUE * LFO2_OSC1_PITCH
                            + ENV1_VALUE * ENV1_OSC1_PITCH
                            + ENV2_VALUE * ENV2_OSC1_PITCH) * 4294967296.0);
                osc_speed2 = OSC1_SPEED;
                Left_Samples1 = Left_Samples;
                if(OSC1_WAVEFORM != 5)
                {
                    switch(OSC1_WAVEFORM)
                    {
                        case 0:
                            Left_Samples1 = STOCK_SIN;
                            break;
                        case 1:
                            Left_Samples1 = STOCK_SAW;
                            break;
                        case 2:
                            Left_Samples1 = STOCK_PUL;
                            break;
                        case 3:
                            Left_Samples1 = STOCK_WIT;
                            break;
#if defined(PTK_SYNTH_PINKNOISE)
                        case 6:
                            Left_Samples1 = STOCK_PIN;
                            break;
#endif
                    }
                    osc_speed2 *= 65;
                    osc_speed1 *= 65;
                    Length = SIZE_WAVEFORMS;
                    Rns = SIZE_WAVEFORMS;
                    Loop_Sub = SIZE_WAVEFORMS;
                    Loop_Type = SMP_LOOP_FORWARD;
                    Stereo = 1;
                }

                if(Left_Samples1)
                {
                    pos_osc1_disto = pos_osc1;

                    res_dec = pos_osc1_disto->half.last;
                    i_POSITION = pos_osc1_disto->half.first;
                    if(Loop_Type == SMP_LOOP_NONE) if(i_POSITION > Length - 2) i_POSITION = Length - 2;
                    if((int) i_POSITION < 0) i_POSITION = 0;

                    mul_datL = 1.0f;
                    mul_datR = 1.0f;
                    if(OSC1_PW)
                    {
                        T_OSC_PW = OSC1_PW * 
                                  ((LFO1_VALUE * LFO1_OSC1_PW) 
                                 + (LFO2_VALUE * LFO2_OSC1_PW)
                                 + (ENV1_VALUE * ENV1_OSC1_PW)
                                 + (ENV2_VALUE * ENV2_OSC1_PW));
                        if(*(Left_Samples1 + i_POSITION) > 0) mul_datL = T_OSC_PW * 2.0f;
                        if(Stereo == 2) if(*(Right_Samples + i_POSITION) > 0) mul_datR = T_OSC_PW * 2.0f;
                    }

                    GS_VAL = (Cubic_Work(
                                (float) (*(Left_Samples1 + i_POSITION - 1)) * mul_datL,
                                (float) (*(Left_Samples1 + i_POSITION)) * mul_datL,
                                (float) (*(Left_Samples1 + i_POSITION + 1)) * mul_datL,
                                (float) (*(Left_Samples1 + i_POSITION + 2)) * mul_datL, res_dec, i_POSITION, Rns) * vol) * T_OSC1_VOLUME;

                    // Stereo sample
                    if(Stereo == 2)
                    {
                        GS_VAL2 = (Cubic_Work(
                                        (float) (*(Right_Samples + i_POSITION - 1)) * mul_datR,
                                        (float) (*(Right_Samples + i_POSITION)) * mul_datR,
                                        (float) (*(Right_Samples + i_POSITION + 1)) * mul_datR,
                                        (float) (*(Right_Samples + i_POSITION + 2)) * mul_datR, res_dec, i_POSITION, Rns) * vol) * T_OSC1_VOLUME;
                    }

                    osc_speed2 += osc_speed1;
                    if(osc_speed2 < 16) osc_speed2 = 16;

                    if(ENV1_LOOP_BACKWARD == TRUE) pos_osc1->absolu -= osc_speed2;
                    else pos_osc1->absolu += osc_speed2;

                    switch(Loop_Type)
                    {
                        case SMP_LOOP_NONE:
                            if(pos_osc1->half.first >= Length)
                            {
                                pos_osc1->half.first = Length;
                                *track = 0;
                            }
                            break;
                        case SMP_LOOP_FORWARD:
                            if(pos_osc1->half.first >= Length)
                            {
                                pos_osc1->half.first -= Loop_Sub;
                            }
                            break;
                        case SMP_LOOP_PINGPONG:
                            if(ENV1_LOOP_BACKWARD == TRUE)
                            {
                                if((int) pos_osc1->half.first <= (int) (Length - Loop_Sub))
                                {
                                    pos_osc1->half.first = Length - Loop_Sub;
                                    ENV1_LOOP_BACKWARD = FALSE;
                                }
                            }
                            else
                            {
                                if(pos_osc1->half.first >= Length)
                                {
                                    pos_osc1->half.first = Length;
                                    ENV1_LOOP_BACKWARD = TRUE;
                                }
                            }
                            break;
                    }
                }
            }
        }
    }

    if(ENV2_STAGE)
    {
        /* Oscillator2 On */
        if(OSC2_WAVEFORM != 4)
        {
            if(*track2)
            {
                osc_speed_tune = osc_speed + (int64) ((double) OSC2_FINETUNE * 536870912.0)
                                           + (int64) ((double) OSC2_DETUNE * 536870912.0);
                if(PTC_GLIDE64 != 0 && OSC2_SPEED != 0)
                {
                    if(osc_speed_tune > OSC2_SPEED)
                    {
                        OSC2_SPEED += PTC_GLIDE64;
                        if(OSC2_SPEED > osc_speed_tune) OSC2_SPEED = osc_speed_tune;
                    }
                    else
                    {
                        OSC2_SPEED -= PTC_GLIDE64;
                        if(OSC2_SPEED < osc_speed_tune) OSC2_SPEED = osc_speed_tune;
                    }
                }
                else
                {
                    OSC2_SPEED = osc_speed_tune;
                }

                osc_speed1b = ((int64) ((double) (
                                  LFO1_VALUE * LFO1_OSC2_PITCH
                                + LFO2_VALUE * LFO2_OSC2_PITCH
                                + ENV1_VALUE * ENV1_OSC2_PITCH
                                + ENV2_VALUE * ENV2_OSC2_PITCH) * 4294967296.0));
                osc_speed2 = OSC2_SPEED;
                if(OSC2_WAVEFORM != 5)
                {
                    switch(OSC2_WAVEFORM)
                    {
                        case 0:
                            Left_Samples = STOCK_SIN;
                            break;
                        case 1:
                            Left_Samples = STOCK_SAW;
                            break;
                        case 2:
                            Left_Samples = STOCK_PUL;
                            break;
                        case 3:
                            Left_Samples = STOCK_WIT;
                            break;
#if defined(PTK_SYNTH_PINKNOISE)
                        case 6:
                            Left_Samples = STOCK_PIN;
                            break;
#endif
                    }
                    osc_speed1b *= 65;
                    osc_speed2 *= 65;
                    Length = SIZE_WAVEFORMS;
                    Rns = SIZE_WAVEFORMS;
                    Loop_Sub = SIZE_WAVEFORMS;
                    Loop_Type = SMP_LOOP_FORWARD;
                    Stereo = 1;
                }

                if(Left_Samples)
                {
                    T_OSC2_VOLUME = ((LFO1_VALUE * LFO1_OSC2_VOLUME + LFO2_VALUE * LFO2_OSC2_VOLUME) + ENV2_MIN)
                    * ENV2_VOLUME_1 * ENV2_VOLUME_2;

                    res_dec = pos_osc2->half.last;
                    i_POSITION = pos_osc2->half.first;
                    if(Loop_Type == SMP_LOOP_NONE) if(i_POSITION > Length - 2) i_POSITION = Length - 2;
                    if((int) i_POSITION < 0) i_POSITION = 0;

                    mul_datL = 1.0f;
                    mul_datR = 1.0f;
                    if(OSC2_PW)
                    {
                        T_OSC_PW = OSC2_PW * 
                                 ((LFO1_VALUE * LFO1_OSC2_PW) 
                                + (LFO2_VALUE * LFO2_OSC2_PW)
                                + (ENV1_VALUE * ENV1_OSC2_PW)
                                + (ENV2_VALUE * ENV2_OSC2_PW));
                        if(*(Left_Samples + i_POSITION) > 0) mul_datL = T_OSC_PW * 2.0f;
                        if(Stereo == 2) if(*(Right_Samples + i_POSITION) > 0) mul_datR = T_OSC_PW * 2.0f;
                    }

                    GS_VAL += (Cubic_Work(
                                (float) (*(Left_Samples + i_POSITION - 1)) * mul_datL,
                                (float) (*(Left_Samples + i_POSITION)) * mul_datL,
                                (float) (*(Left_Samples + i_POSITION + 1)) * mul_datL,
                                (float) (*(Left_Samples + i_POSITION + 2)) * mul_datL,
                                res_dec, i_POSITION, Rns) * vol) * T_OSC2_VOLUME;

                    // Stereo sample
                    if(Stereo == 2)
                    {
                        GS_VAL2 += (Cubic_Work(
                                    (float) (*(Right_Samples + i_POSITION - 1)) * mul_datR,
                                    (float) (*(Right_Samples + i_POSITION)) * mul_datR,
                                    (float) (*(Right_Samples + i_POSITION + 1)) * mul_datR,
                                    (float) (*(Right_Samples + i_POSITION + 2)) * mul_datR,
                                    res_dec, i_POSITION, Rns) * vol) * T_OSC2_VOLUME;
                    }

                    osc_speed2 += osc_speed1b;
                    if(osc_speed2 < 16) osc_speed2 = 16;

                    if(ENV2_LOOP_BACKWARD == TRUE) pos_osc2->absolu -= osc_speed2;
                    else pos_osc2->absolu += osc_speed2;

                    switch(Loop_Type)
                    {
                        case SMP_LOOP_NONE:
                            if(pos_osc2->half.first >= Length)
                            {
                                pos_osc2->half.first = Length;
                                *track2 = 0;
                            }
                            break;
                        case SMP_LOOP_FORWARD:
                            if(pos_osc2->half.first >= Length)
                            {
                                pos_osc2->half.first -= Loop_Sub;
                            }
                            break;
                        case SMP_LOOP_PINGPONG:
                            if(ENV2_LOOP_BACKWARD == TRUE)
                            {
                                if((int) pos_osc2->half.first <= (int) (Length - Loop_Sub))
                                {
                                    pos_osc2->half.first = Length - Loop_Sub;
                                    ENV2_LOOP_BACKWARD = FALSE;
                                }
                            }
                            else
                            {
                                if(pos_osc2->half.first >= Length)
                                {
                                    pos_osc2->half.first = Length;
                                    ENV2_LOOP_BACKWARD = TRUE;
                                }
                            }
                            break;
                    }
                }
            }
        }
    }

#if defined(PTK_SYNTH_OSC3)
    if(OSC3_SWITCH)
    {

        /* SubOscillator On */
        if(OSC1_WAVEFORM != 4)
        {

            if(*track)
            {

                osc_speed2 = OSC1_SPEED / 2;
                osc_speed1 = ((int64) ((double) (
                                + LFO1_VALUE * LFO1_OSC1_PITCH
                                + LFO2_VALUE * LFO2_OSC1_PITCH
                                + ENV1_VALUE * ENV1_OSC1_PITCH
                                + ENV2_VALUE * ENV2_OSC1_PITCH) * 4294967296.0)) / 2;

                if(OSC1_WAVEFORM != 5)
                {
                    osc_speed2 *= 65;
                    osc_speed1 *= 65;
                    Length = SIZE_WAVEFORMS;
                    Rns = SIZE_WAVEFORMS;
                    Loop_Sub = SIZE_WAVEFORMS;
                    Loop_Type = SMP_LOOP_FORWARD;
                    Stereo = 1;
                }

                if(Left_Samples1)
                {

                    res_dec = pos_osc3->half.last;
                    i_POSITION = pos_osc3->half.first;
                    if(Loop_Type == SMP_LOOP_NONE) if(i_POSITION > Length - 2) i_POSITION = Length - 2;
                    if((int) i_POSITION < 0) i_POSITION = 0;

                    mul_datL = 1.0f;
                    mul_datR = 1.0f;
                    if(OSC1_PW)
                    {
                        T_OSC_PW = OSC1_PW * 
                                 ((LFO1_VALUE * LFO1_OSC1_PW) 
                                + (LFO2_VALUE * LFO2_OSC1_PW)
                                + (ENV1_VALUE * ENV1_OSC1_PW)
                                + (ENV2_VALUE * ENV2_OSC1_PW));
                        if(*(Left_Samples1 + i_POSITION) > 0) mul_datL = T_OSC_PW * 2.0f;
                        if(Stereo == 2) if(*(Right_Samples + i_POSITION) > 0) mul_datR = T_OSC_PW * 2.0f;
                    }

                    GS_VAL += (Cubic_Work(
                                (float) (*(Left_Samples1 + i_POSITION - 1)) * mul_datL,
                                (float) (*(Left_Samples1 + i_POSITION)) * mul_datL,
                                (float) (*(Left_Samples1 + i_POSITION + 1)) * mul_datL,
                                (float) (*(Left_Samples1 + i_POSITION + 2)) * mul_datL,
                                res_dec, i_POSITION, Rns) * vol) * T_OSC1_VOLUME * OSC3_VOLUME;

                    if(Stereo == 2)
                    {
                        GS_VAL2 += (Cubic_Work(
                                (float) (*(Right_Samples + i_POSITION - 1)) * mul_datR,
                                (float) (*(Right_Samples + i_POSITION)) * mul_datR,
                                (float) (*(Right_Samples + i_POSITION + 1)) * mul_datR,
                                (float) (*(Right_Samples + i_POSITION + 2)) * mul_datR,
                                res_dec, i_POSITION, Rns) * vol) * T_OSC1_VOLUME * OSC3_VOLUME;
                    }
                }

                osc_speed2 += osc_speed1;
                if(osc_speed2 < 16) osc_speed2 = 16;

                if(ENV3_LOOP_BACKWARD == TRUE) pos_osc3->absolu -= osc_speed2;
                else pos_osc3->absolu += osc_speed2;

                switch(Loop_Type)
                {
                    case SMP_LOOP_NONE:
                        if(pos_osc3->half.first >= Length)
                        {
                            pos_osc3->half.first = Length;
                            *track = 0;
                        }
                        break;
                    case SMP_LOOP_FORWARD:
                        if(pos_osc3->half.first >= Length)
                        {
                            pos_osc3->half.first -= Loop_Sub;
                        }
                        break;
                    case SMP_LOOP_PINGPONG:
                        if(ENV3_LOOP_BACKWARD == TRUE)
                        {
                            if((int) pos_osc3->half.first <= (int) (Length - Loop_Sub))
                            {
                                pos_osc3->half.first = Length - Loop_Sub;
                                ENV3_LOOP_BACKWARD = FALSE;
                            }
                        }
                        else
                        {
                            if(pos_osc3->half.first >= Length)
                            {
                                pos_osc3->half.first = Length;
                                ENV3_LOOP_BACKWARD = TRUE;
                            }
                        }
                        break;
                }
            }
        }
    }
#endif // PTK_SYNTH_OSC3

#if defined(PTK_SYNTH_DISTO)
    if(DISTO != 1.0f)
    {
        GS_VAL /= 32767.0f;
        GS_VAL = GS_VAL * (fabsf(GS_VAL) + DISTO) / ((GS_VAL * GS_VAL) + (DISTO - 1) *
                 fabsf(GS_VAL) + 1);
        GS_VAL *= 32767.0f;
    }
#endif

#if defined(PTK_SYNTH_FILTER)
    if(VCF_TYPE < 2)
    {
        FILT_CUTO = VCF_CUTOFF
                    + LFO1_VALUE * LFO1_VCF_CUTOFF
                    + LFO2_VALUE * LFO2_VCF_CUTOFF
                    + ENV1_VALUE * ENV1_VCF_CUTOFF
                    + ENV2_VALUE * ENV2_VCF_CUTOFF;
        FILT_RESO = VCF_RESONANCE
                    + LFO1_VALUE * LFO1_VCF_RESONANCE
                    + LFO2_VALUE * LFO2_VCF_RESONANCE
                    + ENV1_VALUE * ENV1_VCF_RESONANCE
                    + ENV2_VALUE * ENV2_VCF_RESONANCE;
        if(FILT_CUTO < 0.05f) FILT_CUTO = 0.05f;
        if(FILT_CUTO > 0.90f) FILT_CUTO = 0.90f;
        if(FILT_RESO < 0.05f) FILT_RESO = 0.05f;
        if(FILT_RESO > 0.98f) FILT_RESO = 0.98f;
        FILT_A = float(1.0f - FILT_CUTO); 
        FILT_B = float(FILT_RESO * (1.0f + (1.0f / FILT_A)));
        GS_VAL = FilterL();
    }
#endif

    GS_VAL *= GLB_VOLUME;

    if(Stereo == 2)
    {

#if defined(PTK_SYNTH_DISTO)
        if(DISTO != 1.0f)
        {
            GS_VAL2 /= 32767.0f;
            GS_VAL2 = GS_VAL2 * (fabsf(GS_VAL2) + DISTO) / ((GS_VAL2 * GS_VAL2) + (DISTO - 1) *
                      fabsf(GS_VAL2) + 1);
            GS_VAL2 *= 32767.0f;
        }
#endif

#if defined(PTK_SYNTH_FILTER)
        if(VCF_TYPE < 2) GS_VAL2 = FilterR();
#endif

        GS_VAL2 *= GLB_VOLUME;
        *Right_Signal += GS_VAL2 * GLOBAL_VOLUME;
    }

    /* Advance all, oscillator, envelopes, and lfo's */
    EnvRun(track, track2);
    LfoAdvance();

    /* Return value */
    return GS_VAL * GLOBAL_VOLUME;
}

void CSynth::ChangeParameters(SynthParameters TSP)
{
    OSC1_WAVEFORM = TSP.osc1_waveform;
    OSC2_WAVEFORM = TSP.osc2_waveform;

    OSC1_PW = (float) (TSP.osc1_pw - 256) / 256.0f;
    OSC2_PW = (float) (TSP.osc2_pw - 256) / 256.0f;

    OSC2_DETUNE = (float) (TSP.osc2_detune - 64.0f) * 0.0625f;
    OSC2_FINETUNE = (float) TSP.osc2_finetune * 0.0078125f;

    VCF_CUTOFF = (float) TSP.vcf_cutoff * 0.0078125f;
    VCF_RESONANCE = (float) TSP.vcf_resonance * 0.0078125f;

#if defined(PTK_SYNTH_FILTER)
    VCF_TYPE = TSP.vcf_type;
#endif

    // Sustain = 0 128
    ENV1_ATTACK = ((float) (TSP.env1_attack + 1)) / 512.0f;
    ENV1_DECAY = ((float) (TSP.env1_decay + 1)) / 512.0f;
    ENV1_SUSTAIN = (float) TSP.env1_sustain * 0.0078125f;          // / 128
    ENV1_RELEASE = ((float) (TSP.env1_release + 1)) / 512.0f;
    if(ENV1_ATTACK < 0.1f) ENV1_ATTACK = 0.1f;
    if(ENV1_RELEASE < 0.15f) ENV1_RELEASE = 0.15f;

    ENV2_ATTACK = ((float) (TSP.env2_attack + 1)) / 512.0f;
    ENV2_DECAY = ((float) (TSP.env2_decay + 1)) / 512.0f;
    ENV2_SUSTAIN = (float) (TSP.env2_sustain * 0.0078125f);        // / 128
    ENV2_RELEASE = ((float) (TSP.env2_release + 1)) / 512.0f;
    if(ENV2_ATTACK < 0.1f) ENV2_ATTACK = 0.1f;
    if(ENV2_RELEASE < 0.15f) ENV2_RELEASE = 0.15f;
  
    LFO1_PERIOD = (float) (TSP.lfo1_period * 2) + 1;
    LFO1_SUBGRMAX = f2i(((float) SamplesPerTick * 0.000277f) * LFO1_PERIOD);

    LFO2_PERIOD = (float) (TSP.lfo2_period * 2) + 1;
    LFO2_SUBGRMAX = f2i(((float) SamplesPerTick * 0.000277f) * LFO2_PERIOD);

    /* Envelopes and LFO's matrix modulation variables */
    LFO1_OSC1_PW =       ((float) TSP.lfo1_osc1_pw - 64) * 0.015625f;
    LFO1_OSC2_PW =       ((float) TSP.lfo1_osc2_pw - 64) * 0.015625f;
    LFO1_OSC1_PITCH =    ((float) TSP.lfo1_osc1_pitch - 64) * 0.015625f;
    LFO1_OSC2_PITCH =    ((float) TSP.lfo1_osc2_pitch - 64) * 0.015625f;
    LFO1_OSC1_VOLUME =   ((float) TSP.lfo1_osc1_volume - 64) * 0.015625f;
    LFO1_OSC2_VOLUME =   ((float) TSP.lfo1_osc2_volume - 64) * 0.015625f;
    LFO1_VCF_CUTOFF =    ((float) TSP.lfo1_vcf_cutoff - 64) * 0.015625f;
    LFO1_VCF_RESONANCE = ((float) TSP.lfo1_vcf_resonance - 64) * 0.015625f;

    LFO2_OSC1_PW =       ((float) TSP.lfo2_osc1_pw - 64) * 0.015625f;
    LFO2_OSC2_PW =       ((float) TSP.lfo2_osc2_pw - 64) * 0.015625f;
    LFO2_OSC1_PITCH =    ((float) TSP.lfo2_osc1_pitch - 64) * 0.015625f;
    LFO2_OSC2_PITCH =    ((float) TSP.lfo2_osc2_pitch - 64) * 0.015625f;
    LFO2_OSC1_VOLUME =   ((float) TSP.lfo2_osc1_volume - 64) * 0.015625f;
    LFO2_OSC2_VOLUME =   ((float) TSP.lfo2_osc2_volume - 64) * 0.015625f;
    LFO2_VCF_CUTOFF =    ((float) TSP.lfo2_vcf_cutoff - 64) * 0.015625f;
    LFO2_VCF_RESONANCE = ((float) TSP.lfo2_vcf_resonance - 64) * 0.015625f;

    ENV1_OSC1_PW =       ((float) TSP.env1_osc1_pw - 64) * 0.015625f;
    ENV1_OSC2_PW =       ((float) TSP.env1_osc2_pw - 64) * 0.015625f;
    ENV1_OSC1_PITCH =    ((float) TSP.env1_osc1_pitch - 64) * 0.015625f;
    ENV1_OSC2_PITCH =    ((float) TSP.env1_osc2_pitch - 64) * 0.015625f;
    ENV1_OSC1_VOLUME =   ((float) TSP.env1_osc1_volume - 64) * 0.015625f;
    ENV1_OSC2_VOLUME =   ((float) TSP.env1_osc2_volume - 64) * 0.015625f;
    ENV1_VCF_CUTOFF =    ((float) TSP.env1_vcf_cutoff - 64) * 0.015625f;
    ENV1_VCF_RESONANCE = ((float) TSP.env1_vcf_resonance - 64) * 0.015625f;

    ENV2_OSC1_PW =       ((float) TSP.env2_osc1_pw - 64) * 0.015625f;
    ENV2_OSC2_PW =       ((float) TSP.env2_osc2_pw - 64) * 0.015625f;
    ENV2_OSC1_PITCH =    ((float) TSP.env2_osc1_pitch - 64) * 0.015625f;
    ENV2_OSC2_PITCH =    ((float) TSP.env2_osc2_pitch - 64) * 0.015625f;
    ENV2_OSC1_VOLUME =   ((float) TSP.env2_osc1_volume - 64) * 0.015625f;
    ENV2_OSC2_VOLUME =   ((float) TSP.env2_osc2_volume - 64) * 0.015625f;
    ENV2_VCF_CUTOFF =    ((float) TSP.env2_vcf_cutoff - 64) * 0.015625f;
    ENV2_VCF_RESONANCE = ((float) TSP.env2_vcf_resonance - 64) * 0.015625f;

#if defined(PTK_SYNTH_OSC3)
    OSC3_VOLUME =        ((float) TSP.osc3_volume - 64) * 0.015625f;
    OSC3_SWITCH =        TSP.osc3_switch;
#endif

    PTC_GLIDE =          ((float) TSP.ptc_glide * (float) TSP.ptc_glide) * 0.0000015625f;
    PTC_GLIDE64 =         (int64) ((double) PTC_GLIDE * 4294967296.0);

    GLB_VOLUME =         ((float) TSP.glb_volume) * 0.0078125f;

#if defined(PTK_SYNTH_DISTO)
    DISTO =              (((float) TSP.disto)) + 1.0f;
#endif

    LFO1_ATTACK =        ((float) (TSP.lfo1_attack + 1)) / 512.0f;
    LFO1_DECAY =         ((float) (TSP.lfo1_decay + 1)) / 512.0f;
    LFO1_SUSTAIN =       (float) TSP.lfo1_sustain * 0.0078125f;
    LFO1_RELEASE =       ((float) (TSP.lfo1_release + 1)) / 512.0f;
    if(LFO1_ATTACK < 0.1f) LFO1_ATTACK = 0.1f;
    if(LFO1_RELEASE < 0.15f) LFO1_RELEASE = 0.15f;

    LFO2_ATTACK =        ((float) (TSP.lfo2_attack + 1)) / 512.0f;
    LFO2_DECAY =         ((float) (TSP.lfo2_decay + 1)) / 512.0f;
    LFO2_SUSTAIN =       (float) TSP.lfo2_sustain * 0.0078125f;
    LFO2_RELEASE =       ((float) (TSP.lfo2_release + 1)) / 512.0f;
    if(LFO2_ATTACK < 0.1f) LFO2_ATTACK = 0.1f;
    if(LFO2_RELEASE < 0.15f) LFO2_RELEASE = 0.15f;
}

#if defined(PTK_SYNTH_FILTER)
float CSynth::FilterL(void)
{
    GS_VAL++;
    sbuf0L = FILT_A * sbuf0L + FILT_CUTO * (GS_VAL + FILT_B * (sbuf0L - sbuf1L)); 
    sbuf1L = FILT_A * sbuf1L + FILT_CUTO * sbuf0L;
    return(VCF_TYPE == 0 ? sbuf1L : GS_VAL - sbuf1L);
}

float CSynth::FilterR(void)
{
    GS_VAL2++;
    sbuf0R = FILT_A * sbuf0R + FILT_CUTO * (GS_VAL2 + FILT_B * (sbuf0R - sbuf1R));
    sbuf1R = FILT_A * sbuf1R + FILT_CUTO * sbuf0R;
    return(VCF_TYPE == 0 ? sbuf1R : GS_VAL2 - sbuf1R);
}
#endif
