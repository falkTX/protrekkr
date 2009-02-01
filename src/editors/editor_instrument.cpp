// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/editor_instrument.h"
#include "include/editor_sample.h"

// ------------------------------------------------------
// Variables
int Allow_Buttons;
int Allow_Sliders;
int Allow_Buttons_Pushed;
int Allow_Global;
int Allow_Global_Pushed;
int Allow_Global_Sliders;

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

#if !defined(__NOCODEC__)
            Gui_Draw_Button_Box(640, 466, 88, 16, "Packing Scheme", BUTTON_NORMAL | BUTTON_DISABLED);
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
                Gui_Draw_Button_Box(268, 466, 88, 16, "Loop Fine Editor", Allow_Buttons);
                Gui_Draw_Button_Box(268, 492, 88, 16, "Save Instrument", Allow_Global);
                Gui_Draw_Button_Box(268, 510, 88, 16, "Export .Wav File", Allow_Buttons);

                // Instrument editor mode
                if(gode == 0 || gode == 1)
                {
                    if(SampleVol[ped_patsam][ped_split] > 4.0f) SampleVol[ped_patsam][ped_split] = 4.0f;
                    if(SampleVol[ped_patsam][ped_split] < 0) SampleVol[ped_patsam][ped_split] = 0;
                    Realslider(426, 538, f2i(SampleVol[ped_patsam][ped_split] * 32.0f), Allow_Sliders);
                    outlong_small(575, 538, f2i(SampleVol[ped_patsam][ped_split] * 100.0f), 1, 56, BUTTON_NORMAL | BUTTON_DISABLED);
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
                    Realslider(52, 528, f2i(FDecay[ped_patsam][ped_split] * 8192.0f), Allow_Sliders);
                    outlong(201, 528, f2i(FDecay[ped_patsam][ped_split] * 8192.0f), 0);
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
#if !defined(__NOMIDI__)
                    if(Midiprg[ped_patsam] == -1)
                    {
#endif
                        Gui_Draw_Button_Box(570, 502, 16, 16, "\03", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                        Gui_Draw_Button_Box(570 + 18, 502, 24, 16, "N/A", BUTTON_DISABLED);
                        Gui_Draw_Button_Box(570 + 44, 502, 16, 16, "\04", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
#if !defined(__NOMIDI__)
                    }
                    else
                    {
                        value_box(570, 502, Midiprg[ped_patsam] + 1, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    }
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

                    if(strlen(SampleName[ped_patsam][ped_split])) RF.OpenForWrite(SampleName[ped_patsam][ped_split], 44100, 16, SampleChannels[ped_patsam][ped_split]);
                    else RF.OpenForWrite("Untitled.wav", 44100, 16, SampleChannels[ped_patsam][ped_split]);

                    bool t_stereo;

                    if(SampleChannels[ped_patsam][ped_split] == 1) t_stereo = false;
                    else t_stereo = true;

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
                        header.ckID = 'lpms';
                        header.ckSize = 0x3c;
                        memset(&datas, 0, sizeof(datas));
                        datas.Num_Sample_Loops = 1;
                        datas.Start = LoopStart[ped_patsam][ped_split];
                        datas.End = LoopEnd[ped_patsam][ped_split];
                        RF.WriteData((void *) &header, sizeof(header));
                        RF.WriteData((void *) &datas, sizeof(datas));
                    }

                    RF.Close();
                    if(strlen(SampleName[ped_patsam][ped_split])) sprintf(buffer, "File '%s' saved...", SampleName[ped_patsam][ped_split]);
                    else sprintf(buffer, "File 'Untitled.wav' saved...");
                    mess_box(buffer);

                    Read_SMPT();
                    last_index = -1;
                    ltActualize(0);
                }

                if(gode == 0 || gode == 15)
                {
                    if(CustomVol[ped_patsam] > 1.0f) CustomVol[ped_patsam] = 1.0f;
                    if(CustomVol[ped_patsam] < 0.0f) CustomVol[ped_patsam] = 0.0f;
                    Realslider(52, 546, f2i(CustomVol[ped_patsam] * 128.0f), Allow_Global_Sliders);
                    outlong(201, 546, f2i(CustomVol[ped_patsam] * 100.0f), 1);
                }

                if(gode == 0 || gode == 16)
                {
                    switch(SampleCompression[ped_patsam])
                    {
#if !defined(__NOCODEC__)
                        case SMP_PACK_GSM:
                            Gui_Draw_Button_Box(640, 484, 88, 16, "Gsm 6.10", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(640, 484 + 18, 88, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 88, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 88, 16, "At3 (PSP only)", Allow_Global | BUTTON_DISABLED);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 88, 16, "None", Allow_Global);
                            break;
                        case SMP_PACK_MP3:
                            Gui_Draw_Button_Box(640, 484, 88, 16, "Gsm 6.10", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + 18, 88, 16, "Mp3", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 88, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 88, 16, "At3 (PSP only)", Allow_Global | BUTTON_DISABLED);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 88, 16, "None", Allow_Global);
                            break;
                        case SMP_PACK_TRUESPEECH:
                            Gui_Draw_Button_Box(640, 484, 88, 16, "Gsm 6.10", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + 18, 88, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 88, 16, "True Speech", Allow_Global_Pushed);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 88, 16, "At3 (PSP only)", Allow_Global | BUTTON_DISABLED);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 88, 16, "None", Allow_Global);
                            break;
                        /*case SMP_PACK_AT3:
                            Gui_Draw_Button_Box(640, 484, 88, 16, "Gsm 6.10", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + 18, 88, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 88, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 88, 16, "At3 (PSP only)", Allow_Global_Pushed | BUTTON_DISABLED);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 88, 16, "None", Allow_Global);
                            break;*/
#endif
                        case SMP_PACK_NONE:
                            Gui_Draw_Button_Box(640, 484, 88, 16, "Gsm 6.10", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + 18, 88, 16, "Mp3", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 2), 88, 16, "True Speech", Allow_Global);
                            Gui_Draw_Button_Box(640, 484 + (18 * 3), 88, 16, "At3 (PSP only)", Allow_Global | BUTTON_DISABLED);
                            Gui_Draw_Button_Box(640, 484 + (18 * 4), 88, 16, "None", Allow_Global_Pushed);
                            break;
                    }
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
                CustomVol[ped_patsam] = float(Mouse.old_x - 62) * 0.0078125f;
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

