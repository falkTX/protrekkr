// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _EDITOR_303_H_
#define _EDITOR_303_H_

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
// Variables
extern int snamesel;
extern SDL_Surface *Main_Screen;

extern SDL_Surface *SKIN303;

extern gear303 tb303engine[2];
extern para303 tb303[2];
extern unsigned char sl3;

// ------------------------------------------------------
// Functions
void Draw_303_Ed(void);
void Actualize_303_Ed(char gode);
void number303(unsigned char number,int x,int y);
void Skincopy(int xd, int yd, int xs, int ys, int w, int h);
void knob(int x, int y, unsigned char number);
void Mouse_Right_303_Ed(void);

int f2i(float d);

void Mouse_Left_303_Ed(void);
void Mouse_Sliders_303_Ed(void);
void Mouse_Wheel_303_Ed(int roll_amount);

#endif
