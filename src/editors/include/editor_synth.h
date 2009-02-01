// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_SYNTH_H_
#define _EDITOR_SYNTH_H_

// ------------------------------------------------------
// Includes
#if defined(__GCC__)
#include "../../include/variables.h"
#include "../../ui/include/misc_draw.h"
#else
#include "../include/variables.h"
#include "../ui/include/misc_draw.h"
#endif

// ------------------------------------------------------
// Constants
#define UPDATE_SYNTH_ED_ALL 0
#define UPDATE_SYNTH_ED_VALUES 1
#define UPDATE_SYNTH_ED_OSC1_PARAMS 2
#define UPDATE_SYNTH_ED_OSC1_WAVEFORM 3
#define UPDATE_SYNTH_ED_OSC2_WAVEFORM 4
#define UPDATE_SYNTH_ED_OSC3_SWITCH 5
#define UPDATE_SYNTH_ED_VALIDATE_PARAMS 6
#define UPDATE_SYNTH_ED_VCF_TYPE 7
#define UPDATE_SYNTH_ED_OSC2_PARAMS 8
#define UPDATE_SYNTH_ED_VCF_PARAMS 9
#define UPDATE_SYNTH_ED_LFO1_PARAMS 10
#define UPDATE_SYNTH_ED_LFO2_PARAMS 11
#define UPDATE_SYNTH_ED_ENV1_PARAMS 12
#define UPDATE_SYNTH_ED_ENV2_PARAMS 13
#define UPDATE_SYNTH_ED_Misc_PARAMS 14
#define UPDATE_SYNTH_CHANGE_NAME 15
#define UPDATE_SYNTH_CHANGE_ASSOCIATED_SAMPLE 16

// ------------------------------------------------------
// Functions
void Draw_Synth_Ed(void);
void Actualize_Synth_Ed(char gode);
void Mouse_Sliders_Synth_Ed(void);
void Mouse_Right_Synth_Ed(void);
void Mouse_Left_Synth_Ed(void);
void Actualize_SynthParSlider(void);
void IniCsParNames(void);

#endif
