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
extern HMIDIIN midiin_handle;
extern HMIDIOUT midiout_handle;

extern MIDIINCAPS caps_midiin[255];
extern MIDIOUTCAPS caps_midiout[255];

// ------------------------------------------------------
// Functions
void MidiIn_Init(void);
void MidiOut_Init(void);

#endif

#endif
