// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _FILES_H_
#define _FILES_H_

// ------------------------------------------------------
// Includes
#include <stdio.h>
//#include <direct.h>

// ------------------------------------------------------
// Constants
#define SAVE_WRITE 0
#define SAVE_CALCLEN 1
#define SAVE_WRITEMEM 2

#define LOAD_READ FALSE
#define LOAD_READMEM TRUE

// ------------------------------------------------------
// Variables
extern int Final_Mod_Length;
extern int mt_tags[13];
extern int mt_channels[13];

// ------------------------------------------------------
// Functions
void LoadAmigaMod(char *FileName, int channels);
void LoadMod(char *FileName);
int SaveMod(char *FileName, int NewFormat, int Simulate, Uint8 *Memory);
int Pack_Module(char *FileName);
int TestMod(void);
void LoadSynth(char *FileName);
void LoadInst(char *FileName);
void Load303(char *FileName);
void SaveInst(void);
void SaveSynth(void);
void Save303(void);
void SaveConfig(void);
void LoadConfig(void);

void LoadSettings(void);
void SaveSettings(void);

void SongStop(void);
void Free_Samples(void);

Uint8 *Pack_Data(Uint8 *Memory, int *Len);
Uint8 *Depack_Data(Uint8 *Memory, int Size);
int Get_File_Size(FILE *Handle);

#endif