#if !defined(__NOMIDI__)
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
                beatsync[ped_patsam] = false;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                teac = 12;
            }
            if(zcheckMouse(176, 492, 28, 16))
            {
                beatsync[ped_patsam] = true;
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

#if !defined(__NOCODEC__)
            if(zcheckMouse(640, 484, 88, 16))
            {
                SampleCompression[ped_patsam] = SMP_PACK_GSM;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            if(zcheckMouse(640, 484 + (18 * 1), 88, 16))
            {
                SampleCompression[ped_patsam] = SMP_PACK_MP3;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            if(zcheckMouse(640, 484 + (18 * 2), 88, 16))
            {
                SampleCompression[ped_patsam] = SMP_PACK_TRUESPEECH;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }

            if(zcheckMouse(640, 484 + (18 * 3), 88, 16))
            {
                SampleCompression[ped_patsam] = SMP_PACK_AT3;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }
#endif

            if(zcheckMouse(640, 484 + (18 * 4), 88, 16))
            {
                SampleCompression[ped_patsam] = SMP_PACK_NONE;
                teac = 16;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
            }
        }

        if(SampleType[ped_patsam][ped_split])
        {
            if(zcheckMouse(448, 520, 60, 16))
            {
                LoopType[ped_patsam][ped_split] = SMP_LOOP_FORWARD;
                teac = 5;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                draw_sampled_wave = true;
            }
            if(zcheckMouse(448 + 62, 520, 58, 16))
            {
                LoopType[ped_patsam][ped_split] = SMP_LOOP_PINGPONG;
                teac = 5;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                draw_sampled_wave = true;
            }
            if(zcheckMouse(448 + 62 * 2 - 2, 520, 60, 16))
            {
                LoopType[ped_patsam][ped_split] = SMP_LOOP_NONE;
                teac = 5;
                gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                draw_sampled_wave = true;
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

#if !defined(__NOMIDI__)
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
                DrawVLine(a + 220, 552, 520 - v, COL_SCOPESSAMPLES);
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
                DrawVLine(b + 20, 552, 520 - v, COL_SCOPESSAMPLES);
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
    Uint32 *Cur_Loop_Start = &LoopStart[ped_patsam][ped_split];
    Uint32 *Cur_Loop_End = &LoopEnd[ped_patsam][ped_split];

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
        if(zcheckMouse(394, 470, 16, 16) && *Cur_Loop_End < SampleNumSamples[ped_patsam][ped_split] - 1)
        {
            *Cur_Loop_End += 1;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
    }
}

void Mouse_Right_Repeat_Instrument_Ed(void)
{
    Uint32 *Cur_Loop_Start = &LoopStart[ped_patsam][ped_split];
    Uint32 *Cur_Loop_End = &LoopEnd[ped_patsam][ped_split];

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
        if(zcheckMouse(394, 470, 16, 16) && *Cur_Loop_End < SampleNumSamples[ped_patsam][ped_split] - 1)
        {
            *Cur_Loop_End += 10;
            if(*Cur_Loop_End > SampleNumSamples[ped_patsam][ped_split] - 1) *Cur_Loop_End = SampleNumSamples[ped_patsam][ped_split] - 1;
            gui_action = GUI_CMD_UPDATE_LOOP_EDITOR_ED;
            teac = 5;
        }
    }
}
