// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_PATTERN_H_
#define _EDITOR_PATTERN_H_

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
// Constants
#define PAT_COL_SHIFT 4
#define PAT_COL_NOTE 24
#define PAT_COL_INSTRUMENT 52
#define PAT_COL_VOLUME 72
#define PAT_COL_PANNING 92
#define PAT_COL_EFFECTS 112
#define PAT_COL_MAX 130

// ------------------------------------------------------
// Functions
void draw_pated(int track, int line, int petrack, int row);
void Actupated(int modac);
void draw_pated_highlight(int track, int line, int petrack, int row);
void Actualize_Patterned(void);
int AllocPattern(void);
void Bound_Patt_Pos(void);

#endif
