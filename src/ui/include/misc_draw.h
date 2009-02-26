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

#ifndef _MISC_DRAW_H_
#define _MISC_DRAW_H_

// ------------------------------------------------------
// Includes
#include <SDL/SDL.h>

// ------------------------------------------------------
// Constants
#define USE_FONT 0
#define USE_FONT_LOW 1

#define COL_BACKGROUND 1
#define COL_STATIC_HI 2
#define COL_STATIC_MED 3
#define COL_STATIC_LO 4
#define COL_HI 5
#define COL_MED 6
#define COL_LO 7
#define COL_PUSHED_HI 8
#define COL_PUSHED_MED 9
#define COL_PUSHED_LO 10
#define COL_SLIDER_HI 11
#define COL_SLIDER_MED 12
#define COL_SLIDER_LO 13
#define COL_VUMETER 14
#define COL_VUMETERPEAK 15
#define COL_SCOPESSAMPLES 16
#define COL_FONT_HI 17
#define COL_FONT_LO 18
#define COL_PATTERN_LO_BACK 19
#define COL_PATTERN_LO_FORE 20
#define COL_PATTERN_HI_BACK 21
#define COL_PATTERN_HI_FORE 22
#define COL_PATTERN_SEL_BACK 23
#define COL_PATTERN_SEL_FORE 24

#define COL_NOTE_LO_FORE 26
#define COL_NOTE_HI_FORE 28
#define COL_NOTE_SEL_FORE 30

#define COL_MUTE 31
#define COL_PLAY 32
#define COL_MUTE_PLAY_INVERT 33

#define COL_PATTERN_LO_BACK_SHADOW 34
#define COL_PATTERN_LO_FORE_SHADOW 35
#define COL_PATTERN_HI_BACK_SHADOW 36
#define COL_PATTERN_HI_FORE_SHADOW 37
#define COL_PATTERN_SEL_BACK_SHADOW 38
#define COL_PATTERN_SEL_FORE_SHADOW 39
#define COL_NOTE_LO_BACK_SHADOW 40
#define COL_NOTE_LO_FORE_SHADOW 41
#define COL_NOTE_HI_BACK_SHADOW 42
#define COL_NOTE_HI_FORE_SHADOW 43
#define COL_NOTE_SEL_BACK_SHADOW 44
#define COL_NOTE_SEL_FORE_SHADOW 45

#define NUMBER_COLORS 20

#define BUTTON_NORMAL 1
#define BUTTON_PUSHED 2
#define BUTTON_TEXT_CENTERED 4
#define BUTTON_DISABLED 8
#define BUTTON_NO_BORDER 16

// ------------------------------------------------------
// Structures
typedef struct
{
    int r;
    int g;
    int b;
} LONGRGB, *LPLONGRGB;

// ------------------------------------------------------
// Variables
extern SDL_Surface *Main_Screen;
extern char *Labels_Palette[];
extern SDL_Color Ptk_Palette[256 * 2];
extern LONGRGB Phony_Palette[];
extern int Idx_Palette[];
extern int curr_tab_highlight;

// ------------------------------------------------------
// Functions
void out_note(int x, int y, int note, int color);
void out_decchar(int x,int y, int number, char smith);
void out_hexchar(int x,int y, int number, char smith, int color);
void value_box(int x, int y, int val, int flags);
void Gui_Draw_Arrows_Number_Box(int x, int y, int val, int flags);
void Gui_Draw_Arrows_Number_Box2(int x, int y, int val, int flags);
void value_box4(int x, int y, int val);
void value_box3(int x, int y, char val, int flags);
void outlong(int x, int y, int cant, int mode);
void outlong_small(int x, int y, int cant, int mode, int size, int flags);
void outfloat(int x, int y, float cant, int mode);
void outfloat_small(int x, int y, float cant, int mode, int size, int flags);

void Gui_Draw_Flat_Box(const char* str);
void Gui_Draw_Button_Box(int x, int y, int sx, int sy, const char *str, int pushed);
void Gui_Draw_Static_Box(int x, int y, int sx, int sy, const char *str, int flags);

void Gui_Clear_Array(int x, int y, int sx, int sy);
void bjbox(int x, int y, int sx, int sy);

void Draw_Editors_Bar(int Highlight);
void mess_box(char const *str);

void Realslider(int x, int y, int val, int Enabled);
void Realslider_Size(int x, int y, int size, int val, int Enabled);
void Realslider2(int x, int y, int val, int Enabled);
int Slider_Get_Center(unsigned int Size, unsigned int Maximum, int Pixels);
void Realslider_Horiz(int x, int y, int value, int displayed, int maximum, int size, int enable);
void Realslider_Vert(int x, int y, int value, int displayed, int maximum, int size, int enable);

void Coolwrite(int x, int y, int colour, char *txt);
void Refresh_UI_Context(void);

void blitnote(int x, int y, int note, int y1, int y2);
void blitlargenote(int x, int y, int note, int y1, int y2);
void blitsmallnote(int x, int y, int note, int y1, int y2);
void Note_Letter(int x, int y, char ltr, int ys, int y2);
void Letter(int x, int y, char ltr, int ys, int y2);
void Large_Letter(int x, int y, char ltr, int ys, int y2);
void Small_Letter(int x, int y, char ltr, int ys, int y2);
void Note_Large_Letter(int x, int y, char ltr, int ys, int y2);
extern void (*Letter_Function)(int x, int y, char ltr, int ys, int y2);

void Draw_Ask_Exit(void);
void DrawLine(int x1, int y1, int x2, int y2);
void DrawPixel(int x, int y, int Color);
void DrawHLine(int y, int x1, int x2, int Color);
void DrawVLine(int x, int y1, int y2, int Color);
void SetColor(int color);
void Fillrect(int x1, int y1, int x2, int y2);
void Copy(SDL_Surface *Source, int x, int y, int x1, int y1, int x2, int y2);
void Copy_To_Surface(SDL_Surface *Source, SDL_Surface *dest, int x, int y, int x1, int y1, int x2, int y2);
void Print_String(char *str, int x, int y, int size_x, int flag);
void PrintXY(int x, int y, int Font_Type, char *String);
int Get_Size_Text(char *String);
SDL_Surface *Load_Picture(char *FileName);
int Create_Font_Datas(char *FontName);
int Set_Pictures_Colors(void);
void Set_Main_Palette(void);
void Set_Logo_Palette(void);
void Refresh_Palette(void);
void Init_UI(void);
void Get_Phony_Palette(void);
void Set_Phony_Palette(void);
void Restore_Default_Palette(void);
void Display_Mouse_Pointer(int x, int y, int clear);
void Destroy_UI(void);

#endif
