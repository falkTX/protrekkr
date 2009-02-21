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

// ------------------------------------------------------
// Includes
#define NEED_SDL_GETENV
#include <SDL/SDL_types.h>
#include <string.h>
#include "include/main.h"
#include "include/timer.h"
#include "include/resource.h"

#if defined(__WIN32__)
#include <windows.h>
#include <shlwapi.h>
#endif

#if defined(__MACOSX__)
#include <mach-o/dyld.h>
#endif

#include "../include/ptk.h"

// ------------------------------------------------------
// Constants
#define MAX_EVENTS 16

// ------------------------------------------------------
// Types
typedef struct
{
    int code;
    int trans;
    int base;
} KEYCORE, LPKEYCORE;

// ------------------------------------------------------
// Variables
#if defined(__WIN32__)
HWND Main_Window;
#define SDL_NEED C_LINKAGE
#else
#define SDL_NEED
#endif

SDL_Surface *Main_Screen;
SDL_SysWMinfo WMInfo;
int Prog_End;
MOUSE Mouse;
unsigned short Keys[SDLK_LAST];
unsigned short Keys_Sym[SDLK_LAST];
unsigned short Keys_Raw_Off[65535];
unsigned short Keys_Raw_Repeat[65535];
unsigned short Keys_Raw[65535];
unsigned short Keys_Unicode[65535];
int Keyboard_Nbr_Events;
int Keyboard_Events[256];
int Keyboard_Notes_Type[256];
int Keyboard_Notes_Bound[256];
int Key_Unicode;
char FullScreen = FALSE;
char AutoSave;
char Window_Title[256];
extern int gui_pushed;
int Env_Change;
int key_on = 0;
float delay_refresh;
float delay_refresh2;
extern int display_title;
int exiting;

char *ExePath;

static SDL_Event Events[MAX_EVENTS];

int Nbr_Keyboards;
int Keyboard_Idx;
char Keyboard_Labels[256][256];
char Keyboard_FileNames[256][256];

const char *Keyboards;
char Keyboard_Name[MAX_PATH];

// The currently used keyboard
KEYCORE Keyboard[] =
{
    {  0, 0x03, '2' },     // Do#
    {  0, 0x04, '3' },     // Re#
    {  0, 0x06, '5' },     // Fa#
    {  0, 0x07, '6' },     // Sol#
    {  0, 0x08, '7' },     // La#
    {  0, 0x0a, '9' },     // Do#
    {  0, 0x0b, '0' },     // Re#
    {  0, 0x0d, '=' },     // Fa#

    {  0, 0x10, 'q' },     // Do
    {  0, 0x11, 'w' },     // Re
    {  0, 0x12, 'e' },     // Mi
    {  0, 0x13, 'r' },     // Fa
    {  0, 0x14, 't' },     // Sol
    {  0, 0x15, 'y' },     // La
    {  0, 0x16, 'u' },     // Si
    {  0, 0x17, 'i' },     // Do
    {  0, 0x18, 'o' },     // Re
    {  0, 0x19, 'p' },     // Mi
    {  0, 0x1a, '[' },     // Fa
    {  0, 0x1b, ']' },     // Sol

    {  0, 0x1f, 's' },     // Do#
    {  0, 0x20, 'd' },     // Re#
    {  0, 0x22, 'g' },     // Fa#
    {  0, 0x23, 'h' },     // Sol#
    {  0, 0x24, 'j' },     // La#
    {  0, 0x26, 'l' },     // Do#
    {  0, 0x27, ';' },     // Re#

    {  0, 0x2c, 'z' },     // Do
    {  0, 0x2d, 'x' },     // Re
    {  0, 0x2e, 'c' },     // Mi
    {  0, 0x2f, 'v' },     // Fa
    {  0, 0x30, 'b' },     // Sol
    {  0, 0x31, 'n' },     // La
    {  0, 0x32, 'm' },     // Si
    {  0, 0x33, ',' },     // Do
    {  0, 0x34, '.' },     // Re
    {  0, 0x35, '/' },     // Mi
};

