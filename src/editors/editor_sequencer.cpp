// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/editor_sequencer.h"

// ------------------------------------------------------
// Structures
typedef struct
{
    int pattern;
    bool active_state[16];
} SEQ_POS, *LPSEQ_POS;

// ------------------------------------------------------
// Variables
int Cur_Seq_Buffer = 0;

SEQ_POS Seq_Buffers[4] =
{
    { 0, { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, } },
    { 0, { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, } },
    { 0, { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, } },
    { 0, { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, } },
};

int cur_seq_buffer[] =
{
    BUTTON_PUSHED,
    BUTTON_NORMAL,
    BUTTON_NORMAL,
    BUTTON_NORMAL
};

// ------------------------------------------------------
// Functions
void SeqFill(int st, int en, bool n);
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

    Gui_Draw_Button_Box(4, 466, 80, 16,"Clear All", BUTTON_NORMAL);
    Gui_Draw_Button_Box(4, 484, 80, 16,"Clear Position", BUTTON_NORMAL);
    Gui_Draw_Button_Box(4, 522, 80, 16,"Reset All", BUTTON_NORMAL);
    Gui_Draw_Button_Box(4, 540, 80, 16,"Reset Position", BUTTON_NORMAL);

    Gui_Draw_Button_Box(396, 466, 32, 16,"Cut", BUTTON_NORMAL);
    Gui_Draw_Button_Box(396 + 34, 466, 32, 16,"Copy", BUTTON_NORMAL);
    Gui_Draw_Button_Box(396, 484, 66, 16,"Paste", BUTTON_NORMAL);
    Gui_Draw_Button_Box(396, 502, 66, 16,"Insert/Paste", BUTTON_NORMAL);

    Display_Seq_Buffer();

    Gui_Draw_Button_Box(308, 466, 80, 16,"Ptn->Pos [Cur]", BUTTON_NORMAL);
    Gui_Draw_Button_Box(308, 484, 80, 16,"Ptn->Pos [Sng]", BUTTON_NORMAL);
    Gui_Draw_Button_Box(308, 522, 80, 16,"Insert Position", BUTTON_NORMAL);
    Gui_Draw_Button_Box(308, 540, 80, 16,"Delete Position", BUTTON_NORMAL);

    Gui_Draw_Button_Box(288, 466, 16, 16,"\07", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, 484, 16, 16,"\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, 522, 16, 16,"\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, 540, 16, 16,"\10", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(89, 466, 25, 90, "", BUTTON_NORMAL);
    Gui_Draw_Button_Box(257, 466, 25, 90, "", BUTTON_NORMAL);
    Gui_Draw_Button_Box(120, 466, 131, 90, "", BUTTON_NORMAL);
}

void Actualize_Seq_Ed(void)
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
                    if(SACTIVE[rel][rel2]) out_nibble(124 + rel2 * 8, 505 + lseq * 12, USE_FONT, rel2);
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
    }
}

