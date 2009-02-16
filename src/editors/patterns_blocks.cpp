// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "../include/variables.h"
#include "include/patterns_blocks.h"

// ------------------------------------------------------
// Structures
typedef struct
{
    int x_start;
    int y_start;
    int x_end;
    int y_end;
} SELECTION, *LPSELECTION;

// ------------------------------------------------------
// Variables
int block_start_track_nibble = -1;
int save_block_start_track_nibble = -1;
int block_start_track = -1;
int block_end_track_nibble = -1;
int block_end_track = -1;
int block_start = 0;
int block_end = 0;
int block_in_selection = 0;
int b_buff_xsize = 0;
int b_buff_ysize = 0;

int swap_block_start_track = -1;
int swap_block_start_track_nibble = -1;
int swap_block_end_track = -1;
int swap_block_end_track_nibble = -1;
int swap_block_start = 0;
int swap_block_end = 0;
unsigned char *BuffBlock;
unsigned char *BuffPatt;
unsigned char *BuffTrack;

// ------------------------------------------------------
// Init the blocks datas and buffers
int Init_Block_Work(void)
{
    int ipcut;
    int i;

    BuffPatt = (unsigned char *) malloc(PATTERN_LEN);
    if(!BuffPatt) return(FALSE);
    BuffTrack = (unsigned char *) malloc(PATTERN_TRACK_LEN);
    if(!BuffTrack) return(FALSE);
    BuffBlock = (unsigned char *) malloc(PATTERN_LEN);
    if(!BuffBlock) return(FALSE);

    for(ipcut = 0; ipcut < PATTERN_TRACK_LEN; ipcut += PATTERN_BYTES)
    {
        for(i = 0; i < MAX_POLYPHONY; i++)
        {
            *(BuffTrack + ipcut + PATTERN_NOTE1 + (i * 2)) = 121;
            *(BuffTrack + ipcut + PATTERN_INSTR1 + (i * 2)) = 255;
        }        
        *(BuffTrack + ipcut + PATTERN_VOLUME) = 255;
        *(BuffTrack + ipcut + PATTERN_PANNING) = 255;
        *(BuffTrack + ipcut + PATTERN_FX) = 0;
        *(BuffTrack + ipcut + PATTERN_FXDATA) = 0;
    }  

    for(ipcut = 0; ipcut < PATTERN_LEN; ipcut += PATTERN_BYTES)
    {
        for(i = 0; i < MAX_POLYPHONY; i++)
        {
            *(BuffPatt + ipcut + PATTERN_NOTE1 + (i * 2)) = 121;
            *(BuffPatt + ipcut + PATTERN_INSTR1 + (i * 2)) = 255;
        }        
        *(BuffPatt + ipcut + PATTERN_VOLUME) = 255;
        *(BuffPatt + ipcut + PATTERN_PANNING) = 255;
        *(BuffPatt + ipcut + PATTERN_FX) = 0;
        *(BuffPatt + ipcut + PATTERN_FXDATA) = 0;

        for(i = 0; i < MAX_POLYPHONY; i++)
        {
            *(BuffBlock + ipcut + PATTERN_NOTE1 + (i * 2)) = 121;
            *(BuffBlock + ipcut + PATTERN_INSTR1 + (i * 2)) = 255;
        }        
        *(BuffBlock + ipcut + PATTERN_VOLUME) = 255;
        *(BuffBlock + ipcut + PATTERN_PANNING) = 255;
        *(BuffBlock + ipcut + PATTERN_FX) = 0;
        *(BuffBlock + ipcut + PATTERN_FXDATA) = 0;
    }
    return(TRUE);
}

