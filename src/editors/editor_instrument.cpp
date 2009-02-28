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
#include "include/editor_instrument.h"
#include "include/editor_sample.h"
#include "../files/include/samples_pack.h"
#include "../files/include/files_list.h"

#include "../../release/distrib/replay/lib/include/samples_unpack.h"

#include "../../release/distrib/replay/lib/include/endianness.h"

// ------------------------------------------------------
// Variables
int Allow_Buttons;
int Allow_Sliders;
int Allow_Buttons_Pushed;
int Allow_Global;
int Allow_Global_Pushed;
int Allow_Global_Sliders;

int Instrs_ykar = 0;
int Instrs_items = 128;
int Instrs_index = 0;
int Instrs_curr = 0;
int Instrs_last_index = -1;

extern char Mp3_BitRate[MAX_INSTRS];
extern int Type_Mp3_BitRate[];
extern char At3_BitRate[MAX_INSTRS];
extern int Type_At3_BitRate[];

void Lock_Sample(int instr_nbr, int split);

// ------------------------------------------------------
// Functions
void set_instr_global(void)
{
    int i;
    Allow_Global = BUTTON_NORMAL | BUTTON_DISABLED;
    Allow_Global_Pushed = BUTTON_PUSHED | BUTTON_DISABLED;
    Allow_Global_Sliders = FALSE;
    
    for(i = 0; i < 16; i++)
    {
        if(SampleType[ped_patsam][i])
        {
            Allow_Global = BUTTON_NORMAL;
            Allow_Global_Pushed = BUTTON_PUSHED;
            Allow_Global_Sliders = TRUE;
            return;
        }
    }
}

