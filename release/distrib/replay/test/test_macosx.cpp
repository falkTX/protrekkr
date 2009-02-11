/*
   How to use:

   1. Copy the .ptp and the Ptk_Properties.h file into the "Replay" directory.
   2. Edit the module.asm file for visual C or module.s for MingW
      and replace the ../YOUR_MODULE.PTP by the filename of your module file.
   3. Compile the replay routine (which will be fine tuned for your module).
   4. Compile this test example.

   Note: LATENCY should be increased if the cpu is extensively used for other purposes
         to avoid lags and stuttering.
*/

#ifdef __WIN32__
#define _WIN32_WINNT 0x0500
#include <Windows.h>
#endif

#if defined(__LINUX__) || defined(__MACOSX__)
#include <unistd.h>
#endif

#if defined(__PSP__)
#include <pspkernel.h>
#include <pspctrl.h>
#include <psppower.h>

PSP_MODULE_INFO("", PSP_MODULE_KERNEL, 1, 1);
PSP_MAIN_THREAD_ATTR(0);
#endif

#include "../lib/include/ptkreplay.h"

#include <stdio.h>

extern "C"
{
    extern unsigned int _PTK_MODULE;
}

/* Initialize with 20 milliseconds of latency */
#define LATENCY 20

#if defined(__PSP__)
extern "C" void _start(void)
#else
int main(void)
#endif
{

#if defined(__PSP__)
	  FILE *test;
	  int value;

		__asm("break\n");

/*	  test = fopen("host0:/FUCK", "wb");
	  if(test)
	  {
	  	fwrite(&value, sizeof(int), 1, test);
	  	fclose(test);
		}
*/

    // Set processor & bus speed
    scePowerSetClockFrequency(333, 333, 166);
#endif
/*
    // Init the sound driver and the various tables
#if defined(__WIN32__)
    if(!Ptk_InitDriver(GetConsoleWindow(), LATENCY)) return(0);
#else
    if(!Ptk_InitDriver(LATENCY)) return(0);
#endif

    // Load it
    if(!Ptk_InitModule((unsigned char *) &_PTK_MODULE, 0))
    {
        Ptk_ReleaseDriver();
        return(0);
    }

    // Start playing it
    Ptk_Play();

#if defined(__WIN32__)
    while(!GetAsyncKeyState(VK_ESCAPE))
    {
        printf("   :   ");
        printf("\r");
        printf("%.2d:%.2d", Ptk_GetPosition(), Ptk_GetRow());
        printf("\r");

        Sleep(10);
    }
#endif

#if defined(__LINUX__) || defined(__MACOSX__)
    while(1)
    {
        usleep(10);
    }
#endif
*/
#if defined(__PSP__)
    SceCtrlData Ctrl_Buf;

    while(1)
    {
        sceCtrlPeekBufferPositive(&Ctrl_Buf, 1);
        if(Ctrl_Buf.Buttons & PSP_CTRL_HOME) sceKernelExitGame();
 
   }

#endif

  /*  Ptk_Stop();
    Ptk_ReleaseDriver();

#if defined(__WIN32__)
    ExitProcess(0);
#endif*/
}
