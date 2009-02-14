// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_PATTERN_H_
#define _EDITOR_PATTERN_H_

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
#define PAT_COL_SHIFT 4
#define PAT_COL_NOTE 24
#define PAT_COL_INSTRUMENT 52
#define PAT_COL_VOLUME 72
#define PAT_COL_PANNING 92
#define PAT_COL_EFFECTS 112

#define PAT_COL_CHAR 8

#define DISPLAYED_LINES 28

#define MAX_PATT_SCREEN_X 778

// ------------------------------------------------------
// Functions
void draw_pated(int track, int line, int petrack, int row);
void Actupated(int modac);
void draw_pated_highlight(int track, int line, int petrack, int row);
void Actualize_Patterned(void);
int Alloc_Patterns_Pool(void);
void Bound_Patt_Pos(void);
void Reset_Pattern_Scrolling_Horiz(void);
void Mouse_Wheel_Pattern_Ed(int roll_amount);
void Mouse_Sliders_Pattern_Ed(void);
void Mouse_Sliders_Right_Pattern_Ed(void);
void Mouse_Left_Pattern_Ed(void);
void Mouse_Right_Pattern_Ed(void);
int Get_Visible_Tracks_Size(void);
int Get_Current_Position(void);
void Goto_Previous_Row(void);
void Goto_Next_Row(void);
void Goto_Previous_Page(void);
void Goto_Next_Page(void);
void Goto_Previous_Column(void);
void Goto_Next_Column(void);
void Goto_Top_Left(void);
void Goto_Bottom_Right(void);
void Goto_Row(int row);
int Get_Visible_Complete_Tracks(void);

#endif
