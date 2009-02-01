// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_INSTRUMENT_H_
#define _EDITOR_INSTRUMENT_H_

// ------------------------------------------------------
// Includes
#if defined(__GCC__)
#include "../../include/variables.h"
#include "../../ui/include/misc_draw.h"
#include "../../wav/include/riff.h"
#else
#include "../include/variables.h"
#include "../ui/include/misc_draw.h"
#include "../wav/include/riff.h"
#endif

// ------------------------------------------------------
// Functions
void Draw_Instrument_Ed(void);
void Actualize_Instrument_Ed(int typex, char gode);
void Mouse_Sliders_Instrument_Ed(void);
void Mouse_Left_Instrument_Ed(void);
void Mouse_Right_Instrument_Ed(void);
void Mouse_Left_Repeat_Instrument_Ed(void);
void Mouse_Right_Repeat_Instrument_Ed(void);
void Afloop(void);

#endif
