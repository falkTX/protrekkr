// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/editor_pattern.h"
#include "include/editor_diskio.h"

// ------------------------------------------------------
// Variables
int VIEWLINE = 15; // BOOKMARK
int VIEWLINE2 = -13;
int YVIEW = 300; 
char is_editing = 0;
char is_recording = 0;
char is_recording_2 = 0;
char is_record_key = 0;

// Min 1 Max 16
int patt_highlight = 4;
int Continuous_Scroll;

MARKER Patterns_Marker;

int ped_pattad = 1;
int ped_patoct = 4;

char sr_isrecording = 0;

#define HIGH_COLOR_NOTE 0
#define HIGH_COLOR_INSTRUMENT_1 1
#define HIGH_COLOR_INSTRUMENT_2 2
#define HIGH_COLOR_VOLUME_1 3
#define HIGH_COLOR_VOLUME_2 4
#define HIGH_COLOR_PANNING_1 5
#define HIGH_COLOR_PANNING_2 6
#define HIGH_COLOR_EFFECT_1 7
#define HIGH_COLOR_EFFECT_2 8
#define HIGH_COLOR_EFFECT_3 9
#define HIGH_COLOR_EFFECT_4 10

int color_1b[11];
int color_2b[11];

char table_decimal[] =
{
    20, 20, 0,
    20, 20, 1,
    20, 20, 2,
    20, 20, 3,
    20, 20, 4,
    20, 20, 5,
    20, 20, 6,
    20, 20, 7,
    20, 20, 8,
    20, 20, 9,

    20, 1, 0,
    20, 1, 1,
    20, 1, 2,
    20, 1, 3,
    20, 1, 4,
    20, 1, 5,
    20, 1, 6,
    20, 1, 7,
    20, 1, 8,
    20, 1, 9,

    20, 2, 0,
    20, 2, 1,
    20, 2, 2,
    20, 2, 3,
    20, 2, 4,
    20, 2, 5,
    20, 2, 6,
    20, 2, 7,
    20, 2, 8,
    20, 2, 9,

    20, 3, 0,
    20, 3, 1,
    20, 3, 2,
    20, 3, 3,
    20, 3, 4,
    20, 3, 5,
    20, 3, 6,
    20, 3, 7,
    20, 3, 8,
    20, 3, 9,

    20, 4, 0,
    20, 4, 1,
    20, 4, 2,
    20, 4, 3,
    20, 4, 4,
    20, 4, 5,
    20, 4, 6,
    20, 4, 7,
    20, 4, 8,
    20, 4, 9,

    20, 5, 0,
    20, 5, 1,
    20, 5, 2,
    20, 5, 3,
    20, 5, 4,
    20, 5, 5,
    20, 5, 6,
    20, 5, 7,
    20, 5, 8,
    20, 5, 9,

    20, 6, 0,
    20, 6, 1,
    20, 6, 2,
    20, 6, 3,
    20, 6, 4,
    20, 6, 5,
    20, 6, 6,
    20, 6, 7,
    20, 6, 8,
    20, 6, 9,

    20, 7, 0,
    20, 7, 1,
    20, 7, 2,
    20, 7, 3,
    20, 7, 4,
    20, 7, 5,
    20, 7, 6,
    20, 7, 7,
    20, 7, 8,
    20, 7, 9,

    20, 8, 0,
    20, 8, 1,
    20, 8, 2,
    20, 8, 3,
    20, 8, 4,
    20, 8, 5,
    20, 8, 6,
    20, 8, 7,
    20, 8, 8,
    20, 8, 9,

    20, 9, 0,
    20, 9, 1,
    20, 9, 2,
    20, 9, 3,
    20, 9, 4,
    20, 9, 5,
    20, 9, 6,
    20, 9, 7,
    20, 9, 8,
    20, 9, 9,

    1, 0, 0,
    1, 0, 1,
    1, 0, 2,
    1, 0, 3,
    1, 0, 4,
    1, 0, 5,
    1, 0, 6,
    1, 0, 7,
    1, 0, 8,
    1, 0, 9,

    1, 1, 0,
    1, 1, 1,
    1, 1, 2,
    1, 1, 3,
    1, 1, 4,
    1, 1, 5,
    1, 1, 6,
    1, 1, 7,
    1, 1, 8,
    1, 1, 9,

    1, 2, 0,
    1, 2, 1,
    1, 2, 2,
    1, 2, 3,
    1, 2, 4,
    1, 2, 5,
    1, 2, 6,
    1, 2, 7,
    1, 2, 8,
    1, 2, 9,
};

