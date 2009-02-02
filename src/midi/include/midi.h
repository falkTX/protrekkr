// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _CAPTURE_MIDI_H_
#define _CAPTURE_MIDI_H_

#if !defined(__NOMIDI__)

// ------------------------------------------------------
// Includes
#if defined(__WIN32__)
#include <Windows.h>
#include <mmsystem.h>
#endif

// ------------------------------------------------------
// Variables
extern signed char n_midioutdevices;
extern signed char n_midiindevices;

extern int midiin_changed;
extern int midiout_changed;

// ------------------------------------------------------
// Functions
void MidiGetAll(void);
void MidiFreeAll(void);

void MidiIn_Init(void);
void MidiIn_Close(void);
void MidiOut_Init(void);
void MidiOut_Close(void);

void MidiNoteOff(int track);
void MidiSend(int nbr_track, int eff_dat, int row_dat);

char *GetMidiInName(void);
char *GetMidiOutName(void);

#endif

#endif