void Draw_Instrument_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_INSTRUMENT_EDIT);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);

    switch(seditor)
    {
        case 0:
            Gui_Draw_Flat_Box("Instrument Editor [Sampler]");

#if !defined(__NO_CODEC__)
            Gui_Draw_Button_Box(640, 466, 80, 16, "Packing", BUTTON_NORMAL | BUTTON_DISABLED);
#endif

            Gui_Draw_Button_Box(8, 528, 44, 16, "F.Decay", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 546, 44, 16, "Def.Vol", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 492, 134, 16, "Loop Tempo Auto Sync.", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 510, 134, 16, "Synchro Length (In Lines)", BUTTON_NORMAL | BUTTON_DISABLED);

            Gui_Draw_Button_Box(382, 538, 44, 16, "Amplify", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(382, 556, 44, 16, "Tune", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(382, 466, 64, 16, "Length", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(382, 484, 64, 16, "Loop Start", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(382, 502, 64, 16, "Loop End", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(382, 520, 64, 16, "Loop Type", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(510, 466, 58, 16, "Split", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(510, 484, 58, 16, "Base Note", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(510, 502, 58, 16, "Midi Prg.", BUTTON_NORMAL | BUTTON_DISABLED);

#if !defined(__NO_CODEC__)
            Gui_Draw_Button_Box(729, 466, 60, 16, "Mp3 BitRate", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(729, 506, 60, 16, "At3 BitRate", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
#endif

            break;

        case 1:
            Gui_Draw_Flat_Box("Instrument Editor [Loop Editor]");
            Gui_Draw_Button_Box(424, 470, 58, 16, "LS. Value", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(424, 488, 58, 16, "LE. Value", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(20, 470, 56, 16, "Loop Start", BUTTON_NORMAL | BUTTON_DISABLED);
            break;
    } //Switch Sampler Screen
}

void Actualize_Instrument_Ed(int typex, char gode)
{
    int FineTune_Value;
    Check_Loops();

    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT)
    {
        if(SampleType[ped_patsam][ped_split])
        {
            Allow_Buttons = BUTTON_NORMAL;
            Allow_Buttons_Pushed = BUTTON_PUSHED;
            Allow_Sliders = TRUE;
        }
        else
        {
            Allow_Buttons = BUTTON_NORMAL | BUTTON_DISABLED;
            Allow_Buttons_Pushed = BUTTON_PUSHED | BUTTON_DISABLED;
            Allow_Sliders = FALSE;
        }
        set_instr_global();

        switch(seditor)
        {
            case 0:
                Gui_Draw_Button_Box(268, 466, 88, 16, "Fine Loop Editor", Allow_Buttons);
                Gui_Draw_Button_Box(268, 492, 88, 16, "Save Instrument", Allow_Global);
                Gui_Draw_Button_Box(268, 510, 88, 16, "Export .Wav File", Allow_Buttons);

                // Instrument editor mode
                if(gode == 0 || gode == 1)
                {
                    if(SampleVol[ped_patsam][ped_split] > 4.0f) SampleVol[ped_patsam][ped_split] = 4.0f;
                    if(SampleVol[ped_patsam][ped_split] < 0) SampleVol[ped_patsam][ped_split] = 0;
                    Realslider(426, 538, (int) (SampleVol[ped_patsam][ped_split] * 32.0f), Allow_Sliders);
                    outlong_small(575, 538, (int) (SampleVol[ped_patsam][ped_split] * 100.0f), 1, 56, BUTTON_NORMAL | BUTTON_DISABLED);
                }

                if(gode == 0 || gode == 2)
                {
                    FineTune_Value = Finetune[ped_patsam][ped_split];
                    if(FineTune_Value > 127)
                    {
                        Finetune[ped_patsam][ped_split] = 127;
                    }
                    if(FineTune_Value < -127)
                    {
                        Finetune[ped_patsam][ped_split] = -127;
                    }
                    Realslider(426, 556, 64 + (Finetune[ped_patsam][ped_split] / 2), Allow_Sliders);
                    outlong_small(575, 556, (long) Finetune[ped_patsam][ped_split], 0, 56, BUTTON_NORMAL | BUTTON_DISABLED);
                }

                if(gode == 0 || gode == 3)
                {
                    if(FDecay[ped_patsam][ped_split] > 0.015625f) FDecay[ped_patsam][ped_split] = 0.015625f;
                    if(FDecay[ped_patsam][ped_split] < 0.0f) FDecay[ped_patsam][ped_split] = 0.0f;
                    Realslider(52, 528, (int) (FDecay[ped_patsam][ped_split] * 8192.0f), Allow_Sliders);
                    outlong(201, 528, (int) (FDecay[ped_patsam][ped_split] * 8192.0f), 0);
                }

                char temprout[256];
                if(gode == 0)
                {
                    if(typex == 0 || typex == 1 || typex == 2)
                    {
                        if(SampleType[ped_patsam][ped_split] != 0)
                        {
                            if(SampleChannels[ped_patsam][ped_split] == 2)
                            {
                                sprintf(temprout, "%s [Stereo]", SampleName[ped_patsam][ped_split]);
                            }
                            else
                            {
                                sprintf(temprout, "%s [Mono]", SampleName[ped_patsam][ped_split]);
                            }
                        }
                        else
                        {
                            sprintf(temprout, "No Sample Loaded");
                        }
                        Gui_Draw_Button_Box(7, 466, 256, 16, temprout, BUTTON_NORMAL | BUTTON_DISABLED);
                    }
                } // typex

                if(gode == 0 || gode == 4)
                {
                    outlong(448, 466, SampleNumSamples[ped_patsam][ped_split], 0);
                }

                if(gode == 0 || gode == 5)
                {
                    outlong(448, 484, LoopStart[ped_patsam][ped_split], 0);
                    outlong(448, 502, LoopEnd[ped_patsam][ped_split], 0);

                    switch(LoopType[ped_patsam][ped_split])
                    {
                        case SMP_LOOP_FORWARD:
                            Gui_Draw_Button_Box(448, 520, 60, 16, "Forward", Allow_Buttons_Pushed);
                            Gui_Draw_Button_Box(448 + 62, 520, 58, 16, "Ping Pong", Allow_Buttons);
                            Gui_Draw_Button_Box(448 + 62 * 2 - 2, 520, 60, 16, "Off", Allow_Buttons);
                            break;

                        case SMP_LOOP_PINGPONG:
                            Gui_Draw_Button_Box(448, 520, 60, 16, "Forward", Allow_Buttons);
                            Gui_Draw_Button_Box(448 + 62, 520, 58, 16, "Ping Pong", Allow_Buttons_Pushed);
                            Gui_Draw_Button_Box(448 + 62 * 2 - 2, 520, 60, 16, "Off", Allow_Buttons);
                            break;

                        case SMP_LOOP_NONE:
                            Gui_Draw_Button_Box(448, 520, 60, 16, "Forward", Allow_Buttons);
                            Gui_Draw_Button_Box(448 + 62, 520, 58, 16, "Ping Pong", Allow_Buttons);
                            Gui_Draw_Button_Box(448 + 62 * 2 - 2, 520, 60, 16, "Off", Allow_Buttons_Pushed);
                            break;
                    }
                }

                if(gode == 0 || gode == 8)
                {
                    value_box(570, 466, ped_split, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    value_box3(570, 484, Basenote[ped_patsam][ped_split], Allow_Buttons | BUTTON_TEXT_CENTERED);
                }

                if(gode == 0 || gode == 9)
                {
                    value_box3(570, 484, Basenote[ped_patsam][ped_split], Allow_Buttons | BUTTON_TEXT_CENTERED);
                }
                if(gode == 0 || gode == 10)
                {

#if !defined(__NO_MIDI__)
                    if(Midiprg[ped_patsam] == -1)
                    {
                        Gui_Draw_Button_Box(570, 502, 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                        Gui_Draw_Button_Box(570 + 44, 502, 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#else
                        Gui_Draw_Button_Box(570, 502, 16, 16, "\03", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
                        Gui_Draw_Button_Box(570 + 44, 502, 16, 16, "\04", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
#endif

                        Gui_Draw_Button_Box(570 + 18, 502, 24, 16, "-", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);

#if !defined(__NO_MIDI__)
                    }
                    else
                    {
                        Gui_Draw_Arrows_Number_Box(570, 502, Midiprg[ped_patsam], BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    }

                    Actualize_Instruments_Synths_List(1);
#endif
                }
                if(gode == 0 || gode == 12)
                {
                    if(beatsync[ped_patsam])
                    {
                        Gui_Draw_Button_Box(144, 492, 28, 16, "Off", Allow_Global);
                        Gui_Draw_Button_Box(176, 492, 28, 16, "On", Allow_Global_Pushed);
                    }
                    else
                    {
                        Gui_Draw_Button_Box(144, 492, 28, 16, "Off", Allow_Global_Pushed);
                        Gui_Draw_Button_Box(176, 492, 28, 16, "On", Allow_Global);
                    }
                }

                if(gode == 0 || gode == 13)
                {
                    Gui_Draw_Arrows_Number_Box(144, 510, beatlines[ped_patsam], Allow_Global | BUTTON_TEXT_CENTERED);
                }

                if(gode == 14)
                {
                    char buffer[64];

                    mess_box("Writing Wav Header And Sample Data...");

                    WaveFile RF;

                    if(strlen(SampleName[ped_patsam][ped_split]))
                    {
                        RF.OpenForWrite(SampleName[ped_patsam][ped_split],
                                        44100,
                                        16,
                                        SampleChannels[ped_patsam][ped_split]);
                    }
                    else
                    {
                        RF.OpenForWrite("Untitled.wav",
                                        44100,
                                        16,
                                        SampleChannels[ped_patsam][ped_split]);
                    }

                    char t_stereo;

                    if(SampleChannels[ped_patsam][ped_split] == 1) t_stereo = FALSE;
                    else t_stereo = TRUE;

                    Uint32 woff = 0;

                    short *eSamples = RawSamples[ped_patsam][0][ped_split];
                    short *erSamples = RawSamples[ped_patsam][1][ped_split];

                    while(woff < SampleNumSamples[ped_patsam][ped_split])
                    {
                        if(t_stereo) RF.WriteStereoSample(*eSamples++, *erSamples++);
                        else RF.WriteMonoSample(*eSamples++);
                        woff++;
                    }

                    // Write the looping info
                    if(LoopType[ped_patsam][ped_split])
                    {
                        RiffChunkHeader header;
                        WaveSmpl_ChunkData datas;

                        header.ckID = FourCC("smpl");
                        header.ckSize = 0x3c;

                        memset(&datas, 0, sizeof(datas));
                        datas.Num_Sample_Loops = 1;
                        datas.Start = LoopStart[ped_patsam][ped_split];
                        datas.End = LoopEnd[ped_patsam][ped_split];

                        header.ckSize = Swap_32(header.ckSize);

                        datas.Num_Sample_Loops = Swap_32(datas.Num_Sample_Loops);
                        datas.Start = Swap_32(datas.Start);
                        datas.End = Swap_32(datas.End);

                        RF.WriteData((void *) &header, sizeof(header));
                        RF.WriteData((void *) &datas, sizeof(datas));
                    }

                    RF.Close();
                    if(strlen(SampleName[ped_patsam][ped_split])) sprintf(buffer, "File '%s' saved...", SampleName[ped_patsam][ped_split]);
                    else sprintf(buffer, "File 'Untitled.wav' saved...");
                    mess_box(buffer);

                    Read_SMPT();
                    last_index = -1;
                    Actualize_Files_List(0);
                }

                if(gode == 0 || gode == 15)
                {
                    if(CustomVol[ped_patsam] > 1.0f) CustomVol[ped_patsam] = 1.0f;
                    if(CustomVol[ped_patsam] < 0.0f) CustomVol[ped_patsam] = 0.0f;
                    Realslider(52, 546, (int) (CustomVol[ped_patsam] * 128.0f), Allow_Global_Sliders);
                    outlong(201, 546, (int) (CustomVol[ped_patsam] * 100.0f), 1);
                }

                if(gode == 0 || gode == 16)
                {
                    switch(SampleCompression[ped_patsam])
                    {

#if !defined(__NO_CODEC__)
                        case SMP_PACK_GSM:
                            Gui_Draw_Button_Box(640, 484, 39, 16, "Gsm", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(681, 484, 39, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 1), 80, 16, "ADPCM", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 80, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 80, 16, "At3 (PSP only)", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 80, 16, "None", Allow_Global);
                            break;
                        case SMP_PACK_MP3:
                            Gui_Draw_Button_Box(640, 484, 39, 16, "Gsm", Allow_Global);
                            Gui_Draw_Button_Box(681, 484, 39, 16, "Mp3", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(640, 484 + (18 * 1), 80, 16, "ADPCM", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 80, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 80, 16, "At3 (PSP only)", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 80, 16, "None", Allow_Global);
                            break;
                        case SMP_PACK_ADPCM:
                            Gui_Draw_Button_Box(640, 484, 39, 16, "Gsm", Allow_Global);
                            Gui_Draw_Button_Box(681, 484, 39, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 1), 80, 16, "ADPCM", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 80, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 80, 16, "At3 (PSP only)", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 80, 16, "None", Allow_Global);
                            break;
                        case SMP_PACK_TRUESPEECH:
                            Gui_Draw_Button_Box(640, 484, 39, 16, "Gsm", Allow_Global);
                            Gui_Draw_Button_Box(681, 484, 39, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 1), 80, 16, "ADPCM", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 80, 16, "True Speech", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 80, 16, "At3 (PSP only)", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 80, 16, "None", Allow_Global);
                            break;
                        case SMP_PACK_AT3:
                            Gui_Draw_Button_Box(640, 484, 39, 16, "Gsm", Allow_Global);
                            Gui_Draw_Button_Box(681, 484, 39, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 1), 80, 16, "ADPCM", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 80, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 80, 16, "At3 (PSP only)", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 80, 16, "None", Allow_Global);
                            break;
                        case SMP_PACK_NONE:
                            Gui_Draw_Button_Box(640, 484, 39, 16, "Gsm", Allow_Global);
                            Gui_Draw_Button_Box(681, 484, 39, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 1), 80, 16, "ADPCM", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 80, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 80, 16, "At3 (PSP only)", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 80, 16, "None", Allow_Global_Pushed);
                            break;
#endif

                    }
                    if(gode == 16) gode = 19;
                }

#if !defined(__NO_CODEC__)
                if(gode == 0 || gode == 17)
                {
                    if(Mp3_BitRate[ped_patsam] < 0) Mp3_BitRate[ped_patsam] = 0;
                    if(Mp3_BitRate[ped_patsam] > 5) Mp3_BitRate[ped_patsam] = 5;
                    Gui_Draw_Arrows_Number_Box(729, 484, Type_Mp3_BitRate[Mp3_BitRate[ped_patsam]], Allow_Global | BUTTON_TEXT_CENTERED);
                    if(gode == 17) gode = 19;
                }
#endif

#if !defined(__NO_CODEC__)
                if(gode == 0 || gode == 18)
                {
                    if(At3_BitRate[ped_patsam] < 0) At3_BitRate[ped_patsam] = 0;
                    if(At3_BitRate[ped_patsam] > 2) At3_BitRate[ped_patsam] = 2;
                    Gui_Draw_Arrows_Number_Box(729, 524, Type_At3_BitRate[At3_BitRate[ped_patsam]], Allow_Global | BUTTON_TEXT_CENTERED);
                    if(gode == 18) gode = 19;
                }
#endif

#if !defined(__NO_CODEC__)
                if(gode == 0 || gode == 19)
                {
                    if(SamplesSwap[ped_patsam])
                    {
                        Gui_Draw_Button_Box(729, 484 + (18 * 4), 60, 16, "Lock", Allow_Global_Pushed | BUTTON_TEXT_CENTERED);
                    }
                    else
                    {
                        Gui_Draw_Button_Box(729, 484 + (18 * 4), 60, 16, "Lock", Allow_Global | BUTTON_TEXT_CENTERED);
                    }
                    if(gode == 19)
                    {
                        int i;
                        if(SamplesSwap[ped_patsam])
                        {
                            for(i = 0; i < 16; i++)
                            {
                                Lock_Sample(ped_patsam, i);
                            }
                        }
                        else
                        {
                            for(i = 0; i < 16; i++)
                            {
                                Restore_WaveForm(ped_patsam, 0, i);
                                Restore_WaveForm(ped_patsam, 1, i);
                                if(RawSamples_Swap[ped_patsam][0][i])
                                {
                                    free(RawSamples_Swap[ped_patsam][0][i]);
                                    RawSamples_Swap[ped_patsam][0][i] = NULL;
                                }
                                if(RawSamples_Swap[ped_patsam][1][i])
                                {
                                    free(RawSamples_Swap[ped_patsam][1][i]);
                                    RawSamples_Swap[ped_patsam][1][i] = NULL;
                                }
                            }
                        }
                    }
                }
#endif

                if(gode == 16 || gode == 17 || gode == 18 || gode == 19)
                {
                    Actualize_Instruments_Synths_List(1);
                }

                break;

            case 1:

                Gui_Draw_Button_Box(424, 506, 58, 16, "Exit Editor", Allow_Buttons);
                Gui_Draw_Button_Box(78, 470, 16, 16, "\05", Allow_Buttons | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(96, 470, 16, 16, "\03", Allow_Buttons | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(176, 470, 16, 16, "\04", Allow_Buttons | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(194, 470, 16, 16, "\06", Allow_Buttons | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(220, 470, 56, 16, "Loop End", Allow_Buttons | BUTTON_DISABLED);
                Gui_Draw_Button_Box(278, 470, 16, 16, "\05", Allow_Buttons | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(296, 470, 16, 16, "\03", Allow_Buttons | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(376, 470, 16, 16, "\04", Allow_Buttons | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(394, 470, 16, 16, "\06", Allow_Buttons | BUTTON_TEXT_CENTERED);

                // Fine loop editor mode
                if(typex == 0)
                {
                    outlong(484, 470, *(RawSamples[ped_patsam][0][ped_split] + LoopStart[ped_patsam][ped_split]), 0);
                    outlong(114, 470, LoopStart[ped_patsam][ped_split], 0);
                    actuloop = 1;
                }

                if(typex == 1)
                {
                    outlong(484, 488, *(RawSamples[ped_patsam][0][ped_split] + LoopEnd[ped_patsam][ped_split]), 0);
                    outlong(314, 470, LoopEnd[ped_patsam][ped_split], 0);
                    actuloop = 2;
                }

                if(typex == 2)
                {
                    outlong(484, 470, *(RawSamples[ped_patsam][0][ped_split] + LoopStart[ped_patsam][ped_split]), 0);
                    outlong(484, 488, *(RawSamples[ped_patsam][0][ped_split] + LoopEnd[ped_patsam][ped_split]), 0);
                    outlong(114, 470, LoopStart[ped_patsam][ped_split], 0);
                    outlong(314, 470, LoopEnd[ped_patsam][ped_split], 0);
                    actuloop = 3;
                }

                break;
        } //SWitch draw sampleeditor
    } //User screen == 2
}

void Mouse_Sliders_Instrument_Ed(void)
{
    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT)
    {
        if(SampleType[ped_patsam][ped_split])
        {
            if(zcheckMouse(426, 538, 148, 16))
            {
                gui_action = GUI_CMD_SET_INSTRUMENT_AMPLI;
            }
            if(zcheckMouse(436, 556, 128, 16))
            {
                gui_action = GUI_CMD_SET_INSTRUMENT_FINETUNE;
            }
            if(zcheckMouse(52, 528, 148, 16))
            {
                gui_action = GUI_CMD_SET_INSTRUMENT_DECAY;
            }
        }
        if(Allow_Global_Sliders)
        {
            if(zcheckMouse(52, 546, 148, 16))
            {
                CustomVol[ped_patsam] = float(Mouse.x - 62) * 0.0078125f;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 15;
            }
        }
    }
}

void Mouse_Left_Instrument_Ed(void)
{
    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT && seditor == 1)
    {
        if(zcheckMouse(96, 470, 16, 16) &&
           LoopStart[ped_patsam][ped_split] > 0 &&
           SampleType[ped_patsam][ped_split])
        {
            LoopStart[ped_patsam][ped_split]--;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 5;
        }
        if(zcheckMouse(176, 470, 16, 16) &&
           LoopStart[ped_patsam][ped_split] < LoopEnd[ped_patsam][ped_split] &&
           SampleType[ped_patsam][ped_split])
        {
            LoopStart[ped_patsam][ped_split]++;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 5;
        }
        if(zcheckMouse(296, 470, 16, 16) &&
           LoopEnd[ped_patsam][ped_split] > LoopStart[ped_patsam][ped_split] &&
           SampleType[ped_patsam][ped_split])
        {
            LoopEnd[ped_patsam][ped_split]--;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
        if(zcheckMouse(376, 470, 16, 16) &&
           LoopEnd[ped_patsam][ped_split] < SampleNumSamples[ped_patsam][ped_split] - 1 &&
           SampleType[ped_patsam][ped_split])
        {
            LoopEnd[ped_patsam][ped_split]++;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
        // Return to instrument editor
        if(zcheckMouse(424, 506, 58, 16))
        {
            seditor = 0;
            gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
        }
    }

    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT && seditor == 0)
    {
        if(zcheckMouse(570, 466, 16, 16) && ped_split > 0)
        {
            ped_split--;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 0;
            NewWav();
        }
        if(zcheckMouse(614, 466, 16, 16) && ped_split < 15)
        {
            ped_split++;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 0;
            NewWav();
        }

#if !defined(__NO_MIDI__)
        if(zcheckMouse(570, 502, 16, 16) && Midiprg[ped_patsam] > -1)
        {
            Midiprg[ped_patsam]--;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 10;
        }
        if(zcheckMouse(614, 502, 16, 16) && Midiprg[ped_patsam] < 127)
        {
            Midiprg[ped_patsam]++;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 10;
        }
#endif

        if(Allow_Global_Sliders)
        {
            if(zcheckMouse(144, 492, 28, 16))
            {
                beatsync[ped_patsam] = FALSE;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 12;
            }
            if(zcheckMouse(176, 492, 28, 16))
            {
                beatsync[ped_patsam] = TRUE;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 12;
            }
        }

        if(Allow_Global_Sliders)
        {
            if(zcheckMouse(144, 510, 16, 16) && beatlines[ped_patsam] > 1)
            {
                beatlines[ped_patsam]--;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 13;
            }
            if(zcheckMouse(188, 510, 16, 16) && beatlines[ped_patsam] < 128)
            {
                beatlines[ped_patsam]++;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 13;
            }
        }

        // Go to loop editor
        if(zcheckMouse(268, 466, 88, 16) && SampleType[ped_patsam][ped_split])
        {
            seditor = 1;
            gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
        }

        if(Allow_Global_Sliders)
        {
            if(zcheckMouse(268, 492, 88, 16))
            {
                gui_action = GUI_CMD_SAVE_INSTRUMENT;
            }
        }

        // Export .wav
        if(zcheckMouse(268, 510, 88, 16) && SampleType[ped_patsam][ped_split])
        {
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 14;
        }

        if(zcheckMouse(570, 484, 16, 16) &&
           Basenote[ped_patsam][ped_split] > 0 &&
           SampleType[ped_patsam][ped_split])
        {
            Basenote[ped_patsam][ped_split]--;
            teac = 9;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
        }

        if(zcheckMouse(614, 484, 16, 16) &&
           Basenote[ped_patsam][ped_split] < 119 &&
           SampleType[ped_patsam][ped_split])
        {
            Basenote[ped_patsam][ped_split]++;
            teac = 9;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
        }

        if(Allow_Global_Sliders)
        {

#if !defined(__NO_CODEC__)

            // Select GSM
            if(zcheckMouse(640, 484, 39, 16))
            {
                if(SampleCompression[ped_patsam] != SMP_PACK_GSM)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SampleCompression[ped_patsam] = SMP_PACK_GSM;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            // Select MP3
            if(zcheckMouse(681, 484, 39, 16))
            {
                if(SampleCompression[ped_patsam] != SMP_PACK_MP3)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SampleCompression[ped_patsam] = SMP_PACK_MP3;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            // Select ADPCM
            if(zcheckMouse(640, 484 + (18 * 1), 80, 16))
            {
                if(SampleCompression[ped_patsam] != SMP_PACK_ADPCM)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SampleCompression[ped_patsam] = SMP_PACK_ADPCM;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            // Select TRUESPEECH
            if(zcheckMouse(640, 484 + (18 * 2), 80, 16))
            {
                if(SampleCompression[ped_patsam] != SMP_PACK_TRUESPEECH)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SampleCompression[ped_patsam] = SMP_PACK_TRUESPEECH;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            // Select AT3
            if(zcheckMouse(640, 484 + (18 * 3), 80, 16))
            {
                if(SampleCompression[ped_patsam] != SMP_PACK_AT3)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SampleCompression[ped_patsam] = SMP_PACK_AT3;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            // Select NONE
            if(zcheckMouse(640, 484 + (18 * 4), 80, 16))
            {
                if(SampleCompression[ped_patsam] != SMP_PACK_NONE)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SampleCompression[ped_patsam] = SMP_PACK_NONE;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            // Mp3 BitRate
            if(zcheckMouse(729, 484, 16, 16))
            {
                if(SampleCompression[ped_patsam] == SMP_PACK_MP3)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SamplesSwap[ped_patsam] = FALSE;
                Mp3_BitRate[ped_patsam]--;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 17;
            }

            // Mp3 BitRate
            if(zcheckMouse(729 + 44, 484, 16, 16))
            {
                if(SampleCompression[ped_patsam] == SMP_PACK_MP3)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SamplesSwap[ped_patsam] = FALSE;
                Mp3_BitRate[ped_patsam]++;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 17;
            }

            // At3 BitRate
            if(zcheckMouse(729, 524, 16, 16))
            {
                if(SampleCompression[ped_patsam] == SMP_PACK_AT3)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SamplesSwap[ped_patsam] = FALSE;
                At3_BitRate[ped_patsam]--;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 18;
            }

            // At3 BitRate
            if(zcheckMouse(729 + 44, 524, 16, 16))
            {
                if(SampleCompression[ped_patsam] == SMP_PACK_AT3)
                {
                    SamplesSwap[ped_patsam] = FALSE;
                }
                SamplesSwap[ped_patsam] = FALSE;
                At3_BitRate[ped_patsam]++;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 18;
            }

            // Lock sample
            if(zcheckMouse(729, 484 + (18 * 4), 60, 16))
            {
                SamplesSwap[ped_patsam] = !SamplesSwap[ped_patsam];
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 19;
            }
#endif

        }

        if(SampleType[ped_patsam][ped_split])
        {
            if(zcheckMouse(448, 520, 60, 16))
            {
                LoopType[ped_patsam][ped_split] = SMP_LOOP_FORWARD;
                teac = 5;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                draw_sampled_wave = TRUE;
            }
            if(zcheckMouse(448 + 62, 520, 58, 16))
            {
                LoopType[ped_patsam][ped_split] = SMP_LOOP_PINGPONG;
                teac = 5;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                draw_sampled_wave = TRUE;
            }
            if(zcheckMouse(448 + 62 * 2 - 2, 520, 60, 16))
            {
                LoopType[ped_patsam][ped_split] = SMP_LOOP_NONE;
                teac = 5;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                draw_sampled_wave = TRUE;
            }
        }
    }
}

void Mouse_Right_Instrument_Ed(void)
{
    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT && seditor == 0)
    {
        if(Allow_Global_Sliders)
        {
            if(zcheckMouse(144, 511, 16, 16) && beatlines[ped_patsam] > 1)
            {
                beatlines[ped_patsam] -= 16;
                if(beatlines[ped_patsam] < 1) beatlines[ped_patsam] = 1;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 13;
            }

            if(zcheckMouse(188, 511, 16, 16) && beatlines[ped_patsam] < 128)
            {
                beatlines[ped_patsam] += 16;
                if(beatlines[ped_patsam] > 128) beatlines[ped_patsam] = 128;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 13;
            }
        }

#if !defined(__NO_MIDI__)
        if(zcheckMouse(570, 502, 16, 16) && Midiprg[ped_patsam] > -1)
        {
            if(Midiprg[ped_patsam] > 14) Midiprg[ped_patsam] -= 16;
            else Midiprg[ped_patsam] = -1;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 10;
        }

        if(zcheckMouse(614, 502, 16, 16) && Midiprg[ped_patsam] < 127)
        {
            if(Midiprg[ped_patsam] < 111) Midiprg[ped_patsam] += 16;
            else Midiprg[ped_patsam] = 127;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 10;
        }
#endif

/* NOT IMPLEMENTED YET
#if !defined(__NO_CODEC__)
        if(Allow_Global_Sliders)
        {
            // Lock all samples
            if(zcheckMouse(729, 484 + (18 * 4), 60, 16))
            {
                int i;
                int j;
                int Lock;
                for(i = 0; i < MAX_INSTRS; i++)
                {
                    Lock = FALSE;
                    for(j = 0; j < 16; j++)
                    {
                        if(SampleType[i][j])
                        {
                            Lock = TRUE;
                            break;
                        }
                    }
                    if(Lock)
                    {
                        SamplesSwap[i] = !SamplesSwap[i];
                    }
                    gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                }
                teac = 19;
            }
        }
#endif
*/

        if(SampleType[ped_patsam][ped_split])
        {
            if(zcheckMouse(570, 484, 16, 16) && Basenote[ped_patsam][ped_split] > 0)
            {
                if(Basenote[ped_patsam][ped_split] > 11) Basenote[ped_patsam][ped_split] -= 12;
                else Basenote[ped_patsam][ped_split] = 0;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 9;
            }

            if(zcheckMouse(614, 484, 16, 16) && Basenote[ped_patsam][ped_split] < 119)
            {
                if(Basenote[ped_patsam][ped_split] < 107) Basenote[ped_patsam][ped_split] += 12;
                else Basenote[ped_patsam][ped_split] = 119;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 9;
            }
        }
    }
}

void Afloop(void)
{
    int iose;
    if(actuloop == 1 || actuloop == 3)
    {
        for(int a = 0; a < 200; a++)
        {
            Uint32 ose = a + LoopStart[ped_patsam][ped_split];
            if(ose < SampleNumSamples[ped_patsam][ped_split])
            {
                int v = *(RawSamples[ped_patsam][0][ped_split] + ose) / 1024;
                DrawVLine(a + 220, 552, 520 - v, COL_VUMETER);
                DrawVLine(a + 220, 520 - v, 488, COL_BACKGROUND);
            }
            else
            {
                DrawVLine(a + 20, 488, 552, COL_BACKGROUND);
            }
        }
    }

    if(actuloop == 2 || actuloop == 3)
    {
        for(int b = 0; b < 200; b++)
        {
            Uint32 ose = (LoopEnd[ped_patsam][ped_split] - 200) + b;
            iose = ose;
            if(iose > -1 && ose < SampleNumSamples[ped_patsam][ped_split])
            {
                int v = *(RawSamples[ped_patsam][0][ped_split] + ose) / 1024;
                DrawVLine(b + 20, 552, 520 - v, COL_VUMETERPEAK);
                DrawVLine(b + 20, 520 - v, 488, COL_BACKGROUND);
            }
            else
            {
                DrawVLine(b + 20, 488, 552, COL_BACKGROUND);
            }
        }
    }
    actuloop = 0;
}

void Mouse_Left_Repeat_Instrument_Ed(void)
{
    int32 *Cur_Loop_Start = (int32 *) &LoopStart[ped_patsam][ped_split];
    int32 *Cur_Loop_End = (int32 *) &LoopEnd[ped_patsam][ped_split];

    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT && seditor == 1)
    {
        gui_action = GUI_CMD_NOP;

        if(zcheckMouse(78, 470, 16, 16) && *Cur_Loop_Start > 0)
        {
            *Cur_Loop_Start -= 1;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 5;
        }
        if(zcheckMouse(194, 470, 16, 16) && *Cur_Loop_Start < *Cur_Loop_End)
        {
            *Cur_Loop_Start += 1;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 5;
        }
        if(zcheckMouse(278, 470, 16, 16) && *Cur_Loop_End > *Cur_Loop_Start)
        {
            *Cur_Loop_End -= 1;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
        if(zcheckMouse(394, 470, 16, 16) && *Cur_Loop_End < (int32) SampleNumSamples[ped_patsam][ped_split] - 1)
        {
            *Cur_Loop_End += 1;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
    }
}

void Mouse_Sliders_Right_Instrument_Ed(void)
{
    int32 *Cur_Loop_Start = (int32 *) &LoopStart[ped_patsam][ped_split];
    int32 *Cur_Loop_End = (int32 *) &LoopEnd[ped_patsam][ped_split];

    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT && seditor == 1)
    {
        gui_action = GUI_CMD_NOP;

        if(zcheckMouse(78, 470, 16, 16) && *Cur_Loop_Start > 0)
        {
            *Cur_Loop_Start -= 10;
            if(*Cur_Loop_Start < 0) *Cur_Loop_Start = 0;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 5;
        }
        if(zcheckMouse(194, 470, 16, 16) && *Cur_Loop_Start < *Cur_Loop_End)
        {
            *Cur_Loop_Start += 10;
            if(*Cur_Loop_Start > *Cur_Loop_End) *Cur_Loop_Start = *Cur_Loop_End;
            gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            teac = 5;
        }
        if(zcheckMouse(278, 470, 16, 16) && *Cur_Loop_End > *Cur_Loop_Start)
        {
            *Cur_Loop_End -= 10;
            if(*Cur_Loop_End < *Cur_Loop_Start) *Cur_Loop_End = *Cur_Loop_Start;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
        if(zcheckMouse(394, 470, 16, 16) && *Cur_Loop_End < (int32) SampleNumSamples[ped_patsam][ped_split] - 1)
        {
            *Cur_Loop_End += 10;
            if(*Cur_Loop_End > (int32) SampleNumSamples[ped_patsam][ped_split] - 1) *Cur_Loop_End = SampleNumSamples[ped_patsam][ped_split] - 1;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
    }
}

// ------------------------------------------------------
// Display the instruments or synths list on screen
void Dump_Instruments_Synths_List(int xr, int yr)
{
    char Line[200];
    int i;
    int Nbr_Splits;
    int Font;

    switch(Scopish)
    {
        case SCOPE_ZONE_INSTR_LIST:
        case SCOPE_ZONE_SYNTH_LIST:

            SetColor(COL_BACKGROUND);
            bjbox(xr - 1, yr + 1, 227 + restx, 135);

            switch(Scopish)
            {
                case SCOPE_ZONE_INSTR_LIST:
                    PrintXY(398, 26, USE_FONT, "Instruments List");
                    break;
                case SCOPE_ZONE_SYNTH_LIST:
                    PrintXY(398, 26, USE_FONT, "Synths List");
                    break;
            }

            for(int counter = 0; counter < 11; counter++)
            {
                int rel_val = Instrs_index + counter;

                if(Instrs_index + counter < MAX_INSTRS)
                {
                    if(Instrs_index + counter == ped_patsam)
                    {
                        SetColor(COL_PUSHED_MED);
                        bjbox(xr - 1, yr + (counter * 12) + 2, 227 + restx, 12);
                    }

                    switch(Scopish)
                    {
                        case SCOPE_ZONE_INSTR_LIST:

                            // Instruments view
                            Nbr_Splits = 0;
                            Font = USE_FONT_LOW;
                            for(i = 0; i < MAX_INSTRS_SPLITS; i++)
                            {
                                if(SampleType[rel_val][i])
                                {
                                    Nbr_Splits++;
                                }
                            }
                            if(Nbr_Splits) Font = USE_FONT;

                            sprintf(Line, "%.2x:", rel_val);
                            PrintXY(xr, yr + (counter * 12), Font, Line);
                            sprintf(Line, "%s", nameins[rel_val]);
                            PrintXY(xr + 18, yr + (counter * 12), Font, Line);
        
                            sprintf(Line, "%s", nameins[rel_val]);
                            PrintXY(xr + 18, yr + (counter * 12), Font, Line);
                            switch(Nbr_Splits)
                            {
                                case 0:
                                    sprintf(Line, "No waveform");
                                    break;

                                case 1:
#if !defined(__NO_CODEC__)
                                    if(SamplesSwap[rel_val])
                                    {
                                        sprintf(Line, "*Locked* (%d)", Nbr_Splits);
                                    }
                                    else
#endif
                                    {
                                        sprintf(Line, "%d waveform", Nbr_Splits);
                                    }
                                    break;

                                default:
#if !defined(__NO_CODEC__)
                                    if(SamplesSwap[rel_val])
                                    {
                                        sprintf(Line, "*Locked* (%d)", Nbr_Splits);
                                    }
                                    else
#endif
                                    {
                                        sprintf(Line, "%d waveforms", Nbr_Splits);
                                    }
                                    break;
                            }
                            PrintXY(xr + 168, yr + (counter * 12), Font, Line);

                            if(Nbr_Splits)
                            {
                                switch(SampleCompression[rel_val])
                                {
                                    case SMP_PACK_GSM:
                                        sprintf(Line, "Pck: Gsm");
                                        PrintXY(xr + 240, yr + (counter * 12), Font, Line);
                                        break;

                                    case SMP_PACK_MP3:
                                        sprintf(Line, "Pck: Mp3 (%d)", Type_Mp3_BitRate[Mp3_BitRate[rel_val]]);
                                        PrintXY(xr + 240, yr + (counter * 12), Font, Line);
                                        break;

                                    case SMP_PACK_TRUESPEECH:
                                        sprintf(Line, "Pck: TrueSp.");
                                        PrintXY(xr + 240, yr + (counter * 12), Font, Line);
                                        break;

                                    case SMP_PACK_ADPCM:
                                        sprintf(Line, "Pck: ADPCM");
                                        PrintXY(xr + 240, yr + (counter * 12), Font, Line);
                                        break;

                                    case SMP_PACK_NONE:
                                        sprintf(Line, "Pck: None");
                                        PrintXY(xr + 240, yr + (counter * 12), Font, Line);
                                        break;

                                    case SMP_PACK_AT3:
                                        sprintf(Line, "Pck: At3 (%d)", Type_At3_BitRate[At3_BitRate[rel_val]]);
                                        PrintXY(xr + 240, yr + (counter * 12), Font, Line);
                                        break;
                                }
                            }
                            else
                            {
                                PrintXY(xr + 240, yr + (counter * 12), Font, "Pack: -");
                            }

                            if(Midiprg[rel_val] == -1)
                            {
                                sprintf(Line, "Midi prg.: -");
                                PrintXY(xr + 320, yr + (counter * 12), Font, Line);
                            }
                            else
                            {
                                sprintf(Line, "Midi prg.: %.2d", Midiprg[rel_val]);
                                PrintXY(xr + 320, yr + (counter * 12), USE_FONT, Line);
                            }
                            
                            break;

                        case SCOPE_ZONE_SYNTH_LIST:

                            Font = USE_FONT;
                            if(Synthprg[rel_val] == 0) Font = USE_FONT_LOW;
                            
                            // Synths view
                            sprintf(Line, "%.2x:", rel_val);
                            PrintXY(xr, yr + (counter * 12), Font, Line);

                            sprintf(Line, "%s", PARASynth[rel_val].presetname);
                            PrintXY(xr + 18, yr + (counter * 12), Font, Line);

                            int instr_to_search = Synthprg[rel_val];
                            if(instr_to_search == 0)
                            {
                                sprintf(Line, "No waveform");
                            }
                            else
                            {
                                Nbr_Splits = 0;
                                if(instr_to_search == 1) instr_to_search = rel_val;
                                else instr_to_search -= 2;
                                for(i = 0; i < MAX_INSTRS_SPLITS; i++)
                                {
                                    if(SampleType[instr_to_search][i])
                                    {
                                        Nbr_Splits++;
                                    }
                                }
                                switch(Nbr_Splits)
                                {
                                    case 0:
                                        sprintf(Line, "No waveform");
                                        break;

                                    case 1:
                                        sprintf(Line, "%d waveform", Nbr_Splits);
                                        break;

                                    default:
                                        sprintf(Line, "%d waveforms", Nbr_Splits);
                                        break;
                                }
                            }
                            PrintXY(xr + 168, yr + (counter * 12), Font, Line);

                            if(Synthprg[rel_val] == 0)
                            {
                                sprintf(Line, "No Instr.");
                                PrintXY(xr + 260, yr + (counter * 12), Font, Line);
                            }
                            else if(Synthprg[rel_val] == 1)
                            {
                                sprintf(Line, "Rel. Instr.: Curr");
                                PrintXY(xr + 260, yr + (counter * 12), USE_FONT, Line);
                            }
                            else
                            {
                                sprintf(Line, "Rel. Instr.: %.2x", Synthprg[rel_val] - 2);
                                PrintXY(xr + 260, yr + (counter * 12), USE_FONT, Line);
                            }

                            break;
                    }
                }
            }
            break;
    }
}

// ------------------------------------------------------
// Redraw the instruments or synths list
void Actualize_Instruments_Synths_List(int modeac)
{
    int const brolim = Instrs_items - 11;

    switch(Scopish)
    {
        case SCOPE_ZONE_INSTR_LIST:
        case SCOPE_ZONE_SYNTH_LIST:

            if(modeac == 0)
            {
                if(Instrs_ykar > 70) Instrs_ykar = 70;
                if(Instrs_ykar < 0) Instrs_ykar = 0;
                Instrs_index = (Instrs_ykar * brolim) / 70;
            }

            if(modeac == 2)
            {
                if(ped_patsam >= Instrs_index + 11)
                {
                    Instrs_index += ped_patsam - (Instrs_index + 10);
                }

                if(ped_patsam < Instrs_index)
                {
                    Instrs_index -= Instrs_index - ped_patsam;
                }
            }

            if(Instrs_index > brolim) Instrs_index = brolim;
            if(Instrs_index < 0) Instrs_index = 0;
            if(modeac != 0)
            {
                if(brolim)
                {
                    Instrs_ykar = (Instrs_index * 70) / brolim;
                }
                else
                {
                    Instrs_ykar = (Instrs_index * 70);
                }
            }

            SetColor(COL_SLIDER_LO);
            bjbox(395 - 1, 59 - 1, 15 + 2, 101 + 2);
            SetColor(COL_SLIDER_HI);
            bjbox(395, 59, 15 + 1, 101 + 1);
            SetColor(COL_SLIDER_MED);
            bjbox(395, 59, 15, 101);
            Gui_Draw_Button_Box(394 + 1, 58 + Instrs_ykar + 1, 16 - 2, 32 - 2, "", BUTTON_NORMAL);
            Dump_Instruments_Synths_List(413, 41);
            break;
    }
}

// ------------------------------------------------------
// Lock a sample in order to test it
#if !defined(__NO_CODEC__)
void Lock_Sample(int instr_nbr, int split)
{
    int PackedLen = 0;
    short *PackedSample;
    short *Dest_Buffer;
    short *Sample_Dest_Buffer;
    int iSmp;
    short Sample1;
    short Sample2;

    short *AlignedSample;
    int Aligned_Size;
    short *Sample;
    short *Smp_Dats;
    int Size;
    char Pack_Type;
    int BitRate;
    int i;

    for(i = 0; i < SampleChannels[instr_nbr][split]; i++)
    {
        PackedSample = NULL;
        Smp_Dats = NULL;
        Dest_Buffer = NULL;
        Sample_Dest_Buffer = NULL;
        Smp_Dats = NULL;
        Size = SampleNumSamples[instr_nbr][split];
        Sample = RawSamples[instr_nbr][i][split];
        Pack_Type = SampleCompression[instr_nbr];
        BitRate = SampleCompression[instr_nbr] == SMP_PACK_MP3 ?
                                                  Type_Mp3_BitRate[Mp3_BitRate[instr_nbr]] :
                                                  Type_At3_BitRate[At3_BitRate[instr_nbr]];
        if(RawSamples_Swap[instr_nbr][i][split])
        {
            free(RawSamples_Swap[instr_nbr][i][split]);
            RawSamples_Swap[instr_nbr][i][split] = NULL;
        }

        RawSamples_Swap[instr_nbr][i][split] = (short *) malloc(Size * 2);

        Save_WaveForm(instr_nbr, i, split);

        Dest_Buffer = (short *) malloc(Size * 2);

        if(Pack_Type != SMP_PACK_NONE)
        {
            // Halve the sample
            Size /= 2;
            Smp_Dats = (short *) malloc(Size * 2);
            for(iSmp = 0; iSmp < Size; iSmp++)
            {
                Smp_Dats[iSmp] = *(Sample + (iSmp * 2));
            }
            Sample = Smp_Dats;
        }

        // Pack it first
        switch(Pack_Type)
        {
            case SMP_PACK_AT3:
                PackedSample = (short *) malloc(Size * 2);
                memset(PackedSample, 0, Size * 2);
                PackedLen = ToAT3(Sample, PackedSample, Size * 2, BitRate);
                break;

            case SMP_PACK_GSM:
                PackedSample = (short *) malloc(Size * 2);
                memset(PackedSample, 0, Size * 2);
                PackedLen = ToGSM(Sample, PackedSample, Size * 2);
                break;

            case SMP_PACK_MP3:
                PackedSample = (short *) malloc(Size * 2);
                memset(PackedSample, 0, Size * 2);
                PackedLen = ToMP3(Sample, PackedSample, Size * 2, BitRate);
                break;

            case SMP_PACK_TRUESPEECH:
                Aligned_Size = (Size * 2) + 0x400;
                AlignedSample = (short *) malloc(Aligned_Size);
                if(AlignedSample)
                {
                    memset(AlignedSample, 0, Aligned_Size);
                    memcpy(AlignedSample, Sample, Size * 2);
                    // Size must be aligned
                    PackedSample = (short *) malloc(Aligned_Size);
                    if(PackedSample)
                    {
                        memset(PackedSample, 0, Aligned_Size);
                        PackedLen = ToTrueSpeech(AlignedSample, PackedSample, Aligned_Size);
                    }
                    free(AlignedSample);
                }
                break;

            case SMP_PACK_ADPCM:
                Aligned_Size = (Size * 2) + 0x1000;
                AlignedSample = (short *) malloc(Aligned_Size);
                if(AlignedSample)
                {
                    memset(AlignedSample, 0, Aligned_Size);
                    memcpy(AlignedSample, Sample, Size * 2);
                    // Size must be aligned
                    PackedSample = (short *) malloc(Aligned_Size);
                    if(PackedSample)
                    {
                        memset(PackedSample, 0, Aligned_Size);
                        PackedLen = ToADPCM(AlignedSample, PackedSample, Aligned_Size);
                    }
                    free(AlignedSample);
                }
                break;

            case SMP_PACK_NONE:
                PackedLen = 0;
                break;
        }

        // Depack it now
        if(PackedLen)
        {
            switch(Pack_Type)
            {
                case SMP_PACK_AT3:
                    UnpackAT3((Uint8 *) PackedSample, Dest_Buffer, PackedLen, Size, BitRate);
                    break;

                case SMP_PACK_GSM:
                    UnpackGSM((Uint8 *) PackedSample, Dest_Buffer, PackedLen, Size);
                    break;

                case SMP_PACK_MP3:
                    UnpackMP3((Uint8 *) PackedSample, Dest_Buffer, PackedLen, Size, BitRate);
                    break;

                case SMP_PACK_TRUESPEECH:
                    UnpackTrueSpeech((Uint8 *) PackedSample, Dest_Buffer, PackedLen, Size);
                    break;

                case SMP_PACK_ADPCM:
                    UnpackADPCM((Uint8 *) PackedSample, Dest_Buffer, PackedLen, Size);
                    break;
            }

            // Interpolate the sample
            Sample_Dest_Buffer = RawSamples[instr_nbr][i][split];

            for(iSmp = 0; iSmp < Size; iSmp++)
            {
                Sample1 = Dest_Buffer[iSmp];
                Sample2 = Dest_Buffer[iSmp + 1];
                if(iSmp == Size - 1)
                {
                    if(LoopType[instr_nbr][split])
                    {
                        Sample2 = Sample_Dest_Buffer[LoopStart[instr_nbr][split]];
                    }
                    else
                    {
                        Sample2 = Dest_Buffer[iSmp];
                    }
                }
                Sample_Dest_Buffer[(iSmp * 2)] = Sample1;
                Sample_Dest_Buffer[(iSmp * 2) + 1] = Sample1 + ((Sample2 - Sample1) / 2);
            }

        }

        if(Dest_Buffer) free(Dest_Buffer);
        if(PackedSample) free(PackedSample);
        if(Smp_Dats) free(Smp_Dats);
    }

}

// ------------------------------------------------------
// Copy a sample into the back buffer
void Save_WaveForm(int Instr_Nbr, int Channel, int Split)
{
    int i;

    if(RawSamples[Instr_Nbr][Channel][Split] && RawSamples_Swap[Instr_Nbr][Channel][Split])
    {
        for(i = 0; i < (int) SampleNumSamples[Instr_Nbr][Split]; i++)
        {
            RawSamples_Swap[Instr_Nbr][Channel][Split][i] = RawSamples[Instr_Nbr][Channel][Split][i];
        }
    }
}

// ------------------------------------------------------
// Copy a sample from the back buffer
void Restore_WaveForm(int Instr_Nbr, int Channel, int Split)
{
    int i;

    if(RawSamples[Instr_Nbr][Channel][Split] && RawSamples_Swap[Instr_Nbr][Channel][Split])
    {
        for(i = 0; i < (int) SampleNumSamples[Instr_Nbr][Split]; i++)
        {
            RawSamples[Instr_Nbr][Channel][Split][i] = RawSamples_Swap[Instr_Nbr][Channel][Split][i];
        }
    }
}

#endif
