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
// Constructor
PtkTimer::PtkTimer()
{
    Set_Frames_Counter();
}

// ------------------------------------------------------
// Init the frames counter
void PtkTimer::Set_Frames_Counter(void)
{
    Ref_Ticks = ((float) SDL_GetTicks());
}

// ------------------------------------------------------
// Amount of milliseconds between 2 frames
float PtkTimer::Get_Frames_Delay(void)
{
    float Delay;
    float Cur_Ticks = ((float) SDL_GetTicks());

    Delay = (float) (Cur_Ticks - Ref_Ticks);
    Ref_Ticks = Cur_Ticks;
    return(Delay);
}