// ------------------------------------------------------
// Start the block marking stuff
void Mark_Block_Start(int start_nibble, int start_track, int start_line)
{
    swap_block_start_track = (start_nibble + Get_Track_Nibble_Start(start_track)) + start_track;
    swap_block_end_track = swap_block_start_track;
    swap_block_start = start_line;
    swap_block_end = swap_block_start;

    if(swap_block_end_track < swap_block_start_track) swap_block_end_track = swap_block_start_track;
    if(swap_block_end < swap_block_start) swap_block_end = swap_block_start;

    if(swap_block_end_track < 0) swap_block_end_track = 0;
    block_start_track = swap_block_start_track;
    block_end_track = swap_block_end_track;
    block_start = swap_block_start;
    block_end = swap_block_end;
    block_in_selection = TRUE;
    Actupated(0);
}

// ------------------------------------------------------
// Validate the block marking stuff
void Mark_Block_End(int start_nibble, int start_track, int start_line, int Modif)
{
    int swap_value;

    start_nibble += Get_Track_Nibble_Start(start_track);

    if(Modif & BLOCK_MARK_TRACKS)
    {
        swap_value = start_nibble + start_track;
        if(swap_block_start_track >= swap_value)
        {
            block_start_track = swap_value;
            block_end_track = swap_block_start_track;
        }
        else
        {
            block_end_track = swap_value;
            block_start_track = swap_block_start_track;
        }
        if(block_end_track < 0) block_end_track = 0;
        if(block_start_track < 0) block_start_track = 0;
    }
    if(Modif & BLOCK_MARK_ROWS)
    {
        swap_value = start_line;

        if(swap_block_start >= swap_value)
        {
            block_start = swap_value;
            block_end = swap_block_start;
        }
        else
        {
            block_end = swap_value;
            block_start = swap_block_start;
        }
    }
    Actupated(0);
}

// ------------------------------------------------------
// Blocks routines support stuff
int Corres_Table[] =
{
    0,
    1, 1, 2, 2, 3, 3, 4, 4, 5, 5
};
int Delete_Table[] =
{
    121,
    0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f,
    0, 0, 0, 0
};

int Get_Pattern_Column(int Position, int xbc, int ybc)
{
   return(*(RawPatterns + (pSequence[Position] * PATTERN_LEN) +
          (ybc * PATTERN_ROW_LEN) + ((xbc / Get_Max_Nibble_Track(xbc)) * PATTERN_BYTES) +
          Corres_Table[xbc % Get_Max_Nibble_Track(xbc)]));
}

void Set_Pattern_Column(int Position, int xbc, int ybc, int Data)
{
   *(RawPatterns + (pSequence[Position] * PATTERN_LEN) +
    (ybc * PATTERN_ROW_LEN) + ((xbc / Get_Max_Nibble_Track(xbc)) * PATTERN_BYTES) +
    Corres_Table[xbc % Get_Max_Nibble_Track(xbc)]) = Data;
}

void Set_Buff_Column(int Position, int xbc, int ybc, int Data)
{
   *(BuffBlock + (ybc * PATTERN_ROW_LEN) +
    ((xbc / Get_Max_Nibble_Track(xbc)) * PATTERN_BYTES) +
    Corres_Table[xbc % Get_Max_Nibble_Track(xbc)]) = Data;
}

int Get_Buff_Column(int Position, int xbc, int ybc)
{
   return(*(BuffBlock + (ybc * PATTERN_ROW_LEN) +
          ((xbc / Get_Max_Nibble_Track(xbc)) * Get_Max_Nibble_Track(xbc)) +
          Corres_Table[xbc % Get_Max_Nibble_Track(xbc)]));
}

int Read_Pattern_Column(int Position, int xbc, int ybc)
{
    int datas;
    int track = Get_Track_From_Nibble(xbc);

    switch(xbc % Get_Max_Nibble_Track(track))
    {
        case 0:
            datas = Get_Pattern_Column(Position, xbc, ybc);
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
            datas = Get_Pattern_Column(Position, xbc, ybc);
            datas &= 0xf0;
            break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
            datas = Get_Pattern_Column(Position, xbc, ybc);
            datas &= 0x0f;
            break;
    }
    return(datas);
}