// 28 lines on the screen
int Table_Mouse_Lines[] =
{
    0, 
    8 * 1 + 1,
    8 * 2 + 1,
    8 * 3 + 1,
    8 * 4 + 1,
    8 * 5 + 1,
    8 * 6 + 1,
    8 * 7 + 1,
    8 * 8 + 1,
    8 * 9 + 1,
    8 * 10 + 1,
    8 * 11 + 1,
    8 * 12 + 1,
    8 * 13 + 1,
    8 * 14 + 8 + 1,
    8 * 15 + 8 + 1,
    8 * 16 + 8 + 1,
    8 * 17 + 8 + 1,
    8 * 18 + 8 + 1,
    8 * 19 + 8 + 1,
    8 * 20 + 8 + 1,
    8 * 21 + 8 + 1,
    8 * 22 + 8 + 1,
    8 * 23 + 8 + 1,
    8 * 24 + 8 + 1,
    8 * 25 + 8 + 1,
    8 * 26 + 8 + 1,
    8 * 27 + 8 + 1,
    8 * 28 + 8 + 1,
};

int Table_Columns[] =
{
    0,
    PAT_COL_INSTRUMENT - PAT_COL_NOTE + PAT_COL_SHIFT,
    (PAT_COL_INSTRUMENT - PAT_COL_NOTE) + 8 + PAT_COL_SHIFT,
    PAT_COL_VOLUME - PAT_COL_NOTE + PAT_COL_SHIFT,
    (PAT_COL_VOLUME - PAT_COL_NOTE) + 8 + PAT_COL_SHIFT,
    PAT_COL_PANNING - PAT_COL_NOTE + PAT_COL_SHIFT,
    (PAT_COL_PANNING - PAT_COL_NOTE) + 8 + PAT_COL_SHIFT,
    PAT_COL_EFFECTS - PAT_COL_NOTE + PAT_COL_SHIFT,
    (PAT_COL_EFFECTS - PAT_COL_NOTE) + 8 + PAT_COL_SHIFT,
    (PAT_COL_EFFECTS - PAT_COL_NOTE) + 16 + PAT_COL_SHIFT,
    (PAT_COL_EFFECTS - PAT_COL_NOTE) + 24 + PAT_COL_SHIFT,
    (PAT_COL_EFFECTS - PAT_COL_NOTE) + 32 + PAT_COL_SHIFT,
    (PAT_COL_EFFECTS - PAT_COL_NOTE) + 40 + PAT_COL_SHIFT
};

// ------------------------------------------------------
// Functions
int Get_Nibble_Color(int row, int column, int multi, int Shadow);
int Get_Nibble_Color_Highlight(int row, int column);
void Display_Patt_Line(int In_Prev_Next, int Shadow_Pattern, int y, int rel, int track, int tvisiblecolums, int pattern);

void draw_pated(int track, int line, int petrack, int row)
{
    int rel = 0;
    int color = 0;
    int liner;

    int Cur_Position;
    int Cur_Position2;
    int Shadow_Pattern;
    int pattern;
    int In_Prev_Next;
    int In_Prev_Next2;
    int rel2;
    int i;
    int rel2_size;
    int rel2_size2;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    int tvisiblecolums = visiblecolums;
    if(tvisiblecolums > Songtracks) tvisiblecolums = Songtracks;

    SetColor(COL_PATTERN_LO_BACK);
    bjbox(0, 183, (PAT_COL_MAX * tvisiblecolums) + 16 + 4, 13);

    // Tracks headers
    for(liner = 0; liner < tvisiblecolums; liner++)
    {
        color = liner * PAT_COL_MAX;

        if(ped_track == (track + liner)) SetColor(COL_PUSHED_MED);
        else SetColor(COL_PATTERN_LO_BACK);

        bjbox(29 + color, 187, 52, 7);

        // On / off
        if(SACTIVE[Cur_Position][track + liner]) Letter(120 + color, 187, 23, 0, 0);
        else Letter(120 + color, 187, 24, 0, 0);

        if(TRACKSTATE[track + liner]) Letter(91 + color, 187, 25, 0, 0);
        else Letter(91 + color, 187, 26, 0, 0);

        Letter(82 + color, 187, liner + track, 0, 7);
    }

    int y = YVIEW + VIEWLINE2 * 8;
    Cur_Position2 = Cur_Position;
    Shadow_Pattern = 0;
    In_Prev_Next = FALSE;
    In_Prev_Next2 = FALSE;

    SetColor(COL_PATTERN_LO_BACK);
    bjbox(0, 196, (PAT_COL_MAX * tvisiblecolums) + 16 + 4, (8 * 29));

    for(liner = VIEWLINE2; liner < VIEWLINE; liner++)
    {

        rel2 = 0;
        rel = liner + line;
        In_Prev_Next = In_Prev_Next2;

        if(See_Prev_Next_Pattern)
        {
            // Can't see past the first & last positions
            if(Cur_Position2)
            {
                if(rel <= -1)
                {
                    // See previous pattern
                    rel2 = patternLines[pSequence[Cur_Position2 - 1]] + rel;
                    if(rel2 < 0)
                    {
                        rel2_size = -rel;
                        rel2_size2 = rel2_size;
                        // Find the first line to display
                        if(rel2 < 0)
                        {
                            rel2 = 0;
                            while(rel2_size2 != 0)
                            {
                                Cur_Position2--;
                                rel2_size2 -= patternLines[pSequence[Cur_Position2]];
                                if(rel2_size2 < 0)
                                {
                                    rel2 += -rel2_size2;
                                    rel2_size2 = 0;
                                }
                                if(!Cur_Position2) break;
                            }
                            y += rel2_size2 * 8;
                            liner += rel2_size2;
                            rel2_size -= rel2_size2;
                        }

                        // Display the lines before our position
                        rel = rel2;
                        In_Prev_Next = TRUE;
                        for(i = 0; i < rel2_size; i++)
                        {
                            pattern = pSequence[Cur_Position2];
                            Display_Patt_Line(In_Prev_Next, Shadow_Pattern, y, rel, track, tvisiblecolums, pattern);
                            y += 8;
                            rel++;
                            if(rel >= patternLines[pSequence[Cur_Position2]])
                            {
                                Cur_Position2++;
                                if(Cur_Position == Cur_Position2)
                                {
                                    In_Prev_Next = FALSE;
                                }
                                rel = 0;
                            }
                            liner++;
                        }
                        liner--;
                        In_Prev_Next2 = FALSE;
                        In_Prev_Next = FALSE;
                        goto Disp;
                    }
                    rel = rel2;
                    In_Prev_Next = TRUE;
                    pattern = pSequence[Cur_Position2 - 1];
                    goto Go_Display;
                } 
            }
            if(Cur_Position2 < sLength - 1)
            {
                if(rel >= patternLines[pSequence[Cur_Position2]])
                {
                    // See next pattern
                    line -= patternLines[pSequence[Cur_Position2]];
                    rel = liner + line;
                    Cur_Position2++;
                    In_Prev_Next2 = TRUE;
                    In_Prev_Next = In_Prev_Next2;
                } 
            } 
        }

        if(rel > -1 && rel < patternLines[pSequence[Cur_Position2]])
        {
            pattern = pSequence[Cur_Position2];
            // Do not show the line at the current caret location
Go_Display:
            if(liner != 0)
            {
                Display_Patt_Line(In_Prev_Next, Shadow_Pattern, y, rel, track, tvisiblecolums, pattern);
            }
            else
            {
                y += 8;
            }
        }
        y += 8;
        Disp:;
    }
}

