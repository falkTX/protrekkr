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
#include "include/editor_pattern.h"
#include "include/editor_diskio.h"
#include "include/patterns_blocks.h"
#include "../support/include/timer.h"
#include "include/editor_instrument.h"
#include "include/editor_sequencer.h"

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
    8 * 29 + 8 + 1,
    8 * 30 + 8 + 1,
    8 * 31 + 8 + 1,
    8 * 32 + 8 + 1,
    8 * 33 + 8 + 1,
    8 * 34 + 8 + 1,
    8 * 35 + 8 + 1,
    8 * 36 + 8 + 1,
    8 * 37 + 8 + 1,
    8 * 38 + 8 + 1,
    8 * 39 + 8 + 1,
    8 * 40 + 8 + 1,
    8 * 41 + 8 + 1,
    8 * 42 + 8 + 1,
    8 * 43 + 8 + 1,
    8 * 44 + 8 + 1
};

int Cur_Char_size[MAX_TRACKS];

LETTER_FUNCTION Cur_Char_Function[MAX_TRACKS];
NOTE_FUNCTION Cur_Note_Function[MAX_TRACKS];

PtkTimer Pattern_Timer_Horiz;
int Pattern_Scrolling_Horiz;
float Pattern_First_Delay_Horiz;
float Pattern_Delay_Horiz;

PtkTimer Pattern_Timer_Vert;
int Pattern_Scrolling_Vert;
float Pattern_First_Delay_Vert;
float Pattern_Delay_Vert;

char Patterns_Lines = DISPLAYED_LINES_SMALL;
int Patterns_Lines_Offset = 0;

// ------------------------------------------------------
// Functions
int Get_Nibble_Color(int row, int column, int multi, int Shadow);
int Get_Nibble_Color_Highlight(int row, int column);
void Display_Patt_Line(int In_Prev_Next, int Shadow_Pattern, int y, int rel, int track, int tVisible_Columns, int pattern);
void Solo_Track(int track_to_solo);