// English keyboard
KEYCORE Default_Keyboard[] =
{
    {  '2', 0x03, '2' },     // Do#
    {  '3', 0x04, '3' },     // Re#
    {  '5', 0x06, '5' },     // Fa#
    {  '6', 0x07, '6' },     // Sol#
    {  '8', 0x08, '7' },     // La#
    {  '9', 0x0a, '9' },     // Do#
    {  '0', 0x0b, '0' },     // Re#
    {  '-', 0x0d, '-' },     // Fa#

    {  'q', 0x10, 'q' },     // Do
    {  'w', 0x11, 'w' },     // Re
    {  'e', 0x12, 'e' },     // Mi
    {  'r', 0x13, 'r' },     // Fa
    {  't', 0x14, 't' },     // Sol
    {  'y', 0x15, 'y' },     // La
    {  'u', 0x16, 'u' },     // Si
    {  'i', 0x17, 'i' },     // Do
    {  'o', 0x18, 'o' },     // Re
    {  'p', 0x19, 'p' },     // Mi
    {  '[', 0x1a, '[' },     // Fa
    {  ']', 0x1b, ']' },     // Sol

    {  's', 0x1f, 's' },     // Do#
    {  'd', 0x20, 'd' },     // Re#
    {  'g', 0x22, 'g' },     // Fa#
    {  'h', 0x23, 'h' },     // Sol#
    {  'j', 0x24, 'j' },     // La#
    {  'l', 0x26, 'l' },     // Do#
    {  ';', 0x27, ';' },     // Re#

    {  'z', 0x2c, 'z' },     // Do
    {  'x', 0x2d, 'x' },     // Re
    {  'c', 0x2e, 'c' },     // Mi
    {  'v', 0x2f, 'v' },     // Fa
    {  'b', 0x30, 'b' },     // Sol
    {  'n', 0x31, 'n' },     // La
    {  'm', 0x32, 'm' },     // Si
    {  ',', 0x33, ',' },     // Do
    {  '.', 0x34, '.' },     // Re
    {  '/', 0x35, '/' },     // Mi
};

LPKEYCORE *Cur_Keyboard = Default_Keyboard;

// ------------------------------------------------------
// Functions
int Get_LShift(void)
{
    if(SDL_GetModState() & KMOD_LSHIFT) return(TRUE);
    return(FALSE);
}

int Get_RShift(void)
{
    if(SDL_GetModState() & KMOD_RSHIFT) return(TRUE);
    return(FALSE);
}

int Get_LAlt(void)
{
    if(SDL_GetModState() & KMOD_LALT) return(TRUE);
    return(FALSE);
}

int Get_RAlt(void)
{
    if(SDL_GetModState() & KMOD_RALT) return(TRUE);
    return(FALSE);
}

int Get_LCtrl(void)
{
    if(SDL_GetModState() & KMOD_LCTRL) return(TRUE);
    return(FALSE);
}

int Get_RCtrl(void)
{
    if(SDL_GetModState() & KMOD_RCTRL) return(TRUE);
    return(FALSE);
}

// ------------------------------------------------------
// SDL scancode doesn't make it, so we have to blast
int Translate_Locale_Key(int Key)
{
    int i;

    for(i = 0; i < 37; i++)
    {

        // apparently, windows doesn't care about that localization
#if defined(__WIN32__)
        if(Cur_Keyboard[i].base == Key)
#else
        if(Cur_Keyboard[i].code == Key)
#endif
        {
            return(Cur_Keyboard[i].trans);
        }
    }
    return(0);
}

// ------------------------------------------------------
// Get the full name of the currently selected keyboard
char *Get_Keyboard_Label(void)
{
    return(Keyboard_Labels[Keyboard_Idx]);
}

// ------------------------------------------------------
// Get the filename of the currently selected keyboard
char *Get_Keyboard_FileName(void)
{
    return(Keyboard_FileNames[Keyboard_Idx]);
}

// ------------------------------------------------------
// Load a keyboard definition file
void Load_Keyboard_Def(char *FileName)
{
    char KbData[64];
    FILE *KbFile;
    int j;
    int Key_Value;
    char *KbDataEnd;
    char KbFileName[MAX_PATH];
    char Keyboard_Label[256];

#if defined(__WIN32__)
    strcpy(KbFileName, ExePath);
    strcat(KbFileName, "\\skins\\");
#else
    strcpy(KbFileName, ExePath);
    strcat(KbFileName, "/skins/");
#endif

    strcat(KbFileName, FileName);

    // Current keyboard filename (saved in ptk.cfg)
    sprintf(Keyboard_Name, "%s", FileName);

    KbFile = fopen(KbFileName, "r");
    if(KbFile)
    {
        fscanf(KbFile, "%s", &Keyboard_Label);
        j = 0;
        while(!feof(KbFile) && j < 37)
        {
            // Retrieve the data and store it
            fscanf(KbFile, "%s", &KbData);
            if(KbData[0] == '0' && (KbData[1] == 'x' || KbData[1] == 'X'))
            {
                // Hexadecimal data
                Key_Value = strtol(KbData, &KbDataEnd, 16);
            }
            else
            {
                // Plain ASCII
                Key_Value = KbData[0];
            }
            Keyboard[j].code = Key_Value;
            j++;
        }
        fclose(KbFile);
    }
}

