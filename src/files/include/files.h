// ------------------------------------------------------
// Protrekkr
// Based on Juan Antonio Arguelles Rius's NoiseTrekker.
//
// Copyright (C) 2008-2009 Franck Charlet.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL FRANCK CHARLET OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
// ------------------------------------------------------

#ifndef _FILES_H_
#define _FILES_H_

// ------------------------------------------------------
// Includes
#include <stdio.h>
#include "../../include/variables.h"

#if defined(__AROS__)
#include <stdint.h>
#include <string.h>
#define int32 int32_t
#endif

// ------------------------------------------------------
// Constants
#define SAVE_WRITE 0
#define SAVE_CALCLEN 1
#define SAVE_WRITEMEM 2

#define LOAD_READ FALSE
#define LOAD_READMEM TRUE

#if defined(__WIN32__)
#define SLASH "\\"
#else
#define SLASH "/"
#endif

// ------------------------------------------------------
// Variables
extern int Final_Mod_Length;
extern int mt_tags[13];
extern int mt_channels[13];
extern char Dir_Mods[MAX_PATH];
extern char Dir_Instrs[MAX_PATH];
extern char Dir_Presets[MAX_PATH];
extern char Dir_Reverbs[MAX_PATH];
extern char Dir_Patterns[MAX_PATH];
extern char Dir_Samples[MAX_PATH];

// ------------------------------------------------------
// Functions
void LoadAmigaMod(char *FileName, int channels);
int LoadMod(char *FileName);
int SaveMod(char *FileName, int NewFormat, int Simulate, Uint8 *Memory);
int Pack_Module(char *FileName);
int TestMod(void);
void LoadSynth(char *FileName);
void LoadInst(char *FileName);
void Load303(char *FileName);
void LoadReverb(char *FileName);
void LoadPattern(char *FileName);
void SaveInst(void);
void SaveSynth(void);
void Save303(void);
void SaveReverb(void);
void SavePattern(void);
void SaveConfig(void);
void LoadConfig(void);

void LoadSettings(void);
void SaveSettings(void);

void SongStop(void);
void Free_Samples(void);

Uint8 *Pack_Data(Uint8 *Memory, int *Len);
Uint8 *Depack_Data(Uint8 *Memory, int Size, int size_out);
int Get_File_Size(FILE *Handle);

int Calc_Length(void);
void Reset_Song_Length(void);

void Load_Old_Reverb_Presets(int Type);

void Clear_Input(void);

int File_Exist(char *Format, char *Directory, char *FileName);

#endif
