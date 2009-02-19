// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_DISKIO_ED_H_
#define _EDITOR_DISKIO_ED_H_

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
// Variables
extern int Tracks_To_Render[MAX_TRACKS];

// ------------------------------------------------------
// Functions
void Draw_DiskIO_Ed(void);
void Actualize_DiskIO_Ed(int gode);
void Mouse_Right_DiskIO_Ed(void);
void Mouse_Left_DiskIO_Ed(void);
void Display_Song_Length(void);
void Reset_Tracks_To_Render(void);

#endif
