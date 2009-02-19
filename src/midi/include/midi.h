// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _CAPTURE_MIDI_H_
#define _CAPTURE_MIDI_H_

#if !defined(__NO_MIDI__)

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
void Midi_AllNotesOff(void);
void Midi_Reset(void);

void Midi_GetAll(void);
void Midi_FreeAll(void);

void Midi_InitIn(void);
void Midi_CloseIn(void);
void Midi_InitOut(void);
void Midi_CloseOut(void);

void Midi_NoteOff(int channel, int note);
void Midi_Send(int nbr_track, int eff_dat, int row_dat);

char *Midi_GetInName(void);
char *Midi_GetOutName(void);

#endif

#endif