// ------------------------------------------------------
// Main part of the tracker interface
#if defined(__WIN32__)
extern SDL_NEED int SDL_main(int argc, char *argv[])
#else
	#if defined(__LINUX__)
		int main(int argc, char *argv[])
	#else
		extern "C" int main(int argc, char *argv[])
	#endif
#endif

{
    int Startup_Mouse_Pos_x;
    int Startup_Mouse_Pos_y;
    SDL_KeyboardEvent *kb_evnt;
    char KbFileName[MAX_PATH];
    char KbFileNameToLoad[MAX_PATH];
    char KbData[64];
    char *KbDataEnd;
    int next_kb_offset;
    int i;
    int j;
    int Key_Value;
    int Uni_Trans;
    FILE *KbFile;
    int in_note;
    Uint32 ExePath_Size = MAX_PATH;

#if defined(__MACOSX__)
    Uint32 Path_Length;
#endif

    SDL_putenv("SDL_VIDEO_WINDOW_POS=center");
    SDL_putenv("SDL_VIDEO_CENTERED=1");

#if defined(__WIN32__)
    SDL_putenv("SDL_VIDEO_DRIVER=directx");
#else
    SDL_putenv("SDL_VIDEO_DRIVER=opengl");
#endif

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) < 0)
    {
        Message_Error("Can't open SDL.");
        exit(0);
    }
    atexit(Destroy_Context);

    // Show the restrictions:
    char *NoMidi = "";
    char *NoCodec = "";

#if defined(__NO_MIDI__)
    NoMidi = "no midi";
#endif
#if defined(__NO_CODEC__)
#if defined(__NO_MIDI__)
    NoCodec = " - ";
#endif
    NoCodec = "no codec";
#endif
#if !defined(__NO_MIDI__) && !defined(__NO_CODEC__)
    NoMidi = "none";
#endif

    sprintf(Window_Title, "%s - Build restrictions: %s%s", VERSION, NoMidi, NoCodec);
    SDL_WM_SetCaption(Window_Title, NULL);

    // Obtain SDL window
    SDL_GetWMInfo(&WMInfo);

#if defined(__WIN32__)
    Main_Window = WMInfo.window;
    // Set the icon of the window
    SetClassLong(Main_Window, GCL_HICON,
                 (LONG) LoadIcon(GetModuleHandle(NULL),
                 MAKEINTRESOURCE(IDI_ICON1)));
#endif

    ExePath = (char *) malloc(ExePath_Size + 1);
    if(ExePath == NULL)
    {
        Message_Error("Can't open alloc memory.");
        SDL_Quit();
        exit(0);
    }
    memset(ExePath, 0, ExePath_Size + 1);

#if defined(__LINUX__)
    // Note:
    // it looks like some distros don't have /proc/self
    // Maybe a better (?) solution would be to use:
    // sprintf(ExeProc, "/proc/$d/exe", getpid());
    // readlink(ExeProc, ExePath, sizeof(ExePath));
    readlink("/proc/self/exe", ExePath, ExePath_Size);
    int exename_len = strlen(ExePath);
    while(exename_len--)
    {
        if(ExePath[exename_len] == '/')
        {
            ExePath[exename_len] = 0;
            exename_len++;
            break;
        }
    }
    CHDIR(ExePath);
#else
    #if defined(__MACOSX__)
        Path_Length = ExePath_Size;
        _NSGetExecutablePath(ExePath, &Path_Length);
        while(Path_Length--)
        {
            if(ExePath[Path_Length] == '/')
            {
                ExePath[Path_Length] = 0;
                Path_Length++;
                break;
            }
        }
        CHDIR(ExePath);
    #else
        GETCWD(ExePath, MAX_PATH);
    #endif
