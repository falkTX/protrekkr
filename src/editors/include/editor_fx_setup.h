// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_FX_H_
#define _EDITOR_FX_H_

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
void Draw_Fx_Ed(void);
void Actualize_Fx_Ed(char gode);
void Mouse_Sliders_Fx_Ed(void);
void Mouse_Right_Fx_Ed(void);
void Mouse_Left_Fx_Ed(void);

#endif