// ------------------------------------------------------
// Draw a pattern on screen
void draw_pated(int track, int line, int petrack, int row)
{
    int rel = 0;
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
    int dover;
    int cur_track;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    int tVisible_Columns = Visible_Columns;

    int max_size = Get_Visible_Tracks_Size();
    if(max_size < MAX_PATT_SCREEN_X)
    {
        if((gui_track + tVisible_Columns) < Songtracks)
        {
            tVisible_Columns++;
        }
    }

    // Clear headers line
    SetColor(COL_BACKGROUND);
    bjbox(0, 183, CONSOLE_WIDTH, 13);

    dover = PAT_COL_NOTE;

    // Tracks headers
    for(liner = 0; liner < tVisible_Columns; liner++)
    {
        cur_track = track + liner;

        if(ped_track == cur_track) SetColor(COL_PUSHED_MED);
        else SetColor(COL_BACKGROUND);

        dover += 2;
        if(dover >= MAX_PATT_SCREEN_X) break;

        dover += 4;
        if(dover >= MAX_PATT_SCREEN_X) break;

        Cur_Char_Function[cur_track].Fnc(dover - 1, 187, cur_track, 71, 71 + 6);

        // Mute on/off
        if((dover + (Cur_Char_size[cur_track])) >= MAX_PATT_SCREEN_X) break;
        if(CHAN_MUTE_STATE[cur_track]) Cur_Char_Function[cur_track].Fnc(dover + Cur_Char_size[cur_track], 187, 25, 0, 0);
        else Cur_Char_Function[cur_track].Fnc(dover + Cur_Char_size[cur_track], 187, 26, 0, 0);

        // Active on/off
        if((dover + Cur_Char_size[cur_track] + 29) >= MAX_PATT_SCREEN_X) break;
        if(CHAN_ACTIVE_STATE[Cur_Position][cur_track]) Cur_Char_Function[cur_track].Fnc(dover + Cur_Char_size[cur_track] + 29, 187, 23, 0, 0);
        else Cur_Char_Function[cur_track].Fnc(dover + Cur_Char_size[cur_track] + 29, 187, 24, 0, 0);

        // Zoom on/off
        if((dover + Cur_Char_size[cur_track] + 29 + 29) >= MAX_PATT_SCREEN_X) break;
        if(Get_Track_Type(cur_track) != TRACK_NORMAL) Cur_Char_Function[cur_track].Fnc(dover + Cur_Char_size[cur_track] + 29 + 29, 187, 27, 0, 0);
        else Cur_Char_Function[cur_track].Fnc(dover + Cur_Char_size[cur_track] + 29 + 29, 187, 28, 0, 0);

        // Caret track marker
        if((dover + Cur_Char_size[cur_track] + 29 + 29 + 17) >= MAX_PATT_SCREEN_X) break;
        if(ped_track == cur_track) SetColor(COL_VUMETERPEAK);
        else SetColor(COL_BACKGROUND);
        bjbox(dover + Cur_Char_size[cur_track] + 29 + 29 + 17, 187, 12, 7);

        // On / off
        for(i = 0; i < Channels_MultiNotes[cur_track]; i++)
        {
            dover += Cur_Char_size[cur_track] * 3;
            if(dover >= MAX_PATT_SCREEN_X) break;

            dover += PAT_COL_SHIFT - 2;
            if(dover >= MAX_PATT_SCREEN_X) break;

            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
            dover += 2;
            if(dover >= MAX_PATT_SCREEN_X) break;
        }
        if(dover >= MAX_PATT_SCREEN_X) break;

        dover += PAT_COL_SHIFT;
        if(dover >= MAX_PATT_SCREEN_X) break;
        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;
        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;

        dover += PAT_COL_SHIFT;
        if(dover >= MAX_PATT_SCREEN_X) break;
        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;
        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;

        dover += PAT_COL_SHIFT;
        if(dover >= MAX_PATT_SCREEN_X) break;
       
        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;

        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;

        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;

        dover += Cur_Char_size[cur_track];
        if(dover >= MAX_PATT_SCREEN_X) break;
    }

    int y = YVIEW + (VIEWLINE2 * 8);
    Cur_Position2 = Cur_Position;
    Shadow_Pattern = 0;
    In_Prev_Next = FALSE;
    In_Prev_Next2 = FALSE;

    SetColor(COL_PATTERN_LO_BACK);
    bjbox(0,
          196,
          CONSOLE_WIDTH ,
          (8 * (Patterns_Lines + 1) + (Patterns_Lines == DISPLAYED_LINES_LARGE ? 4 : 0))
         );

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
                            Display_Patt_Line(In_Prev_Next, Shadow_Pattern, y, rel, track, tVisible_Columns, pattern);
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
                Display_Patt_Line(In_Prev_Next, Shadow_Pattern, y, rel, track, tVisible_Columns, pattern);
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

// ------------------------------------------------------
// Draw a pattern row
void Display_Patt_Line(int In_Prev_Next, int Shadow_Pattern,
                       int y, int rel, int track, int tVisible_Columns, int pattern)
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
    int cur_track;
    int i;

    if(!In_Prev_Next) Shadow_Pattern = 0;
    else Shadow_Pattern = 1;

    multip = FALSE;
    if(patt_highlight > 1)
    {
        if(rel % patt_highlight == 0) multip = TRUE;
    }

    cur_column = Get_Track_Nibble_Start(Channels_MultiNotes, track) + track;

    // Browse all tracks to seek synchro markers
    for(tracky = 0; tracky < Songtracks; tracky++)
    {
        offset_t = Get_Pattern_Offset(pattern, tracky, rel);
        unsigned char p_e_sync = *(RawPatterns + offset_t + PATTERN_FX);
        unsigned char p_eh_sync = p_e_sync & 0xf;

        if((p_e_sync >> 4) == 0) if(p_eh_sync == 0x07) synchro_fx = TRUE;
    }
    
    dover = PAT_COL_NOTE;

    for(tracky = 0; tracky < tVisible_Columns; tracky++)
    {
        cur_track = track + tracky;

        // Read the datas
        offset_t = Get_Pattern_Offset(pattern, cur_track, rel);

        unsigned char p_a;
        unsigned char p_b;
        unsigned char p_bh;
        
        unsigned char p_c = *(RawPatterns + offset_t + PATTERN_VOLUME);
        unsigned char p_ch = p_c & 0xf;
        unsigned char p_d = *(RawPatterns + offset_t + PATTERN_PANNING);
        unsigned char p_dh = p_d & 0xf;
        unsigned char p_e = *(RawPatterns + offset_t + PATTERN_FX);
        unsigned char p_eh = p_e & 0xf;
        unsigned char p_f = *(RawPatterns + offset_t + PATTERN_FXDATA);
        unsigned char p_fh = p_f & 0xf;

        // Row
        cur_color = Get_Nibble_Color(rel, cur_column, multip, Shadow_Pattern);
        Letter(dover, y, 20, cur_color, cur_color + 7);
        dover += 2;
        if(dover >= MAX_PATT_SCREEN_X) break;
        Letter(dover, y, 30, cur_color, cur_color + 7);
        dover += 4;
        if(dover >= MAX_PATT_SCREEN_X) break;

        // Notes/Instruments
        for(i = 0; i < Channels_MultiNotes[cur_track]; i++)
        {
            p_a = *(RawPatterns + offset_t + PATTERN_NOTE1 + (i * 2));
            p_b = *(RawPatterns + offset_t + PATTERN_INSTR1 + (i * 2));
            p_bh = p_b & 0xf;
     
            Cur_Note_Function[cur_track].Fnc(dover, y, p_a, cur_color, cur_color + 7);

            dover += Cur_Char_size[cur_track] * 3;
            if(dover >= MAX_PATT_SCREEN_X) break;

            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, 29, cur_color, cur_color + 7);
            dover += PAT_COL_SHIFT - 2;
            if(dover >= MAX_PATT_SCREEN_X) break;
            // Instrument
            if(p_b != 255)
            {
                Cur_Char_Function[cur_track].Fnc(dover, y, p_b >> 4, cur_color, cur_color + 7);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
                Cur_Char_Function[cur_track].Fnc(dover, y, p_bh, cur_color, cur_color + 7);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            else
            {
                Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
                Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, 29, cur_color, cur_color + 7);
            dover += 2;
        }
        if(dover >= MAX_PATT_SCREEN_X) break;

        Cur_Char_Function[cur_track].Fnc(dover, y, 29, cur_color, cur_color + 7);
        dover += PAT_COL_SHIFT;
        if(dover >= MAX_PATT_SCREEN_X) break;
        // Volume
        if(p_c != 255)
        {
            Cur_Char_Function[cur_track].Fnc(dover, y, p_c >> 4, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, p_ch, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        }
        else
        {
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        }

        cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
        Cur_Char_Function[cur_track].Fnc(dover, y, 29, cur_color, cur_color + 7);
        dover += PAT_COL_SHIFT;
        if(dover >= MAX_PATT_SCREEN_X) break;
        // Panning
        if(p_d != 255)
        {
            Cur_Char_Function[cur_track].Fnc(dover, y, p_d >> 4, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, p_dh, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        }
        else
        {
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        }

        cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
        // Effect
        Cur_Char_Function[cur_track].Fnc(dover, y, 29, cur_color, cur_color + 7);
        dover += PAT_COL_SHIFT;
        if(dover >= MAX_PATT_SCREEN_X) break;

        if(!p_e && !p_f)
        {
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, 21, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        }
        else
        {
            Cur_Char_Function[cur_track].Fnc(dover, y, p_e >> 4, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, p_eh, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, p_f >> 4, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        
            cur_color = Get_Nibble_Color(rel, ++cur_column, multip, Shadow_Pattern);
            Cur_Char_Function[cur_track].Fnc(dover, y, p_fh, cur_color, cur_color + 7);
            dover += Cur_Char_size[cur_track];
            if(dover >= MAX_PATT_SCREEN_X) break;
        }


        cur_column++;
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

    // Display the row index
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

    // Display the synchro marker
    if(synchro_fx)
    {
        Letter(24, y, Fx_Color, 0, 0);
    }
}

// ------------------------------------------------------
// Draw the caret pattern row
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
    int high_col;
    int i;

    int tVisible_Columns = Visible_Columns;

    int max_size = Get_Visible_Tracks_Size();
    if(max_size < MAX_PATT_SCREEN_X)
    {
        if((gui_track + tVisible_Columns) < Songtracks)
        {
            tVisible_Columns++;
        }
    }

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    pattern = pSequence[Cur_Position];

    if(RawPatterns)
    {
        if(tVisible_Columns > Songtracks) tVisible_Columns = Songtracks;

        // Browse all tracks to seek synchro markers
        for(tracky = 0; tracky < Songtracks; tracky++)
        {
            offset_t = Get_Pattern_Offset(pattern, tracky, line);
            unsigned char p_e_sync = *(RawPatterns + offset_t + PATTERN_FX);
            unsigned char p_eh_sync = p_e_sync & 0xf;

            if((p_e_sync >> 4) == 0) if(p_eh_sync == 0x07) synchro_fx = TRUE;
        }

        SetColor(COL_PATTERN_LO_BACK);
        bjbox(0, YVIEW, CHANNELS_WIDTH + 2, 16);

        dover = PAT_COL_NOTE;

        cur_column = Get_Track_Nibble_Start(Channels_MultiNotes, track) + track;

        for(tracky = 0; tracky < tVisible_Columns; tracky++)
        {
            int cur_track = track + tracky;

            offset_t = Get_Pattern_Offset(pattern, cur_track, line);

            unsigned char p_a;
            unsigned char p_b;
            unsigned char p_bh;

            unsigned char p_c = *(RawPatterns + offset_t + PATTERN_VOLUME);
            unsigned char p_ch = p_c & 0xf;
            unsigned char p_d = *(RawPatterns + offset_t + PATTERN_PANNING);
            unsigned char p_dh = p_d & 0xf;
            unsigned char p_e = *(RawPatterns + offset_t + PATTERN_FX);
            unsigned char p_eh = p_e & 0xf;
            unsigned char p_f = *(RawPatterns + offset_t + PATTERN_FXDATA);
            unsigned char p_fh = p_f & 0xf;

            // Row
            cur_color = Get_Nibble_Color_Highlight(line, cur_column);
            Letter(dover, YVIEW, 20, cur_color, cur_color + 15);
            dover += 2;
            if(dover >= MAX_PATT_SCREEN_X) break;
            Letter(dover, YVIEW, 30, cur_color, cur_color + 15);
            dover += 4;
            if(dover >= MAX_PATT_SCREEN_X) break;

            high_col = 0;

            // Notes/Instruments
            for(i = 0; i < Channels_MultiNotes[cur_track]; i++)
            {
                p_a = *(RawPatterns + offset_t + PATTERN_NOTE1 + (i * 2));
                p_b = *(RawPatterns + offset_t + PATTERN_INSTR1 + (i * 2));
                p_bh = p_b & 0xf;

                // Note
                if(row == high_col && cur_track == petrack) Cur_Note_Function[cur_track].Fnc(dover, YVIEW, p_a, 48, 48 + 15);
                else Cur_Note_Function[cur_track].Fnc(dover, YVIEW, p_a, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track] * 3;
                high_col++;
                if(dover >= MAX_PATT_SCREEN_X) break;

                // Instrument hi
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                // (gap)
                Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 29, cur_color, cur_color + 15);
                dover += PAT_COL_SHIFT - 2;
                if(dover >= MAX_PATT_SCREEN_X) break;
                if(row == high_col && cur_track == petrack)
                {
                    if(p_b != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_b >> 4, 48, 48 + 15);
                    else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                    dover += Cur_Char_size[cur_track];
                    if(dover >= MAX_PATT_SCREEN_X) break;
                }
                else
                {
                    if(p_b != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_b >> 4, cur_color, cur_color + 15);
                    else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                    dover += Cur_Char_size[cur_track];
                    if(dover >= MAX_PATT_SCREEN_X) break;
                }
                high_col++;

                // Instrument lo
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                if(row == high_col && cur_track == petrack)
                {
                    if(p_b != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_bh, 48, 48 + 15);
                    else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                    dover += Cur_Char_size[cur_track];
                    if(dover >= MAX_PATT_SCREEN_X) break;
                }
                else
                {
                    if(p_b != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_bh, cur_color, cur_color + 15);
                    else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                    dover += Cur_Char_size[cur_track];
                    if(dover >= MAX_PATT_SCREEN_X) break;
                }

                // (gap)
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 29, cur_color, cur_color + 15);
                dover += 2;

                high_col++;
            }
            if(dover >= MAX_PATT_SCREEN_X) break;

            // Volume hi
            Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 29, cur_color, cur_color + 15);
            dover += PAT_COL_SHIFT;
            if(dover >= MAX_PATT_SCREEN_X) break;
            if(row == high_col && cur_track == petrack)
            {
                if(p_c != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_c >> 4, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            else
            {
                if(p_c != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_c >> 4, cur_color, cur_color + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            high_col++;

            // Volume lo
            cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
            if(row == high_col && cur_track == petrack)
            {
                if(p_c != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_ch, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            else
            {
                if(p_c != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_ch, cur_color, cur_color + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            high_col++;

            // Panning hi
            cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
            Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 29, cur_color, cur_color + 15);
            dover += PAT_COL_SHIFT;
            if(dover >= MAX_PATT_SCREEN_X) break;
            if(row == high_col && cur_track == petrack)
            {
                if(p_d != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_d >> 4, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            else
            {
                if(p_d != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_d >> 4, cur_color, cur_color + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            high_col++;

            // Panning lo
            cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
            if(row == high_col && cur_track == petrack)
            {
                if(p_d != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_dh, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            else
            {
                if(p_d != 255) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_dh, cur_color, cur_color + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
            }
            high_col++;

            //  Effect hi
            cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
            Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 29, cur_color, cur_color + 15);
            dover += PAT_COL_SHIFT;
            if(dover >= MAX_PATT_SCREEN_X) break;

            if(!p_e && !p_f)
            {
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;

                //  Effect lo
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;

                //  Effect data hi
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                if((p_e >> 4) == 0) if(p_eh == 0x07) synchro_fx = TRUE;
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;

                //  Effect data lo
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, 21, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;
            }
            else
            {
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_e >> 4, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_e >> 4, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;

                //  Effect lo
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_eh, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_eh, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;

                //  Effect data hi
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                if((p_e >> 4) == 0) if(p_eh == 0x07) synchro_fx = TRUE;
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_f >> 4, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_f >> 4, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;

                //  Effect data lo
                cur_color = Get_Nibble_Color_Highlight(line, ++cur_column);
                if(row == high_col && cur_track == petrack) Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_fh, 48, 48 + 15);
                else Cur_Char_Function[cur_track].Fnc(dover, YVIEW, p_fh, cur_color, cur_color + 15);
                dover += Cur_Char_size[cur_track];
                if(dover >= MAX_PATT_SCREEN_X) break;
                high_col++;
            }

            ++cur_column;

            if(sr_isrecording)
            {
                if(liveparam > 0 && ped_track == cur_track && Songplaying)
                {
                    if(livevalue < 0) livevalue = 0;
                    if(livevalue > 255) livevalue = 255;

                    switch(liveparam)
                    {
                        case LIVE_PARAM_TRACK_CUTOFF:
                            *(RawPatterns + offset_t + PATTERN_FX) = 8;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_RESONANCE:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x14;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_REVERB_SEND:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x11;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_THRESHOLD:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x12;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_CLAMP:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x13;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_TRACK_PANNING:
                            if(livevalue > 0x80) livevalue = 0x80;
                            *(RawPatterns + offset_t + PATTERN_PANNING) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_CUTOFF:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x33;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_CUTOFF:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x34;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_RESONANCE:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x35;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_RESONANCE:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x36;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_ENVMOD:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x37;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_ENVMOD:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x38;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_DECAY:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x39;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_DECAY:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x3a;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_TUNE:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x3d;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_TUNE:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x3e;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_1_ACCENT:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x3b;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
                            break;
                        case LIVE_PARAM_303_2_ACCENT:
                            *(RawPatterns + offset_t + PATTERN_FX) = 0x3c;
                            *(RawPatterns + offset_t + PATTERN_FXDATA) = livevalue;
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

        // Synchro marker
        if(synchro_fx)
        {
            Letter(24, YVIEW + 4, 33, 0, 0);
        }
    }
}

// ------------------------------------------------------
// Update the displayed pattern data
void Actupated(int modac)
{
    int nlines;
    int Cur_Position;
    int cur_line;
    // Buffers blocks
    int High[] =
    {
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL
    };

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

    int max_channel_dat = Get_Max_Nibble_Track(Channels_MultiNotes, ped_track) - 1;
    if(ped_col > max_channel_dat)
    {
        ped_col = 0;
        ped_track++;
    }
    if(ped_col < 0)
    {
        ped_track--;
        if(ped_track < 0)
        {
            max_channel_dat = Get_Max_Nibble_Track(Channels_MultiNotes, Songtracks - 1) - 1;
        }
        else
        {
            max_channel_dat = Get_Max_Nibble_Track(Channels_MultiNotes, ped_track) - 1;
        }
        ped_col = max_channel_dat;
    }
    if(ped_track > Songtracks - 1)
    {
        ped_track = 0;
        gui_track = 0;
    }
    Visible_Columns = Get_Visible_Complete_Tracks();

    // Keep the caret in focus
    if(ped_track < 0)
    {
        ped_track = Songtracks - 1;
        gui_track = Songtracks - (Visible_Columns);
        if(gui_track < 0) gui_track = 0;
    }

    if(!modac)
    {
        // Right
        if(ped_track >= gui_track + Visible_Columns)
        {
            gui_track = ped_track - (Visible_Columns - 1);
        }

        // Left
        if(ped_track < gui_track)
        {
            gui_track -= gui_track - ped_track;
        }
    }
    else
    {
        // Right
        if(ped_track >= gui_track + Visible_Columns)
        {
            ped_track = gui_track + (Visible_Columns - 1);
        }
        // Left
        if(ped_track < gui_track)
        {
            ped_track = gui_track;
        }
    }

    // ----
    Set_Track_Slider(gui_track);

    // We need to check if the column where ped_track is can't be displayed
    // after the correction
    if(!modac)
    {
        if(ped_track >= gui_track + Visible_Columns)
        {
            gui_track = ped_track - (Visible_Columns - 1);
            Set_Track_Slider(gui_track);
        }
    }

    if(Songplaying)
    {
        cur_line = ped_line_delay;
        Cur_Position = cPosition_delay;
    }
    else
    {
        cur_line = ped_line;
        Cur_Position = cPosition;
    }

    draw_pated(gui_track, cur_line, ped_track, ped_col);
    draw_pated_highlight(gui_track, cur_line, ped_track, ped_col);

    Realslider_Vert(781, 212, cur_line, Patterns_Lines, patternLines[pSequence[Cur_Position]] + Patterns_Lines, 136 + Patterns_Lines_Offset, TRUE);
    Gui_Draw_Button_Box(781, 196, 16, 14, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(781, 349 + Patterns_Lines_Offset, 16, 14, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    
    High[Curr_Buff_Block] = BUTTON_PUSHED;

    Gui_Draw_Button_Box(781, 349 + (16 * 1) + Patterns_Lines_Offset, 16, 14, "1",
                        High[0] | BUTTON_TEXT_CENTERED | (Buff_Full[0] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(781, 349 + (16 * 2) + Patterns_Lines_Offset, 16, 14, "2",
                        High[1] | BUTTON_TEXT_CENTERED | (Buff_Full[1] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(781, 349 + (16 * 3) + Patterns_Lines_Offset, 16, 14, "3",
                        High[2] | BUTTON_TEXT_CENTERED | (Buff_Full[2] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(781, 349 + (16 * 4) + Patterns_Lines_Offset, 16, 14, "4",
                        High[3] | BUTTON_TEXT_CENTERED | (Buff_Full[3] ? 0 : BUTTON_LOW_FONT));
}

// ------------------------------------------------------
// Actually the extra data associated with the pattern
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

    if(snamesel == INPUT_INSTRUMENT_NAME) Gui_Draw_Button_Box(90, 134, 166, 16, tcp, BUTTON_PUSHED);
    else Gui_Draw_Button_Box(90, 134, 166, 16, nameins[ped_patsam], BUTTON_NORMAL);

    Gui_Draw_Arrows_Number_Box2(90, 152, ped_pattad, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    value_box(258, 152, ped_patoct, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    value_box(258, 134, ped_patsam, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Actualize_Instruments_Synths_List(2);
}

// ------------------------------------------------------
// Return the correct color of a character (select / highlighted or not)
int Get_Nibble_Color(int row, int column, int multi, int Shadow)
{
    int color = (Shadow * 87);
    
    if(!Shadow)
    {
        if(column >= block_start_track[Curr_Buff_Block] &&
           column <= block_end_track[Curr_Buff_Block] &&
           row >= block_start[Curr_Buff_Block] && row <= block_end[Curr_Buff_Block])
        {
            color = 8;
        }
    }
    if(multi) color += 71;
    return(color);
}

// ------------------------------------------------------
// Return the color of a character in the caret row
int Get_Nibble_Color_Highlight(int row, int column)
{
    int color = 0;
    if(column >= block_start_track[Curr_Buff_Block] &&
       column <= block_end_track[Curr_Buff_Block] &&
       row >= block_start[Curr_Buff_Block] && row <= block_end[Curr_Buff_Block])
    {
        color = 16;
    }
    return(color + 16);
}

// ------------------------------------------------------
// Return the size in pixel of a given track
int Get_Track_Size(int Track)
{
    int i;
    int dover = 0;
    int old_dover = 0;

    // Gap
    old_dover = dover;
    dover += 2;
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    old_dover = dover;
    dover += 4;
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    // On / off
    for(i = 0; i < Channels_MultiNotes[Track]; i++)
    {
        // Note
        old_dover = dover;
        dover += Cur_Char_size[Track] * 3;
        if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

        // Gap
        old_dover = dover;
        dover += PAT_COL_SHIFT - 2;
        if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

        // Instrument
        old_dover = dover;
        dover += Cur_Char_size[Track];
        if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

        old_dover = dover;
        dover += Cur_Char_size[Track];
        if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

        // Gap
        old_dover = dover;
        dover += 2;
        if(dover >= MAX_PATT_SCREEN_X) return(old_dover);
    }

    // Gap
    old_dover = dover;
    dover += PAT_COL_SHIFT;
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    // Volume
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    // Gap
    old_dover = dover;
    dover += PAT_COL_SHIFT;
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    // Panning
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    // Gap
    old_dover = dover;
    dover += PAT_COL_SHIFT;
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    // Effect
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);
    old_dover = dover;
    dover += Cur_Char_size[Track];
    if(dover >= MAX_PATT_SCREEN_X) return(old_dover);

    return(dover);
}

// ------------------------------------------------------
// Return the size in pixel of all visible tracks
int Get_Visible_Tracks_Size(void)
{
    int i;
    int size = 0;

    for(i = gui_track; i < (gui_track + Visible_Columns); i++)
    {
        size += Get_Track_Size(i);
    }
    return(size);
}

// ------------------------------------------------------
// Return the index of the tracker located under the mouse pointer
int Get_Track_Over_Mouse(void)
{
    int i;
    int bound_left = 0;
    int bound_right = 0;
    int under_mouse = 0;
    int mouse_coord = Mouse.x;
    int found_track;
    int track_size;

    if(mouse_coord > CHANNELS_WIDTH)
    {
        under_mouse = (gui_track + 1) + Visible_Columns - 1;

        // Right scrolling
        // It will scroll
        if(under_mouse >= (gui_track + Visible_Columns - 1))
        {
            if(Pattern_Scrolling_Horiz)
            {
                Pattern_Delay_Horiz += Pattern_Timer_Horiz.Get_Frames_Delay();
                if(Pattern_Delay_Horiz < Pattern_First_Delay_Horiz)
                {
                    // Wait before scrolling
                    under_mouse--;
                }
                else
                {
                    // Scroll it
                    Pattern_Delay_Horiz = 0;
                    Pattern_First_Delay_Horiz = 200.0f;
                    if(under_mouse > (Songtracks - 1)) under_mouse = Songtracks - 1;
                }
            }
            else
            {
                Pattern_Timer_Horiz.Set_Frames_Counter();
                Pattern_Scrolling_Horiz = TRUE;
                Pattern_Delay_Horiz = 0;
                Pattern_First_Delay_Horiz = 200.0f;
                under_mouse--;
            }
        }
        else
        {
            Pattern_Delay_Horiz = 0;
            Pattern_Scrolling_Horiz = FALSE;
        }
    }
    else
    {
        mouse_coord -= PAT_COL_NOTE;
        found_track = FALSE;
        int max_size = Get_Visible_Tracks_Size();    
        int tVisible_Columns = Visible_Columns;
        if(max_size < MAX_PATT_SCREEN_X)
        {
            if((gui_track + tVisible_Columns) < Songtracks)
            {
                tVisible_Columns++;
            }
        }
        for(i = gui_track; i < (gui_track + tVisible_Columns); i++)
        {
            track_size = Get_Track_Size(i);
            bound_right += track_size;
            if(mouse_coord >= bound_left && mouse_coord < bound_right)
            {
                under_mouse = i;
                found_track = TRUE;
                break;
            }
            bound_left = bound_right;
        }
        if(found_track == FALSE && mouse_coord >= 0)
        {
            under_mouse = Songtracks - 1;
        }
    }

    // Left scrolling
    if(under_mouse < gui_track)
    {
        if(Pattern_Scrolling_Horiz)
        {
            Pattern_Delay_Horiz += Pattern_Timer_Horiz.Get_Frames_Delay();
            if(Pattern_Delay_Horiz < Pattern_First_Delay_Horiz)
            {
                // Wait before scrolling
                under_mouse = gui_track;
            }
            else
            {
                under_mouse = gui_track - 1;
                // Scroll it
                Pattern_Delay_Horiz = 0;
                Pattern_First_Delay_Horiz = 150.0f;
            }
        }
        else
        {
            Pattern_Timer_Horiz.Set_Frames_Counter();
            Pattern_Scrolling_Horiz = TRUE;
            Pattern_Delay_Horiz = 0;
            Pattern_First_Delay_Horiz = 150.0f;
            under_mouse = gui_track;
        }
    }

    if(under_mouse > Songtracks - 1) under_mouse = Songtracks - 1;
    if(under_mouse < 0) under_mouse = 0;
    return(under_mouse);
}

// ------------------------------------------------------
// Return the index of a track column according to a coordinate
int Get_Column_Idx(int track, int mouse_coord)
{
    int ret_value = 0;
    int old_dover = 0;
    int dover = 0;
    int i;
    
    if(mouse_coord < 0) return(0);

    // Gap
    old_dover = dover;
    dover += 2;
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);
    
    old_dover = dover;
    dover += 4;
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    // On / off
    for(i = 0; i < Channels_MultiNotes[track]; i++)
    {
        // Note
        old_dover = dover;
        dover += Cur_Char_size[track] * 3;
        if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

        ret_value++;
        // Gap
        old_dover = dover;
        dover += PAT_COL_SHIFT - 2;
        if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

        // Instrument
        old_dover = dover;
        dover += Cur_Char_size[track];
        if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

        ret_value++;
        old_dover = dover;
        dover += Cur_Char_size[track];
        if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

        ret_value++;
        // Gap
        old_dover = dover;
        dover += 2;
        if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);
    }

    // Gap
    old_dover = dover;
    dover += PAT_COL_SHIFT;
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    // Volume
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);
    
    ret_value++;
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    // Gap
    ret_value++;
    old_dover = dover;
    dover += PAT_COL_SHIFT;
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    // Panning
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    ret_value++;
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    // Gap
    ret_value++;
    old_dover = dover;
    dover += PAT_COL_SHIFT;
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    // Effect
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    ret_value++;
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    ret_value++;
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    ret_value++;
    old_dover = dover;
    dover += Cur_Char_size[track];
    if(mouse_coord >= old_dover && mouse_coord < dover) return(ret_value);

    return(ret_value);
}

// ------------------------------------------------------
// Return the index of the column located under the mouse pointer
int Get_Column_Over_Mouse(void)
{
    int i;
    int max_tr = 6;
    int dover = 0;
    int old_dover = 0;
    int track_size;

    int track = Get_Track_Over_Mouse();
    int mouse_coord = Mouse.x - PAT_COL_NOTE;
    int column = 0;

    for(i = track - 1; i >= gui_track; i--)
    {
        track_size = Get_Track_Size(i);
        mouse_coord -= track_size;
    }
    column = Get_Column_Idx(track, mouse_coord);
    return(column);
}

// ------------------------------------------------------
// return the index of the row located under the mouse pointer
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
            mouse_line = i + VIEWLINE2 + ped_line;
            break;   
        }
    }
    if(mouse_line > patternLines[pSequence[Cur_Position]] - 1) mouse_line = patternLines[pSequence[Cur_Position]] - 1;
    if(mouse_line < 0) mouse_line = 0;
    return(mouse_line);
}

// ------------------------------------------------------
// Return the number of tracks completly displayed on screen
int Get_Visible_Complete_Tracks(void)
{
    int pixel_visible_tracks = PAT_COL_NOTE;
    int channel_size;
    int i;
    int nbr_tracks = 0;

    for(i = gui_track; i < Songtracks; i++)
    {
        channel_size = Get_Track_Size(i);
        if((pixel_visible_tracks + channel_size - 1) > CHANNELS_WIDTH)
        {
            break;
        }
        nbr_tracks++;
        pixel_visible_tracks += channel_size;
    }
    return(nbr_tracks);
}

// ------------------------------------------------------
// Return the number of tracks partially displayed on screen
int Get_Visible_Partial_Tracks(void)
{
    int tracks = Visible_Columns;
    int max_size = Get_Visible_Tracks_Size();    
    if(max_size < MAX_PATT_SCREEN_X)
    {
        if((gui_track + tracks) < Songtracks)
        {
            tracks++;
        }
    }
    return(tracks);
}

// ------------------------------------------------------
// Make sure the position isn't beyond current pattern lines range
void Bound_Patt_Pos(void)
{
    if(ped_line >= patternLines[pSequence[cPosition]])
    {
        ped_line = patternLines[pSequence[cPosition]] - 1;
    }
}

// ------------------------------------------------------
// Reset the mouse scrolling timing
void Reset_Pattern_Scrolling_Horiz(void)
{
    Pattern_Delay_Horiz = 0;
    Pattern_Scrolling_Horiz = FALSE;
    Pattern_Delay_Vert = 0;
    Pattern_Scrolling_Vert = FALSE;
}

// ------------------------------------------------------
// Handle the mouse wheel event
void Mouse_Wheel_Pattern_Ed(int roll_amount)
{
    int Cur_Position;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    // Scroll the patterns
    if(zcheckMouse_nobutton(0, 182, CONSOLE_WIDTH, 246 + Patterns_Lines_Offset) == 1)
    {
        ped_line += roll_amount;
        if(Continuous_Scroll && !Cur_Position) if(ped_line < 0) ped_line = 0;
        if(Continuous_Scroll && (Cur_Position == sLength - 1))
        {
            if(ped_line >= patternLines[pSequence[Cur_Position]])
            {
                ped_line = patternLines[pSequence[Cur_Position]] - 1;
            }
        }
        Actupated(0);
    }
}

// ------------------------------------------------------
// Handle the sliders event (right mouse button)
void Mouse_Sliders_Right_Pattern_Ed(void)
{
    int sched_line;

    // Position the caret on the specified track/column with the mouse
    if(zcheckMouse(1, 194, CHANNELS_WIDTH, 234 + Patterns_Lines_Offset))
    {
        ped_track = Get_Track_Over_Mouse();
        ped_col = Get_Column_Over_Mouse();
        Actupated(0);
        gui_action = GUI_CMD_SET_FOCUS_TRACK;
    }

    // Go to the row selected with the mouse
    if(!Songplaying)
    {
        if(zcheckMouse(1, 194, CHANNELS_WIDTH, 234 + Patterns_Lines_Offset))
        {
            if(!is_recording)
            {
                sched_line = Get_Line_Over_Mouse();

                if(Pattern_Scrolling_Vert)
                {
                    Pattern_Delay_Vert += Pattern_Timer_Vert.Get_Frames_Delay();
                    if(Pattern_Delay_Vert >= Pattern_First_Delay_Vert)
                    {
                        // Scroll it
                        Pattern_Delay_Vert = 0;
                        Pattern_First_Delay_Vert = 250.0f;
                        ped_line = sched_line;
                    }
                }
                else
                {
                    Pattern_Timer_Vert.Set_Frames_Counter();
                    Pattern_Scrolling_Vert = TRUE;
                    Pattern_Delay_Vert = 0;
                    Pattern_First_Delay_Vert = 0.0f;
                }
            }
            Actupated(0);
        }
    }
}

// ------------------------------------------------------
// Set the layout of the tracks slider and bound the caret
void Set_Track_Slider(int pos)
{
    Visible_Columns = Get_Visible_Complete_Tracks();
    if(ped_track >= pos + Visible_Columns) ped_track = pos + Visible_Columns;
    if(ped_track < pos) ped_track = pos;
    float fpos = (float) pos;

    if(Songtracks - Visible_Columns)
    {
        fpos /= Songtracks - Visible_Columns;
        fpos *= 74 - 16;
    }
    else
    {
        fpos = 0.0f;
    }
    Realslider_Horiz(724, 429 + Patterns_Lines_Offset,
                     (int) fpos, 16, 74, 74, TRUE);
}

// ------------------------------------------------------
// Handle the sliders event (left mouse button)
void Mouse_Sliders_Pattern_Ed(void)
{
    // Current track slider
    if(zcheckMouse(726, 429 + Patterns_Lines_Offset,
                   72, 16))
    {
        float Pos_Mouse = (float) (Mouse.x - 726 - 4);
        if(Pos_Mouse < 0) Pos_Mouse = 0;
        // Normalize and scale
        Pos_Mouse = (Pos_Mouse / (72.0f - 16.0f));
        if(Pos_Mouse > 1.0f) Pos_Mouse = 1.0f;
        Visible_Columns = Get_Visible_Complete_Tracks();

        Pos_Mouse = Pos_Mouse * (Songtracks - Visible_Columns);
        gui_track = (int) Pos_Mouse;
        Actupated(1);
    }

    // Rows slider
    if(zcheckMouse(781, 212, 16 + 1, 136 + Patterns_Lines_Offset) & !Songplaying)
    {
        int final_row;
        int Cur_Position = Get_Current_Position();
        int max_length = patternLines[pSequence[Cur_Position]] + Patterns_Lines;
        int Center = Slider_Get_Center(Patterns_Lines, max_length, 136 + Patterns_Lines_Offset);
        float Pos_Mouse = ((float) ((Mouse.y - 212) - (Center / 2))) / (136.0f + (float) Patterns_Lines_Offset);
        if(Pos_Mouse > 1.0f) Pos_Mouse = 1.0f;
        float s_offset = (Pos_Mouse * max_length);
        if(s_offset > (float) (max_length - Patterns_Lines))
        {
            s_offset = (float) (max_length - Patterns_Lines);
        }
        final_row = (int32) s_offset;
        if(final_row < 0) final_row = 0;
        if(final_row > patternLines[pSequence[Cur_Position]] - 1) final_row = patternLines[pSequence[Cur_Position]] - 1;
        ped_line = final_row;
        Actupated(0);
    }

    // End of the marking stuff
    if(zcheckMouse(1, 194, CHANNELS_WIDTH, 234 + Patterns_Lines_Offset) && !Songplaying)
    {
        Mark_Block_End(Get_Column_Over_Mouse(), Get_Track_Over_Mouse(), Get_Line_Over_Mouse(), 3);
    }
}

// ------------------------------------------------------
// Handle the left mouse button event
void Mouse_Left_Pattern_Ed(void)
{
    int i;
    int start_mute_check_x;
    int tracks;

    // Start of the marking block
    if(zcheckMouse(1, 194, CHANNELS_WIDTH, 234 + Patterns_Lines_Offset) && !Songplaying)
    {
        Mark_Block_Start(Get_Column_Over_Mouse(), Get_Track_Over_Mouse(), Get_Line_Over_Mouse());
    }

    // Next/Prev rows buttons
    if(zcheckMouse(781, 196, 16 + 1, 14) & !Songplaying)
    {
        Goto_Previous_Row();
    }

    if(zcheckMouse(781, 349 + Patterns_Lines_Offset, 16 + 1, 14) & !Songplaying)
    {
        Goto_Next_Row();
        teac = 0;
        gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }

    if(zcheckMouse(781, 349 + (16 * 1) + Patterns_Lines_Offset, 16 + 1, 14))
    {
        Curr_Buff_Block = 0;
        Actupated(0);
        teac = 0;
        gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }
    if(zcheckMouse(781, 349 + (16 * 2) + Patterns_Lines_Offset, 16 + 1, 14))
    {
        Curr_Buff_Block = 1;
        Actupated(0);
        teac = 0;
        gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }
    if(zcheckMouse(781, 349 + (16 * 3) + Patterns_Lines_Offset, 16 + 1, 14))
    {
        Curr_Buff_Block = 2;
        Actupated(0);
        teac = 0;
        gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }
    if(zcheckMouse(781, 349 + (16 * 4) + Patterns_Lines_Offset, 16 + 1, 14))
    {
        Curr_Buff_Block = 3;
        Actupated(0);
        teac = 0;
        gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }

    // Track mute
    start_mute_check_x = PAT_COL_NOTE + 1 + 4;
    tracks = Get_Visible_Partial_Tracks();
    for(i = gui_track; i < gui_track + tracks; i++)
    {
        if(zcheckMouse(start_mute_check_x + Cur_Char_size[i], 184, 28, 10))
        {
            gui_action = GUI_CMD_SWITCH_TRACK_MUTE_STATE;
            break;
        }
        start_mute_check_x += Get_Track_Size(i);
    }

    // Track on/off
    start_mute_check_x = PAT_COL_NOTE + 1 + 4 + 29;
    tracks = Get_Visible_Partial_Tracks();
    for(i = gui_track; i < gui_track + tracks; i++)
    {
        if(zcheckMouse(start_mute_check_x + Cur_Char_size[i], 184, 28, 10))
        {
            int Cur_Position = cPosition;
            if(Songplaying) Cur_Position = cPosition_delay;
            int tmp_track = Get_Track_Over_Mouse();
            Toggle_Track_On_Off_Status(Cur_Position, tmp_track);
            break;
        }
        start_mute_check_x += Get_Track_Size(i);
    }

    // Track zoom > large
    start_mute_check_x = PAT_COL_NOTE + 1 + 4 + (29 * 2);
    tracks = Get_Visible_Partial_Tracks();
    for(i = gui_track; i < gui_track + tracks; i++)
    {
        if(zcheckMouse(start_mute_check_x + Cur_Char_size[i], 184, 16, 10))
        {
            gui_action = GUI_CMD_SWITCH_TRACK_LARGE_STATE;
            break;
        }
        start_mute_check_x += Get_Track_Size(i);
    }
}

// ------------------------------------------------------
// Handle the right mouse button event
void Mouse_Right_Pattern_Ed(void)
{
    int start_mute_check_x;
    int tracks;
    int i;

    // Decrease/Increase steps
    if(zcheckMouse(90, 152, 16, 16))
    {
        ped_pattad = 0;
        gui_action = GUI_CMD_UPDATE_PATTERN_ED;
    }
    if(zcheckMouse(134, 152, 16, 16))
    {
        ped_pattad = 16;
        gui_action = GUI_CMD_UPDATE_PATTERN_ED;
    }

    // Solo track
    start_mute_check_x = PAT_COL_NOTE + 1 + 4;
    tracks = Get_Visible_Partial_Tracks();
    for(i = gui_track; i < gui_track + tracks; i++)
    {
        if(zcheckMouse(start_mute_check_x + Cur_Char_size[i], 184, 28, 10))
        {
            int tmp_track = Get_Track_Over_Mouse();
            Solo_Track(tmp_track);
            // Will unmute the correct track
            gui_action = GUI_CMD_SWITCH_TRACK_MUTE_STATE;
        }
        start_mute_check_x += Get_Track_Size(i);
    }

    // All tracks off but the selected one
    start_mute_check_x = PAT_COL_NOTE + 1 + 4 + 29;
    tracks = Get_Visible_Partial_Tracks();
    for(i = gui_track; i < gui_track + tracks; i++)
    {
        if(zcheckMouse(start_mute_check_x + Cur_Char_size[i], 184, 28, 10))
        {
            int Cur_Position = cPosition;
            if(Songplaying) Cur_Position = cPosition_delay;
            int tmp_track = Get_Track_Over_Mouse();
            Solo_Track_On_Off(Cur_Position, tmp_track);
            break;
        }
        start_mute_check_x += Get_Track_Size(i);
    }


    // Track zoom > small
    start_mute_check_x = PAT_COL_NOTE + 1 + 4 + (29 * 2);
    tracks = Get_Visible_Partial_Tracks();
    for(i = gui_track; i < gui_track + tracks; i++)
    {
        if(zcheckMouse(start_mute_check_x + Cur_Char_size[i], 184, 16, 10)) gui_action = GUI_CMD_SWITCH_TRACK_SMALL_STATE;
        start_mute_check_x += Get_Track_Size(i);
    }
}

// ------------------------------------------------------
// Mute all tracks but a given one
void Solo_Track(int track_to_solo)
{
    // Unmute all if the user clicked on a solo track
    if(CHAN_MUTE_STATE[track_to_solo] == 0)
    {
        int Was_Solo = FALSE;
        
        // Check if all other tracks are muted
        for(int solify = 0; solify < MAX_TRACKS; solify++)
        {
            if((solify != track_to_solo) && CHAN_MUTE_STATE[solify] == 0)
            {
                Was_Solo = TRUE;
            }
        }
        if(!Was_Solo)
        {
            // Unmute all
            for(int solify = 0; solify < MAX_TRACKS; solify++)
            {
                CHAN_MUTE_STATE[solify] = 0;
            }
            CHAN_MUTE_STATE[track_to_solo] = 1;
        }
        else
        {
            // Else mute all
            for(int solify = 0; solify < MAX_TRACKS; solify++)
            {
                CHAN_MUTE_STATE[solify] = 1;
            }
        }
    }
    else
    {
        // Else mute all
        for(int solify = 0; solify < MAX_TRACKS; solify++)
        {
            CHAN_MUTE_STATE[solify] = 1;
        }
    }
}

// ------------------------------------------------------
// Move one row above
void Goto_Previous_Row(void)
{
    int Cur_Position = Get_Current_Position();

    Select_Block_Keyboard(BLOCK_MARK_ROWS);
    ped_line--;
    if(Continuous_Scroll && !Cur_Position) if(ped_line < 0) ped_line = 0;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_ROWS);
}

// ------------------------------------------------------
// Move one row below
void Goto_Next_Row(void)
{
    int Cur_Position = Get_Current_Position();

    Select_Block_Keyboard(BLOCK_MARK_ROWS);
    ped_line++;
    if(Continuous_Scroll && (Cur_Position == sLength - 1)) if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_ROWS);
}

// ------------------------------------------------------
// Move one page above
void Goto_Previous_Page(void)
{
    int Cur_Position = Get_Current_Position();

    Select_Block_Keyboard(BLOCK_MARK_ROWS);
    ped_line -= 16;
    if(!is_recording && !Continuous_Scroll) if(ped_line < 0) ped_line = 0;
    if(Continuous_Scroll && !Cur_Position) if(ped_line < 0) ped_line = 0;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_ROWS);
}

// ------------------------------------------------------
// Move one page below
void Goto_Next_Page(void)
{
    int Cur_Position = Get_Current_Position();

    Select_Block_Keyboard(BLOCK_MARK_ROWS);
    ped_line += 16;
    if(!is_recording && !Continuous_Scroll) if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
    if(Continuous_Scroll && (Cur_Position == sLength - 1)) if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_ROWS);
}

// ------------------------------------------------------
// Move one column on the left
void Goto_Previous_Column(void)
{
    Select_Block_Keyboard(BLOCK_MARK_TRACKS);
    ped_col--;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_TRACKS);
    gui_action = GUI_CMD_SET_FOCUS_TRACK;
}

// ------------------------------------------------------
// Move one column on the right
void Goto_Next_Column(void)
{
    Select_Block_Keyboard(BLOCK_MARK_TRACKS);
    ped_col++;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_TRACKS);
    gui_action = GUI_CMD_SET_FOCUS_TRACK;
}

// ------------------------------------------------------
// Move to the top left of the pattern
void Goto_Top_Left(void)
{
    Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
    ped_col = 0;
    ped_track = 0;
    if(Get_LCtrl()) ped_line = 0;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
    gui_action = GUI_CMD_SET_FOCUS_TRACK;
}

// ------------------------------------------------------
// Move to the bottom right of the pattern
void Goto_Bottom_Right(void)
{
    Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
    ped_col = 0;
    ped_track = Songtracks - 1;
    if(Get_LCtrl()) ped_line = patternLines[pSequence[cPosition_delay]] - 1;
    Actupated(0);
    gui_action = GUI_CMD_SET_FOCUS_TRACK;
    Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
}

// ------------------------------------------------------
// Move to a given row of the pattern
void Goto_Row(int row)
{
    Select_Block_Keyboard(BLOCK_MARK_ROWS);
    ped_line = row;
    Actupated(0);
    Select_Block_Keyboard(BLOCK_MARK_ROWS);
}

// ------------------------------------------------------
// Return the current sequence position
int Get_Current_Position(void)
{
    if(Songplaying) return(cPosition_delay);
    else return(cPosition);
}

// ------------------------------------------------------
// Set all the tracks to small size font
void Reset_Patterns_Zoom(void)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        Cur_Char_size[i] = PAT_COL_CHAR;
        Cur_Char_Function[i].Fnc = Letter;
        Cur_Note_Function[i].Fnc = blitnote;
    }
}

// ------------------------------------------------------
// Toggle the zoom status of a track
void Toggle_Track_Zoom(int track, int large)
{
    if(large)
    {
        if(Get_Track_Type(track) == TRACK_LARGE)
        {
            Set_Track_Zoom(track, TRACK_NORMAL);
        }
        else
        {
            Set_Track_Zoom(track, TRACK_LARGE);
        }
    }
    else
    {
        if(Get_Track_Type(track) == TRACK_SMALL)
        {
            Set_Track_Zoom(track, TRACK_NORMAL);
        }
        else
        {
            Set_Track_Zoom(track, TRACK_SMALL);
        }
    }
}

// ------------------------------------------------------
// Return the type of a track
TRACK_TYPE Get_Track_Type(int track)
{
    switch(Cur_Char_size[track])
    {
        case PAT_COL_CHAR:
            return(TRACK_NORMAL);

        case PAT_COL_CHAR_LARGE:
            return(TRACK_LARGE);

        case PAT_COL_CHAR_SMALL:
            return(TRACK_SMALL);

        default:
            return(TRACK_NORMAL);
    }
}

// ------------------------------------------------------
// Force a track to be zoomed out
void Clear_Track_Zoom(int track)
{
    Cur_Char_size[track] = PAT_COL_CHAR;
    Cur_Char_Function[track].Fnc = Letter;
    Cur_Note_Function[track].Fnc = blitnote;
}

// ------------------------------------------------------
// Set the zoom status of a track
void Set_Track_Zoom(int track, TRACK_TYPE type)
{
    switch(type)
    {
        case TRACK_SMALL:
            Cur_Char_size[track] = PAT_COL_CHAR_SMALL;
            Cur_Char_Function[track].Fnc = Small_Letter;
            Cur_Note_Function[track].Fnc = blitsmallnote;
            break;

        case TRACK_NORMAL:
            Clear_Track_Zoom(track);
            break;

        case TRACK_LARGE:
            Cur_Char_size[track] = PAT_COL_CHAR_LARGE;
            Cur_Char_Function[track].Fnc = Large_Letter;
            Cur_Note_Function[track].Fnc = blitlargenote;
            // Track is too damn large to be zoomed
            if(Channels_MultiNotes[track] > 11)
            {
                Clear_Track_Zoom(track);
            }
            break;
    }
}
