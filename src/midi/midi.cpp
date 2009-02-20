// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#if !defined(__NO_MIDI__)

// ------------------------------------------------------
// Includes
#include "../include/Variables.h"
#include "../include/ptk.h"

#include "include/midi.h"

// ------------------------------------------------------
// Variables
extern int Nbr_Sub_NoteOff;
extern int key_record_first_time;
extern int old_key_ped_line;

int Midi_Notes_History[MAX_TRACKS][256];
int Midi_Current_Notes[MAX_TRACKS][MAX_POLYPHONY];
int Midi_Notes_History_Amount;

// ------------------------------------------------------
// Driver functions
void _Midi_Send(int nbr_track, int eff_dat, int row_dat);

// ------------------------------------------------------
// Return the instrument associated with the midi program
int Midi_GetProgram(int midi_program)
{
    int i;

    for(i = 0; i < 128; i++)
    {
        if(midi_program == Midiprg[i]) return(i);
    }
    return(-1);
}

// ------------------------------------------------------
// Handle the midi events
void Midi_CallBackIn(Uint32 dwParam1)
{
    int Midi_Channel_Number;
    int Midi_Command;
    int Midi_Datas_1;
    int Midi_Datas_2;
    int Midi_Velocity;
    int Instrument_Number; 
    int tmp_note;
    int Unknown_Message;
    Uint32 Param1 = dwParam1;

    Midi_Channel_Number = Param1 & 0xf;
    Midi_Command = Param1 & 0xf0;
    if(Midi_Command == 0x90 || Midi_Command == 0x80)
    {
        if(is_recording && !is_recording_2 && key_record_first_time)
        {
            // Start recording
            is_recording_2 = 1;
            Nbr_Sub_NoteOff = 0;
            is_record_key = FALSE;
            is_editing = TRUE;
            Songplaying = TRUE;
            ped_line_delay = ped_line;
            key_record_first_time = FALSE;
            old_key_ped_line = ped_line;
            Clear_Midi_Channels_Pool();
        }
    }
    switch(Midi_Command)
    {
        // Control Change
        case 0xb0:
            Midi_Datas_1 = (Param1 >> 8) & 0xff;
            Midi_Datas_2 = (Param1 >> 16) & 0xff;
            switch(Midi_Datas_1)
            {
                case 0x40:
                    // Sustain pedal
                    break;

                case 0x7:
                    // Master volume
                    mas_vol = ((float) Midi_Datas_2 / 128.0f);
                    if(mas_vol < 0.01f) mas_vol = 0.01f;
                    if(mas_vol > 1.0f) mas_vol = 1.0f;
                    Display_Master_Volume();
                    break;

                default:
                    Unknown_Message = Param1;
                    break;
            }
            break;

        // Program Change
        case 0xc0:
            Instrument_Number = Midi_GetProgram((Param1 >> 8) & 0xff);
            if(Instrument_Number > -1)
            {
                ped_patsam = Instrument_Number;
                if(snamesel == 2) snamesel = 0;
                Actualize_Patterned();
                RefreshSample();
                NewWav();
                Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
            }
            break;

        // Note Off
        case 0x80:
            // Make sure the velocity is 0
            Param1 &= 0x00ffff;

        // Note On
        case 0x90:

            Midi_Velocity = (Param1 >> 16) & 0xff;
            if(Midi_Velocity > 0xfe) Midi_Velocity = 0xfe;
            tmp_note = (Param1 >> 8) & 0xff;
            if(tmp_note > NOTE_MAX) tmp_note = NOTE_MAX;

            if(Midi_Velocity)
            {
                if(is_recording_2 || is_editing)
                {
                    Send_Note(tmp_note, TRUE, FALSE);
                }
                if(is_recording_2 || !is_editing)
                { 
                    Note_Jazz(ped_track, tmp_note);
                }
            }
            else
            {
                // A note off
                if(is_recording_2 || is_editing)
                {
                    Send_Note(tmp_note | 0x80, TRUE, FALSE);
                }
                if(is_recording_2 || !is_editing)
                {
                    Note_Jazz_Off(tmp_note);
                }
            }
            break;

        default:

            Unknown_Message = Param1;
            break;
    }
}

// ------------------------------------------------------
// Turn all midi notes off
void Midi_AllNotesOff(void)
{
    if(c_midiout != -1)
    {
        for(int no_track = 0; no_track < MAX_TRACKS; no_track++)
        {
            Midi_NoteOff(no_track, -1);
        }
    }
}

// ------------------------------------------------------
// Reset midi out device
void Midi_Reset(void)
{
    Midi_AllNotesOff();

    for(int mreset = 0; mreset < 16; mreset++)
    {
        LastProgram[mreset] = -1;
    }
}

// ------------------------------------------------------
// Send a command to the midi out device
void Midi_Send(int nbr_track, int eff_dat, int row_dat)
{
    int i;

    if(c_midiout != -1)
    {
        if((nbr_track & 0xfff0) == 144)
        {
            for(i = 0; i < 256; i++)
            {
                // First empty channel
                if(Midi_Notes_History[nbr_track & 0xf][i] == 0)
                {
                    Midi_Notes_History[nbr_track & 0xf][i] = eff_dat + 1;
                    break;
                }
            }
        }
        _Midi_Send(nbr_track, eff_dat, row_dat);
    }
}

// ------------------------------------------------------
// Turn a midi channel off
void Midi_NoteOff(int channel, int note)
{
    int i;

    if(c_midiout != -1)
    {
        note++;
        _Midi_Send(176 + CHAN_MIDI_PRG[channel], 0x40, 0);
        if(note != -1)
        {
            for(i = 0; i < 256; i++)
            {
                if(Midi_Notes_History[CHAN_MIDI_PRG[channel]][i] == note)
                {
                    _Midi_Send(0x80 + CHAN_MIDI_PRG[channel], (note - 1), 127);
                    Midi_Notes_History[CHAN_MIDI_PRG[channel]][i] = 0;
                    break;
                }
            }
        }
        else
        {
            for(i = 0; i < 256; i++)
            {
                _Midi_Send(0x80 + CHAN_MIDI_PRG[channel], (Midi_Notes_History[CHAN_MIDI_PRG[channel]][i] - 1), 127);
                Midi_Notes_History[CHAN_MIDI_PRG[channel]][i] = 0;
            }
            Midi_Notes_History_Amount = 0;
        }
    }
}

#endif