int Write_Pattern_Column(int Position, int xbc, int ybc, int datas)
{
    int datas_nibble;
    int track = Get_Track_From_Nibble(xbc);

    switch(xbc % Get_Max_Nibble_Track(track))
    {
        case 0:
            Set_Pattern_Column(Position, xbc, ybc, datas);
            break;

        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
            datas_nibble = Get_Pattern_Column(Position, xbc, ybc);
            datas_nibble &= 0x0f;
            datas_nibble |= datas;
            Set_Pattern_Column(Position, xbc, ybc, datas_nibble);
            break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
            datas_nibble = Get_Pattern_Column(Position, xbc, ybc);
            datas_nibble &= 0xf0;
            datas_nibble |= datas;
            Set_Pattern_Column(Position, xbc, ybc, datas_nibble);
            break;
    }
    return(datas);
}

int Read_Buff_Column(int Position, int xbc, int ybc)
{
    int datas;

    switch(xbc % Get_Max_Nibble_Track(xbc))
    {
        case 0:
            datas = Get_Buff_Column(Position, xbc, ybc);
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
            datas = Get_Buff_Column(Position, xbc, ybc);
            datas &= 0xf0;
            break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
            datas = Get_Buff_Column(Position, xbc, ybc);
            datas &= 0x0f;
            break;
    }
    return(datas);
}

int Write_Buff_Column(int Position, int xbc, int ybc, int datas)
{
    int datas_nibble;

    switch(xbc % Get_Max_Nibble_Track(xbc))
    {
        case 0:
            Set_Buff_Column(Position, xbc, ybc, datas);
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
            datas_nibble = Get_Buff_Column(Position, xbc, ybc);
            datas_nibble &= 0x0f;
            datas_nibble |= datas;
            Set_Buff_Column(Position, xbc, ybc, datas_nibble);
            break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
            datas_nibble = Get_Buff_Column(Position, xbc, ybc);
            datas_nibble &= 0xf0;
            datas_nibble |= datas;
            Set_Buff_Column(Position, xbc, ybc, datas_nibble);
            break;
    }
    return(datas);
}

// ------------------------------------------------------
// Delete a selected block
int Delete_Selection(int Position)
{
    if(block_start_track != -1 && block_end_track != -1)
    {
        // Delete the entire selection
        for(int ybc = block_start; ybc < block_end + 1; ybc++)
        {
            for(int xbc = block_start_track; xbc < block_end_track+ 1; xbc++)
            {
                Write_Pattern_Column(Position, xbc, ybc, Delete_Table[xbc % Get_Max_Nibble_Track(xbc)]);
            }
        }
        return(TRUE);
    }
    return(FALSE);
}

// ------------------------------------------------------
// Copy a selected block into the secondary buffer
void Copy_Selection_To_Buffer(int Position)
{
    int axbc;
    int aybc;

    block_start_track_nibble = block_start_track;
    block_end_track_nibble = block_end_track;
    aybc = 0;
    for(int ybc = block_start; ybc < block_end + 1; ybc++)
    {
        axbc = (block_start_track_nibble % Get_Max_Nibble_Track(block_start_track_nibble));
        for(int xbc = block_start_track_nibble; xbc < block_end_track_nibble + 1; xbc++)
        {
            Write_Buff_Column(Position, axbc, aybc, Read_Pattern_Column(Position, xbc, ybc));
            axbc++;
        }
        aybc++;
    }
}

