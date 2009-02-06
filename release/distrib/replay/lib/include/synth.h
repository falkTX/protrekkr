// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _SYNTH_H_
#define _SYNTH_H_

// ------------------------------------------------------
// Includes
#include <math.h>

#if defined(__STAND_ALONE__)
    #if defined(__WINAMP__)
        #include "ptk_def_properties.h"
    #else
        #if defined(__GCC__)
        #include "../../ptk_properties.h"
        #else
        #include "../ptk_properties.h"
        #endif
    #endif
#else
    #include "ptk_def_properties.h"
#endif

// ------------------------------------------------------
// Constants
#define SIZE_WAVEFORMS_SPACE 88200

// ------------------------------------------------------
// Types
#if defined(__WIN32__) && !defined(__GCC__)
typedef __int64 int64;
typedef unsigned __int64 Uint64;
#else
typedef long long int64;
typedef unsigned long long Uint64;
#endif

// ------------------------------------------------------
// Structures

/* Struct used to store/update synthesizer parameters */

#if defined(__STAND_ALONE__) && !defined(__WINAMP__)
    #if !defined(__GCC__)
    #pragma pack(push)
    #pragma pack(1)
    #else
    #pragma pack(push, 1)
    #endif
#endif

struct SynthParameters
{
#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    char presetname[20];
#endif

    unsigned char osc1_waveform;
    unsigned char osc2_waveform;

    int osc1_pw;
    int osc2_pw;   

    unsigned char osc2_detune;
    unsigned char osc2_finetune;

    unsigned char vcf_cutoff;
    unsigned char vcf_resonance;
    unsigned char vcf_type;

    /* Envelopes and LFO's properties */

    int env1_attack;
    int env1_decay;
    unsigned char env1_sustain;
    int env1_release;

    int env2_attack;
    int env2_decay;
    unsigned char env2_sustain;
    int env2_release;

    int lfo1_period;
    int lfo2_period;

    /* Envelopes and LFO's modulation variables */

    unsigned char lfo1_osc1_pw;
    unsigned char lfo1_osc2_pw;
    unsigned char lfo1_osc1_pitch;
    unsigned char lfo1_osc2_pitch;
    unsigned char lfo1_osc1_volume;
    unsigned char lfo1_osc2_volume;
    unsigned char lfo1_vcf_cutoff;
    unsigned char lfo1_vcf_resonance;

    unsigned char lfo2_osc1_pw;
    unsigned char lfo2_osc2_pw;
    unsigned char lfo2_osc1_pitch;
    unsigned char lfo2_osc2_pitch;
    unsigned char lfo2_osc1_volume;
    unsigned char lfo2_osc2_volume;
    unsigned char lfo2_vcf_cutoff;
    unsigned char lfo2_vcf_resonance;

    unsigned char env1_osc1_pw;
    unsigned char env1_osc2_pw;
    unsigned char env1_osc1_pitch;
    unsigned char env1_osc2_pitch;
    unsigned char env1_osc1_volume;
    unsigned char env1_osc2_volume;  
    unsigned char env1_vcf_cutoff;
    unsigned char env1_vcf_resonance;

    unsigned char env2_osc1_pw;
    unsigned char env2_osc2_pw;
    unsigned char env2_osc1_pitch;
    unsigned char env2_osc2_pitch;
    unsigned char env2_osc1_volume;
    unsigned char env2_osc2_volume;  
    unsigned char env2_vcf_cutoff;
    unsigned char env2_vcf_resonance;

    unsigned char osc3_volume;
    unsigned char osc3_switch;

    unsigned char ptc_glide;
    unsigned char glb_volume;

    unsigned char disto;

    int lfo1_attack;
    int lfo1_decay;
    unsigned char lfo1_sustain;
    int lfo1_release;

    int lfo2_attack;
    int lfo2_decay;
    unsigned char lfo2_sustain;
    int lfo2_release;
};

#if defined(__STAND_ALONE__) && !defined(__WINAMP__)
    #pragma pack(pop)
#endif

// ------------------------------------------------------
// Variables
extern long SamplesPerTick;

/* Sine float-precalculated table, in absolute degrees. */
extern float SIN[360];
extern short STOCK_SIN[SIZE_WAVEFORMS_SPACE * 2];
extern short STOCK_SAW[SIZE_WAVEFORMS_SPACE * 2];
extern short STOCK_PUL[SIZE_WAVEFORMS_SPACE * 2];
extern short STOCK_WIT[SIZE_WAVEFORMS_SPACE * 2];

#if defined(PTK_SYNTH_PINKNOISE)
    extern short STOCK_PIN[SIZE_WAVEFORMS_SPACE * 2];
#endif

extern int SIZE_WAVEFORMS;

// ------------------------------------------------------
// Classes
class CSynth
{
    public:
   
        char OSC1_WAVEFORM;
        char OSC2_WAVEFORM;

        char ENV1_STAGE;
        char ENV2_STAGE;

        void Reset(void);
        void ChangeParameters(SynthParameters TSP);

        /* Work functions */

        float GetSample(short *Left_Samples,
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
                        int64 osc_speed);

        void NoteOn(float noten, float speed, int Looping, unsigned int Length,
                    unsigned int Loop_Length, float note_smp);
        void NoteOff(void);

        float FilterL(void);
        float FilterR(void);

    private:

        /* Internal Use */

        void LfoAdvance(void);
        void EnvRun(int *track, int *track2);

        /* Synthesizer properties */
#if defined(PTK_SYNTH_FILTER)
        float FILT_CUTO;
        float FILT_RESO;
        float FILT_A;
        float FILT_B;
#endif

        float OSC1_PW;
        float OSC2_PW; 

