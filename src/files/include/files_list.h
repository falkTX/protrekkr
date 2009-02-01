// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _FILES_LIST_H_
#define _FILES_LIST_H_

// ------------------------------------------------------
// Includes
#if defined(MINGW)
#include "../../include/variables.h"
#include "../../ui/include/misc_draw.h"
#else
#include "../include/variables.h"
#include "../ui/include/misc_draw.h"
#endif

#ifdef __WIN32__
#include <io.h>
#include <direct.h>
#endif

// ------------------------------------------------------
// Variables
extern int lt_ykar;
extern int lt_items;
extern int lt_index;
extern int lt_curr;

// ------------------------------------------------------
// Functions
void Set_Current_Dir(void);
void Read_SMPT(void);
void DumpList(int xr, int yr, int y);
void ltActualize(int modeac);
void Clear_Files_List(void);
char *Get_Current_FileName(void);
unsigned int Get_Current_FileType(void);

#endif