// ------------------------------------------------------
// Copy a block into a pattern
void Copy_Selection_From_Buffer(int Position)
{
    int axbc;
    int aybc;
    int start_x = (ped_track * Get_Max_Nibble_Track(ped_track)) + ped_row;
    int end_x = start_x % Get_Max_Nibble_Track(start_x);
    int min_x;
    int byte;
    int old_byte;
    int old_byte2;
    int min_x_start = (end_x - (save_block_start_track_nibble % Get_Max_Nibble_Track(save_block_start_track_nibble)));

    aybc = 0;
    for(int ybc = ped_line; ybc < ped_line + b_buff_ysize; ybc++)
    {
        axbc = end_x;
        min_x = (end_x - (save_block_start_track_nibble % Get_Max_Nibble_Track(save_block_start_track_nibble)));
        for(int xbc = start_x; xbc < ((start_x + b_buff_xsize)) - min_x_start; xbc++)
        {
            if(min_x >= 0)
            {
                if(xbc < (16 * Get_Max_Nibble_Track(xbc)) && ybc < MAX_ROWS)
                {
                    // We need to check if we're on an odd byte for the instrument/volume or panning
                    // and see if the byte was 0xff if that's the case we need to put 0 in the upper nibble
                    byte = Read_Buff_Column(Position, axbc, aybc);
                    if(start_x == xbc)
                    {
                        switch(xbc % Get_Max_Nibble_Track(xbc))
                        {
                            case 2:
                            case 4:
                            case 6:
                                old_byte = Read_Pattern_Column(Position, xbc - 1, ybc);
                                old_byte |= Read_Pattern_Column(Position, xbc, ybc);
                                old_byte2 = byte;
                                old_byte2 |= Read_Buff_Column(Position, axbc - 1, aybc);
                                if(old_byte == 0xff)
                                {
                                    if(old_byte2 != 0xff) Write_Pattern_Column(Position, xbc - 1, ybc, 0);
                                }
                                else
                                {
                                    if(old_byte2 == 0xff) byte = 0;
                                }
                                break;
                        }
                    }
                    Write_Pattern_Column(Position, xbc, ybc, byte);
                }
            }
            axbc++;
            min_x++;
        }
        aybc++;
    }
}

// ------------------------------------------------------
// Cut a selected block
void Cut_Selection(int Position)
{
    Copy_Selection_To_Buffer(Position);
    if(is_editing) Delete_Selection(Position);
    Calc_selection();
    Unselect_Selection();
    Actupated(0);
}

// ------------------------------------------------------
// Copy a selected block
void Copy_Selection(int Position)
{
    Copy_Selection_To_Buffer(Position);
    Calc_selection();
    Actupated(0);
}

// ------------------------------------------------------
// Paste a block into a pattern
void Paste_Block(int Position)
{
    Copy_Selection_From_Buffer(Position);
    Actupated(0);
}

// ------------------------------------------------------
// Interpolate a selected effects column
SELECTION Get_Real_Selection(void)
{
    SELECTION Cur_Sel;
    Cur_Sel.y_start = block_start;
    Cur_Sel.y_end = block_end;
    Cur_Sel.x_start = block_start_track;
    Cur_Sel.x_end = block_end_track;
    if(!(block_end_track - block_start_track) || !(block_end - block_start))
    {
        Cur_Sel.y_start = 0;
        Cur_Sel.y_end = patternLines[pSequence[cPosition]] - 1;
        Cur_Sel.x_start = ped_track * Get_Max_Nibble_Track(ped_track);
        Cur_Sel.x_end = (ped_track * Get_Max_Nibble_Track(ped_track)) + (Get_Max_Nibble_Track(ped_track) - 1);
    }
    return(Cur_Sel);
}

