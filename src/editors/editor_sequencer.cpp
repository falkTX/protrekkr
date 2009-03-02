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
#include "include/editor_sequencer.h"
#include "include/editor_pattern.h"
#include "include/patterns_blocks.h"

// ------------------------------------------------------
// Structures
typedef struct
{
    int pattern;
    char active_state[16];
} SEQ_POS, *LPSEQ_POS;

// ------------------------------------------------------
// Variables
int Cur_Seq_Buffer = 0;

SEQ_POS Seq_Buffers[4] =
{
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
};

int cur_seq_buffer[] =
{
    BUTTON_PUSHED,
    BUTTON_NORMAL,
    BUTTON_NORMAL,
    BUTTON_NORMAL
};

int Remap_From;
int Remap_To;

// ------------------------------------------------------
// Functions
void SeqFill(int st, int en, char n);
void SeqDelete(int st);
void SeqInsert(int st);
void SeqCopy(int st);
void Display_Seq_Buffer(void);
void SeqPaste(int st);
void Bound_Patt_Pos(void);

void Draw_Sequencer_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_SEQUENCER);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Track Part Sequencer");

    Gui_Draw_Button_Box(4, 466, 80, 16, "Clear All", BUTTON_NORMAL);
    Gui_Draw_Button_Box(4, 484, 80, 16, "Clear Position", BUTTON_NORMAL);
    Gui_Draw_Button_Box(4, 522, 80, 16, "Reset All", BUTTON_NORMAL);
    Gui_Draw_Button_Box(4, 540, 80, 16, "Reset Position", BUTTON_NORMAL);

    Gui_Draw_Button_Box(396, 466, 32, 16, "Cut", BUTTON_NORMAL);
    Gui_Draw_Button_Box(396 + 34, 466, 32, 16, "Copy", BUTTON_NORMAL);
    Gui_Draw_Button_Box(396, 484, 66, 16, "Paste", BUTTON_NORMAL);
    Gui_Draw_Button_Box(396, 502, 66, 16, "Insert/Paste", BUTTON_NORMAL);

    Display_Seq_Buffer();

    Gui_Draw_Button_Box(308, 466, 80, 16, "Ptn->Pos [Cur]", BUTTON_NORMAL);
    Gui_Draw_Button_Box(308, 484, 80, 16, "Ptn->Pos [Sng]", BUTTON_NORMAL);
    Gui_Draw_Button_Box(308, 522, 80, 16, "Insert Position", BUTTON_NORMAL);
    Gui_Draw_Button_Box(308, 540, 80, 16, "Delete Position", BUTTON_NORMAL);

    Gui_Draw_Button_Box(288, 466, 16, 16, "\07", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, 484, 16, 16, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, 522, 16, 16, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, 540, 16, 16, "\10", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(89, 466, 25, 90, "", BUTTON_NORMAL);
    Gui_Draw_Button_Box(257, 466, 25, 90, "", BUTTON_NORMAL);
    Gui_Draw_Button_Box(120, 466, 131, 90, "", BUTTON_NORMAL);

    Gui_Draw_Button_Box(480, 466, 250, 26, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(480, 472, 190, 26, "Zoom all tracks", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER);
    
    Gui_Draw_Button_Box(565, 472, 50, 16, "Small", BUTTON_NORMAL);
    Gui_Draw_Button_Box(620, 472, 50, 16, "Normal", BUTTON_NORMAL);
    Gui_Draw_Button_Box(675, 472, 50, 16, "Large", BUTTON_NORMAL);
    
    Gui_Draw_Button_Box(480, 501, 250, 64, "Remap Instrument", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(480, 523, 60, 26, "From", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER);
    Gui_Draw_Button_Box(480, 544, 60, 26, "To", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER);

    Gui_Draw_Button_Box(600, 524, 60, 16, "Selection", BUTTON_NORMAL);
    Gui_Draw_Button_Box(600, 544, 60, 16, "Track", BUTTON_NORMAL);
    Gui_Draw_Button_Box(662, 524, 60, 16, "Pattern", BUTTON_NORMAL);
    Gui_Draw_Button_Box(662, 544, 60, 16, "Song", BUTTON_NORMAL);
}

void Actualize_Seq_Ed(char gode)
{
    if(userscreen == USER_SCREEN_SEQUENCER)
    {
        SetColor(COL_BACKGROUND);
        bjbox(92, 469, 20, 85);
        bjbox(124, 469, 124, 85);
        bjbox(260, 469, 20, 85);

        // Green bar
        SetColor(COL_PUSHED_MED);
        bjbox(124, 505, 124, 12);

        for(int lseq = -3; lseq < 4; lseq++)
        {
            int rel;
            if(Songplaying) rel = lseq + cPosition_delay;
            else rel = lseq + cPosition;
            if(rel > -1 && rel < sLength)
            {
                out_decchar(93, 505 + lseq * 12, rel, 0);
                out_decchar(261, 505 + lseq * 12, pSequence[rel], 0);

                for(int rel2 = 0; rel2 < Songtracks; rel2++)
                {
                    if(CHAN_ACTIVE_STATE[rel][rel2]) out_nibble(124 + rel2 * 8, 505 + lseq * 12, USE_FONT, rel2);
                    else out_nibble(124 + rel2 * 8, 505 + lseq * 12, USE_FONT_LOW, rel2);
                } // sub for
            }
            else
            { // rel range OK
                PrintXY(93, 505 + lseq * 12, USE_FONT, "000");
                PrintXY(261, 505 + lseq * 12, USE_FONT, "000");
            }
        } // for end
        Actupated(0);

        // From instrument
        if(gode == 0 || gode == 1)
        {
            if(Remap_From < 0) Remap_From = 0;
            if(Remap_From > 0x7f) Remap_From = 0x7f;
            value_box(520, 524, Remap_From, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }

        // To instrument
        if(gode == 0 || gode == 2)
        {
            if(Remap_To < 0) Remap_To = 0;
            if(Remap_To > 0x7f) Remap_To = 0x7f;
            value_box(520, 544, Remap_To, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }
    }
}

void Mouse_Left_Sequencer_Ed(void)
{
    int i;
    int j;
    int Cur_Position = cPosition;

    if(Songplaying) Cur_Position = cPosition_delay;

    if(userscreen == USER_SCREEN_SEQUENCER)
    {
        // Remap Selection
        if(zcheckMouse(600, 524, 60, 16))
        {
            Instrument_Remap_Block(Cur_Position, Get_Real_Selection(FALSE), Remap_From, Remap_To);
        }
        // Remap Track
        if(zcheckMouse(600, 544, 60, 16))
        {
            Instrument_Remap_Block(Cur_Position, Select_Track(ped_track), Remap_From, Remap_To);
        }
        // Remap Pattern
        if(zcheckMouse(662, 524, 60, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                Instrument_Remap_Block(Cur_Position, Select_Track(i), Remap_From, Remap_To);
            }
        }
        // Remap Song
        if(zcheckMouse(662, 544, 60, 16))
        {
            for(j = 0; j < sLength; j++)
            {
                for(i = 0; i < Songtracks; i++)
                {
                    Instrument_Remap_Block(j, Select_Track(i), Remap_From, Remap_To);
                }
            }
        }

        // From Instrument
        if(zcheckMouse(520, 524, 16, 16) == 1)
        {
            Remap_From--;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // From Instrument
        if(zcheckMouse(520 + 44, 524, 16, 16) == 1)
        {
            Remap_From++;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // To Instrument
        if(zcheckMouse(520, 544, 16, 16) == 1)
        {
            Remap_To--;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // To Instrument
        if(zcheckMouse(520 + 44, 544, 16, 16) == 1)
        {
            Remap_To++;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // Zoom'em small
        if(zcheckMouse(565, 472, 50, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                Set_Track_Zoom(i, TRACK_SMALL);
            }
            Actupated(0);
        }
        // Zoom'em normal
        if(zcheckMouse(620, 472, 50, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                Set_Track_Zoom(i, TRACK_NORMAL);
            }
            Actupated(0);
        }
        // Zoom'em large
        if(zcheckMouse(675, 472, 50, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                Set_Track_Zoom(i, TRACK_LARGE);
            }
            Actupated(0);
        }

        // Clear all
        if(zcheckMouse(4, 466, 80, 16))
        {
            SeqFill(0, 256, FALSE);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Clear position
        if(zcheckMouse(4, 484, 80, 16))
        {
            SeqFill(Cur_Position, Cur_Position + 1, FALSE);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Reset all
        if(zcheckMouse(4, 522, 80, 16))
        {
            SeqFill(0, 256, TRUE);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Reset position
        if(zcheckMouse(4, 540, 80, 16))
        {
            SeqFill(Cur_Position, Cur_Position + 1, TRUE);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Insert position
        if(zcheckMouse(308, 522, 80, 16))
        {
            SeqInsert(Cur_Position);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Delete position
        if(zcheckMouse(308, 540, 80, 16))
        {
            SeqDelete(Cur_Position);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Select the copy buffers
        if(zcheckMouse(396, 522, 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_PUSHED;
            cur_seq_buffer[1] = BUTTON_NORMAL;
            cur_seq_buffer[2] = BUTTON_NORMAL;
            cur_seq_buffer[3] = BUTTON_NORMAL;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 0;
        }
        if(zcheckMouse(396 + 17, 522, 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_NORMAL;
            cur_seq_buffer[1] = BUTTON_PUSHED;
            cur_seq_buffer[2] = BUTTON_NORMAL;
            cur_seq_buffer[3] = BUTTON_NORMAL;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 1;
        }
        if(zcheckMouse(396 + (17 * 2), 522, 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_NORMAL;
            cur_seq_buffer[1] = BUTTON_NORMAL;
            cur_seq_buffer[2] = BUTTON_PUSHED;
            cur_seq_buffer[3] = BUTTON_NORMAL;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 2;
        }
        if(zcheckMouse(396 + (17 * 3), 522, 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_NORMAL;
            cur_seq_buffer[1] = BUTTON_NORMAL;
            cur_seq_buffer[2] = BUTTON_NORMAL;
            cur_seq_buffer[3] = BUTTON_PUSHED;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 3;
        }

        // Cut
        if(zcheckMouse(396, 466, 32, 16))
        {
            SeqCopy(Cur_Position);
            SeqDelete(Cur_Position);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Copy
        if(zcheckMouse(396 + 34, 466, 32, 16))
        {
            SeqCopy(Cur_Position);
        }

        // Paste
        if(zcheckMouse(396, 484, 66, 16))
        {
            SeqPaste(Cur_Position);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Insert/Paste
        if(zcheckMouse(396, 502, 66, 16))
        {
            SeqInsert(Cur_Position);
            SeqPaste(Cur_Position);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Ptn->Pos[Cur]
        if(zcheckMouse(308, 466, 80, 16))
        {
            if(Cur_Position < 128)
            {
                pSequence[Cur_Position] = Cur_Position;
                Anat(Cur_Position);
                gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Ptn->Pos[Sng]
        if(zcheckMouse(308, 484, 80, 16))
        {
            for(int xpos = 0; xpos < 128; xpos++)
            {
                pSequence[xpos] = xpos;
            }
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Up 10 pos
        if(zcheckMouse(288, 466, 16, 16))
        {
            gui_action = GUI_CMD_REDUCE_POSITIONS_10;
        }
        // Up 1 pos
        if(zcheckMouse(288, 484, 16, 16))
        {
            gui_action = GUI_CMD_PREVIOUS_POSITION;
        }
        // Down 1 pos
        if(zcheckMouse(288, 522, 16, 16)){
            gui_action = GUI_CMD_NEXT_POSITION;
        }
        // Down 10 pos
        if(zcheckMouse(288, 540, 16, 16))
        {
            gui_action = GUI_CMD_INCREASE_POSITIONS_10;
        }

        // Add 100 to the selected pattern
        if(zcheckMouse(260, 468, 7, 84))
        {
            int posindex = ((Mouse.y - 469) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength)
            {
                if(pSequence[posindex] < 127)
                {
                    pSequence[posindex] += 100;   
                    if(pSequence[posindex] >= 128) pSequence[posindex] = 127;
                    Anat(posindex);

                    if(posindex != Cur_Position) gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else gui_action = GUI_CMD_UPDATE_SEQUENCER;

                }
                else
                {
                    gui_action = GUI_CMD_PATTERNS_POOL_EXHAUSTED;
                }
            }
        }

        // Add 10 to the selected pattern
        if(zcheckMouse(266, 468, 7, 84))
        {
            int posindex = ((Mouse.y - 469) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength)
            {
                if(pSequence[posindex] < 127)
                {
                    pSequence[posindex] += 10; 
                    if(pSequence[posindex] >= 128) pSequence[posindex] = 127;
                    Anat(posindex);

                    if(posindex != Cur_Position) gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else gui_action = GUI_CMD_UPDATE_SEQUENCER;

                }
                else
                {
                    gui_action = GUI_CMD_PATTERNS_POOL_EXHAUSTED;
                }
            }
        }

        // Add 1 to the selected pattern
        if(zcheckMouse(272, 468, 7, 84))
        {
            int posindex = ((Mouse.y - 469) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength)
            {
                if(pSequence[posindex] < 127)
                {
                    pSequence[posindex]++;
                    Anat(posindex);
                    if(posindex != Cur_Position) gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else gui_action = GUI_CMD_UPDATE_SEQUENCER;
                }
                else
                {
                    gui_action = GUI_CMD_PATTERNS_POOL_EXHAUSTED;
                }
            }
        }

        // Enable/Disable channels
        if(zcheckMouse(123, 469, 129, 84) == 1)
        {
            int posindex = ((Mouse.y - 470) / 12) - 3;
            posindex += Cur_Position;
            Toggle_Track_On_Off_Status(posindex, (Mouse.x - 123) / 8);
        }

        // Scroll the positions
        if(zcheckMouse(89, 469, 24, 84))
        {
            int posindex = ((Mouse.y - 469) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength && posindex != Cur_Position)
            {
                cPosition = posindex;
                gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }
    }
}

void Mouse_Right_Sequencer_Ed(void)
{
    int Cur_Position = cPosition;
    int i;
    if(Songplaying) Cur_Position = cPosition_delay;

    if(userscreen == USER_SCREEN_SEQUENCER)
    {
        // From Instrument
        if(zcheckMouse(520, 524, 16, 16) == 1)
        {
            Remap_From -= 10;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // From Instrument
        if(zcheckMouse(520 + 44, 524, 16, 16) == 1)
        {
            Remap_From += 10;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // To Instrument
        if(zcheckMouse(520, 544, 16, 16) == 1)
        {
            Remap_To -= 10;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // To Instrument
        if(zcheckMouse(520 + 44, 544, 16, 16) == 1)
        {
            Remap_To += 10;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
            teac = 1;
        }

        // Sub 100 to the selected pattern
        if(zcheckMouse(260, 468, 7, 84))
        {
            int posindex = ((Mouse.y - 469) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength)
            {
                int reak = pSequence[posindex];
                reak -= 100;
                if(reak < 0) reak = 0;
                pSequence[posindex] = reak;
                Anat(posindex);
                if(posindex != Cur_Position) gui_action = GUI_CMD_UPDATE_SEQ_ED;
                else gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Sub 10 to the selected pattern
        if(zcheckMouse(266, 468, 7, 84))
        {
            int posindex = ((Mouse.y - 469) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength)
            {
                int reak = pSequence[posindex];
                reak -= 10;
                if(reak < 0) reak = 0;
                pSequence[posindex] = reak;
                Anat(posindex);

                if(posindex != Cur_Position) gui_action = GUI_CMD_UPDATE_SEQ_ED;
                else gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Sub 1 to the selected pattern
        if(zcheckMouse(272, 468, 7, 84))
        {
            int posindex = ((Mouse.y - 469) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength)
            {
                if(pSequence[posindex] > 0)
                {
                    pSequence[posindex]--;
                    Anat(posindex);

                    if(posindex != Cur_Position) gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else gui_action = GUI_CMD_UPDATE_SEQUENCER;
                }
            }
        }

        // Solo a track
        if(zcheckMouse(123, 469, 129, 84) == 1)
        {
            int posindex = ((Mouse.y - 470) / 12) - 3;
            posindex += Cur_Position;
            Solo_Track_On_Off(posindex, (Mouse.x - 123) / 8);
        }

        // Insert 10 positions
        if(zcheckMouse(308, 522, 80, 16))
        {
            for(i = 0; i < 10; i++)
            {
                SeqInsert(Cur_Position);
            }
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Delete 10 positions
        if(zcheckMouse(308, 540, 80, 16))
        {
            for(i = 0; i < 10; i++)
            {
                SeqDelete(Cur_Position);
            }
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }
    }
}

void Actualize_Sequencer(void)
{
    int value;
    int Cur_Position;
    int i;

    if(Songplaying)
    {
        if(cPosition_delay < 0) cPosition_delay = 0;
        if(cPosition_delay > sLength - 1) cPosition_delay = sLength - 1;
        Cur_Position = cPosition_delay;
        if(cPosition < 0)
        {
            cPosition = 0;
        }
        if(cPosition > sLength - 1)
        {
            cPosition = sLength - 1;
            Bound_Patt_Pos();
            Actupated(0);
        }
        for(i = 0; i < MAX_TRACKS; i++)
        {
            CHAN_HISTORY_STATE[cPosition][i] = FALSE;
        }
    }
    else
    {
        if(cPosition < 0)
        {
            cPosition = 0;
        }
        if(cPosition > sLength - 1)
        {
            cPosition = sLength - 1;
            Bound_Patt_Pos();
            Actupated(0);
        }
        Cur_Position = cPosition;
    }
    value = pSequence[Cur_Position];
    if(value > 127) pSequence[Cur_Position] = 127;
    if(value < 0) pSequence[Cur_Position] = 0;

    Gui_Draw_Arrows_Number_Box(188, 44, Cur_Position, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Arrows_Number_Box(188, 62, pSequence[Cur_Position], BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Anat(Cur_Position);
    if(Rows_Decimal) Gui_Draw_Arrows_Number_Box(188, 98, patternLines[pSequence[Cur_Position]], BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    else value_box(188, 98, patternLines[pSequence[Cur_Position]], BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Arrows_Number_Box(188, 80, sLength, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    if(userscreen == USER_SCREEN_SEQUENCER) Actualize_Seq_Ed(0);
}

void SeqFill(int st, int en, char n)
{
    for(int cl = st; cl < en; cl++)
    {
        for(char trk = 0; trk < Songtracks; trk++)
        {
            CHAN_ACTIVE_STATE[cl][trk] = n;
            CHAN_HISTORY_STATE[cl][trk] = FALSE;
        }
    }
}     

// ------------------------------------------------------
// Delete a position
void SeqDelete(int st)
{
    int cl;

    if(sLength > 1)
    {
        for(cl = st; cl < sLength - 1; cl++)
        {
            pSequence[cl] = pSequence[cl + 1];
            for(char trk = 0; trk < Songtracks; trk++)
            {
                CHAN_ACTIVE_STATE[cl][trk] = CHAN_ACTIVE_STATE[cl + 1][trk];
                CHAN_HISTORY_STATE[cl][trk] = CHAN_HISTORY_STATE[cl + 1][trk];
            }
        }
        pSequence[cl] = 0;
        for(char trk = 0; trk < Songtracks; trk++)
        {
            CHAN_ACTIVE_STATE[cl][trk] = TRUE;
            CHAN_HISTORY_STATE[cl][trk] = FALSE;
        }
        sLength--;
    }
}     

// ------------------------------------------------------
// Insert a position
void SeqInsert(int st)
{
    int cl;

    if(sLength < 255)
    {
        for(cl = sLength - 1; cl >= st; cl--)
        {
            pSequence[cl + 1] = pSequence[cl];
            for(char trk = 0; trk < Songtracks; trk++)
            {
                CHAN_ACTIVE_STATE[cl + 1][trk] = CHAN_ACTIVE_STATE[cl][trk];
                CHAN_HISTORY_STATE[cl + 1][trk] = CHAN_HISTORY_STATE[cl][trk];
            }
        }
        pSequence[st] = 0;
        for(char trk = 0; trk < Songtracks; trk++)
        {
            CHAN_ACTIVE_STATE[st][trk] = TRUE;
            CHAN_HISTORY_STATE[st][trk] = FALSE;
        }
    sLength++;
    }
}     

void Anat(int posil)
{
    if(pSequence[posil] >= nPatterns)
    {
        nPatterns = pSequence[posil] + 1;
    }
}

// ------------------------------------------------------
// Copy a position
void SeqCopy(int st)
{
    Seq_Buffers[Cur_Seq_Buffer].pattern = pSequence[st];
    for(char trk = 0; trk < Songtracks; trk++)
    {
        Seq_Buffers[Cur_Seq_Buffer].active_state[trk] = CHAN_ACTIVE_STATE[st][trk];
    }
}     

// ------------------------------------------------------
// Paste a position
void SeqPaste(int st)
{
    pSequence[st] = Seq_Buffers[Cur_Seq_Buffer].pattern;
    for(char trk = 0; trk < Songtracks; trk++)
    {
        CHAN_ACTIVE_STATE[st][trk] = Seq_Buffers[Cur_Seq_Buffer].active_state[trk];
        CHAN_HISTORY_STATE[st][trk] = FALSE;
    }
}     

// ------------------------------------------------------
// Notify the user selected buffer visually
void Display_Seq_Buffer(void)
{
    Gui_Draw_Button_Box(396, 522, 15, 16, "1", cur_seq_buffer[0]);
    Gui_Draw_Button_Box(396 + 17, 522, 15, 16, "2", cur_seq_buffer[1]);
    Gui_Draw_Button_Box(396 + (17 * 2), 522, 15, 16, "3", cur_seq_buffer[2]);
    Gui_Draw_Button_Box(396 + (17 * 3), 522, 15, 16, "4", cur_seq_buffer[3]);
}

// ------------------------------------------------------
// Turn a channel active state on/off
void Toggle_Track_On_Off_Status(int posindex, int seqindex)
{
    if(posindex >= 0 && posindex < sLength)
    {
        if(seqindex < 0) seqindex = 0;
        if(seqindex > Songtracks - 1) seqindex = Songtracks - 1;
        if(!CHAN_ACTIVE_STATE[posindex][seqindex])
        {
            CHAN_ACTIVE_STATE[posindex][seqindex] = TRUE;
            CHAN_HISTORY_STATE[posindex][seqindex] = FALSE;
        }
        else
        {
            CHAN_ACTIVE_STATE[posindex][seqindex] = FALSE;
            CHAN_HISTORY_STATE[posindex][seqindex] = FALSE;
        }
        Actupated(0);
        gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }
}

// ------------------------------------------------------
// Turn all channel active states on/off but one
void Solo_Track_On_Off(int posindex, int seqindex)
{
    int Already_Solo;

    if(posindex >= 0 && posindex < sLength)
    {
        if(seqindex < 0) seqindex = 0;
        if(seqindex > Songtracks - 1) seqindex = Songtracks - 1;

        if(CHAN_ACTIVE_STATE[posindex][seqindex])
        {
            // Check if it was the only track turned on
            Already_Solo = 0;
            for(int alphac = 0; alphac < Songtracks; alphac++)
            {
                if(CHAN_ACTIVE_STATE[posindex][alphac] == TRUE) Already_Solo++;
            }
            if(Already_Solo == 1)
            {
                // Was already soloed: turn'em all on
                for(int alphac = 0; alphac < Songtracks; alphac++)
                {
                    CHAN_ACTIVE_STATE[posindex][alphac] = TRUE;
                    CHAN_HISTORY_STATE[posindex][alphac] = FALSE;
                }
            }
            else
            {
                // Solo it
                for(int alphac = 0; alphac < Songtracks; alphac++)
                {
                    CHAN_ACTIVE_STATE[posindex][alphac] = FALSE;
                    CHAN_HISTORY_STATE[posindex][alphac] = FALSE;
                }
            }
        }
        else
        {
            // Solo it
            for(int alphac = 0; alphac < Songtracks; alphac++)
            {
                CHAN_ACTIVE_STATE[posindex][alphac] = FALSE;
                CHAN_HISTORY_STATE[posindex][alphac] = FALSE;
            }
        }
        // Active it
        CHAN_ACTIVE_STATE[posindex][seqindex] = TRUE;
        CHAN_HISTORY_STATE[posindex][seqindex] = FALSE;
        Actupated(0);
        gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }
}