#endif

    // Default values
    sprintf(Keyboard_Name, "%s", "kben.txt");
    sprintf(Keyboard_FileNames[0], "%s", "kben.txt");
    sprintf(Keyboard_Labels[0], "%s", "English");
    Nbr_Keyboards = 1;
    Cur_Keyboard = Default_Keyboard;

    // Load the configuration file
    Restore_Default_Palette();
    LoadConfig();

    if(!strlen(Keyboard_Name)) sprintf(Keyboard_Name, "%s", "kben.txt");

    if(!XML_Init())
    {
        SDL_Quit();
        exit(0);
    }

    if((Keyboards = XML_get_string("files", "file", "keyboards", "value")) != NULL)
    {
        memset(KbFileNameToLoad, 0, sizeof(KbFileNameToLoad));

        Nbr_Keyboards = 0;

        next_kb_offset = 0;
        i = 0;
        while(next_kb_offset < strlen(Keyboards))
        {
            sscanf(Keyboards + next_kb_offset, "%s", &KbFileName);
            next_kb_offset += strlen(KbFileName) + 1;

            // Load the keyboard file

#if defined(__WIN32__)
            strcpy(KbFileNameToLoad, ExePath);
            strcat(KbFileNameToLoad, "\\skins\\");
#else
            strcpy(KbFileNameToLoad, ExePath);
            strcat(KbFileNameToLoad, "/skins/");
#endif

            strcat(KbFileNameToLoad, KbFileName);

            // Store it
            sprintf(Keyboard_FileNames[i], "%s", KbFileName);
            sprintf(Keyboard_Labels[i], "%s", "");

            KbFile = fopen(KbFileNameToLoad, "r");
            if(KbFile)
            {
                // Get the full name
                fscanf(KbFile, "%s", &Keyboard_Labels[i]);
                Nbr_Keyboards++;

                if(SDL_strcasecmp(KbFileName, Keyboard_Name) == 0)
                {
                    Keyboard_Idx = i;
                    // Parse it
                    j = 0;
                    Cur_Keyboard = Keyboard;
                    while(!feof(KbFile) && j < 37)
                    {
                        // Retrieve the data and store it
                        fscanf(KbFile, "%s", &KbData);
                        if(KbData[0] == '0' && (KbData[1] == 'x' || KbData[1] == 'X'))
                        {
                            // Hexadecimal data
                            Key_Value = strtol(KbData, &KbDataEnd, 16);
                        }
                        else
                        {
                            // Plain ASCII
                            Key_Value = (unsigned char) KbData[0];
                        }
                        Keyboard[j].code = Key_Value;
                        j++;
                    }
                }
                fclose(KbFile);
            }
            i++;
        }
    }

    // Avoid a possible flash
    int Save_R = Ptk_Palette[0].r;
    int Save_G = Ptk_Palette[0].g;
    int Save_B = Ptk_Palette[0].b;

    Ptk_Palette[0].r = 0;
    Ptk_Palette[0].g = 0;
    Ptk_Palette[0].b = 0;

    if(!Switch_FullScreen())
    {
        Message_Error("Can't open screen.");
        SDL_Quit();
        exit(0);
    }
    Ptk_Palette[0].r = Save_R;
    Ptk_Palette[0].g = Save_G;
    Ptk_Palette[0].b = Save_B;

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_EnableUNICODE(TRUE);

    Prog_End = FALSE;

#if !defined(__NO_MIDI__)
    // Load midi devices infos
    Midi_GetAll();