// ------------------------------------------------------
// Interpolate a selected effects column
void Interpolate_Block(int Position)
{
    int startvalue[3] = { 0, 0, 0 };
    int endvalue[3] = { 0, 0, 0 };
    int ranlen;
    int cran;
    int tran;
    int start_value;
    int end_value;
    int xbc;
    int ybc;

    SELECTION Sel = Get_Real_Selection();

    for(xbc = Sel.x_start; xbc <= Sel.x_end; xbc++)
    {
        switch(xbc % Get_Max_Nibble_Track(xbc))
        {
            case 3:
            case 4:
                startvalue[0] |= Read_Pattern_Column(Position, xbc, Sel.y_start);
                endvalue[0] |= Read_Pattern_Column(Position, xbc, Sel.y_end);
                break;

            case 5:
            case 6:
                startvalue[1] |= Read_Pattern_Column(Position, xbc, Sel.y_start);
                endvalue[1] |= Read_Pattern_Column(Position, xbc, Sel.y_end);
                break;
            case 9:
            case 10:
                startvalue[2] |= Read_Pattern_Column(Position, xbc, Sel.y_start);
                endvalue[2] |= Read_Pattern_Column(Position, xbc, Sel.y_end);
                break;
        }
    }

    for(xbc = Sel.x_start; xbc <= Sel.x_end; xbc++)
    {
        switch(xbc % Get_Max_Nibble_Track(xbc))
        {
            case 3:
            case 4:
                start_value = startvalue[0];
                end_value = endvalue[0];
                break;
            case 5:
            case 6:
                start_value = startvalue[1];
                end_value = endvalue[1];
                break;
            case 9:
            case 10:
                start_value = startvalue[2];
                end_value = endvalue[2];
                break;
        }

        switch(xbc % Get_Max_Nibble_Track(xbc))
        {
            case 3:
            case 4:
            case 5:
            case 6:
                if(start_value == 0xff) start_value = 0;
                if(end_value == 0xff) end_value = 0;
            case 9:
            case 10:
                if(start_value != 0xff || end_value != 0xff)
                {
                    ranlen = Sel.y_end - Sel.y_start;
                    if(ranlen == 0) ranlen = 1;
                    cran = 0;
                    tran = end_value - start_value;
                    for(ybc = Sel.y_start; ybc <= Sel.y_end; ybc++)
                    {
                        if(xbc < (16 * Get_Max_Nibble_Track(xbc)) && ybc < MAX_ROWS)
                        {
                            int c_val = (cran * tran) / ranlen;
                            Write_Pattern_Column(Position, xbc, ybc, start_value + c_val);
                            cran++;
                        }
                    }
                }
                break;
        }
    }
    Actupated(0);
}

// ------------------------------------------------------
// Randomize a selected effects column
void Randomize_Block(int Position)
{
    int ybc;
    int xbc;
    int value;

    SELECTION Sel = Get_Real_Selection();
    for(ybc = Sel.y_start; ybc <= Sel.y_end; ybc++)
    {
        for(xbc = Sel.x_start; xbc <= Sel.x_end; xbc++)
        {
            if(xbc < (16 * Get_Max_Nibble_Track(xbc)) && ybc < MAX_ROWS)
            {
                switch(xbc % Get_Max_Nibble_Track(xbc))
                {
                    case 3:
                    case 4:
                        Write_Pattern_Column(Position, xbc, ybc, (rand() / 128));
                        break;
                    case 5:
                        value = (rand() / 128) & 0xf0;
                        if(value > 0x90) value = 0x90;
                        if((Read_Pattern_Column(Position, xbc + 1, ybc) & 0xf) > 0 &&
                                                value == 0x90)
                        {
                            value = 0x80;
                        }
                        Write_Pattern_Column(Position, xbc, ybc, value & 0xf0);
                        break;
                    case 6:
                        value = rand() / 128;
                        if((Read_Pattern_Column(Position, xbc - 1, ybc) & 0xf0) == 0x90)
                        {
                            value = 0;
                        }
                        Write_Pattern_Column(Position, xbc, ybc, value & 0xf);
                        break;
                    case 9:
                    case 10:
                        Write_Pattern_Column(Position, xbc, ybc, (rand() / 128));
                        break;
                }
            }
        }
    }
    Actupated(0);
}

// ------------------------------------------------------
// Transpose a block to 1 semitone higher
void Semitone_Up_Block(int Position)
{
    int ybc;
    int xbc;
    int note;
    int i;
    int track;

    SELECTION Sel = Get_Real_Selection();
    for(ybc = Sel.y_start; ybc <= Sel.y_end; ybc++)
    {
        for(xbc = Sel.x_start; xbc <= Sel.x_end; xbc++)
        {
            track = Get_Track_From_Nibble(xbc);
            if(xbc < (16 * Get_Max_Nibble_Track(track)) && ybc < MAX_ROWS)
            {
                for(i = 0; i < Channels_MultiNotes[track]; i++)
                {
                    if(xbc % Get_Max_Nibble_Track(track) == (i * 3))
                    {
                        note = Read_Pattern_Column(Position, xbc, ybc);
                        if(note < 120)
                        {
                            note++;
                            if(note > 119) note = 119;
                        }
                        Write_Pattern_Column(Position, xbc, ybc, note);
                    }
                }
            }
        }
    }
    Actupated(0);
}