void Mouse_Left_Sequencer_Ed(void)
{
    int Cur_Position = cPosition;

    if(Songplaying) Cur_Position = cPosition_delay;

    if(userscreen == USER_SCREEN_SEQUENCER)
    {

        // Clear all
        if(zcheckMouse(4, 466, 80, 16))
        {
            SeqFill(0, 256, false);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Clear position
        if(zcheckMouse(4, 484, 80, 16))
        {
            SeqFill(Cur_Position, Cur_Position + 1, false);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Reset all
        if(zcheckMouse(4, 522, 80, 16))
        {
            SeqFill(0, 256, true);
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Reset position
        if(zcheckMouse(4, 540, 80, 16))
        {
            SeqFill(Cur_Position, Cur_Position + 1, true);
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
            int posindex = ((Mouse.old_y - 469) / 12) - 3;
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
            int posindex = ((Mouse.old_y - 469) / 12) - 3;
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
            int posindex = ((Mouse.old_y - 469) / 12) - 3;
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
            int posindex = ((Mouse.old_y - 470) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < sLength)
            {
                int seqindex = (Mouse.old_x - 123) / 8;
                if(seqindex < 0) seqindex = 0;
                if(seqindex > Songtracks - 1) seqindex = Songtracks - 1;
                if(!SACTIVE[posindex][seqindex])
                {
                    SACTIVE[posindex][seqindex] = true;
                    SHISTORY[posindex][seqindex] = false;
                }
                else
                {
                    SACTIVE[posindex][seqindex] = false;
                    SHISTORY[posindex][seqindex] = false;
                }
                gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Scroll the positions
        if(zcheckMouse(89, 469, 24, 84))
        {
            int posindex = ((Mouse.old_y - 469) / 12) - 3;
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
    int Already_Solo;
    int i;
    if(Songplaying) Cur_Position = cPosition_delay;

    if(userscreen == USER_SCREEN_SEQUENCER)
    {

        // Sub 100 to the selected pattern
        if(zcheckMouse(260, 468, 7, 84))
        {
            int posindex = ((Mouse.old_y - 469) / 12) - 3;
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
            int posindex = ((Mouse.old_y - 469) / 12) - 3;
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
            int posindex = ((Mouse.old_y - 469) / 12) - 3;
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
            int posindex = ((Mouse.old_y - 470) / 12) - 3;
            posindex += Cur_Position;

            if(posindex >= 0 && posindex < sLength)
            {
                int seqindex = (Mouse.old_x - 123) / 8;
                if(seqindex < 0) seqindex = 0;
                if(seqindex > Songtracks - 1) seqindex = Songtracks - 1;

                if(SACTIVE[posindex][seqindex])
                {
                    Already_Solo = 0;
                    for(int alphac = 0; alphac < Songtracks; alphac++)
                    {
                        if(SACTIVE[posindex][alphac] == true) Already_Solo++;
                    }
                    if(Already_Solo == 1)
                    {
                        for(int alphac = 0; alphac < Songtracks; alphac++)
                        {
                            SACTIVE[posindex][alphac] = true;
                            SHISTORY[posindex][alphac] = false;
                        }
                    }
                    else
                    {
                        for(int alphac = 0; alphac < Songtracks; alphac++)
                        {
                            SACTIVE[posindex][alphac] = false;
                            SHISTORY[posindex][alphac] = false;
                        }
                    }
                }
                // Active it
                SACTIVE[posindex][seqindex] = true;
                SHISTORY[posindex][seqindex] = false;
                gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
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
            SHISTORY[cPosition][i] = false;
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

    if(userscreen == USER_SCREEN_SEQUENCER) Actualize_Seq_Ed();
}

void SeqFill(int st, int en, bool n)
{
    for(int cl = st; cl < en; cl++)
    {
        for(char trk = 0; trk < Songtracks; trk++)
        {
            SACTIVE[cl][trk] = n;
            SHISTORY[cl][trk] = false;
        }
    }
}     

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
                SACTIVE[cl][trk] = SACTIVE[cl + 1][trk];
                SHISTORY[cl][trk] = SHISTORY[cl + 1][trk];
            }
        }
        pSequence[cl] = 0;
        for(char trk = 0; trk < Songtracks; trk++)
        {
            SACTIVE[cl][trk] = true;
            SHISTORY[cl][trk] = false;
        }
        sLength--;
    }
}     

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
                SACTIVE[cl + 1][trk] = SACTIVE[cl][trk];
                SHISTORY[cl + 1][trk] = SHISTORY[cl][trk];
            }
        }
        pSequence[st] = 0;
        for(char trk = 0; trk < Songtracks; trk++)
        {
            SACTIVE[st][trk] = true;
            SHISTORY[st][trk] = false;
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

// Copy a position
void SeqCopy(int st)
{
    Seq_Buffers[Cur_Seq_Buffer].pattern = pSequence[st];
    for(char trk = 0; trk < Songtracks; trk++)
    {
        Seq_Buffers[Cur_Seq_Buffer].active_state[trk] = SACTIVE[st][trk];
    }
}     

// Paste a position
void SeqPaste(int st)
{
    pSequence[st] = Seq_Buffers[Cur_Seq_Buffer].pattern;
    for(char trk = 0; trk < Songtracks; trk++)
    {
        SACTIVE[st][trk] = Seq_Buffers[Cur_Seq_Buffer].active_state[trk];
        SHISTORY[st][trk] = false;
    }
}     

// Notify the user selected buffer visually
void Display_Seq_Buffer(void)
{
    Gui_Draw_Button_Box(396, 522, 15, 16, "1", cur_seq_buffer[0]);
    Gui_Draw_Button_Box(396 + 17, 522, 15, 16, "2", cur_seq_buffer[1]);
    Gui_Draw_Button_Box(396 + (17 * 2), 522, 15, 16, "3", cur_seq_buffer[2]);
    Gui_Draw_Button_Box(396 + (17 * 3), 522, 15, 16, "4", cur_seq_buffer[3]);
}
