// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include <SDL/SDL.h>
#include "include/timer.h"

// ------------------------------------------------------
// Variables
int Ref_Ticks;

// ------------------------------------------------------
// Init the frames counter
void Set_Frames_Counter(void) {
    Ref_Ticks = SDL_GetTicks();
}

// ------------------------------------------------------
// Amount of milliseconds between 2 frames
float Get_Frames_Delay(void) {
    float Delay;
    int Cur_Ticks = SDL_GetTicks();

    Delay = (float) (Cur_Ticks - Ref_Ticks);
    Ref_Ticks = Cur_Ticks;
    return(Delay);
}
