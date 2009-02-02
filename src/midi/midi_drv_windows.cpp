// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#if !defined(__NOMIDI__)

// ------------------------------------------------------
// Includes
#include "../include/main.h"

#include "include/midi.h"

// ------------------------------------------------------
// Variables
HMIDIOUT midiout_handle = NULL;
HMIDIIN midiin_handle = NULL;

MIDIINCAPS *caps_midiin;
MIDIOUTCAPS *caps_midiout;

int midiin_changed = 0;
int midiout_changed = 0;

signed char c_midiin = -1;
signed char c_midiout = -1;

signed char n_midioutdevices = 0;
signed char n_midiindevices = 0;

extern int Midi_Track_Notes[MAX_TRACKS];

void MidiIncallBack(Uint32 dwParam1, Uint32 dwParam2);

void CALLBACK MidiInProc(HMIDIIN hMidiIn,
                         UINT wMsg,
                         Uint32 dwInstance,
                         Uint32 dwParam1,
                         Uint32 dwParam2)
{
    MidiIncallBack(dwParam1, dwParam2);
}

// ------------------------------------------------------
// Open the midi in device
void MidiIn_Init(void)
{
    if(midiin_changed != 0)
    {
        if(c_midiin < -1) c_midiin = n_midiindevices - 1;
        if(c_midiin == n_midiindevices) c_midiin = -1;

        if(c_midiin != -1)
        {
            MidiIn_Close();
            if(midiInOpen(&midiin_handle, c_midiin, (Uint32) &MidiInProc, 0xdeadc0fe, CALLBACK_FUNCTION | MIDI_IO_STATUS) == MMSYSERR_NOERROR)
            {
                midiInStart(midiin_handle);
                if(midiin_changed == 1) mess_box("Midi In device activated...");
            }
            else
            {
                c_midiin = -1;
                if(midiin_changed == 1) mess_box("Midi In device failed to open...");
            }
        }
        else
        {
            if(midiin_changed == 1) mess_box("Midi In device disconnected...");
        }
        midiin_changed = 0;
    }
}

// ------------------------------------------------------
// Close the midi in device
void MidiIn_Close(void)
{
    if(midiin_handle != NULL)
    {
        midiInStop(midiin_handle);
        midiInClose(midiin_handle);
        midiin_handle = NULL;
    }
}

// ------------------------------------------------------
// Open the midi out device
void MidiOut_Init(void) {
    if(midiout_changed != 0)
    {
        MidiReset();
        if(c_midiout < -1) c_midiout = n_midioutdevices - 1;
        if(c_midiout == n_midioutdevices) c_midiout = -1;

        if(c_midiout != -1)
        {
            MidiOut_Close();
            if(midiOutOpen(&midiout_handle, c_midiout, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
            {
                if(midiout_changed == 1) mess_box("Midi Out device activated...");
            }
            else
            {
                c_midiout = -1;
                if(midiout_changed == 1) mess_box("Midi Out device failed to open...");
            }
        }
        else
        {
            if(midiout_changed == 1) mess_box("Midi Out device disconnected...");
        }
        midiout_changed = 0;
    }
}

// ------------------------------------------------------
// Close the midi out device
void MidiOut_Close(void)
{
    if(midiout_handle != NULL) midiOutClose(midiout_handle);
}

// ------------------------------------------------------
// Retrieve all midi in/out interfaces
void MidiGetAll(void)
{
    int m;

    n_midiindevices = midiInGetNumDevs();
    n_midioutdevices = midiOutGetNumDevs();

    caps_midiin = (MIDIINCAPS *) malloc(n_midiindevices * sizeof(MIDIINCAPS));
    if(caps_midiin)
    {
        for(m = 0; m < n_midiindevices; m++)
        {
            midiInGetDevCaps(m, &caps_midiin[m], sizeof(MIDIINCAPS));
        }
    }

    caps_midiout = (MIDIOUTCAPS *) malloc(n_midioutdevices * sizeof(MIDIOUTCAPS));
    if(caps_midiout)
    {
        for(m = 0; m < n_midioutdevices; m++)
        {
            midiOutGetDevCaps(m, &caps_midiout[m], sizeof(MIDIOUTCAPS));
        }
    }
}

// ------------------------------------------------------
// Free allocated interfaces resources
void MidiFreeAll(void)
{
    if(caps_midiin) free(caps_midiin);
    if(caps_midiout) free(caps_midiout);
}

// ------------------------------------------------------
// Turn a midi channel off
void MidiNoteOff(int track)
{
    if(c_midiout != -1)
    {
        midiOutShortMsg(midiout_handle, (176 + TRACKMIDICHANNEL[track]) | (0x40 << 8) | (0 << 16)); 
        midiOutShortMsg(midiout_handle, (0x80 + TRACKMIDICHANNEL[track]) | (Midi_Track_Notes[TRACKMIDICHANNEL[track]] << 8) | (127 << 16)); 
    }
}

// ------------------------------------------------------
// Send a command to the midi out device
void MidiSend(int nbr_track, int eff_dat, int row_dat)
{
    if(c_midiout != -1)
    {
        midiOutShortMsg(midiout_handle, nbr_track | (eff_dat << 8) | (row_dat << 16)); 
    }
}

// ------------------------------------------------------
// Return the name of the current midi in device
char *GetMidiInName(void)
{
    if(c_midiin == -1) return("");
    return(caps_midiin[c_midiin].szPname);
}

// ------------------------------------------------------
// Return the name of the current midi out device
char *GetMidiOutName(void)
{
    if(c_midiout == -1) return("");
    return(caps_midiout[c_midiout].szPname);
}

#endif