void Display_Patt_Line(int In_Prev_Next, int Shadow_Pattern,
                       int y, int rel, int track, int tvisiblecolums, int pattern)
{
    int cur_column;
    int offset_t;
    int cur_color;
    char *ptr_table_decimal;
    int high_color;
    char multip;
    int dover;
    int synchro_fx = FALSE;
    int Fx_Color;
    int tracky;

    if(!In_Prev_Next) Shadow_Pattern = 0;
    else Shadow_Pattern = 1;

    multip = FALSE;
    if(patt_highlight > 1)
    {
        if(rel % patt_highlight == 0) multip = TRUE;
    }
    cur_column = (track * 11);

    // Browse all tracks to seek synchro markers
    for(tracky = 0; tracky < Songtracks; tracky++)
    {
        offset_t = (rel * 96 + (tracky * 6)) + pattern * 12288;
        unsigned char p_e_sync = *(RawPatterns + offset_t + 4);
        unsigned char p_eh_sync = p_e_sync & 0xf;

        if((p_e_sync >> 4) == 0) if(p_eh_sync == 0x07) synchro_fx = TRUE;
    }
    
    for(tracky = 0; tracky < tvisiblecolums; tracky++)
    {
        dover = tracky * PAT_COL_MAX;

        // Read the datas
        offset_t = (rel * 96 + ((track + tracky) * 6)) + pattern * 12288;
        unsigned char p_a = *(RawPatterns + offset_t);
        unsigned char p_b = *(RawPatterns + offset_t + 1);
        unsigned char p_bh = p_b & 0xf;
        unsigned char p_c = *(RawPatterns + offset_t + 2);
        unsigned char p_ch = p_c & 0xf;
        unsigned char p_d = *(RawPatterns + offset_t + 3);
        unsigned char p_dh = p_d & 0xf;
        unsigned char p_e = *(RawPatterns + offset_t + 4);
        unsigned char p_eh = p_e & 0xf;
        unsigned char p_f = *(RawPatterns + offset_t + 5);
        unsigned char p_fh = p_f & 0xf;


        // Note
        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        Letter(PAT_COL_NOTE - 8 + PAT_COL_SHIFT + dover, y, 20, cur_color, cur_color + 7);
        Letter(PAT_COL_NOTE - 10 + PAT_COL_SHIFT + dover, y, 30, cur_color, cur_color + 7);
        blitnote(PAT_COL_NOTE + PAT_COL_SHIFT + dover, y, p_a, cur_color, cur_color + 7);

        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        Letter(PAT_COL_INSTRUMENT - 4 + PAT_COL_SHIFT + dover, y, 29, cur_color, cur_color + 7);
        // Instrument
        if(p_b != 255)
        {
            Letter(PAT_COL_INSTRUMENT + PAT_COL_SHIFT + dover, y, p_b >> 4, cur_color, cur_color + 7);
            cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
            Letter(PAT_COL_INSTRUMENT + 8 + PAT_COL_SHIFT + dover, y, p_bh, cur_color, cur_color + 7);
        }
        else
        {
            Letter(PAT_COL_INSTRUMENT + PAT_COL_SHIFT + dover, y, 21, cur_color, cur_color + 7);
            cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
            Letter(PAT_COL_INSTRUMENT + 8 + PAT_COL_SHIFT + dover, y, 21, cur_color, cur_color + 7);
        }

        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        Letter(PAT_COL_VOLUME - 4 + PAT_COL_SHIFT + dover, y, 29, cur_color, cur_color + 7);
        // Volume
        if(p_c != 255)
        {
            Letter(PAT_COL_VOLUME + PAT_COL_SHIFT + dover, y, p_c >> 4, cur_color, cur_color + 7);
            cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
            Letter(PAT_COL_VOLUME + 8 + PAT_COL_SHIFT + dover, y, p_ch, cur_color, cur_color + 7);
        }
        else
        {
            Letter(PAT_COL_VOLUME + PAT_COL_SHIFT + dover, y, 21, cur_color, cur_color + 7);
            cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
            Letter(PAT_COL_VOLUME + 8 + PAT_COL_SHIFT + dover, y, 21, cur_color, cur_color + 7);
        }

        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        Letter(PAT_COL_PANNING - 4 + PAT_COL_SHIFT + dover, y, 29, cur_color, cur_color + 7);
        // Panning
        if(p_d != 255)
        {
            Letter(PAT_COL_PANNING + PAT_COL_SHIFT + dover, y, p_d >> 4, cur_color, cur_color + 7);
            cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
            Letter(PAT_COL_PANNING + 8 + PAT_COL_SHIFT + dover, y, p_dh, cur_color, cur_color + 7);
        }
        else
        {
            Letter(PAT_COL_PANNING + PAT_COL_SHIFT + dover, y, 21, cur_color, cur_color + 7);
            cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
            Letter(PAT_COL_PANNING + 8 + PAT_COL_SHIFT + dover, y, 21, cur_color, cur_color + 7);
        }

        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        // Effect
        Letter(PAT_COL_EFFECTS - 4 + PAT_COL_SHIFT + dover, y, 29, cur_color, cur_color + 7);
        Letter(PAT_COL_EFFECTS + PAT_COL_SHIFT + dover, y, p_e >> 4, cur_color, cur_color + 7);
        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        Letter(PAT_COL_EFFECTS + 8 + PAT_COL_SHIFT + dover, y, p_eh, cur_color, cur_color + 7);
        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        
        Letter(PAT_COL_EFFECTS + 16 + PAT_COL_SHIFT + dover, y, p_f >> 4, cur_color, cur_color + 7);
        cur_color = Get_Nibble_Color(rel, cur_column++, multip, Shadow_Pattern);
        Letter(PAT_COL_EFFECTS + 24 + PAT_COL_SHIFT + dover, y, p_fh, cur_color, cur_color + 7);

    } // Track

    // Display the rows numbers
    if(Shadow_Pattern)
    {
        if(multip)
        {
            high_color = 71 + 87;
            Fx_Color = 32;
        }
        else
        {
            high_color = 0 + 87;
            Fx_Color = 31;
        }
    }
    else
    {
        if(multip)
        {
            high_color = 71;
            Fx_Color = 32;
        }
        else
        {
            high_color = 0;
            Fx_Color = 31;
        }
    }

    ptr_table_decimal = table_decimal + (rel * 3);
    if(Rows_Decimal)
    {
        Letter(0, y, ptr_table_decimal[0], high_color, high_color + 7);
        Letter(8, y, ptr_table_decimal[1], high_color, high_color + 7);
        Letter(16, y, ptr_table_decimal[2], high_color, high_color + 7);
    }
    else
    {
        Letter(0, y, 20, high_color, high_color + 7);
        Letter(8, y, rel >> 4, high_color, high_color + 7);
        Letter(16, y, rel & 0xf, high_color, high_color + 7);
    }
    if(synchro_fx)
    {
        Letter(24, y, Fx_Color, 0, 0);
    }
}

