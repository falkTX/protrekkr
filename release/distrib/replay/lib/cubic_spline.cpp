// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/cubic_spline.h"

// ------------------------------------------------------
// Variables
float at[RESOLUTION];
float bt[RESOLUTION];
float ct[RESOLUTION];
float dt[RESOLUTION];

// ------------------------------------------------------
// Initialize the working table
void Cubic_Init(void)
{
    for(int i = 0; i < RESOLUTION; i++)
    {
        float x = (float) i / (float) RESOLUTION;
        at[i] = -0.5f * x * x * x + x * x - 0.5f * x;
        bt[i] = 1.5f * x * x * x - 2.5f * x * x + 1.0f;
        ct[i] = -1.5f * x * x * x + 2.0f * x * x + 0.5f * x;
        dt[i] = 0.5f * x * x * x - 0.5f * x * x;
    }
}

// ------------------------------------------------------
// Work function. Where all is cooked :]
// yo = y[-1] [sample at x-1]
// y0 = y[0]  [sample at x (input)]
// y1 = y[1]  [sample at x+1]
// y2 = y[2]  [sample at x+2]

// res= distance between two neighbours sample points [y0 and y1] 
//      ,so [0...1.0]. You have to multiply this distance * RESOLUTION used
//      on the spline conversion table. [256 by default]
// If you are using 256 is assumed you are using 8 bit decimal
// fixed point offsets for resampling.

// offset = sample offset [info to avoid go out of bounds on sample reading ]
// offset = sample length [info to avoid go out of bounds on sample reading ]
float Cubic_Work(float yo, float y0, float y1, float y2,
                 unsigned int res, long offset, long length)
{
    res = res >> 22;
    if(offset == 0) yo = 0;
    if(offset + 2 > length) y1 = 0;
    if(offset + 3 > length) y2 = 0;

    return at[res] * yo + bt[res] * y0 + ct[res] * y1 + dt[res] * y2;
}
