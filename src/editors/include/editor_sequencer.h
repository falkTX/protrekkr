// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_SEQUENCER_ED_H_
#define _EDITOR_SEQUENCER_ED_H_

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
void Draw_Sequencer_Ed(void);
void Actualize_Seq_Ed(void);
void Mouse_Left_Sequencer_Ed(void);
void Mouse_Right_Sequencer_Ed(void);
void Actualize_Sequencer(void);
void Anat(int posil);

#endif