// ------------------------------------------------------
// Transpose a block to 1 semitone lower
void Semitone_Down_Block(int Position)
{
    int ybc;
    int xbc;
    int note;

    SELECTION Sel = Get_Real_Selection();
    for(ybc = Sel.y_start; ybc <= Sel.y_end; ybc++)
    {
        for(xbc = Sel.x_start; xbc <= Sel.x_end; xbc++)
        {
            if(xbc < (16 * Get_Max_Nibble_Track(xbc)) && ybc < MAX_ROWS)
            {
                switch(xbc % Get_Max_Nibble_Track(xbc))
                {
                    case 0:
                        note = Read_Pattern_Column(Position, xbc, ybc);
                        if(note < 120)
                        {
                            note--;
                            if(note < 0) note = 0;
                        }
                        Write_Pattern_Column(Position, xbc, ybc, note);
                        break;
                }
            }
        }
    }
    Actupated(0);
}

// ------------------------------------------------------
// Transpose a block to 1 semitone higher for the current instrument
void Instrument_Semitone_Up_Block(int Position)
{
    int ybc;
    int xbc;
    int note;
    int instrument;

    SELECTION Sel = Get_Real_Selection();
    for(ybc = Sel.y_start; ybc <= Sel.y_end; ybc++)
    {
        for(xbc = Sel.x_start; xbc <= Sel.x_end; xbc++)
        {
            if(xbc < (16 * Get_Max_Nibble_Track(xbc)) && ybc < MAX_ROWS)
            {
                switch(xbc % Get_Max_Nibble_Track(xbc))
                {
                    case 0:
                        instrument = Read_Pattern_Column(Position, xbc + 1, ybc);
                        instrument |= Read_Pattern_Column(Position, xbc + 2, ybc);
                        if(instrument == ped_patsam)
                        {
                            note = Read_Pattern_Column(Position, xbc, ybc);
                            if(note < 120)
                            {
                                note++;
                                if(note > 119) note = 119;
                            }
                            Write_Pattern_Column(Position, xbc, ybc, note);
                        }
                        break;
                }
            }
        }
    }
    Actupated(0);
}

// ------------------------------------------------------
// Transpose a block to 1 semitone lower for the current instrument
void Instrument_Semitone_Down_Block(int Position)
{
    int ybc;
    int xbc;
    int note;
    int instrument;

    SELECTION Sel = Get_Real_Selection();
    for(ybc = Sel.y_start; ybc <= Sel.y_end; ybc++)
    {
        for(xbc = Sel.x_start; xbc <= Sel.x_end; xbc++)
        {
            if(xbc < (16 * Get_Max_Nibble_Track(xbc)) && ybc < MAX_ROWS)
            {
                switch(xbc % Get_Max_Nibble_Track(xbc))
                {
                    case 0:
                        instrument = Read_Pattern_Column(Position, xbc + 1, ybc);
                        instrument |= Read_Pattern_Column(Position, xbc + 2, ybc);
                        if(instrument == ped_patsam)
                        {
                            note = Read_Pattern_Column(Position, xbc, ybc);
                            if(note < 120)
                            {
                                note--;
                                if(note < 0) note = 0;
                            }
                            Write_Pattern_Column(Position, xbc, ybc, note);
                        }
                        break;
                }
            }
        }
    }
    Actupated(0);
}

// ------------------------------------------------------
// Select a complete track
void Select_Track_Block(void)
{
    int nlines;

    if(!Songplaying)
    {
        Mark_Block_Start(0, ped_track, 0);
        nlines = patternLines[pSequence[cPosition]];
        Mark_Block_End(Get_Max_Nibble_Track(ped_track) - 1,
                       ped_track,
                       nlines,
                       BLOCK_MARK_TRACKS | BLOCK_MARK_ROWS);
    }
}

