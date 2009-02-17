// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/misc_draw.h"
#include "../include/variables.h"
#include "../include/ptk.h"
#include "../editors/include/editor_303.h"
#include "../editors/include/editor_diskio.h"
#include "../editors/include/editor_fx_setup.h"
#include "../editors/include/editor_instrument.h"
#include "../editors/include/editor_setup.h"
#include "../editors/include/editor_synth.h"
#include "../editors/include/editor_pattern.h"
#include "../editors/include/editor_sample.h"
#include "../editors/include/editor_sequencer.h"
#include "../editors/include/editor_track.h"
#include "../editors/include/editor_track_fx.h"
#include "../extralibs/sdl_draw/include/sdl_draw.h"

// ------------------------------------------------------
// Variables
int FgColor;
int max_colors_logo;
int max_colors_303;
SDL_Surface *Temp_PFONT;
int Beveled = TRUE;

int Nbr_Letters;
int Font_Height = 11;
char *Font_Ascii = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789&\351\"'(-\350_\347\340)=*+^$\371%\265,;:!?./<>@#[]|\\\001\002\003\004\005\006\007\010 ";
int Font_Pos[256];
int Font_Size[256];

char *Notes[] =
{
    "C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
    "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
    "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
    "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
    "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
    "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
    "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
    "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
    "C-8", "C#8", "D-8", "D#8", "E-8", "F-8", "F#8", "G-8", "G#8", "A-8", "A#8", "B-8",
    "C-9", "C#9", "D-9", "D#9", "E-9", "F-9", "F#9", "G-9", "G#9", "A-9", "A#9", "B-9",
    "off"
};

char *DecChar[] =
{
    "000", "001", "002", "003", "004", "005", "006", "007", "008", "009",
    "010", "011", "012", "013", "014", "015", "016", "017", "018", "019",
    "020", "021", "022", "023", "024", "025", "026", "027", "028", "029",
    "030", "031", "032", "033", "034", "035", "036", "037", "038", "039",
    "040", "041", "042", "043", "044", "045", "046", "047", "048", "049",
    "050", "051", "052", "053", "054", "055", "056", "057", "058", "059",
    "060", "061", "062", "063", "064", "065", "066", "067", "068", "069",
    "070", "071", "072", "073", "074", "075", "076", "077", "078", "079",
    "080", "081", "082", "083", "084", "085", "086", "087", "088", "089",
    "090", "091", "092", "093", "094", "095", "096", "097", "098", "099",
    "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
    "110", "111", "112", "113", "114", "115", "116", "117", "118", "119",
    "120", "121", "122", "123", "124", "125", "126", "127", "128", "129",
    "130", "131", "132", "133", "134", "135", "136", "137", "138", "139",
    "140", "141", "142", "143", "144", "145", "146", "147", "148", "149",
    "150", "151", "152", "153", "154", "155", "156", "157", "158", "159",
    "160", "161", "162", "163", "164", "165", "166", "167", "168", "169",
    "170", "171", "172", "173", "174", "175", "176", "177", "178", "179",
    "180", "181", "182", "183", "184", "185", "186", "187", "188", "189",
    "190", "191", "192", "193", "194", "195", "196", "197", "198", "199",
    "200", "201", "202", "203", "204", "205", "206", "207", "208", "209",
    "210", "211", "212", "213", "214", "215", "216", "217", "218", "219",
    "220", "221", "222", "223", "224", "225", "226", "227", "228", "229",
    "230", "231", "232", "233", "234", "235", "236", "237", "238", "239",
    "240", "241", "242", "243", "244", "245", "246", "247", "248", "249",
    "250", "251", "252", "253", "254", "255", "256"
};

char *DecChar2[] =
{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
};