// Draw the current pattern line
void draw_pated_highlight(int track, int line, int petrack, int row)
{
    int offset_t;
    int dover = 0;
    int cur_column;
    int cur_color;
    char *ptr_table_decimal;
    int pattern;
    int Cur_Position;
    int synchro_fx = FALSE;
    int tracky;

    int tvisiblecolums = visiblecolums;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    pattern = pSequence[Cur_Position];

    if(RawPatterns)
    {
        if(tvisiblecolums > Songtracks) tvisiblecolums = Songtracks;

        // Browse all tracks to seek synchro markers
        for(tracky = 0; tracky < Songtracks; tracky++)
        {
            offset_t = (line * 96 + (tracky * 6)) + pattern * 12288;
            unsigned char p_e_sync = *(RawPatterns + offset_t + 4);
            unsigned char p_eh_sync = p_e_sync & 0xf;

            if((p_e_sync >> 4) == 0) if(p_eh_sync == 0x07) synchro_fx = TRUE;
        }

        SetColor(COL_PATTERN_LO_BACK);
        bjbox(0, YVIEW, (PAT_COL_MAX * tvisiblecolums) + 16 + 4, 16);

        cur_column = (track * 11);
        for(tracky = 0; tracky < tvisiblecolums; tracky++)
        {
            dover = tracky * PAT_COL_MAX;

            char tri = track + tracky;

            offset_t = line * 96 + tri * 6 + pattern * 12288;

            unsigned char p_a = *(RawPatterns + offset_t);
            unsigned char p_b = *(RawPatterns + offset_t + 1);
            unsigned char p_bh = p_b & 0xf;
            unsigned char p_c = *(RawPatterns + offset_t + 2);
            unsigned char p_ch = p_c & 0xf;
            unsigned char p_d = *(RawPatterns + offset_t + 3);
            unsigned char p_dh = p_d & 0xf;
            unsigned char p_e = *(RawPatterns + offset_t + 4);
            unsigned char p_eh = p_e & 0xf;
            unsigned char p_f = *(RawPatterns + offset_t + 5);
            unsigned char p_fh = p_f & 0xf;

            // Note
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);
            Letter(PAT_COL_NOTE - 8 + PAT_COL_SHIFT + dover, YVIEW, 20, cur_color, cur_color + 15);
            Letter(PAT_COL_NOTE - 10 + PAT_COL_SHIFT + dover, YVIEW, 30, cur_color, cur_color + 15);
            if(row == 0 && tri == petrack) blitnote(PAT_COL_NOTE + PAT_COL_SHIFT + dover, YVIEW, p_a, 48, 48 + 15);
            else blitnote(PAT_COL_NOTE + PAT_COL_SHIFT + dover, YVIEW, p_a, cur_color, cur_color + 15);

            // Instrument
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);
            Letter(PAT_COL_INSTRUMENT - 4 + PAT_COL_SHIFT + dover, YVIEW, 29, cur_color, cur_color + 15);
            if(row == 1 && tri == petrack)
            {
                if(p_b != 255) Letter(PAT_COL_INSTRUMENT + PAT_COL_SHIFT + dover, YVIEW, p_b >> 4, 48, 48 + 15);
                else Letter(PAT_COL_INSTRUMENT + PAT_COL_SHIFT + dover, YVIEW, 21, 48, 48 + 15);
            }
            else
            {
                if(p_b != 255) Letter(PAT_COL_INSTRUMENT + PAT_COL_SHIFT + dover, YVIEW, p_b >> 4, cur_color, cur_color + 15);
                else Letter(PAT_COL_INSTRUMENT + PAT_COL_SHIFT + dover, YVIEW, 21, cur_color, cur_color + 15);
            }
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            if(row == 2 && tri == petrack)
            {
                if(p_b != 255) Letter(PAT_COL_INSTRUMENT + 8 + PAT_COL_SHIFT + dover, YVIEW, p_bh, 48, 48 + 15);
                else Letter(PAT_COL_INSTRUMENT + 8 + PAT_COL_SHIFT + dover, YVIEW, 21, 48, 48 + 15);
            }
            else
            {
                if(p_b != 255) Letter(PAT_COL_INSTRUMENT + 8 + PAT_COL_SHIFT + dover, YVIEW, p_bh, cur_color, cur_color + 15);
                else Letter(PAT_COL_INSTRUMENT + 8 + PAT_COL_SHIFT + dover, YVIEW, 21, cur_color, cur_color + 15);
            }
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            // Volume
            Letter(PAT_COL_VOLUME - 4 + PAT_COL_SHIFT + dover, YVIEW, 29, cur_color, cur_color + 15);
            if(row == 3 && tri == petrack)
            {
                if(p_c != 255) Letter(PAT_COL_VOLUME + PAT_COL_SHIFT + dover, YVIEW, p_c >> 4, 48, 48 + 15);
                else Letter(PAT_COL_VOLUME + PAT_COL_SHIFT + dover, YVIEW, 21, 48, 48 + 15);
            }
            else
            {
                if(p_c != 255) Letter(PAT_COL_VOLUME + PAT_COL_SHIFT + dover, YVIEW, p_c >> 4, cur_color, cur_color + 15);
                else Letter(PAT_COL_VOLUME + PAT_COL_SHIFT + dover, YVIEW, 21, cur_color, cur_color + 15);
            }
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            if(row == 4 && tri == petrack)
            {
                if(p_c != 255) Letter(PAT_COL_VOLUME + 8 + PAT_COL_SHIFT + dover, YVIEW, p_ch, 48, 48 + 15);
                else Letter(PAT_COL_VOLUME + 8 + PAT_COL_SHIFT + dover, YVIEW, 21, 48, 48 + 15);
            }
            else
            {
                if(p_c != 255) Letter(PAT_COL_VOLUME + 8 + PAT_COL_SHIFT + dover, YVIEW, p_ch, cur_color, cur_color + 15);
                else Letter(PAT_COL_VOLUME + 8 + PAT_COL_SHIFT + dover, YVIEW, 21, cur_color, cur_color + 15);
            }
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            // Panning
            Letter(PAT_COL_PANNING - 4 + PAT_COL_SHIFT + dover, YVIEW, 29, cur_color, cur_color + 15);
            if(row == 5 && tri == petrack)
            {
                if(p_d != 255) Letter(PAT_COL_PANNING + PAT_COL_SHIFT + dover, YVIEW, p_d >> 4, 48, 48 + 15);
                else Letter(PAT_COL_PANNING + PAT_COL_SHIFT + dover, YVIEW, 21, 48, 48 + 15);
            }
            else
            {
                if(p_d != 255) Letter(PAT_COL_PANNING + PAT_COL_SHIFT + dover, YVIEW, p_d >> 4, cur_color, cur_color + 15);
                else Letter(PAT_COL_PANNING + PAT_COL_SHIFT + dover, YVIEW, 21, cur_color, cur_color + 15);
            }
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            if(row == 6 && tri == petrack)
            {
                if(p_d != 255) Letter(PAT_COL_PANNING + 8 + PAT_COL_SHIFT + dover, YVIEW, p_dh, 48, 48 + 15);
                else Letter(PAT_COL_PANNING + 8 + PAT_COL_SHIFT + dover, YVIEW, 21, 48, 48 + 15);
            }
            else
            {
                if(p_d != 255) Letter(PAT_COL_PANNING + 8 + PAT_COL_SHIFT + dover, YVIEW, p_dh, cur_color, cur_color + 15);
                else Letter(PAT_COL_PANNING + 8 + PAT_COL_SHIFT + dover, YVIEW, 21, cur_color, cur_color + 15);
            }
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            //  Effects
            Letter(PAT_COL_EFFECTS - 4 + PAT_COL_SHIFT + dover, YVIEW, 29, cur_color, cur_color + 15);
            if(row == 7 && tri == petrack) Letter(PAT_COL_EFFECTS + PAT_COL_SHIFT + dover, YVIEW, p_e >> 4, 48, 48 + 15);
            else Letter(PAT_COL_EFFECTS + PAT_COL_SHIFT + dover, YVIEW, p_e >> 4, cur_color, cur_color + 15);
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            if(row == 8 && tri == petrack) Letter(PAT_COL_EFFECTS + 8 + PAT_COL_SHIFT + dover, YVIEW, p_eh, 48, 48 + 15);
            else Letter(PAT_COL_EFFECTS + 8 + PAT_COL_SHIFT + dover, YVIEW, p_eh, cur_color, cur_color + 15);
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);
            if((p_e >> 4) == 0) if(p_eh == 0x07) synchro_fx = TRUE;

            if(row == 9 && tri == petrack) Letter(PAT_COL_EFFECTS + 16 + PAT_COL_SHIFT + dover, YVIEW, p_f >> 4, 48, 48 + 15);
            else Letter(PAT_COL_EFFECTS + 16 + PAT_COL_SHIFT + dover, YVIEW, p_f >> 4, cur_color, cur_color + 15);
            cur_color = Get_Nibble_Color_Highlight(line, cur_column++);

            if(row == 10 && tri == petrack) Letter(PAT_COL_EFFECTS + 24 + PAT_COL_SHIFT + dover, YVIEW, p_fh, 48, 48 + 15);
            else Letter(PAT_COL_EFFECTS + 24 + PAT_COL_SHIFT + dover, YVIEW, p_fh, cur_color, cur_color + 15);

            if(sr_isrecording)
            {
                if(liveparam > 0 && ped_track == tri && Songplaying)
                {
                    if(livevalue < 0) livevalue = 0;
                    if(livevalue > 255) livevalue = 255;

                    switch(liveparam)
                    {
                        case LIVE_PARAM_TRACK_CUTOFF:
                            *(RawPatterns + offset_t + 4) = 8;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_RESONANCE:
                            *(RawPatterns + offset_t + 4) = 0x14;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_REVERB_SEND:
                            *(RawPatterns + offset_t + 4) = 0x11;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_THRESHOLD:
                            *(RawPatterns + offset_t + 4) = 0x12;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_CLAMP:
                            *(RawPatterns + offset_t + 4) = 0x13;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_PANNING:
                            if(livevalue > 128) livevalue = 0x80;
                            *(RawPatterns + offset_t + 3) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_CUTOFF:
                            *(RawPatterns + offset_t + 4) = 0x33;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_CUTOFF:
                            *(RawPatterns + offset_t + 4) = 0x34;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_RESONANCE:
                            *(RawPatterns + offset_t + 4) = 0x35;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_RESONANCE:
                            *(RawPatterns + offset_t + 4) = 0x36;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_ENVMOD:
                            *(RawPatterns + offset_t + 4) = 0x37;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_ENVMOD:
                            *(RawPatterns + offset_t + 4) = 0x38;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_DECAY:
                            *(RawPatterns + offset_t + 4) = 0x39;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_DECAY:
                            *(RawPatterns + offset_t + 4) = 0x3a;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_TUNE:
                            *(RawPatterns + offset_t + 4) = 0x3d;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_TUNE:
                            *(RawPatterns + offset_t + 4) = 0x3e;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_ACCENT:
                            *(RawPatterns + offset_t + 4) = 0x3b;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_ACCENT:
                            *(RawPatterns + offset_t + 4) = 0x3c;
                            *(RawPatterns + offset_t + 5) = livevalue;
                            break;
                    } // Close switch

                    liveparam = 0;
                } // Close liveparam updated
            } // Close is recording
        }

        // Row number
        ptr_table_decimal = table_decimal + (line * 3);
        if(Rows_Decimal)
        {
            Letter(0, YVIEW, ptr_table_decimal[0], 16, 16 + 15);
            Letter(8, YVIEW, ptr_table_decimal[1], 16, 16 + 15);
            Letter(16, YVIEW, ptr_table_decimal[2], 16, 16 + 15);
        }
        else
        {
            Letter(0, YVIEW, 20, 16, 16 + 15);
            Letter(8, YVIEW, line >> 4, 16, 16 + 15);
            Letter(16, YVIEW, line & 0xf, 16, 16 + 15);
        }
        if(synchro_fx)
        {
            Letter(24, YVIEW + 4, 33, 0, 0);
        }
    }
}