#endif

    if(!Init_Context())
    {
        SDL_Quit();
        exit(0);
    }

    SDL_GetMouseState((int *) &Startup_Mouse_Pos_x, (int *) &Startup_Mouse_Pos_y);
    Mouse.x = Startup_Mouse_Pos_x;
    Mouse.y = Startup_Mouse_Pos_y;
    Mouse.old_x = -16;
    Mouse.old_y = -16;

    while(!Prog_End)
    {
        Mouse.wheel = 0;
        if(Mouse.button_oneshot & MOUSE_LEFT_BUTTON) Mouse.button_oneshot &= ~MOUSE_LEFT_BUTTON;
        if(Mouse.button_oneshot & MOUSE_MIDDLE_BUTTON) Mouse.button_oneshot &= ~MOUSE_MIDDLE_BUTTON;
        if(Mouse.button_oneshot & MOUSE_RIGHT_BUTTON) Mouse.button_oneshot &= ~MOUSE_RIGHT_BUTTON;

        memset(Keys, 0, sizeof(Keys));
        memset(Keys_Sym, 0, sizeof(Keys_Sym));
        memset(Keys_Unicode, 0, sizeof(Keys_Raw));

        SDL_PumpEvents();
        int Nbr_Events = SDL_PeepEvents(Events, MAX_EVENTS, SDL_GETEVENT, 0xffffff);
        int Symbol;
        int Scancode;

        for(int i = 0; i < Nbr_Events; i++)
        {
            switch(Events[i].type)
            {
                case SDL_KEYDOWN:
                    Remove_Title();

                    Key_Unicode = Events[i].key.keysym.unicode;

                    Symbol = Events[i].key.keysym.sym;
                    if(Symbol != SDLK_KP_DIVIDE)
                    {
                        Uni_Trans = Events[i].key.keysym.unicode;
                        // This is only used for the digits on all systems
                        // (especially on kb configs where they can only
                        //  be accessed by pressing shift).
                        // Otherwise it doesn't work under Mac OSX
                        Keys_Unicode[Uni_Trans] = TRUE;

#if !defined(__MACOSX__)
                        if(!Uni_Trans) Uni_Trans = Symbol;
#else
                        Uni_Trans = Symbol;
#endif
                        Keys[Uni_Trans] = TRUE;

                        Scancode = Translate_Locale_Key(Symbol);

                        //printf("%x %d %x %d\n", Symbol, Events[i].key.keysym.unicode, Uni_Trans, Uni_Trans);

                        Keys_Raw[Scancode] = TRUE;
                        Keys_Raw_Off[Scancode] = FALSE;
                        Keys_Raw_Repeat[Scancode] = TRUE;

                        if(!is_recording_2 && is_editing)
                        {
                            in_note = FALSE;
                            for(j = 0; j < Channels_MultiNotes[ped_track]; j++)
                            {
                                if(ped_col == (j * 3))
                                {
                                    in_note = TRUE;
                                    break;
                                }
                            }
                            if(in_note)
                            {
                                if(!Get_LCtrl() && !Get_LShift() && !Get_LAlt())
                                {
                                    Send_Note(Scancode, FALSE, TRUE);
                                }
                            }
                        }
                    }

                    // Only used for SDLK_KP_DIVIDE and SDLK_KP_MULTIPLY
                    Keys_Sym[Symbol] = TRUE;

                    if(key_on != 2) key_on = 1;

                    if(SDL_GetModState() & KMOD_LALT)
                    {
                        if(Keys[SDLK_RETURN])
                        {
                            FullScreen ^= TRUE;
                            Switch_FullScreen();
                        }
                    }
                    break;

                case SDL_KEYUP:
                    Remove_Title();

                    kb_evnt = (SDL_KeyboardEvent *) &Events[i];
                    if(kb_evnt->state == SDL_RELEASED)
                    {
                        key_on = 0;
                    }

                    // Only used for SDLK_KP_DIVIDE and SDLK_KP_MULTIPLY
                    Symbol = Events[i].key.keysym.sym;
                    Keys_Sym[Symbol] = FALSE;

                    Scancode = Translate_Locale_Key(Symbol);
                    Keys_Raw_Off[Scancode] = TRUE;
                    Keys_Raw_Repeat[Scancode] = FALSE;

                    if(!is_recording_2 && is_editing)
                    {
                        in_note = FALSE;
                        for(j = 0; j < Channels_MultiNotes[ped_track]; j++)
                        {
                            if(ped_col == (j * 3))
                            {
                                in_note = TRUE;
                                break;
                            }
                        }
                        if(in_note)
                        {
                            if(!Get_LCtrl() && !Get_LShift() && !Get_LAlt())
                            {
                                Send_Note(Scancode | 0x80, FALSE, TRUE);
                            }
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                    Mouse.x = Events[i].button.x;
                    Mouse.y = Events[i].button.y;

                    if(!display_title)
                    {
                        Remove_Title();
                    }
                    else
                    {
                        switch(Events[i].button.state)
                        {
                            case SDL_PRESSED:
                                switch(Events[i].button.button)
                                {
                                    case SDL_MOUSE_LEFT_BUTTON:
                                        Mouse.button |= MOUSE_LEFT_BUTTON;
                                        Mouse.button_oneshot |= MOUSE_LEFT_BUTTON;
                                        break;

                                    case SDL_MOUSE_MIDDLE_BUTTON:
                                        Mouse.button |= MOUSE_MIDDLE_BUTTON;
                                        Mouse.button_oneshot |= MOUSE_MIDDLE_BUTTON;
                                        break;

                                    case SDL_MOUSE_RIGHT_BUTTON:
                                        Mouse.button |= MOUSE_RIGHT_BUTTON;
                                        Mouse.button_oneshot |= MOUSE_RIGHT_BUTTON;
                                        break;
                                }
                                if(Events[i].button.button == 4)
                                {
                                    Mouse.wheel = 1;
                                }
                                if(Events[i].button.button == 5)
                                {
                                    Mouse.wheel = -1;
                                }
                                break;

                            case SDL_RELEASED:
                                switch(Events[i].button.button)
                                {
                                    case SDL_MOUSE_LEFT_BUTTON:
                                        Mouse.button &= ~MOUSE_LEFT_BUTTON;
                                        Mouse.button_oneshot &= ~MOUSE_LEFT_BUTTON;
                                        gui_pushed &= ~MOUSE_LEFT_BUTTON;
                                        break;
                                    case SDL_MOUSE_MIDDLE_BUTTON:
                                        Mouse.button &= ~MOUSE_MIDDLE_BUTTON;
                                        Mouse.button_oneshot &= ~MOUSE_MIDDLE_BUTTON;
                                        gui_pushed &= ~MOUSE_MIDDLE_BUTTON;
                                        break;
                                    case SDL_MOUSE_RIGHT_BUTTON:
                                        Mouse.button &= ~MOUSE_RIGHT_BUTTON;
                                        Mouse.button_oneshot &= ~MOUSE_RIGHT_BUTTON;
                                        gui_pushed &= ~MOUSE_RIGHT_BUTTON;
                                        break;
                                }
                                break;
                        }
                    }
                    break;

                case SDL_MOUSEMOTION:
                    Mouse.x = Events[i].motion.x;
                    Mouse.y = Events[i].motion.y;
                    break;

                case SDL_QUIT:
                    Prog_End = TRUE;
                    break;

                case SDL_ACTIVEEVENT:
                    if(Events[i].active.gain)
                    {
                        if(FullScreen)
                        {
                            Env_Change = TRUE;
                            Init_UI();
                        }
                    }
                    else
                    {
                        memset(Keys, 0, sizeof(Keys));
                        memset(Keys_Sym, 0, sizeof(Keys_Sym));
                    }
                    break;

                default:
                    break;
            }
        }

        if(display_title == 3) Display_Mouse_Pointer(Mouse.old_x, Mouse.old_y, TRUE);

        if(!Screen_Update()) break;

        if(display_title == 3) Display_Mouse_Pointer(Mouse.x, Mouse.y, FALSE);

        // Flush all pending blits
        SDL_UpdateRect(Main_Screen, 0, 0, 0, 0);

        SDL_Flip(Main_Screen);

        Mouse.old_x = Mouse.x;
        Mouse.old_y = Mouse.y;

        SDL_Delay(10);
    }
    exiting = TRUE;
    SaveConfig();

	if(ExePath) free(ExePath);

#if !defined(__NO_MIDI__)
    // Close any opened midi devices on any exit
    Midi_CloseIn();
    Midi_CloseOut();
#endif

#if !defined(__NO_MIDI__)
    // Free the devices enumeration
    Midi_FreeAll();
#endif

    SDL_Quit();
    exit(0);
}

// ------------------------------------------------------
// Display an error messagebox
void Message_Error(char *Message)
{
    printf("Error: %s\n", Message);
}

// ------------------------------------------------------
// Swap window/fullscreen mode
int Switch_FullScreen(void)
{
    Env_Change = TRUE;
    if((Main_Screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SCREEN_BPP, SDL_SWSURFACE | (FullScreen ? SDL_FULLSCREEN : 0))) == NULL)
    {
        return(FALSE);
    }
    Init_UI();

    SDL_ShowCursor(0);
    return(TRUE);
}

// ------------------------------------------------------
// Remove the title pic
void Remove_Title(void)
{
    if(!display_title)
    {
        Set_Phony_Palette();
        Set_Main_Palette();
        SetColor(COL_BACKGROUND);
        Fillrect(0, 0, 800, 600);
        display_title = 1;
        Env_Change = TRUE;
    }
}
