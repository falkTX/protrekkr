// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _MAIN_H_
#define _MAIN_H_

// ------------------------------------------------------
// Includes
#include "../support/include/main.h"

#include "../wav/include/riff.h"
#include "../include/variables.h"

#include "../ui/include/misc_draw.h"
#include "../ui/include/xml.h"
#include "../ui/include/xml.h"

#include "../files/include/files.h"
#include "../files/include/files_list.h"

#include "../editors/include/editor_setup.h"
#include "../editors/include/editor_303.h"
#include "../editors/include/editor_fx_setup.h"
#include "../editors/include/editor_synth.h"
#include "../editors/include/editor_track_fx.h"
#include "../editors/include/editor_diskio.h"
#include "../editors/include/editor_sequencer.h"
#include "../editors/include/editor_track.h"
#include "../editors/include/editor_instrument.h"
#include "../editors/include/editor_pattern.h"
#include "../editors/include/editor_sample.h"
#include "../editors/include/patterns_blocks.h"

#include "../support/include/timer.h"

#include "../midi/include/midi.h"

#include "../../release/distrib/replay/lib/include/endianness.h"

#include <string>
#include <list>
#include <sstream>

using namespace std;

class skin_file
{
    public:
        const char *name;
};

extern list <skin_file> skin_files;
extern SDL_Surface *POINTER;

// ------------------------------------------------------
// Variables
extern SDL_Surface *Main_Screen;
extern MOUSE Mouse;

#if defined(__WIN32__)
extern HWND Main_Window;
#endif

#if defined(__WIN32__)
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#if defined(__WIN32__)
#define GETCWD _getcwd
#define CHDIR _chdir
#define GETDRIVE _getdrive
#else
#define GETCWD getcwd
#define CHDIR chdir
#define FA_DIREC 0x10
#define A_SUBDIR FA_DIREC
#define _A_SUBDIR FA_DIREC
#endif

// ------------------------------------------------------
// Functions
int Init_Context(void);
void Destroy_Context(void);
int Screen_Update(void);
void STDCALL Mixer(Uint8 *Buffer, Uint32 Len);
SDL_Surface *Load_Skin_Picture(char *name);

#endif