void Actupated(int modac)
{
    int nlines;

    if(is_editing > 1)
    {
        is_editing = 0;
        StartEdit();
    }

    if(Songplaying)
    {
        nlines = patternLines[pSequence[cPosition_delay]];
        if(ped_line_delay < 0) ped_line_delay += nlines;
        if(ped_line_delay >= nlines) ped_line_delay -= nlines;
        if(ped_line_delay >= nlines) ped_line_delay = nlines - 1;
    }
    else
    {
        nlines = patternLines[pSequence[cPosition]];

        if(cPosition != 0 || cPosition < (sLength - 1))
        {
            if(Continuous_Scroll)
            {
                if(ped_line < 0)
                {
                    nlines = patternLines[pSequence[cPosition - 1]];
                    ped_line = nlines + ped_line;
                    Unselect_Selection();
                    gui_action = GUI_CMD_PREVIOUS_POSITION;
                    return;
                }
                else
                {
                    if(ped_line >= nlines)
                    {
                        ped_line -= nlines;
                        Unselect_Selection();
                        gui_action = GUI_CMD_NEXT_POSITION;
                        return;
                    }
                }
            }
            else
            {
                if(ped_line < 0) ped_line += nlines;
                if(ped_line >= nlines) ped_line -= nlines;
                if(ped_line >= nlines) ped_line = nlines - 1;
            }
        }
        else
        {
            if(ped_line < 0) ped_line += nlines;
            if(ped_line >= nlines) ped_line -= nlines;
            if(ped_line >= nlines) ped_line = nlines - 1;
        }
    }

    if(ped_row > 10)
    {
        ped_row = 0;
        ped_track++;
    }
    if(ped_row < 0)
    {
        ped_row = 10;
        ped_track--;
    }
    if(ped_track > Songtracks - 1)
    {
        ped_track = 0;
        gui_track = 0;
        Set_Track_Slider(gui_track);
        modac = 0;
    }
    if(ped_track < 0)
    {
        ped_track = Songtracks - 1;
        gui_track = Songtracks - (visiblecolums);
        if(gui_track < 0) gui_track = 0;
        Set_Track_Slider(gui_track);
        modac = 0;
    }
    if(ped_track >= gui_track + visiblecolums)
    {
        gui_track += (ped_track + 1) - (gui_track + visiblecolums);
        Set_Track_Slider(gui_track);
        modac = 0;
    }
    if(ped_track < gui_track)
    {
        gui_track -= gui_track - ped_track;
        Set_Track_Slider(gui_track);
        modac = 0;
    }

    if(Songplaying) draw_pated(gui_track, ped_line_delay, ped_track, ped_row);
    else draw_pated(gui_track, ped_line, ped_track, ped_row);

    if(Songplaying) draw_pated_highlight(gui_track, ped_line_delay, ped_track, ped_row);
    else draw_pated_highlight(gui_track, ped_line, ped_track, ped_row);
}