        float T_OSC_PW;
        float T_OSC1_VOLUME;
        float T_OSC2_VOLUME;

        float GLOBAL_VOLUME;

        float OSC2_DETUNE;
        float OSC2_FINETUNE; 

        float VCF_CUTOFF;
        float VCF_RESONANCE;

#if defined(PTK_SYNTH_FILTER)
        char VCF_TYPE;
#endif

        int64 OSC1_SPEED;
        int64 OSC2_SPEED;
        int64 OSC3_SPEED;

        /* Envelopes and LFO's properties */
        float ENV1b_ATTACK;
        float ENV1b_DECAY;
        float ENV1b_RELEASE;

        float ENV2b_ATTACK;
        float ENV2b_DECAY;
        float ENV2b_RELEASE;

        float ENV1_ATTACK;
        float ENV1_DECAY;
        float ENV1_SUSTAIN;
        float ENV1_RELEASE;

        float ENV1_A_COEF;
        float ENV1_D_COEF;
        float ENV1_R_COEF;

        float ENV2_ATTACK;
        float ENV2_DECAY;
        float ENV2_SUSTAIN;
        float ENV2_RELEASE;

        float ENV2_A_COEF;
        float ENV2_D_COEF;
        float ENV2_R_COEF;

        float LFO1_PERIOD;
        float LFO2_PERIOD;

        float LFO1_COUNTER;
        char LFO1_STAGE;
        float LFO1b_ATTACK;
        float LFO1b_DECAY;
        float LFO1b_RELEASE;
        float LFO1_A_COEF;
        float LFO1_D_COEF;
        float LFO1_R_COEF;
        float LFO1_ATTACK;
        float LFO1_DECAY;
        float LFO1_SUSTAIN;
        float LFO1_RELEASE;
        float LFO1_ADSR_VALUE;

        float LFO2_COUNTER;
        char LFO2_STAGE;
        float LFO2b_ATTACK;
        float LFO2b_DECAY;
        float LFO2b_RELEASE;
        float LFO2_A_COEF;
        float LFO2_D_COEF;
        float LFO2_R_COEF;
        float LFO2_ATTACK;
        float LFO2_DECAY;
        float LFO2_SUSTAIN;
        float LFO2_RELEASE;
        float LFO2_ADSR_VALUE;

        int LFO1_GR;
        int LFO2_GR;

        int LFO1_SUBGRCOUNTER;
        int LFO2_SUBGRCOUNTER;

        char ENV1_LOOP_BACKWARD;
        char ENV2_LOOP_BACKWARD;
        char ENV3_LOOP_BACKWARD;

        int LFO1_SUBGRMAX;
        int LFO2_SUBGRMAX;

        float LFO1_VALUE;
        float LFO2_VALUE;

        /* Envelopes and LFO's modulation variables */

        float LFO1_OSC1_PW;
        float LFO1_OSC2_PW;
        float LFO1_OSC1_PITCH;
        float LFO1_OSC2_PITCH;
        float LFO1_OSC1_VOLUME;
        float LFO1_OSC2_VOLUME; 
        float LFO1_VCF_CUTOFF;
        float LFO1_VCF_RESONANCE;  

        float LFO2_OSC1_PW;
        float LFO2_OSC2_PW;
        float LFO2_OSC1_PITCH;
        float LFO2_OSC2_PITCH;
        float LFO2_OSC1_VOLUME;
        float LFO2_OSC2_VOLUME;
        float LFO2_VCF_CUTOFF;
        float LFO2_VCF_RESONANCE;  

        float ENV1_OSC1_PW;
        float ENV1_OSC2_PW;
        float ENV1_OSC1_PITCH;
        float ENV1_OSC2_PITCH;
        float ENV1_OSC1_VOLUME;
        float ENV1_OSC2_VOLUME; 
        float ENV1_VCF_CUTOFF;
        float ENV1_VCF_RESONANCE;  

        float ENV2_OSC1_PW;
        float ENV2_OSC2_PW;
        float ENV2_OSC1_PITCH;
        float ENV2_OSC2_PITCH;
        float ENV2_OSC1_VOLUME;
        float ENV2_OSC2_VOLUME; 
        float ENV2_VCF_CUTOFF;
        float ENV2_VCF_RESONANCE;  

        /* Internal rendering variables */

        float OSC1_STEP;
        float OSC2_STEP;

        float ENV1_COUNTER;
        float ENV2_COUNTER;

        float ENV1_VOLUME_1;
        float ENV1_VOLUME_2;
        float ENV2_VOLUME_1;
        float ENV2_VOLUME_2;

        float ENV1_VALUE;
        float ENV2_VALUE;

        float ENV1_MIN;
        float ENV2_MIN;

#if defined(PTK_SYNTH_OSC3)
        float OSC3_VOLUME;
        unsigned char OSC3_SWITCH;
#endif

        float sbuf0L;
        float sbuf1L;

        float sbuf0R;
        float sbuf1R;

        float GS_VAL;
        float GS_VAL2;

        float PTC_GLIDE;
        int64 PTC_GLIDE64;

        float GLB_VOLUME;
        float DISTO;
};

#if defined(PTK_COMPRESSOR)
class rFilter
{
    private: 

        float buffy0;
        float buffy1;

    public:

        rFilter()
        {
            Reset();
        };

        void Reset()
        {
            buffy0 = 1.0f;
            buffy1 = 0.0f;
        };

        float fWork(float input, float f)
        {

            float fa = 1.0f - f; 

            buffy0 = fa * buffy0 + f * input; 
            buffy1 = fa * buffy1 + f * buffy0;

            return buffy1;  
        };
};
#endif

// ------------------------------------------------------
// Functions
int f2i(float d);
void Initreverb();

#endif
