// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_MASTER_H_
#define _EDITOR_MASTER_H_

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
void Draw_Master_Ed(void);
void Actualize_Master_Ed(char gode);
void Mouse_Left_Master_Ed(void);
void Mouse_Right_Master_Ed(void);
void Mouse_Sliders_Master_Ed(void);
void Actualize_Fx_Ed(char gode);
void MidiIn_Init(void);
void MidiIn_Close(void);
void MidiOut_Init(void);
void MidiOut_Close(void);

#endif