void Actualize_Patterned(void)
{
    if(ped_pattad < 0) ped_pattad = 16;
    if(ped_pattad > 16) ped_pattad = 0;
    if(ped_patoct < 0) ped_patoct = 0;
    if(ped_patoct > 8) ped_patoct = 8;
    if(ped_patsam < 0) ped_patsam = 127;
    if(ped_patsam > 127) ped_patsam = 0;

    char tcp[30];
    sprintf(tcp, "%s_", nameins[ped_patsam]);

    if(snamesel == 2) Gui_Draw_Button_Box(90, 134, 166, 16, tcp, BUTTON_PUSHED);
    else Gui_Draw_Button_Box(90, 134, 166, 16, nameins[ped_patsam], BUTTON_NORMAL);

    Gui_Draw_Arrows_Number_Box2(90, 152, ped_pattad, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    value_box(258, 152, ped_patoct, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    value_box(258, 134, ped_patsam, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Set_Track_Slider(gui_track);
}

int AllocPattern(void)
{
    for(int api = 0; api < 128; api++)
    {
        patternLines[api] = 64;
    }

    nPatterns = 1;

    if((RawPatterns = (unsigned char *) malloc(PBLEN)) != NULL)
    { // 6144 bytes per pattern
        for(int inicial = 0; inicial < PBLEN; inicial += 6)
        {
            *(RawPatterns + inicial) = 121;     //121
            *(RawPatterns + inicial + 1) = 255; //255
            *(RawPatterns + inicial + 2) = 255; //255
            *(RawPatterns + inicial + 3) = 255; //255
            *(RawPatterns + inicial + 4) = 0;   //0
            *(RawPatterns + inicial + 5) = 0;   //0
        }
        return 1; // Allocated
    }
    else
    {
        return 0; // Not allocated
    }
} // End of alloc pattern

// Return the correct color of a character (select / highlighted or not)
int Get_Nibble_Color(int row, int column, int multi, int Shadow)
{
    int color = (Shadow * 87);
    
    if(!Shadow)
    {
        if(column >= block_start_track &&
           column <= block_end_track &&
           row >= block_start && row <= block_end)
        {
            color = 8;
        }
    }
    if(multi) color += 71;
    return(color);
}

int Get_Nibble_Color_Highlight(int row, int column)
{
    int color = 0;
    if(column >= block_start_track &&
       column <= block_end_track &&
       row >= block_start && row <= block_end)
    {
        color = 16;
    }
    return(color + 16);
}

int Get_Track_Over_Mouse(void)
{
    int mouse_track = gui_track + ((Mouse.x - PAT_COL_NOTE) / PAT_COL_MAX);
    if(mouse_track > Songtracks - 1) mouse_track = Songtracks - 1;
    if(mouse_track < 0) mouse_track = 0;
    return(mouse_track);
}

int Get_Column_Over_Mouse(void)
{
    int i;
    int max_tr = 6;
    if(Songtracks - 1 < 6) max_tr = Songtracks - 1;

    int mouse_track = ((Mouse.x - PAT_COL_NOTE) / PAT_COL_MAX);

    if(mouse_track > max_tr) return(10);
    if(mouse_track < 0) return(0);

    int mouse_column = ((Mouse.x - PAT_COL_NOTE) % PAT_COL_MAX);
    for(i = 0; i < (sizeof(Table_Columns) / sizeof(int)) - 1; i++)
    {
        if(mouse_column >= Table_Columns[i] && mouse_column < Table_Columns[i + 1])
        {
            if(i > 10) i = 10;
            return(i);
        }
    }
    if(Mouse.x < PAT_COL_NOTE) return(0);
    return(10);
}

int Get_Line_Over_Mouse(void)
{
    int i;
    int Cur_Position = cPosition;
    if(Songplaying) Cur_Position = cPosition_delay;

    int mouse_line = (Mouse.y - 194) - 1;
    for(i = 0; i < (sizeof(Table_Mouse_Lines) / sizeof(int)) - 1; i++)
    {
        if(mouse_line >= Table_Mouse_Lines[i] &&
           mouse_line < Table_Mouse_Lines[i + 1])
        {
           mouse_line = i - 13 + ped_line;
            break;   
        }
    }
    if(mouse_line > patternLines[pSequence[Cur_Position]] - 1) mouse_line = patternLines[pSequence[Cur_Position]] - 1;
    if(mouse_line < 0) mouse_line = 0;
    return(mouse_line);
}

void Set_Track_Slider(int pos)
{
    Realslider3(726, 429, pos, 6, Songtracks, 72, TRUE);

    if(ped_track >= gui_track + visiblecolums - 1)
    {
        ped_track = gui_track + visiblecolums - 1;
    }
    if(ped_track < gui_track)
    {
        ped_track = gui_track;
    }
}

// Make sure the position isn't beyond current pattern lines range
void Bound_Patt_Pos(void)
{
    if(ped_line >= patternLines[pSequence[cPosition]])
    {
        ped_line = patternLines[pSequence[cPosition]] - 1;
    }
}