char *HexTab[] =
{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
    "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
    "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};

SDL_Color Ptk_Palette[256];
SDL_Color Palette_303[256];
SDL_Color Palette_Logo[256];

char *Labels_Palette[] =
{
    "Background",
    "Foreground",
    "Buttons",
    "Pushed Buttons",
    "Sliders",
    "Vumeter",
    "Vumeter peak",
    "Scopes / Samples",
    "Font",
    "Pattern Lo Backgnd",
    "Pattern Lo Foregnd",
    "Pattern Hi Backgnd",
    "Pattern Hi Foregnd",
    "Pattern Sel Backgnd",
    "Pattern Sel Foregnd",
    "Track Off / Mute",
    "Track On / Play"
};

int Idx_Palette[] =
{
    1,
    3,
    6,
    9,
    12,
    14,
    15,
    16,
    17,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26
};

SDL_Color Default_Palette[] =
{
    { 0x00, 0x00, 0x00, 0x00 },      // 0 real background

    { 0x00, 0x00, 0x00, 0x00 },      // 1

    { 0x00, 0x00, 0x00, 0x00 },      // 2 static highlight (calculated)
    { 0x4a, 0x58, 0x6c, 0x00 },      // 3
    { 0x00, 0x00, 0x00, 0x00 },      // 4 (calculated)

    { 0x00, 0x00, 0x00, 0x00 },      // 5 interactive parts highlight (calculated)
    { 0x3c, 0x62, 0x88, 0x00 },      // 6
    { 0x00, 0x00, 0x00, 0x00 },      // 7 (calculated)

    { 0x00, 0x00, 0x00, 0x00 },      // 8 pushed button highlight (calculated)
    { 0x00, 0x34, 0x66, 0x00 },      // 9
    { 0x00, 0x00, 0x00, 0x00 },      // 10 (calculated)

    { 0x00, 0x00, 0x00, 0x00 },      // 11 slider highlight (calculated)
    { 0x40, 0x4c, 0x7c, 0x00 },      // 12
    { 0x00, 0x00, 0x00, 0x00 },      // 13 (calculated)

    { 0x24, 0x9c, 0x76, 0x00 },      // 14 vumeter
    { 0xee, 0xae, 0x86, 0x00 },      // 15 vumeter peak
    { 0x6a, 0xff, 0xc2, 0x00 },      // 16 scopes / samples

    { 0xff, 0xff, 0xff, 0x00 },      // 17 Font hi
    { 0x77, 0x77, 0x77, 0x00 },      // 18 Font lo

    { 0x00, 0x00, 0x00, 0x00 },      // 19 Pattern lo background
    { 0x68, 0x8c, 0xac, 0x00 },      // 20 Pattern lo foreground

    { 0x28, 0x2c, 0x3c, 0x00 },      // 21 Pattern hi background
    { 0xa0, 0xbe, 0xe4, 0x00 },      // 22 Pattern hi foreground

    { 0x11, 0x55, 0x7f, 0x00 },      // 23 Pattern sel background
    { 0xea, 0xf0, 0xff, 0x00 },      // 24 Pattern sel foreground

    { 0xff, 0x4a, 0x00, 0x00 },      // 25 mute
    { 0x38, 0xbe, 0x88, 0x00 },      // 26 play

    { 0xff, 0xff, 0xff, 0x00 },      // 27 mute/play highlight (fixed)

    { 0x00, 0x00, 0x00, 0x00 },      // 28 Shadow Pattern lo background (calculated)
    { 0x00, 0x00, 0x00, 0x00 },      // 29 Shadow Pattern lo foreground (calculated)

    { 0x00, 0x00, 0x00, 0x00 },      // 30 Shadow Pattern hi background (calculated)
    { 0x00, 0x00, 0x00, 0x00 },      // 31 Shadow Pattern hi foreground (calculated)

    { 0x00, 0x00, 0x00, 0x00 },      // 32 Shadow Pattern sel background (calculated)
    { 0x00, 0x00, 0x00, 0x00 },      // 33 Shadow Pattern sel foreground (calculated)
};

LONGRGB Phony_Palette[sizeof(Default_Palette) / sizeof(SDL_Color)];

SDL_Surface *POINTER;
unsigned char *Pointer_BackBuf;
int bare_color_idx;

// ------------------------------------------------------
// Functions

void out_note(int x, int y, int note, int color)
{
    PrintXY(x, y, USE_FONT, Notes[note]);
}

void out_decchar(int x, int y, int number, char smith)
{
    PrintXY(x, y, USE_FONT, DecChar[number]);
}

void out_decchar2(int x, int y, int number, char smith)
{
    PrintXY(x, y, USE_FONT, DecChar2[number]);
}

void Get_Phony_Palette(void)
{
    int i;

    for(i = 0; i < sizeof(Default_Palette) / sizeof(SDL_Color); i++)
    {
        Phony_Palette[i].r = Ptk_Palette[i].r;
        Phony_Palette[i].g = Ptk_Palette[i].g;
        Phony_Palette[i].b = Ptk_Palette[i].b;
    }
}

void Set_Phony_Palette(void)
{
    int i;
    int ComponentR;
    int ComponentG;
    int ComponentB;

    for(i = 0; i < sizeof(Default_Palette) / sizeof(SDL_Color); i++)
    {
        switch(i)
        {
            case 2:
            case 5:
            case 8:
            case 11:
                ComponentR = Phony_Palette[i + 1].r;
                ComponentG = Phony_Palette[i + 1].g;
                ComponentB = Phony_Palette[i + 1].b;
                ComponentR += 0x20;
                ComponentG += 0x20;
                ComponentB += 0x20;
                if(ComponentR > 0xff) ComponentR = 0xff;
                if(ComponentG > 0xff) ComponentG = 0xff;
                if(ComponentB > 0xff) ComponentB = 0xff;
                Ptk_Palette[i].r = ComponentR;
                Ptk_Palette[i].g = ComponentG;
                Ptk_Palette[i].b = ComponentB;
                break;

            case 4:
            case 7:
            case 10:
            case 13:
                ComponentR = Phony_Palette[i - 1].r;
                ComponentG = Phony_Palette[i - 1].g;
                ComponentB = Phony_Palette[i - 1].b;
                if(!Beveled)
                {
                    ComponentR += 0x20;
                    ComponentG += 0x20;
                    ComponentB += 0x20;
                    if(ComponentR > 0xff) ComponentR = 0xff;
                    if(ComponentG > 0xff) ComponentG = 0xff;
                    if(ComponentB > 0xff) ComponentB = 0xff;
                }
                else
                {
                    ComponentR -= 0x20;
                    ComponentG -= 0x20;
                    ComponentB -= 0x20;
                    if(ComponentR < 0) ComponentR = 0;
                    if(ComponentG < 0) ComponentG = 0;
                    if(ComponentB < 0) ComponentB = 0;
                }
                Ptk_Palette[i].r = ComponentR;
                Ptk_Palette[i].g = ComponentG;
                Ptk_Palette[i].b = ComponentB;
                break;

            // The font
            case 17:
                ComponentR = Phony_Palette[i].r;
                ComponentG = Phony_Palette[i].g;
                ComponentB = Phony_Palette[i].b;
                Ptk_Palette[i].r = ComponentR;
                Ptk_Palette[i].g = ComponentG;
                Ptk_Palette[i].b = ComponentB;
                if(ComponentR == 0) ComponentR = 255;
                if(ComponentG == 0) ComponentG = 255;
                if(ComponentB == 0) ComponentB = 255;
                ComponentR >>= 1;
                ComponentG >>= 1;
                ComponentB >>= 1;
                Ptk_Palette[i + 1].r = ComponentR;
                Ptk_Palette[i + 1].g = ComponentG;
                Ptk_Palette[i + 1].b = ComponentB;
                Phony_Palette[i + 1].r = ComponentR;
                Phony_Palette[i + 1].g = ComponentG;
                Phony_Palette[i + 1].b = ComponentB;
                break;

            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
                ComponentR = Phony_Palette[i].r;
                ComponentG = Phony_Palette[i].g;
                ComponentB = Phony_Palette[i].b;
                Ptk_Palette[i].r = ComponentR;
                Ptk_Palette[i].g = ComponentG;
                Ptk_Palette[i].b = ComponentB;
                ComponentR >>= 1;
                ComponentG >>= 1;
                ComponentB >>= 1;
                Ptk_Palette[i + 9].r = ComponentR;
                Ptk_Palette[i + 9].g = ComponentG;
                Ptk_Palette[i + 9].b = ComponentB;
                Phony_Palette[i + 9].r = ComponentR;
                Phony_Palette[i + 9].g = ComponentG;
                Phony_Palette[i + 9].b = ComponentB;
                break;

            case 25:
                Ptk_Palette[i].r = Phony_Palette[i].r;
                Ptk_Palette[i].g = Phony_Palette[i].g;
                Ptk_Palette[i].b = Phony_Palette[i].b;
                break;

            case 26:
                Ptk_Palette[i].r = Phony_Palette[i].r;
                Ptk_Palette[i].g = Phony_Palette[i].g;
                Ptk_Palette[i].b = Phony_Palette[i].b;
                break;

            default:
                Ptk_Palette[i].r = Phony_Palette[i].r;
                Ptk_Palette[i].g = Phony_Palette[i].g;
                Ptk_Palette[i].b = Phony_Palette[i].b;
                break;
        }
    }
}

void out_hexchar(int x, int y, int number, char smith, int color)
{
    switch(number)
    {
        case 255:
            if(smith == 0) PrintXY(x, y, USE_FONT, "FF");
            if(smith == 1) PrintXY(x, y, USE_FONT, "..");
            break;
        default:
            PrintXY(x, y, USE_FONT, HexTab[number]);
            break;
    }
}

void value_box(int x, int y, int val, int flags)
{
    Gui_Draw_Button_Box(x, y, 16, 16, "\03", flags);
    Gui_Draw_Button_Box(x + 18, y, 24, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(x + 44, y, 16, 16, "\04", flags);
    out_hexchar(x + 24, y + 2, val, 0, 0);
}

void Gui_Draw_Arrows_Number_Box(int x, int y, int val, int flags)
{
    Gui_Draw_Button_Box(x, y, 16, 16, "\03", flags);
    Gui_Draw_Button_Box(x + 18, y, 24, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(x + 44, y, 16, 16, "\04", flags);
    out_decchar(x + 22, y + 2, val, 0);
}

void Gui_Draw_Arrows_Number_Box2(int x, int y, int val, int flags)
{
    Gui_Draw_Button_Box(x, y, 16, 16, "\03", flags);
    Gui_Draw_Button_Box(x + 18, y, 24, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(x + 44, y, 16, 16, "\04", flags);
    out_decchar2(x + 24, y + 2, val, 0);
}

void value_box4(int x, int y, int val)
{
    Gui_Draw_Button_Box(x, y, 24, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
    out_hexchar(x + 7, y + 2, val, 0, 0);
}

void value_box3(int x, int y, char val, int flags)
{
    Gui_Draw_Button_Box(x, y, 16, 16, "\03", flags);
    Gui_Draw_Button_Box(x + 18, y, 24, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(x + 44, y, 16, 16, "\04", flags);
    out_note(x + 21, y + 2, val, 0xFFFFFF);
}

void outlong_small(int x, int y, int cant, int mode, int size, int flags)
{
    char xstr[40];

    switch(mode)
    {
        case 0: sprintf(xstr, "%d", cant); break;
        case 1: sprintf(xstr, "%d%%", cant); break;
        case 2: sprintf(xstr, "%d ms.", cant); break;
        case 3: sprintf(xstr, "%dhz", cant); break;
        case 5: sprintf(xstr, "%d K.", cant); break;
        case 6: sprintf(xstr, "%d Degr.", cant); break;
        case 10: sprintf(xstr, "S: %d", cant); break;
        case 11: sprintf(xstr, "E: %d", cant); break;
        case 12: sprintf(xstr, "L: %d", cant); break;
    }
    Gui_Draw_Button_Box(x, y, size - 1, 16, xstr, flags);
}

void outlong(int x, int y, int cant, int mode)
{
    char xstr[40];

    switch(mode)
    {
        case 0: sprintf(xstr, "%d", cant); break;
        case 1: sprintf(xstr, "%d%%", cant); break;
        case 2: sprintf(xstr, "%d ms.", cant); break;
        case 3: sprintf(xstr, "%d Hz.", cant); break;
        case 5: sprintf(xstr, "%d K.", cant); break;
        case 6: sprintf(xstr, "%d Degr.", cant); break;
        case 7:
            sprintf(xstr, "%.07d Bytes", cant);
            Gui_Draw_Button_Box(x, y, 80, 16, xstr, BUTTON_NORMAL | BUTTON_DISABLED);
            break;
        case 10: sprintf(xstr, "S: %d", cant); break;
        case 11: sprintf(xstr, "E: %d", cant); break;
        case 12: sprintf(xstr, "L: %d", cant); break;
    }
    if(mode != 7) Gui_Draw_Button_Box(x, y, 60, 16, xstr, BUTTON_NORMAL | BUTTON_DISABLED);
}

void outfloat(int x, int y, float cant, int mode)
{
    char xstr[40];

    switch (mode)
    {
        case 0: sprintf(xstr, "%.3f", cant); break;
        case 1: sprintf(xstr, "%.2f%%", cant); break;
        case 2: sprintf(xstr, "%.3f ms.", cant); break;
        case 3: sprintf(xstr, "%.3f Hz.", cant); break;
        case 5: sprintf(xstr, "%.3f K.", cant); break;
        case 8: sprintf(xstr, "%.1f Tk.", cant); break;
    }

    Gui_Draw_Button_Box(x, y, 60, 16, xstr, BUTTON_NORMAL | BUTTON_DISABLED);
}

void outfloat_small(int x, int y, float cant, int mode, int size, int flags)
{
    char xstr[40];

    switch (mode)
    {
        case 0: sprintf(xstr, "%.3f", cant); break;
        case 1: sprintf(xstr, "%.2f%%", cant); break;
        case 2: sprintf(xstr, "%.3f ms.", cant); break;
        case 3: sprintf(xstr, "%.3f Hz", cant); break;
        case 5: sprintf(xstr, "%.3f K.", cant); break;
        case 8: sprintf(xstr, "%.1f Tk", cant); break;
    }

    Gui_Draw_Button_Box(x, y, size - 1, 16, xstr, flags);
}

void Draw_Editors_Bar(int Highlight)
{
    int Highlight_Tab[] =
    {
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL
    };
    Highlight_Tab[Highlight] = BUTTON_PUSHED;

    Gui_Draw_Button_Box(396, 429, 64, 16, "Track FX", Highlight_Tab[7]);
    Gui_Draw_Button_Box(132, 429, 64, 16, "Synth", Highlight_Tab[6]);
    Gui_Draw_Button_Box(594, 429, 64, 16, "Misc. Setup", Highlight_Tab[5]);
    Gui_Draw_Button_Box(0, 429, 64, 16, "Sequencer", Highlight_Tab[4]);
    Gui_Draw_Button_Box(528, 429, 64, 16, "Disk IO", Highlight_Tab[0]);
    Gui_Draw_Button_Box(330, 429, 64, 16, "Track", Highlight_Tab[1]);
    Gui_Draw_Button_Box(66, 429, 64, 16, "Instrument", Highlight_Tab[2]);
    Gui_Draw_Button_Box(462, 429, 64, 16, "FX Setup", Highlight_Tab[3]);
    Gui_Draw_Button_Box(198, 429, 64, 16, "Sample Ed.", Highlight_Tab[8]);
    Gui_Draw_Button_Box(264, 429, 64, 16, "303", Highlight_Tab[9]);
    Gui_Draw_Button_Box(660, 429, 64, 16, "Exit", Highlight_Tab[10]);

    Asking_Exit = FALSE;
}

void Draw_Ask_Exit(void)
{
    Gui_Draw_Button_Box(660, 429, 64, 16, "Sure ?", BUTTON_PUSHED);
}

// ------------------------------------------------------
// Refresh the whole interface context
void Refresh_UI_Context(void)
{
    switch(userscreen)
    {
        case USER_SCREEN_DISKIO_EDIT:
            Draw_DiskIO_Ed();
            break;
        case USER_SCREEN_TB303_EDIT:
            Draw_303_Ed();
            break;
        case USER_SCREEN_INSTRUMENT_EDIT:
            Draw_Instrument_Ed();
            break;
        case USER_SCREEN_SEQUENCER:
            Draw_Sequencer_Ed();
            break;
        case USER_SCREEN_TRACK_EDIT:
            Draw_Track_Ed();
            break;
        case USER_SCREEN_SETUP_EDIT:
            Draw_Master_Ed();
            break;
        case USER_SCREEN_SYNTH_EDIT:
            Draw_Synth_Ed();
            break;
        case USER_SCREEN_SAMPLE_EDIT:
            Draw_Sample_Ed();
            break;
        case USER_SCREEN_FX_SETUP_EDIT:
            Draw_Fx_Ed();
            break;
        case USER_SCREEN_TRACK_FX_EDIT:
            Draw_Track_Fx_Ed();
            break;
    }
    Actualize_DiskIO_Ed(0);
    Actualize_303_Ed(0);
    seditor = 0;
    Actualize_Instrument_Ed(0, 0);
    Actualize_Sequencer();
    Display_Master_Comp();
    Display_Master_Volume();
    Display_Shuffle();
    Actualize_Master(0);
    Actualize_Master(4);
    Actualize_Seq_Ed();
    Actualize_Track_Ed(0);
    Actualize_Master_Ed(0);
    Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
    Actualize_Wave_Ed(0);
    Actualize_Fx_Ed(0);
    Actualize_Track_Fx_Ed(0);
    Actualize_Patterned();
}

// ------------------------------------------------------
// Print a text on the screen
void Print_String(char *str, int x, int y, int size_x, int flags)
{
    if(flags & BUTTON_TEXT_CENTERED)
    {
        x += (size_x - Get_Size_Text(str)) / 2;
    }
    PrintXY(x, y, USE_FONT, (char *) str);
}

// ------------------------------------------------------
// Draw a clickable box with a text
void Gui_Draw_Button_Box(int x, int y, int sx, int sy, const char *str, int pushed)
{
    int x2 = x + sx;
    int y2 = y + sy;

    if(!(pushed & BUTTON_NOBORDER))
    {
        if(pushed & BUTTON_NORMAL)
        {
            if(pushed & BUTTON_DISABLED)
            {
                SetColor(COL_STATIC_MED);
                Fillrect(x, y, x2, y2 + 1);
                SetColor(COL_STATIC_HI);
                DrawLine(x, y, x2 - 1, y);
                DrawLine(x, y, x, y2);
                SetColor(COL_STATIC_LO);
                DrawLine(x, y2, x2, y2);
                DrawLine(x2, y, x2, y2);
            }
            else
            {
                SetColor(COL_MED);
                Fillrect(x, y, x2, y2 + 1);
                SetColor(COL_HI);
                DrawLine(x, y, x2 - 1, y);
                DrawLine(x, y, x, y2);
                SetColor(COL_LO);
                DrawLine(x, y2, x2, y2);
                DrawLine(x2, y, x2, y2);
            }
        }
        else
        {
            if(pushed & BUTTON_DISABLED)
            {
                SetColor(COL_STATIC_MED);
                Fillrect(x, y, x2, y2 + 1);
                SetColor(COL_STATIC_LO);
                DrawLine(x, y, x2 - 1, y);
                DrawLine(x, y, x, y2);
                SetColor(COL_STATIC_HI);
                DrawLine(x, y2, x2, y2);
                DrawLine(x2, y, x2, y2);
            }
            else
            {
                SetColor(COL_PUSHED_MED);
                Fillrect(x, y, x2, y2 + 1);
                SetColor(COL_PUSHED_LO);
                DrawLine(x, y, x2 - 1, y);
                DrawLine(x, y, x, y2);
                SetColor(COL_PUSHED_HI);
                DrawLine(x, y2, x2, y2);
                DrawLine(x2, y, x2, y2);
            }
        }
    }
    if(pushed & BUTTON_TEXT_CENTERED)
    {
        x += ((sx + 1) - Get_Size_Text((char *) str)) / 2;
        PrintXY(x, y + 2, USE_FONT, (char *) str);
    }
    else
    {
        PrintXY(x + 4, y + 2, USE_FONT, (char *) str);
    }
}

// ------------------------------------------------------
// Draw a clear box
void Gui_Clear_Array(int x, int y, int sx, int sy)
{
    SetColor(COL_STATIC_MED);
    bjbox(x, y, sx, sy);
}

// ------------------------------------------------------
// Draw a flat background box with a text
void Gui_Draw_Flat_Box(const char *str)
{
    SetColor(COL_STATIC_MED);

    Fillrect(2, 450, 800 - 6, 576);
    PrintXY(4, 449, USE_FONT, (char *) str);
}

// ------------------------------------------------------
// Draw a rectangle
void bjbox(int x, int y, int sx, int sy)
{
    Fillrect(x, y, x + sx, y + sy);
}

// ------------------------------------------------------
// Display a status message at the bottom of the screen
void mess_box(char const *str)
{
    Gui_Draw_Button_Box(0, CONSOLE_HEIGHT - 21, fsize, 18, str, BUTTON_NORMAL | BUTTON_DISABLED);
}
      
// ------------------------------------------------------
// Display a horizontal slider
void Realslider(int x, int y, int val, int Enabled)
{
    y--;
    if(Enabled)
    {
        SetColor(COL_SLIDER_LO);
        bjbox(x + 2, y + 1, 128 + 17, 17);
        SetColor(COL_SLIDER_HI);
        bjbox(x + 3, y + 2, 128 + 16, 16);
        SetColor(COL_SLIDER_MED);
        bjbox(x + 3, y + 2, 128 + 15, 15);
    }
    else
    {
        SetColor(COL_STATIC_LO);
        bjbox(x + 2, y + 1, 128 + 17, 17);
        SetColor(COL_STATIC_HI);
        bjbox(x + 3, y + 2, 128 + 16, 16);
        SetColor(COL_STATIC_MED);
        bjbox(x + 3, y + 2, 128 + 15, 15);
    }
    if(Enabled)
    {
        Gui_Draw_Button_Box(x + 4 + val, y + 2, 13, 14, "", BUTTON_NORMAL);
    }
    else
    {
        Gui_Draw_Button_Box(x + 4 + val, y + 2, 13, 14, "", BUTTON_NORMAL | BUTTON_DISABLED);
    }
}

void Realslider_Size(int x, int y, int size, int val, int Enabled)
{
    y--;
    if(Enabled)
    {
        SetColor(COL_SLIDER_LO);
        bjbox(x + 2, y + 1, size + 17, 17);
        SetColor(COL_SLIDER_HI);
        bjbox(x + 3, y + 2, size + 16, 16);
        SetColor(COL_SLIDER_MED);
        bjbox(x + 3, y + 2, size + 15, 15);
    }
    else
    {
        SetColor(COL_STATIC_LO);
        bjbox(x + 2, y + 1, size + 17, 17);
        SetColor(COL_STATIC_HI);
        bjbox(x + 3, y + 2, size + 16, 16);
        SetColor(COL_STATIC_MED);
        bjbox(x + 3, y + 2, size + 15, 15);
    }

    if(Enabled)
    {
        Gui_Draw_Button_Box(x + 4 + val, y + 2, 13, 14, "", BUTTON_NORMAL);
    }
    else
    {
        Gui_Draw_Button_Box(x + 4 + val, y + 2, 13, 14, "", BUTTON_NORMAL | BUTTON_DISABLED);
    }
}

void Realslider2(int x, int y, int val, int Enabled)
{
    y--;
    if(Enabled)
    {
        SetColor(COL_SLIDER_LO);
        bjbox(x + 2, y + 1, 64 + 17, 17);
        SetColor(COL_SLIDER_HI);
        bjbox(x + 3, y + 2, 64 + 16, 16);
        SetColor(COL_SLIDER_MED);
        bjbox(x + 3, y + 2, 64 + 15, 15);
    }
    else
    {
        SetColor(COL_STATIC_LO);
        bjbox(x + 2, y + 1, 64 + 17, 17);
        SetColor(COL_STATIC_HI);
        bjbox(x + 3, y + 2, 64 + 16, 16);
        SetColor(COL_STATIC_MED);
        bjbox(x + 3, y + 2, 64 + 15, 15);
    }

    if(Enabled)
    {
        Gui_Draw_Button_Box(x + 4 + val, y + 2, 13, 14, "", BUTTON_NORMAL);
    }
    else
    {
        Gui_Draw_Button_Box(x + 4 + val, y + 2, 13, 14, "", BUTTON_NORMAL | BUTTON_DISABLED);
    }
}

// ------------------------------------------------------
// Get the center position of a slider
int Slider_Get_Center(unsigned int Size, unsigned int Maximum, int Pixels)
{
    // Normalize
    float ratio = (float) Maximum / (float) Size;
    int caret_size = (int) ((float) Pixels / ratio);
    if(caret_size == Pixels) caret_size--;
    return(caret_size);
}

// ------------------------------------------------------
// Calculate the caret size of a slider
float Slider_Calc_Size(int displayed, int maximum, int size)
{
    float caret_size;
    float ratio;

    if(maximum < displayed) displayed = maximum;
    ratio = (float) maximum / (float) displayed;
    caret_size = ((float) size / (float) ratio);
    return(caret_size);
}

// ------------------------------------------------------
// Calculate the caret position of a slider
float Slider_Calc_Pos(int displayed, int maximum, int size, int value)
{
    float Pos_Slider;
    float ratio;

    if(maximum < displayed) displayed = maximum;
    ratio = (float) maximum / (float) displayed;
    if(maximum == displayed) Pos_Slider = 0.0f;
    else Pos_Slider = (float) value * ((((float) size) - ((float) (size) / (float) ratio)) / ((float) maximum - (float) displayed));
    return(Pos_Slider);
}

// ------------------------------------------------------
// Display a horizontal slider
void Realslider_Horiz(int x, int y, int value, int displayed, int maximum, int size, int enable)
{
    float caret_size;
    float Pos_slider;

    size -= 2;

    Pos_slider = Slider_Calc_Pos(displayed, maximum, size, value);

    caret_size = Slider_Calc_Size(displayed, maximum, size);

    if((caret_size + Pos_slider) > (size - 1)) caret_size -= (caret_size + Pos_slider) - (size - 1);
   
    if(enable)
    {
        SetColor(COL_SLIDER_LO);
        bjbox(x, y, size + 2, 16 + 1);
        SetColor(COL_SLIDER_HI);
        bjbox(x + 1, y + 1, size + 1, 16);
        SetColor(COL_SLIDER_MED);
        bjbox(x + 1, y + 1, size, 16 - 1);

        Gui_Draw_Button_Box(x + 1 + (int) Pos_slider, y + 1, caret_size, 16 - 2, "", BUTTON_NORMAL);
    }
    else
    {
        SetColor(0);
        bjbox(x, y, size + 1, 16 + 1);
        bjbox(x + 1, y + 1, size + 1, 16);
        bjbox(x + 1, y + 1, size, 16 - 1);
    }
}

// ------------------------------------------------------
// Display a vertical slider
void Realslider_Vert(int x, int y, int value, int displayed, int maximum, int size, int enable)
{
    float caret_size;
    float Pos_slider;

    Pos_slider = Slider_Calc_Pos(displayed, maximum, size, value);

    size -= 2;

    caret_size = Slider_Calc_Size(displayed, maximum, size);

    if((caret_size + Pos_slider) > size) caret_size -= (caret_size + Pos_slider) - size;

    if(enable)
    {
        SetColor(COL_SLIDER_LO);
        bjbox(x, y, 16 + 1, size + 2);
        SetColor(COL_SLIDER_HI);
        bjbox(x + 1, y + 1, 16, size + 1);
        SetColor(COL_SLIDER_MED);
        bjbox(x + 1, y + 1, 16 - 1, size);
        Gui_Draw_Button_Box(x + 1, y + 1 + (int) Pos_slider, 16 - 2, (int) caret_size, "", BUTTON_NORMAL);
    }
    else
    {
        SetColor(0);
        bjbox(x, y, size + 1, 16 + 1);
        bjbox(x + 1, y + 1, size + 1, 16);
        bjbox(x + 1, y + 1, size, 16 - 1);
    }
}

// ------------------------------------------------------
// dest x
// dest y
// letter
// src y
// length y
void Letter(int x, int y, char ltr, int ys, int y2)
{
    switch(ltr)
    {
        case 0: Copy(PFONT, x, y, 72, ys, 79, y2); break;
        case 1: Copy(PFONT, x, y, 80, ys, 87, y2); break;
        case 2: Copy(PFONT, x, y, 88, ys, 95, y2); break;
        case 3: Copy(PFONT, x, y, 96, ys, 103, y2); break;
        case 4: Copy(PFONT, x, y, 104, ys, 111, y2); break;
        case 5: Copy(PFONT, x, y, 112, ys, 119, y2); break;
        case 6: Copy(PFONT, x, y, 120, ys, 127, y2); break;
        case 7: Copy(PFONT, x, y, 128, ys, 135, y2); break;
        case 8: Copy(PFONT, x, y, 136, ys, 143, y2); break;
        case 9: Copy(PFONT, x, y, 144, ys, 151, y2); break;
        case 10: Copy(PFONT, x, y, 0, ys, 7, y2); break; // A
        case 11: Copy(PFONT, x, y, 8, ys, 15, y2); break;// B
        case 12: Copy(PFONT, x, y, 16, ys, 23, y2); break;// C
        case 13: Copy(PFONT, x, y, 24, ys, 31, y2); break;// D
        case 14: Copy(PFONT, x, y, 32, ys, 39, y2); break;// E
        case 15: Copy(PFONT, x, y, 40, ys, 47, y2); break;// F
        case 16: Copy(PFONT, x, y, 48, ys, 55, y2); break;// G
        case 17: Copy(PFONT, x, y, 64, ys, 71, y2); break; // #
        case 18: Copy(PFONT, x, y, 176, ys, 183, y2); break; // -
        case 19: Copy(PFONT, x, y, 152, ys, 175, y2); break; // Off
        case 20: Copy(PFONT, x, y, 56, ys, 63, y2); break; // Blank
        case 21: Copy(PFONT, x, y, 184, ys, 191, y2); break; // .

        //case 22: Copy(PFONT, x, y, 0, 104, 130, 111); break; // .

        case 23: Copy(PFONT, x, y,  56, 64, 56 + 26, 64 + 6); break; // ON
        case 24: Copy(PFONT, x, y,  84, 64, 84 + 26, 64 + 6); break; // OFF
        case 25: Copy(PFONT, x, y,   0, 64,  0 + 26, 64 + 6); break; // MUTE
        case 26: Copy(PFONT, x, y,  28, 64, 28 + 26, 64 + 6); break; // PLAY

        //case 27: Copy(PFONT, x, y, 78, 64, 109, 64 + 6); break; // EDIT ON!
        //case 28: Copy(PFONT, x, y, 111, 64, 142, 64 + 6); break; // EDIT OFF!

        case 29: Copy(PFONT, x, y, 56, ys, 59, y2); break; // Blank (4 pixels)
        case 30: Copy(PFONT, x, y, 56, ys, 57, y2); break; // Blank (2 pixels)

        case 31: Copy(PFONT, x, y, 111, 64, 111 + 4, 64 + 6); break; // FX ARROW LO BACK
        case 32: Copy(PFONT, x, y, 138, 64, 138 + 4, 64 + 6); break; // FX ARROW HI BACK
        case 33: Copy(PFONT, x, y, 165, 64, 165 + 4, 64 + 6); break; // FX ARROW SEL BACK
    }
}

void blitnote(int x, int y, int note, int y1, int y2)
{
    switch(note)
    {
        case 120: Letter(x, y, 19, y1, y2); break;
        case 121:
            Letter(x, y, 18, y1, y2);
            Letter(x + 8, y, 18, y1, y2);
            Letter(x + 16, y, 18, y1, y2);
            break;

        case 0: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 1: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 2: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 3: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 4: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 5: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 6: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 7: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 8: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 9: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 10: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 11: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 0, y1, y2); break;
        case 12: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 13: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 14: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 15: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 16: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 17: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 18: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 19: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 20: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 21: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 22: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 23: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 1, y1, y2); break;
        case 24: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 25: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 26: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 27: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 28: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 29: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 30: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 31: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 32: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 33: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 34: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 35: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 2, y1, y2); break;
        case 36: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 37: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 38: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 39: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 40: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 41: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 42: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 43: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 44: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 45: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 46: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 47: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 3, y1, y2); break;
        case 48: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 49: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 50: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 51: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 52: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 53: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 54: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 55: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 56: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 57: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 58: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 59: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 4, y1, y2); break;
        case 60: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 61: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 62: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 63: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 64: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 65: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 66: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 67: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 68: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 69: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 70: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 71: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 5, y1, y2); break;
        case 72: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 73: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 74: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 75: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 76: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 77: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 78: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 79: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 80: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 81: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 82: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 83: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 6, y1, y2); break;
        case 84: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 85: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 86: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 87: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 88: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 89: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 90: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 91: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 92: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 93: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 94: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 95: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 7, y1, y2); break;
        case 96: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 97: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 98: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 99: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 100: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 101: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 102: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 103: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 104: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 105: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 106: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 107: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 8, y1, y2); break;
        case 108: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 109: Letter(x, y, 12, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 110: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 111: Letter(x, y, 13, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 112: Letter(x, y, 14, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 113: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 114: Letter(x, y, 15, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 115: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 116: Letter(x, y, 16, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 117: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 118: Letter(x, y, 10, y1, y2); Letter(x + 8, y, 17, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
        case 119: Letter(x, y, 11, y1, y2); Letter(x + 8, y, 18, y1, y2); Letter(x + 16, y, 9, y1, y2); break;
    }
}

// ------------------------------------------------------
// Display a digit
void out_nibble(int x, int y, int Font_Type, int number)
{
    switch(number)
    {
        case 0: PrintXY(x, y, Font_Type, "0"); break;
        case 1: PrintXY(x, y, Font_Type, "1"); break;
        case 2: PrintXY(x, y, Font_Type, "2"); break;
        case 3: PrintXY(x, y, Font_Type, "3"); break;
        case 4: PrintXY(x, y, Font_Type, "4"); break;
        case 5: PrintXY(x, y, Font_Type, "5"); break;
        case 6: PrintXY(x, y, Font_Type, "6"); break;
        case 7: PrintXY(x, y, Font_Type, "7"); break;
        case 8: PrintXY(x, y, Font_Type, "8"); break;
        case 9: PrintXY(x, y, Font_Type, "9"); break;
        case 10: PrintXY(x, y, Font_Type, "A"); break;
        case 11: PrintXY(x, y, Font_Type, "B"); break;
        case 12: PrintXY(x, y, Font_Type, "C"); break;
        case 13: PrintXY(x, y, Font_Type, "D"); break;
        case 14: PrintXY(x, y, Font_Type, "E"); break;
        case 15: PrintXY(x, y, Font_Type, "F"); break;
    }
}

// ------------------------------------------------------
// SDL stuff

// ------------------------------------------------------
// Primitives
void DrawLine(int x1, int y1, int x2, int y2)
{
    Draw_Line(Main_Screen, x1, y1, x2, y2, FgColor);
}

void DrawPixel(int x, int y, int Color)
{
    Draw_Pixel(Main_Screen, x, y, Color);
}

void DrawHLine(int y, int x1, int x2, int Color)
{
    Draw_HLine(Main_Screen, x1, y, x2, Color);
}

void DrawVLine(int x, int y1, int y2, int Color)
{
    Draw_VLine(Main_Screen, x, y1, y2, Color);
}

void SetColor(int color)
{
    FgColor = color;
}

void Fillrect(int x1, int y1, int x2, int y2)
{
    SDL_Rect Dst_Rect;
    Dst_Rect.x = x1;
    Dst_Rect.y = y1;
    Dst_Rect.w = x2 - x1;
    Dst_Rect.h = y2 - y1;
    SDL_FillRect(Main_Screen, &Dst_Rect, FgColor);
}

// ------------------------------------------------------
// Set the current palette
void UISetPalette(SDL_Color *Palette, int Amount)
{
    if(FONT_LOW)
    {
        SDL_SetPalette(FONT_LOW, SDL_PHYSPAL, Palette, 0, Amount);
        SDL_SetPalette(FONT_LOW, SDL_LOGPAL, Palette, 0, Amount);
    }
    if(FONT)
    {
        SDL_SetPalette(FONT, SDL_PHYSPAL, Palette, 0, Amount);
        SDL_SetPalette(FONT, SDL_LOGPAL, Palette, 0, Amount);
    }
    if(PFONT)
    {
        SDL_SetPalette(PFONT, SDL_PHYSPAL, Palette, 0, Amount);
        SDL_SetPalette(PFONT, SDL_LOGPAL, Palette, 0, Amount);
    }
    if(SKIN303)
    {
        SDL_SetPalette(SKIN303, SDL_PHYSPAL, Palette, 0, Amount);
        SDL_SetPalette(SKIN303, SDL_LOGPAL, Palette, 0, Amount);
    }
    if(LOGOPIC)
    {
        SDL_SetPalette(LOGOPIC, SDL_PHYSPAL, Palette, 0, Amount);
        SDL_SetPalette(LOGOPIC, SDL_LOGPAL, Palette, 0, Amount);
    }
    if(Temp_PFONT)
    {
        SDL_SetPalette(Temp_PFONT, SDL_PHYSPAL, Palette, 0, Amount);
        SDL_SetPalette(Temp_PFONT, SDL_LOGPAL, Palette, 0, Amount);
    }
    if(POINTER)
    {
        SDL_SetPalette(POINTER, SDL_PHYSPAL, Palette, 0, Amount);
        SDL_SetPalette(POINTER, SDL_LOGPAL, Palette, 0, Amount);
    }

    SDL_SetPalette(Main_Screen, SDL_PHYSPAL, Palette, 0, Amount);
    SDL_SetPalette(Main_Screen, SDL_LOGPAL, Palette, 0, Amount);
}

// ------------------------------------------------------
// Copy a surface onto the main screen
void Copy(SDL_Surface *Source, int x, int y, int x1, int y1, int x2, int y2)
{
    SDL_Rect Src_Rect;
    SDL_Rect Dst_Rect;

    Dst_Rect.x = x;
    Dst_Rect.y = y;
    Dst_Rect.w = x2 - x1;
    Dst_Rect.h = y2 - y1;

    Src_Rect.x = x1;
    Src_Rect.y = y1;
    Src_Rect.w = Dst_Rect.w + 1;
    Src_Rect.h = Dst_Rect.h + 1;

    SDL_BlitSurface(Source, &Src_Rect, Main_Screen, &Dst_Rect);
}

void Copy_To_Surface(SDL_Surface *Source, SDL_Surface *dest,
                     int x, int y, int x1, int y1, int x2, int y2)
{
    SDL_Rect Src_Rect;
    SDL_Rect Dst_Rect;

    Src_Rect.x = x1;
    Src_Rect.y = y1;
    Src_Rect.w = (x2 - x1);
    Src_Rect.h = (y2 - y1);

    Dst_Rect.x = x;
    Dst_Rect.y = y;
    Dst_Rect.w = (x2 - x1);
    Dst_Rect.h = (y2 - y1);

    SDL_BlitSurface(Source, &Src_Rect, dest, &Dst_Rect);
}

// ------------------------------------------------------
// Return the ordinal position of a given letter
int Get_Char_Position(char *Ascii_Letters, int Max_Letters, char Letter)
{
    int i;

    for(i = 0; i < Max_Letters; i++) {
        if(Ascii_Letters[i] == Letter) return(i);
    }
    return(Max_Letters - 1);
}

// ------------------------------------------------------
// Print a string on the screen
void PrintXY(int x, int y, int Font_Type, char *String)
{
    int Idx;
    int i;
    SDL_Rect Src_Rect;
    SDL_Rect Dst_Rect;

    y += 2;

    Dst_Rect.y = y;
    Dst_Rect.h = Font_Height;
    Src_Rect.y = 0;
    Src_Rect.h = Font_Height;

    for(i = 0; i < (int) strlen(String); i++)
    {
        Idx = Get_Char_Position(Font_Ascii, Nbr_Letters, String[i]);
        Src_Rect.x = Font_Pos[Idx];
        Src_Rect.w = Font_Size[Idx];
        Dst_Rect.x = x;
        Dst_Rect.w = Src_Rect.w;
        if(Font_Type == USE_FONT)
        {
            SDL_BlitSurface(FONT, &Src_Rect, Main_Screen, &Dst_Rect);
        }
        else
        {
            SDL_BlitSurface(FONT_LOW, &Src_Rect, Main_Screen, &Dst_Rect);
        }
        x += Font_Size[Idx];
    }
}

// ------------------------------------------------------
// Get the size of a text in pixels
int Get_Size_Text(char *String)
{
    int Idx;
    int len;
    int i;

    len = 0;
    for(i = 0; i < (int) strlen(String); i++)
    {
        Idx = Get_Char_Position(Font_Ascii, Nbr_Letters, String[i]);
        len += Font_Size[Idx];
    }
    return(len - 1);
}

// ------------------------------------------------------
// Load a .bmp picture into a SDL surface
SDL_Surface *Load_Picture(char *FileName)
{
    return(SDL_LoadBMP(FileName));
}

// ------------------------------------------------------
// Create the font datas
int Create_Font_Datas(char *FontName)
{
    FILE *hFont;
    int Data;
    int i;
    int Pos;

    Nbr_Letters = strlen(Font_Ascii);

    hFont = fopen(FontName, "r");
    if(!hFont) return(FALSE);
    Pos = 0;
    for(i = 0; i < 256; i++)
    {
        Font_Pos[i] = Pos;
        fscanf(hFont, "%d", &Data);
        Font_Size[i] = Data;
        Pos += Data;
    }
    fclose(hFont);
    return(TRUE);
}

void Set_Channel_State_Pic(int x, int color, int inv_color)
{
    int Surface_offset;
    int i;
    int j;
    unsigned char *Pix;

    while(SDL_LockSurface(Temp_PFONT) < 0);

    Pix = (unsigned char *) Temp_PFONT->pixels;

    Pix += (64 * Temp_PFONT->pitch) + x;
    for(j = 0; j < 7; j++)
    {
        for(i = 0; i < 27; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            if(Pix[Surface_offset])
            {
                Pix[Surface_offset] = color;
            }
            else
            {
                Pix[Surface_offset] = inv_color;
            }
        }
    }

    SDL_UnlockSurface(Temp_PFONT);
}

// ------------------------------------------------------
// Create the complete font set to display the patterns
void Create_Pattern_font(void)
{
    unsigned char *Pix;
    unsigned char *Pix3;
    unsigned char *Pix2;
    int Surface_offset;
    int Surface_offset_Dest;
    int i;
    int j;

    // Create the pattern font
    Copy_To_Surface(PFONT, Temp_PFONT, 0, 0, 0, 0, 192, 8);

    // Set the base colors
    while(SDL_LockSurface(Temp_PFONT) < 0);

    Pix = (unsigned char *) Temp_PFONT->pixels;
    Pix3 = Pix + (87 * Temp_PFONT->pitch);

    for(j = 0; j < 8; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            if(Pix[Surface_offset])
            {
                Pix[Surface_offset] = COL_PATTERN_LO_FORE;
                Pix3[Surface_offset] = COL_PATTERN_LO_FORE + 9;
            }
            else
            {
                Pix[Surface_offset] = COL_PATTERN_LO_BACK;
                Pix3[Surface_offset] = COL_PATTERN_LO_BACK + 9;
            }
        }
    }

    // Invert
    Pix2 = Pix;
    Pix2 += (8 * Temp_PFONT->pitch);
    Pix3 = Pix2 + (87 * Temp_PFONT->pitch);
    for(j = 0; j < 8; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            if(Pix[Surface_offset] == COL_PATTERN_LO_FORE)
            {
                Pix2[Surface_offset] = COL_PATTERN_LO_BACK;
                Pix3[Surface_offset] = COL_PATTERN_LO_BACK  + 9;
            }
            else
            {
                Pix2[Surface_offset] = COL_PATTERN_LO_FORE;
                Pix3[Surface_offset] = COL_PATTERN_LO_FORE  + 9;
            }
        }
    }

    SDL_UnlockSurface(Temp_PFONT);

    // Blank line
    Copy_To_Surface(PFONT, Temp_PFONT, 0, 16, 0, 15, 192, 1);

    while(SDL_LockSurface(Temp_PFONT) < 0);

    Pix2 = Pix;
    Pix2 += (16 * Temp_PFONT->pitch);

    for(j = 0; j < 8; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            Surface_offset_Dest = ((j * 2) * Temp_PFONT->pitch) + i;
            Pix2[Surface_offset_Dest] = Pix[Surface_offset] + 4;
            Pix2[Surface_offset_Dest + Temp_PFONT->pitch] = Pix[Surface_offset] + 4;
        }
    }

    // Invert
    Pix2 = (unsigned char *) Temp_PFONT->pixels;
    Pix2 += (32 * Temp_PFONT->pitch);
    Pix = (unsigned char *) Temp_PFONT->pixels;
    Pix += (16 * Temp_PFONT->pitch);
    for(j = 0; j < 16; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            if(Pix[Surface_offset] == COL_PATTERN_SEL_BACK)
            {
                Pix2[Surface_offset] = COL_PATTERN_SEL_FORE;
            }
            else
            {
                Pix2[Surface_offset] = COL_PATTERN_SEL_BACK;
            }
        }
    }

    // Caret font
    Pix2 = (unsigned char *) Temp_PFONT->pixels;
    Pix2 += (48 * Temp_PFONT->pitch);
    Pix = (unsigned char *) Temp_PFONT->pixels;
    Pix += (16 * Temp_PFONT->pitch);
    for(j = 0; j < 16; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            if(Pix[Surface_offset] == COL_PATTERN_SEL_BACK)
            {
                Pix2[Surface_offset] = COL_VUMETERPEAK;
            }
            else
            {
                Pix2[Surface_offset] = COL_PATTERN_LO_BACK;
            }
        }
    }

    // Highlight
    Pix = (unsigned char *) Temp_PFONT->pixels;
    Pix2 = Pix;
    Pix2 += (71 * Temp_PFONT->pitch);
    Pix3 = Pix2 + (87 * Temp_PFONT->pitch);

    for(j = 0; j < 8; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            Pix2[Surface_offset] = Pix[Surface_offset] + 2;
            Pix3[Surface_offset] = Pix[Surface_offset] + 2 + 9;
        }
    }

    // Invert
    Pix = (unsigned char *) Temp_PFONT->pixels;
    Pix += (71 * Temp_PFONT->pitch);
    Pix2 = Pix + (8 * Temp_PFONT->pitch);
    Pix3 = Pix2 + (87 * Temp_PFONT->pitch);
    for(j = 0; j < 8; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            if(Pix[Surface_offset] == COL_PATTERN_HI_FORE)
            {
                Pix2[Surface_offset] = COL_PATTERN_HI_BACK;
                Pix3[Surface_offset] = COL_PATTERN_HI_BACK + 9;
            }
            else
            {
                Pix2[Surface_offset] = COL_PATTERN_HI_FORE;
                Pix3[Surface_offset] = COL_PATTERN_HI_FORE + 9;
            }
        }
    }

    // Highlight
    Pix = (unsigned char *) Temp_PFONT->pixels;
    Pix2 = Pix;
    Pix2 += (71 * Temp_PFONT->pitch);
    Pix3 = Pix2 + (87 * Temp_PFONT->pitch);

    for(j = 0; j < 8; j++)
    {
        for(i = 0; i < Temp_PFONT->w; i++)
        {
            Surface_offset = (j * Temp_PFONT->pitch) + i;
            Pix2[Surface_offset] = Pix[Surface_offset] + 2;
            Pix3[Surface_offset] = Pix[Surface_offset] + 2 + 9;
        }
    }

    SDL_UnlockSurface(Temp_PFONT);

    Copy_To_Surface(PFONT, Temp_PFONT, 0, 64, 0, 8, 192, 7);

    // Channels status
    Set_Channel_State_Pic(0, COL_MUTE, COL_MUTE_PLAY_INVERT);
    Set_Channel_State_Pic(28, COL_PLAY, COL_MUTE_PLAY_INVERT);
    Set_Channel_State_Pic(56, COL_PLAY, COL_MUTE_PLAY_INVERT);
    Set_Channel_State_Pic(84, COL_MUTE, COL_MUTE_PLAY_INVERT);
    Set_Channel_State_Pic(111, COL_MUTE, COL_PATTERN_LO_BACK);
    Set_Channel_State_Pic(138, COL_MUTE, COL_PATTERN_HI_BACK);
    Set_Channel_State_Pic(165, COL_MUTE, COL_PATTERN_SEL_BACK);

    // We don't need it anymore
    SDL_FreeSurface(PFONT);
    PFONT = Temp_PFONT;
}