// ------------------------------------------------------
// Select a complete pattern
void Select_Pattern_Block(void)
{
    int nlines;

    if(!Songplaying)
    {
        Mark_Block_Start(0, 0, 0);
        nlines = patternLines[pSequence[cPosition]];
        Mark_Block_End(Get_Track_Nibble_Start(ped_track) - 1,
                       Songtracks - 1,
                       nlines,
                       BLOCK_MARK_TRACKS | BLOCK_MARK_ROWS);
    }
}

// ------------------------------------------------------
// Calculate the selected range
void Calc_selection(void)
{
    save_block_start_track_nibble = block_start_track_nibble;
    b_buff_xsize = (block_end_track_nibble - block_start_track_nibble) + 1;
    b_buff_ysize = (block_end - block_start) + 1;
}

// ------------------------------------------------------
// Unselect the range
void Unselect_Selection(void)
{
    // Save the size of the copied block
    block_start = 0;
    block_end = 0;
    block_start_track = -1;
    block_end_track = -1;
    block_in_selection = FALSE;
}

// ------------------------------------------------------
// Select a block via the keyboard
void Select_Block_Keyboard(int Type)
{
    if(!Songplaying)
    {
        if(Get_LShift())
        {
            if(block_in_selection == FALSE) Mark_Block_Start(ped_row, ped_track, ped_line);
            Mark_Block_End(ped_row, ped_track, ped_line, Type);
        }
        else
        {
            Unselect_Selection();
        }
    }
}

// ------------------------------------------------------
// Insert a line in a pattern
void Insert_Pattern_Line(int Position)
{
    int i;

    for(i = 0; i < Songtracks - 1; i++)
    {
        Insert_Track_Line(i, Position);
    }
}

// ------------------------------------------------------
// Insert a line in a track
void Insert_Track_Line(int track, int Position)
{
    int xoffseted;
    int i;

    for(int interval = (MAX_ROWS - 2); interval > ped_line - 1; interval--)
    {
        xoffseted = (track * PATTERN_BYTES) + (interval * PATTERN_ROW_LEN) + (pSequence[Position] * PATTERN_LEN);

        for(i = 0; i < MAX_POLYPHONY; i++)
        {
            *(RawPatterns + xoffseted + PATTERN_ROW_LEN + PATTERN_NOTE1 + (i * 2)) = *(RawPatterns + xoffseted + PATTERN_NOTE1 + (i * 2));
            *(RawPatterns + xoffseted + PATTERN_ROW_LEN + PATTERN_INSTR1 + (i * 2)) = *(RawPatterns + xoffseted + PATTERN_INSTR1 + (i * 2));
        }
        *(RawPatterns + xoffseted + PATTERN_ROW_LEN + PATTERN_VOLUME) = *(RawPatterns + xoffseted + PATTERN_VOLUME);
        *(RawPatterns + xoffseted + PATTERN_ROW_LEN + PATTERN_PANNING) = *(RawPatterns + xoffseted + PATTERN_PANNING);
        *(RawPatterns + xoffseted + PATTERN_ROW_LEN + PATTERN_FX) = *(RawPatterns + xoffseted + PATTERN_FX);
        *(RawPatterns + xoffseted + PATTERN_ROW_LEN + PATTERN_FXDATA) = *(RawPatterns + xoffseted + PATTERN_FXDATA);
    }
    xoffseted = Get_Pattern_Offset(pSequence[Position], track, ped_line);
 
    Clear_Track_Data(xoffseted);
    Actupated(0);
}

// ------------------------------------------------------
// Insert a line in a pattern
void Remove_Pattern_Line(int Position)
{
    int i;

    for(i = 0; i < Songtracks - 1; i++)
    {
        Remove_Track_Line(i, Position);
    }
}

