// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _PATTERNS_BLOCKS_H_
#define _PATTERNS_BLOCKS_H_

// ------------------------------------------------------
// Functions
int Init_Block_Work(void);
void Mark_Block_Start(int start_nibble, int start_track, int start_line);
void Mark_Block_End(int start_nibble, int start_track, int start_line, int Modif);
int Delete_Selection(int Position);
void Copy_Selection_To_Buffer(int Position);
void Copy_Selection_From_Buffer(int Position);
void Cut_Selection(int Position);
void Copy_Selection(int Position);
void Paste_Block(int Position);
void Interpolate_Block(int Position);
void Randomize_Block(int Position);
void Seminote_Up_Block(int Position);
void Seminote_Down_Block(int Position);
void Instrument_Seminote_Up_Block(int Position);
void Instrument_Seminote_Down_Block(int Position);
void Select_Track_Block(void);
void Select_Pattern_Block(void);
void Calc_selection(void);
void Unselect_Selection(void);
void Select_Block_Keyboard(int Type);
void Insert_Pattern_Line(int Position);
void Insert_Track_Line(int track, int Position);
void Remove_Pattern_Line(int Position);
void Remove_Track_Line(int track, int Position);

#endif