// ------------------------------------------------------
// Set the main palette with the default one
void Restore_Default_Palette(void)
{
    int i;
    for(i = 0; i < sizeof(Default_Palette) / sizeof(SDL_Color); i++)
    {
        Ptk_Palette[i].r = Default_Palette[i].r;
        Ptk_Palette[i].g = Default_Palette[i].g;
        Ptk_Palette[i].b = Default_Palette[i].b;
        Ptk_Palette[i].unused = Default_Palette[i].unused;
    }
}

// ------------------------------------------------------
// Remap the pictures colors for our usage
int Set_Pictures_Colors(void)
{
    int i;
    unsigned char *Pix;
    int max_colors_pointer = 0;

    SDL_Palette *Pic_Palette;
    int min_idx = sizeof(Default_Palette) / sizeof(SDL_Color);

    Pix = (unsigned char *) FONT->pixels;
    for(i = 0; i < FONT->w * FONT->h; i++)
    {
        if(Pix[i]) Pix[i] = COL_FONT_HI;
    }

    Pix = (unsigned char *) FONT_LOW->pixels;
    for(i = 0; i < FONT_LOW->w * FONT_LOW->h; i++)
    {
        if(Pix[i]) Pix[i] = COL_FONT_LO;
    }
   
    bare_color_idx = min_idx;

    Pix = (unsigned char *) SKIN303->pixels;
    max_colors_303 = 0;
    for(i = 0; i < SKIN303->w * SKIN303->h; i++)
    {
        if(Pix[i] > max_colors_303) max_colors_303 = Pix[i];
        Pix[i] += min_idx;
    }
    max_colors_303++;

    max_colors_logo = 0;
    Pix = (unsigned char *) LOGOPIC->pixels;
    for(i = 0; i < LOGOPIC->w * LOGOPIC->h; i++)
    {
        if(Pix[i] > max_colors_logo) max_colors_logo = Pix[i];
        Pix[i] += min_idx;
    }
    LOGO_BKCOL += min_idx;
    max_colors_logo++;

    Pic_Palette = SKIN303->format->palette;
    for(i = 0; i < max_colors_303; i++)
    {
        Palette_303[i].r = Pic_Palette->colors[i].r;
        Palette_303[i].g = Pic_Palette->colors[i].g;
        Palette_303[i].b = Pic_Palette->colors[i].b;
        Palette_303[i].unused = Pic_Palette->colors[i].unused;
    }

    Pic_Palette = LOGOPIC->format->palette;
    for(i = 0; i < max_colors_logo; i++)
    {
        Palette_Logo[i].r = Pic_Palette->colors[i].r;
        Palette_Logo[i].g = Pic_Palette->colors[i].g;
        Palette_Logo[i].b = Pic_Palette->colors[i].b;
        Palette_Logo[i].unused = Pic_Palette->colors[i].unused;
    }

    Temp_PFONT = SDL_AllocSurface(SDL_HWSURFACE, 192, 87 * 2, 8, 0, 0, 0, 0xff);

    Pointer_BackBuf = (unsigned char *) malloc(POINTER->pitch * POINTER->h * sizeof(unsigned char));
    memset(Pointer_BackBuf, 0, POINTER->pitch * POINTER->h * sizeof(unsigned char));

    Set_Logo_Palette();
    Get_Phony_Palette();
    Set_Phony_Palette();
    Ptk_Palette[0].r = 0;
    Ptk_Palette[0].g = 0;
    Ptk_Palette[0].b = 0;
    Init_UI();

    Create_Pattern_font();
    return(TRUE);
}

