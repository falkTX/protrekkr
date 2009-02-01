// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
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
#define COL_MUTE 25
#define COL_PLAY 26
#define COL_MUTE_PLAY_INVERT 27
#define NUMBER_COLORS 17

#define BUTTON_NORMAL 1
#define BUTTON_PUSHED 2
#define BUTTON_TEXT_CENTERED 4
#define BUTTON_DISABLED 8

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
extern SDL_Color Palette[256];
extern LONGRGB Phony_Palette[];
extern int Idx_Palette[];

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
void outlong(int x, int y, long cant, int mode);
void outlong_small(int x, int y, long cant, int mode, int size, int flags);
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
int Get_Slider_Center(unsigned int Size, unsigned int Maximum);
void Realslider3(int x, int y, int value, int displayed, int maximum, int size, int enable);

void Coolwrite(int x, int y, int colour, char *txt);
void Refresh_UI_Context(void);

void Draw_Ask_Exit(void);
void DrawLine(int x1, int y1, int x2, int y2);
void DrawPixel(int x, int y, int Color);
void DrawHLine(int y, int x1, int x2, int Color);
void DrawVLine(int x, int y1, int y2, int Color);
void SetColor(int color);
void Fillrect(int x1, int y1, int x2, int y2);
void Copy(SDL_Surface *Source, int x, int y, int x1, int y1, int x2, int y2);
void Print_String(char *str, int x, int y, int size_x, int flag);
void PrintXY(int x, int y, int Font_Type, char *String);
int Get_Size_Text(char *String);
SDL_Surface *Load_Picture(char *FileName);
int Create_Font_Datas(char *FontName);
void Set_Pictures_Colors(void);
void Set_Main_Palette(void);
void Set_Logo_Palette(void);
void Refresh_Palette(void);
void Init_UI(void);
void Get_Phony_Palette(void);
void Set_Phony_Palette(void);
void Restore_Default_Palette(void);

#endif