// ------------------------------------------------------
// Remove a line from a track
void Remove_Track_Line(int track, int Position)
{
    int xoffseted;
    int xoffseted2;
    int i;

    for(int interval = ped_line + 1; interval < MAX_ROWS; interval++)
    {
        xoffseted = Get_Pattern_Offset(pSequence[Position], track, interval);
        xoffseted2 = Get_Pattern_Offset(pSequence[Position], track, interval) - PATTERN_ROW_LEN;
        
        for(i = 0; i < MAX_POLYPHONY; i++)
        {
            *(RawPatterns + xoffseted2 + PATTERN_NOTE1 + (i * 2)) = *(RawPatterns + xoffseted + PATTERN_NOTE1 + (i * 2));
            *(RawPatterns + xoffseted2 + PATTERN_INSTR1 + (i * 2)) = *(RawPatterns + xoffseted + PATTERN_INSTR1 + (i * 2));
        }
        *(RawPatterns + xoffseted2 + PATTERN_VOLUME) = *(RawPatterns + xoffseted + PATTERN_VOLUME);
        *(RawPatterns + xoffseted2 + PATTERN_PANNING) = *(RawPatterns + xoffseted + PATTERN_PANNING);
        *(RawPatterns + xoffseted2 + PATTERN_FX) = *(RawPatterns + xoffseted + PATTERN_FX);
        *(RawPatterns + xoffseted2 + PATTERN_FXDATA) = *(RawPatterns + xoffseted + PATTERN_FXDATA);
    }

    xoffseted = Get_Pattern_Offset(pSequence[Position], track, 0) + (PATTERN_LEN - PATTERN_ROW_LEN);

    Clear_Track_Data(xoffseted);

    Actupated(0);
}

// ------------------------------------------------------
// Clear all patterns
void Clear_Patterns_Pool(void)
{
    for(int i = 0; i < PATTERN_POOL_SIZE; i += PATTERN_BYTES)
    {
        Clear_Track_Data(i);
    }
}

// ------------------------------------------------------
// Init the data of a note
void Clear_Track_Data(int offset)
{
    int i;

    for(i = 0; i < MAX_POLYPHONY; i++)
    {
        *(RawPatterns + offset + PATTERN_NOTE1 + (i * 2)) = 121;
        *(RawPatterns + offset + PATTERN_INSTR1 + (i * 2)) = 255;
    }
    *(RawPatterns + offset + PATTERN_VOLUME) = 255;
    *(RawPatterns + offset + PATTERN_PANNING) = 255;
    *(RawPatterns + offset + PATTERN_FX) = 0;
    *(RawPatterns + offset + PATTERN_FXDATA) = 0;
}

// ------------------------------------------------------
// Create a new pattern
int Alloc_Patterns_Pool(void)
{
    for(int api = 0; api < MAX_ROWS; api++)
    {
        patternLines[api] = DEFAULT_PATTERN_LEN;
    }

    nPatterns = 1;

    if((RawPatterns = (unsigned char *) malloc(PATTERN_POOL_SIZE)) != NULL)
    { 
        Clear_Patterns_Pool();
        return TRUE;
    }
    return FALSE;
}

// ------------------------------------------------------
// Return the number of nibbles in a track
int Get_Max_Nibble_Track(int track)
{
    return((Channels_MultiNotes[track] * 3) + 8);
}

// ------------------------------------------------------
// Return the real nibble from where a track is starting
int Get_Track_Nibble_Start(int track)
{
    int i;
    int column = 0;

    for(i = 0; i < track; i++)
    {
        column += Get_Max_Nibble_Track(i) - 1;
    }
    return(column);
}

// ------------------------------------------------------
// Return the index of a track from a nibble/Column
int Get_Track_From_Nibble(int nibble)
{
    int i;
    int min_nibble = 0;
    int max_nibble = 0;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        max_nibble += Get_Max_Nibble_Track(i);
        if(nibble >= min_nibble && nibble < max_nibble)
        {
            return(i);
        }
        min_nibble = max_nibble;
    }
    return(0);
}