void Set_Main_Palette(void)
{
    int i;

Wait_Palette:
    if(SKIN303 == NULL) goto Wait_Palette;

    SDL_Palette *Pic_Palette = SKIN303->format->palette;

    for(i = 0; i < max_colors_303; i++)
    {
        Ptk_Palette[i + bare_color_idx].r = Palette_303[i].r;
        Ptk_Palette[i + bare_color_idx].g = Palette_303[i].g;
        Ptk_Palette[i + bare_color_idx].b = Palette_303[i].b;
        Ptk_Palette[i + bare_color_idx].unused = Palette_303[i].unused;
    }
}

void Set_Logo_Palette(void)
{
    int i;

    for(i = 0; i < max_colors_logo; i++)
    {
        Ptk_Palette[i + bare_color_idx].r = Palette_Logo[i].r;
        Ptk_Palette[i + bare_color_idx].g = Palette_Logo[i].g;
        Ptk_Palette[i + bare_color_idx].b = Palette_Logo[i].b;
        Ptk_Palette[i + bare_color_idx].unused = Palette_303[i].unused;
    }
}

// ------------------------------------------------------
// Display or clear the mouse pointer at given coordinates
void Display_Mouse_Pointer(int x, int y, int clear)
{
    while(SDL_LockSurface(POINTER) < 0);
    while(SDL_LockSurface(Main_Screen) < 0);

    int i;
    int j;
    int Src_offset;
    int Dst_offset;
    int Len_Dst = Main_Screen->pitch * Main_Screen->h;
    unsigned char *SrcPix = (unsigned char *) POINTER->pixels;
    unsigned char *DstPix = (unsigned char *) Main_Screen->pixels;

    for(j = 0; j < POINTER->h; j++)
    {
        for(i = 0; i < POINTER->w; i++)
        {
            Src_offset = (j * POINTER->pitch) + i;
            Dst_offset = ((j + y) * Main_Screen->pitch) + (i + x);
            if(Dst_offset >= 0)
            {
                if(((i + x) < Main_Screen->w) &&
                   ((j + y) < Main_Screen->h))
                {
                    if(clear)
                    {
                        if(SrcPix[Src_offset])
                        {
                            DstPix[Dst_offset] = Pointer_BackBuf[Src_offset];
                        }
                    } 
                    else
                    {
                        if(SrcPix[Src_offset])
                        {
                            Pointer_BackBuf[Src_offset] = DstPix[Dst_offset];
                            DstPix[Dst_offset] = SrcPix[Src_offset];
                        }
                    }
                }
            }
        }
    }
    SDL_UnlockSurface(Main_Screen);
    SDL_UnlockSurface(POINTER);
}

// ------------------------------------------------------
// Set the screen palette
void Refresh_Palette(void)
{
    UISetPalette(Ptk_Palette, 256);
}

// ------------------------------------------------------
// Init the user interface
void Init_UI(void)
{
    Refresh_Palette();
}

// ------------------------------------------------------
// Free the allocated resources
void Destroy_UI(void)
{
    if(Pointer_BackBuf) free(Pointer_BackBuf);
}
