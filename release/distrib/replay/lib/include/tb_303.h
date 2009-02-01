// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _TB303_H_
#define _TB303_H_

// ------------------------------------------------------
// Includes
#include <math.h>

#if defined(STAND_ALONE)
    #if defined(WINAMP)
        #include "ptk_def_properties.h"
    #else
        #if defined(MINGW)
        #include "../../ptk_properties.h"
        #else
        #include "../ptk_properties.h"
        #endif
    #endif
#else
    #include "ptk_def_properties.h"
#endif

#ifdef PTK_303

// ------------------------------------------------------
// Constants
#define TB303_WAVEFORM_SAW 0
#define TB303_WAVEFORM_SQUARE 1

// ------------------------------------------------------
// Structures

#if defined(STAND_ALONE) && !defined(WINAMP)
    #if !defined(MINGW)
    #pragma pack(push)
    #pragma pack(1)
    #else
    #pragma pack(push, 1)
    #endif
#endif

struct flag303
{
    unsigned slide_flag : 1;
    unsigned accent_flag : 1;
    unsigned transposeup_flag : 1;
    unsigned transposedown_flag : 1;
    unsigned pause : 1;
    unsigned reserved2_flag : 1;
    unsigned reserved3_flag : 1;
    unsigned reserved4_flag : 1;
};

struct para303 {
    unsigned char enabled;           //Enabled                 UBYTE           0       0x00 = off, 0x01 = on (pattern mode)
    unsigned char selectedpattern;   //Selected pattern        UBYTE           1       0x00 to 0x20 (pattern mode)
    unsigned char tune;              //Tune                    UBYTE           2       0x00 to 0x7f (pattern mode)
    unsigned char cutoff;            //Cutoff                  UBYTE           3       0x00 to 0x7f (pattern mode)
    unsigned char resonance;         //Resonance               UBYTE           4       0x00 to 0x7f (pattern mode)
    unsigned char envmod;            //EnvMod                  UBYTE           5       0x00 to 0x7f (pattern mode)
    unsigned char decay;             //Decay                   UBYTE           6       0x00 to 0x7f (pattern mode)
    unsigned char accent;            //Accent                  UBYTE           7       0x00 to 0x7f (pattern mode)
    unsigned char waveform;          //Waveform                UBYTE           8       0x00 = triangle, 0x01 = square (pattern mode)
    unsigned char patternlength[32]; // 32 = 4 bank * 8 patterns
    unsigned char tone[32][16];
    struct flag303 flag[32][16];

#if !defined(STAND_ALONE) || defined(WINAMP)
    char pattern_name[32][20];
#endif

    //    32*Pattern (1088 bytes)             9,9+(1*34),9+(2*34),9+(3*34)...
    //    |   Shuffle         UBYTE                   0x00 = off, 0x01 = on
    //    |   Pattern length  UBYTE                   0x01 to 0x10
    //    |
    //    |   16*Step (32 bytes)              11,11+(1*34),11+(2*34),11+(3*34)...
    //    |   |   Tone/pitch  UBYTE                   0x00 to 0x0c
    //    |   |   Flags       BITMASK8                bit 0 = No slide/Slide (0x01)
    //    |   |                                       bit 1 = No accent/Accent (0x02)
    //    |   |                                       bit 2 = Normal/Transpose up (0x04)
    //    |   |                                       bit 3 = Normal/Transpose down (0x08)
    //    \   \                                       bit 4 = Pause/Note (0x10)
};

#if defined(STAND_ALONE) && !defined(WINAMP)
    #pragma pack(pop)
#endif

// ------------------------------------------------------
// Variables
extern long SamplesPerTick;

// ------------------------------------------------------
// Classes
class gear303
{
    public:

        unsigned char tbPattern;   // From 0 to 31, 255 - Off
        unsigned char tbLine;      // From 0 to 15, 255 - Off

        gear303();
        void tbNoteOn(int tbNote, para303 *PARAT303);

        float tbGetSample(void);
        bool hpf;
        float tbBuf0;
        float tbBuf1;
        float tbVolume;

    private:

        float tbFilter(void);

        // 303 Parameters
        float tbCutoff;
        float tbResonance;
        float tbEnvmod;
        float tbDecay;
        float tbAccent;
        float tbSample;
        float tbRealCutoff;
        float tbRealVolume;
        float tbOscSpeedFreak;
        float tbTargetVolume;
        float tbCurrentVolume;
        float tbInnertime;
        // Oscillator variables
        float tbOscPosition;
        float tbOscSpeed;

        // Waveform Type
        char tbWaveform;
};

// ------------------------------------------------------
// Functions
#if !defined(STAND_ALONE) || defined(WINAMP)
void ReInit_303_Parameters(para303 *tbpars);
void Reset_303_Parameters(para303 *tbpars);
#endif

#endif

#endif
