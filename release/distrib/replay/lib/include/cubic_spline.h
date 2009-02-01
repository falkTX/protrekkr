// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _CUBIC_SPLINE_H_
#define _CUBIC_SPLINE_H_

// ------------------------------------------------------
// Constants
#define RESOLUTION 1024

// ------------------------------------------------------
// Functions
void Cubic_Init(void);
float Cubic_Work(float yo, float y0, float y1, float y2, unsigned int res, long offset, long length);

#endif
