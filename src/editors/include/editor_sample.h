// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_SAMPLE_H_
#define _EDITOR_SAMPLE_H_

// ------------------------------------------------------
// Includes
#if defined(MINGW)
#include "../../include/variables.h"
#include "../../ui/include/misc_draw.h"
#else
#include "../include/variables.h"
#include "../ui/include/misc_draw.h"
#endif

// ------------------------------------------------------
// Functions
void Draw_Sample_Ed(void);
void Draw_Sampled_Wave(void);
void Actualize_Wave_Ed(char gode);
void NewWav(void);
void Mouse_Left_Sample_Ed(void);
void Mouse_Right_Sample_Ed(void);
void Mouse_Sliders_Sample_Ed(void);
void Check_Loops(void);

#endif
