// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// ------------------------------------------------------
// Includes
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

// ------------------------------------------------------
// Constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 8

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_MIDDLE_BUTTON 2
#define MOUSE_RIGHT_BUTTON 4

#define SDL_MOUSE_LEFT_BUTTON 1
#define SDL_MOUSE_MIDDLE_BUTTON 2
#define SDL_MOUSE_RIGHT_BUTTON 3

#define TITLE "ProTrekkr"
#define VER_VER "1"
#define VER_REV "84"
#define VERSION TITLE " v"VER_VER"."VER_REV

// ------------------------------------------------------
// Structure
typedef struct
{
    int x;
    int y;
    int old_x;
    int old_y;
    int button;
    int button_oneshot;
    int delta_x;
    int delta_y;
    int wheel;
} MOUSE, *LPMOUSE;

// ------------------------------------------------------
// Variables
extern unsigned short Keys[SDLK_LAST];
extern unsigned short Keys_Sym[SDLK_LAST];
extern int Key_Unicode;
extern unsigned short Keys_Raw[SDLK_LAST];
extern unsigned short Keys_Raw_Off[SDLK_LAST];
extern int Env_Change;
extern int key_on;
extern char Keyboard_Name[];

// ------------------------------------------------------
// Functions
void Run_Interface(void);
void Message_Error(char *Message);
int Switch_FullScreen(void);
int Get_LShift(void);
int Get_RShift(void);
int Get_LAlt(void);
int Get_RAlt(void);
int Get_LCtrl(void);
int Get_RCtrl(void);

#endif
