// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_TRACK_FX_H_
#define _EDITOR_TRACK_FX_H_

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
void Draw_Track_Fx_Ed(void);
void Actualize_Track_Fx_Ed(char gode);
void Mouse_Sliders_Track_Fx_Ed(void);
void Mouse_Left_Track_Fx_Ed(void);

#endif
