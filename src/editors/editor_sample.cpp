// ------------------------------------------------------
// Protrekkr
// Based on Juan Antonio Arguelles Rius's NoiseTrekker.
//
// Copyright (C) 2008-2010 Franck Charlet.
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
#include "include/editor_sample.h"
#include "include/editor_instrument.h"
#include "../samples/include/samples_ops.h"

#if defined(__AROS__)
#include <stdint.h>
#define int32 int32_t
#endif

// ------------------------------------------------------
// Constants
#define WAVE_LEFT 2
#define SAMPLE_LINES_HEIGHT 109
#define SAMPLE_HEIGHT (450 + SAMPLE_LINES_HEIGHT)

// ------------------------------------------------------
// Variables
extern s_access sp_Position[MAX_TRACKS][MAX_POLYPHONY];

int32 axswave = 0;

int32 sed_display_start = 0;
int32 sed_display_length = 0;
int32 sed_range_start = 0;
int32 sed_range_end = 0;
char sed_range_mode = FALSE;

// ------------------------------------------------------
// Functions
void Draw_Wave_PlayBack_Pos(void);
void Draw_Wave_Data3(void);

void Draw_Sample_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_SAMPLE_EDIT);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("");

    Gui_Draw_Button_Box(582, 476, 60, 16, "Select All", BUTTON_NORMAL | BUTTON_TEXT_CENTERED );
    Gui_Draw_Button_Box(582, 494, 60, 16, "Unselect", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(582, 512, 60, 16, "View All", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(582, 530, 60, 16, "VZoom In", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(582, 548, 60, 16, "VZoom Out", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(650, 476, 60, 16, "Zoom In", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(650, 494, 60, 16, "Zoom Out", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(650, 512, 60, 16, "Range", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(712, 512, 60, 16, "View", BUTTON_NORMAL | BUTTON_DISABLED);
        
    Gui_Draw_Button_Box(712, 476, 60, 16, "Set Loop S.", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(712, 494, 60, 16, "Set Loop E.", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    draw_sampled_wave = TRUE;
    Actualize_Sample_Ed(0);
}

void Draw_Wave_Data(void)
{
    int32 sed_real_range_start;
    int32 sed_real_range_end;
    int32 s_offset;
    int32 s_ex;
    int start_rect;
    int set_start_rect;
    int end_rect;

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        sed_real_range_start = sed_range_start;
        sed_real_range_end = sed_range_end;

        if(sed_real_range_start > sed_real_range_end)
        {
            int Swap_Range = sed_real_range_start;
            sed_real_range_start = sed_real_range_end;
            sed_real_range_end = Swap_Range;
        }

        if(draw_sampled_wave)
        {
            // Redraw the bottom slider
            Realslider_Horiz(WAVE_LEFT, 559, sed_display_start, sed_display_length,
                             SampleNumSamples[ped_patsam][ped_split], 512 + 2, TRUE);

            // Layout around the waveform
            SetColor(COL_BACKGROUND);
            Fillrect(WAVE_LEFT, 450, 512 + WAVE_LEFT + 1, SAMPLE_HEIGHT);
            DrawHLine(450, WAVE_LEFT, 512 + WAVE_LEFT + 1, COL_BLACK);
            DrawHLine(SAMPLE_HEIGHT - 1, WAVE_LEFT, 512 + WAVE_LEFT + 1, COL_BLACK);
            DrawVLine(512 + WAVE_LEFT + 1, 450, SAMPLE_HEIGHT - 1, COL_BLACK);
            DrawVLine(WAVE_LEFT, 450, SAMPLE_HEIGHT - 1, COL_BLACK);

            if(SampleType[ped_patsam][ped_split] > 0)
            {   
                // Any Sample Out There?
                int strober = SampleChannels[ped_patsam][ped_split] * 2;
                int s_ey;
                int s_ey2;
                int s_size = 0;
                int s_coef;

                if(strober)
                {
                    s_ey = 450 + 1 + (SAMPLE_LINES_HEIGHT / strober);
                    s_ey2 = s_ey + ((SAMPLE_LINES_HEIGHT / strober) * 2);
                    if(s_ey2 > SAMPLE_HEIGHT - 2) s_ey2 = SAMPLE_HEIGHT - 2;
                    if(s_ey > SAMPLE_HEIGHT - 2) s_ey = SAMPLE_HEIGHT - 2;
                    s_size = SAMPLE_LINES_HEIGHT / strober;
                    s_coef = 32768 / s_size;

                    if(rs_coef > s_coef) rs_coef = s_coef;
                    if(rs_coef < 1) rs_coef = 1;
                }

                // Draw a selection rect if there's any
                start_rect = 0;
                end_rect = 512 - 1;
                set_start_rect = 0;
                for(s_ex = 0; s_ex < 512; s_ex++)
                {
                    s_offset = (s_ex * sed_display_length) / 512 + sed_display_start;

                    if(sed_range_mode &&
                       s_offset >= sed_real_range_start)
                    {
                        if(!set_start_rect)
                        {
                            // Record the start position
                            start_rect = s_ex;
                            set_start_rect = 1;
                        }
                    }
                    if(sed_range_mode &&
                       s_offset >= sed_real_range_end)
                    {
                        // Record the end position and exit
                        end_rect = s_ex;
                        break;
                    }
                }
                if(set_start_rect)
                {
                    SetColor(COL_SCOPESSAMPLES);
                    Fillrect(start_rect + WAVE_LEFT + 1, 450 + 1, end_rect + WAVE_LEFT + 1 + 1, SAMPLE_HEIGHT - 1);
                }

                switch(SampleChannels[ped_patsam][ped_split])
                {
                    case 1:
                        // Mono sample
                        for(s_ex = 0; s_ex < 512; s_ex++)
                        {
                            s_offset = (s_ex * sed_display_length) / 512 + sed_display_start;
                            int h = *(RawSamples[ped_patsam][0][ped_split] + s_offset) / rs_coef;
                            if(h > s_size) h = s_size;
                            if(h < -s_size) h = -s_size;

                            int s_y = s_ey - h;
                            if(s_y > SAMPLE_HEIGHT - 2) s_y = SAMPLE_HEIGHT - 2;

                            if(sed_range_mode &&
                               s_offset >= sed_real_range_start &&
                               s_offset <= sed_real_range_end)
                            {
                                // Sample datas
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_BACKGROUND);
                                // Straight line
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, COL_BACKGROUND);
                            }
                            else
                            {
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_SCOPESSAMPLES);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, COL_SCOPESSAMPLES);
                            }
                        }
                        break;

                    case 2:
                        // Stereo sample
                        for(s_ex = 0; s_ex < 512; s_ex++)
                        {
                            s_offset = (s_ex * sed_display_length) / 512 + sed_display_start;
                            int h = *(RawSamples[ped_patsam][0][ped_split] + s_offset) / rs_coef;
                            int h2 = *(RawSamples[ped_patsam][1][ped_split] + s_offset) / rs_coef;
                            if(h > s_size) h = s_size;
                            if(h < -s_size) h = -s_size;
                            if(h2 > s_size) h2 = s_size;
                            if(h2 < -s_size) h2 = -s_size;
                            int s_y = s_ey - h;
                            int s_y2 = s_ey2 - h2;
                            if(s_y2 > SAMPLE_HEIGHT - 2) s_y2 = SAMPLE_HEIGHT - 2;
                            if(s_y > SAMPLE_HEIGHT - 2) s_y = SAMPLE_HEIGHT - 2;

                            if(sed_range_mode &&
                               s_offset >= sed_real_range_start &&
                               s_offset <= sed_real_range_end)
                            {
                                // Sample datas
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_BACKGROUND);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_y2, COL_BACKGROUND);
                                // Straight line
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, COL_BACKGROUND);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_ey2, COL_BACKGROUND);
                            }
                            else
                            {
                                // Sample datas
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_SCOPESSAMPLES);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_y2, COL_SCOPESSAMPLES);
                                // Straight line
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, COL_SCOPESSAMPLES);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_ey2, COL_SCOPESSAMPLES);
                            }
                        }
                        break;
                }

                // Display the loop infos bars
                if(LoopType[ped_patsam][ped_split])
                {
                    int32 LSX = ((LoopStart[ped_patsam][ped_split] - sed_display_start) * 512) / sed_display_length;
                    int32 LEX = ((LoopEnd[ped_patsam][ped_split] - sed_display_start) * 512) / sed_display_length;

                    if(LSX >= 0 && LSX <= 512)
                    {
                        DrawVLine(LSX + WAVE_LEFT + 1, 450 + 1, SAMPLE_HEIGHT - 2, COL_VUMETERPEAK);
                        DrawHLine(450 + 10, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine(450 + 11, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 2, COL_VUMETERPEAK);
                        DrawHLine(450 + 12, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine(450 + 13, LSX + WAVE_LEFT + 1 + 5, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine(450 + 14, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                    }

                    if(LEX >= 0 && LEX <= 512)
                    {
                        if(LEX == 512) LEX--;
                        DrawVLine(LEX + WAVE_LEFT + 1 + 1, 450 + 1, SAMPLE_HEIGHT - 2, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 14, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 13, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 5, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 12, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 11, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 5, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 10, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                    }
                }
            }
            draw_sampled_wave = FALSE;
        }
        Draw_Wave_PlayBack_Pos();
        if(draw_sampled_wave3)
        {
            draw_sampled_wave = TRUE;
            draw_sampled_wave3 = FALSE;
            Draw_Wave_Data();
        }
    }
}

// ------------------------------------------------------
void Renew_Sample_Ed(void)
{
    draw_sampled_wave = TRUE;
    sed_display_start = 0;
    sed_display_length = SampleNumSamples[ped_patsam][ped_split];
    sed_range_start = 0;
    sed_range_mode = FALSE;
    sed_range_end = 0;
    Actualize_Sample_Ed(0);
}

// ------------------------------------------------------
// Draw the current playback position
void Draw_Wave_PlayBack_Pos(void)
{
    int32 sed_real_range_start;
    int32 sed_real_range_end;
    int32 pos_in_sample;
    int i;

    if(draw_sampled_wave2)
    {
        if(SampleType[ped_patsam][ped_split] > 0)
        { 
            // Any Sample Out There ?
            int strober = SampleChannels[ped_patsam][ped_split] * 2;
            int s_ey = 450 + 1 + (SAMPLE_LINES_HEIGHT / strober);
            int s_ey2 = s_ey + ((SAMPLE_LINES_HEIGHT / strober) * 2);
            int rcolor3;
            int rcolor4;

            pos_in_sample = 0;
            for(i = 0; i < MAX_POLYPHONY; i++)
            {
                if((int32) sp_Position[ped_track][i].half.first > pos_in_sample)
                {
                    if(sp_Stage[ped_track][i] == PLAYING_SAMPLE)
                    {
                        pos_in_sample = sp_Position[ped_track][i].half.first;
                    }
                }
            }

            sed_real_range_start = sed_range_start;
            sed_real_range_end = sed_range_end;

            if(sed_real_range_start > sed_real_range_end)
            {
                int Swap_Range = sed_real_range_start;
                sed_real_range_start = sed_real_range_end;
                sed_real_range_end = Swap_Range;
            }

            if(SampleChannels[ped_patsam][ped_split] == 1)
            {
                for(int32 s_ex = 0; s_ex < 512; s_ex++)
                {
                    int32 s_offset = (s_ex * sed_display_length) / 512 + sed_display_start;

                    if(pos_in_sample > s_offset)
                    {
                        rcolor3 = COL_PUSHED_MED;
                    }
                    else
                    {
                        if(sed_range_mode &&
                           s_offset >= sed_real_range_start &&
                           s_offset <= sed_real_range_end)
                        {
                            rcolor3 = COL_BACKGROUND;
                        }
                        else
                        {
                            rcolor3 = COL_SCOPESSAMPLES;
                        }
                    }
                    DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, rcolor3);
                }
            } // If

            // STEREO DISPLAY
            if(SampleChannels[ped_patsam][ped_split] == 2)
            {
                for(int32 s_ex = 0; s_ex < 512; s_ex++)
                {
                    int32 s_offset = (s_ex * sed_display_length) / 512 + sed_display_start;

                    if(pos_in_sample > s_offset)
                    {
                        rcolor3 = COL_PUSHED_MED;
                        rcolor4 = COL_PUSHED_MED;
                    }
                    else
                    {
                        if(sed_range_mode &&
                           s_offset >= sed_real_range_start &&
                           s_offset <= sed_real_range_end)
                        {
                            rcolor3 = COL_BACKGROUND;
                            rcolor4 = COL_BACKGROUND;
                        }
                        else
                        {
                            rcolor3 = COL_SCOPESSAMPLES;
                            rcolor4 = COL_SCOPESSAMPLES;
                        }
                    }

                    DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, rcolor3);
                    DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_ey2, rcolor4);
                }

            } // If Stereo

        } // IF CHECK SAMPLE

        draw_sampled_wave2 = FALSE;
    }
}

// ------------------------------------------------------
// Display the various element of the editor
// and responds to the various commands
void Actualize_Sample_Ed(char gode)
{
    int32 sed_real_range_start;
    int32 sed_real_range_end;
    int Disabled;

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(gode == 0)
        {
#if !defined(__NO_CODEC__)
            if(SamplesSwap[ped_patsam]) Disabled = BUTTON_DISABLED;
            else
#endif
            {
                Disabled = 0;
            }
            // Those commands aren't available when viewing packed samples
            Gui_Draw_Button_Box(520, 476, 29, 16, "Cut", BUTTON_NORMAL | Disabled | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, 476, 29, 16, "Half", BUTTON_NORMAL | Disabled | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, 494, 60, 16, "Maximize", BUTTON_NORMAL | Disabled | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, 512, 60, 16, "DC Adjust", BUTTON_NORMAL | Disabled | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, 530, 60, 16, "Fade In", BUTTON_NORMAL | Disabled | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, 548, 60, 16, "Fade Out", BUTTON_NORMAL | Disabled | BUTTON_TEXT_CENTERED);
        }

        sed_real_range_start = sed_range_start;
        sed_real_range_end = sed_range_end;

        if(sed_real_range_start > sed_real_range_end)
        {
            int Swap_Range = sed_real_range_start;
            sed_real_range_start = sed_real_range_end;
            sed_real_range_end = Swap_Range;
        }

        if(SampleType[ped_patsam][ped_split])
        {
            if(gode == 3 || gode == 0)
            {
                outlong(712, 530, sed_display_start, 10);
                outlong(712, 548, sed_display_length, 12);
            }

            if(gode == 4 || gode == 5 || gode == 0)
            {
                outlong(650, 530, sed_real_range_start, 10);
            }

            if(gode == 4 || gode == 5 || gode == 0)
            {
                outlong(650, 548, sed_real_range_end, 11);
            }

            if(gode == 0 || gode == 6) Status_Box("Full Vertical View.");

            // Cut Sample
            if(gode == 20)
            {
                if(Sample_Cut(sed_real_range_start, sed_real_range_end))
                {
                    // Adjust after the cut
                    if(sed_display_length > (int32) SampleNumSamples[ped_patsam][ped_split])
                    {
                        sed_display_length = SampleNumSamples[ped_patsam][ped_split];
                    }
                    if(sed_display_length + sed_display_start > (int32) SampleNumSamples[ped_patsam][ped_split])
                    {
                        sed_display_start = SampleNumSamples[ped_patsam][ped_split] - sed_display_length;
                    }
                    sed_range_mode = FALSE;
                    sed_range_start = 0;
                    sed_range_end = 0;
                    draw_sampled_wave = TRUE;
                    outlong(712, 530, sed_display_start, 10);
                    outlong(712, 548, sed_display_length, 12);
                    outlong(650, 530, sed_range_start, 10);
                    outlong(650, 548, sed_range_end, 11);
                    Check_Loops();
                    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT) Actualize_Instrument_Ed(0, 4);
                    AUDIO_Play();
                }
            }

            // DC adjust
            if(gode == 21)
            {
                Sample_DC_Adjust(sed_real_range_start, sed_real_range_end);
            }

            // Normalizing
            if(gode == 22)
            {
                Sample_Maximize(sed_real_range_start, sed_real_range_end);
            }

            // Fade In
            if(gode == 23)
            {
                Sample_FadeIn(sed_real_range_start, sed_real_range_end);
            }

            // Fade Out
            if(gode == 24)
            {
                Sample_FadeOut(sed_real_range_start, sed_real_range_end);
            }

            // Half
            if(gode == 25)
            {
                Sample_Half(sed_real_range_start, sed_real_range_end);
            }

        }
        else
        {
            Gui_Draw_Button_Box(650, 530, 122, 34, "No Sample Loaded", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
        }
    }
}

// ------------------------------------------------------
// Right mouse buttons events
void Mouse_Right_Sample_Ed(void)
{
    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(WAVE_LEFT + 1, 450, 512, CONSOLE_HEIGHT2))
        {
            if(sed_range_mode)
            {
                // Unselect all
                sed_range_start = 0;
                sed_range_end = 0;
                sed_range_mode = FALSE;
                draw_sampled_wave = TRUE;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                teac = 4;
            }
            else
            {
                // Select all
                sed_range_mode = TRUE;
                sed_range_start = sed_display_start;
                sed_range_end = sed_display_start + sed_display_length - 1;
                draw_sampled_wave = TRUE;
                teac = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }
        }
    }
}

// ------------------------------------------------------
// Left mouse buttons events
void Mouse_Left_Sample_Ed(void)
{
    int Allow = TRUE;

#if !defined(__NO_CODEC__)
    if(SamplesSwap[ped_patsam]) Allow = FALSE;
#endif

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(SampleType[ped_patsam][ped_split])
        {
            // Set Loop Start
            if(zcheckMouse(712, 476, 60, 16) && sed_range_mode)
            {
                if(sed_range_start > sed_range_end)
                {
                    LoopStart[ped_patsam][ped_split] = sed_range_end;
                }
                else
                {
                    LoopStart[ped_patsam][ped_split] = sed_range_start;
                }

                if(LoopStart[ped_patsam][ped_split] > LoopEnd[ped_patsam][ped_split])
                {
                    LoopEnd[ped_patsam][ped_split] = LoopStart[ped_patsam][ped_split];
                }

                if(LoopType[ped_patsam][ped_split] == SMP_LOOP_NONE)
                {
                    LoopType[ped_patsam][ped_split] = SMP_LOOP_FORWARD;
                    LoopEnd[ped_patsam][ped_split] = SampleNumSamples[ped_patsam][ped_split] - 1;
                }

                draw_sampled_wave = TRUE;
                if(userscreen == USER_SCREEN_INSTRUMENT_EDIT)
                {
                    gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                    teac = 5;
                }
            }

            // Set Loop End
            if(zcheckMouse(712, 494, 60, 16) && sed_range_mode)
            {
                if(sed_range_start > sed_range_end)
                {
                    LoopEnd[ped_patsam][ped_split] = sed_range_start;
                }
                else
                {
                    LoopEnd[ped_patsam][ped_split] = sed_range_end;
                }

                if(LoopEnd[ped_patsam][ped_split] < LoopStart[ped_patsam][ped_split])
                {
                    LoopStart[ped_patsam][ped_split] = LoopEnd[ped_patsam][ped_split];
                }

                if(LoopType[ped_patsam][ped_split] == SMP_LOOP_NONE)
                {
                    LoopType[ped_patsam][ped_split] = SMP_LOOP_FORWARD;
                    LoopStart[ped_patsam][ped_split] = 0;
                }

                if(LoopEnd[ped_patsam][ped_split] == LoopStart[ped_patsam][ped_split])
                {
                    LoopType[ped_patsam][ped_split] = SMP_LOOP_NONE;
                    LoopStart[ped_patsam][ped_split] = 0;
                    LoopEnd[ped_patsam][ped_split] = 0;
                }

                draw_sampled_wave = TRUE;

                if(userscreen == USER_SCREEN_INSTRUMENT_EDIT)
                {
                    gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                    teac = 5;
                }
            }

            // Cut
            if(zcheckMouse(520, 476, 29, 16) && sed_range_mode && Allow)
            {
                teac = 20;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // DC Adjust
            if(zcheckMouse(520, 512, 60, 16) && sed_range_mode && Allow)
            {
                teac = 21;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Maximize
            if(zcheckMouse(520, 494, 60, 16) && sed_range_mode && Allow)
            {
                teac = 22;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Fade in
            if(zcheckMouse(520, 530, 60, 16) && sed_range_mode && Allow)
            {
                teac = 23;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Fade out
            if(zcheckMouse(520, 548, 60, 16) && sed_range_mode && Allow)
            {
                teac = 24;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Half
            if(zcheckMouse(551, 476, 29, 16) && sed_range_mode && Allow)
            {
                teac = 25;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // View All
            if(zcheckMouse(582, 512, 60, 16))
            {
                rs_coef = 32768;
                sed_display_start = 0;
                sed_display_length = SampleNumSamples[ped_patsam][ped_split];
                draw_sampled_wave = TRUE;
                teac = 3;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // VZoom In
            if(zcheckMouse(582, 530, 60, 16))
            {
                rs_coef /= 2;
                draw_sampled_wave = TRUE;
            }

            // VZoom Out
            if(zcheckMouse(582, 548, 60, 16))
            {
                rs_coef *= 2;
                draw_sampled_wave = TRUE;
            }

            // Unselect
            if(zcheckMouse(582, 494, 60, 16))
            {
                sed_range_mode = FALSE;
                sed_range_start = 0;
                sed_range_end = 0;
                draw_sampled_wave = TRUE;
                teac = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select all
            if(zcheckMouse(582, 476, 60, 16))
            {
                sed_range_mode = TRUE;
                sed_range_start = sed_display_start;
                sed_range_end = sed_display_start + sed_display_length - 1;
                draw_sampled_wave = TRUE;
                teac = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Zoom in
            if(zcheckMouse(650, 476, 60, 16) && sed_range_mode)
            {
                sed_range_mode = FALSE;
                if((int) sed_range_end < (int) sed_range_start)
                {
                    int swap_range = sed_range_start;
                    sed_range_start = sed_range_end;
                    sed_range_end = swap_range;
                }
                sed_display_start = sed_range_start;
                sed_display_length = (sed_range_end - sed_range_start) + 1;
                draw_sampled_wave = TRUE;
                teac = 3;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Zoom out
            if(zcheckMouse(650, 494, 60, 16))
            {
                int start_test;
                sed_display_start -= sed_display_length;
                start_test = sed_display_start;
                if(start_test < 0) sed_display_start = 0;
                sed_display_length *= 3;

                if(sed_display_length > (int32) SampleNumSamples[ped_patsam][ped_split])
                {
                    sed_display_length = SampleNumSamples[ped_patsam][ped_split];
                }
                if(sed_display_length + sed_display_start > (int32) SampleNumSamples[ped_patsam][ped_split])
                {
                    sed_display_start = SampleNumSamples[ped_patsam][ped_split] - sed_display_length;
                }
                draw_sampled_wave = TRUE;
                teac = 3;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }
        }
    }
}

// ------------------------------------------------------
// Mouse wheel events
void Mouse_Wheel_Sample_Ed(int roll_amount)
{
    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(WAVE_LEFT + 1, 450, 512, 109 + 16))
        {
            int max_length = SampleNumSamples[ped_patsam][ped_split];
            sed_display_start += roll_amount * (sed_display_length / 16);

            if(sed_display_start < 0) sed_display_start = 0;
            if(sed_display_start > (max_length - sed_display_length))
            {
                sed_display_start = (max_length - sed_display_length);
            }
            gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            draw_sampled_wave = TRUE;
        }
    }
}

// ------------------------------------------------------
// Sliders events
void Mouse_Sliders_Sample_Ed(void)
{
    int Mouse_Pos;
    double test;

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(WAVE_LEFT, 450, 512, 109))
        {
            // Select a range
            if(SampleType[ped_patsam][ped_split])
            {
                Mouse_Pos = (Mouse.x - WAVE_LEFT) - 1;
                if(Mouse_Pos < 0) Mouse_Pos = 0;
                if(Mouse_Pos > 512) Mouse_Pos = 512;
                axswave = SampleNumSamples[ped_patsam][ped_split];
                sed_range_mode = TRUE;
                test = ((Mouse_Pos) * sed_display_length) / 512;
                sed_range_end = sed_display_start + (int32) test;
                teac = 4;

                if(!sas)
                {
                    // Selection start
                    sed_range_start = sed_range_end;
                    teac = 5;
                }

                if(sed_range_start < sed_display_start)
                {
                    sed_range_start = sed_display_start;
                    teac = 5;
                }
                if(sed_range_end < sed_display_start)
                {
                    sed_range_end = sed_display_start;
                    teac = 5;
                }

                if(sed_range_end > axswave)
                {
                    sed_range_end = axswave;
                }
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                draw_sampled_wave = TRUE;
                sas = TRUE;
            } // SAMPLETYPE
        } // MOUSEBOX

        // Sample slider
        if(zcheckMouse(WAVE_LEFT, 559, 512 + 2, 16))
        {
            if(SampleNumSamples[ped_patsam][ped_split])
            {
                int max_length = SampleNumSamples[ped_patsam][ped_split];
                int Center = Slider_Get_Center(sed_display_length, max_length, 512 + 2);
                float Pos_Mouse = ((float) ((Mouse.x - WAVE_LEFT) - (Center / 2))) / 512.0f;
                if(Pos_Mouse > 1.0f) Pos_Mouse = 1.0f;
                float s_offset = (Pos_Mouse * max_length);

                if(s_offset > (float) (max_length - sed_display_length))
                {
                    s_offset = (float) (max_length - sed_display_length);
                }
                if(s_offset < 0) s_offset = 0;
                sed_display_start = (int32) s_offset;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                draw_sampled_wave = TRUE;
            }
        }
    }
}

// ------------------------------------------------------
// Make sure the loop infos are sane
void Check_Loops(void)
{
    if(LoopStart[ped_patsam][ped_split] < 0) LoopStart[ped_patsam][ped_split] = 0;
    if(LoopStart[ped_patsam][ped_split] >= SampleNumSamples[ped_patsam][ped_split])
    {
        LoopStart[ped_patsam][ped_split] = SampleNumSamples[ped_patsam][ped_split];
    }
    if(LoopEnd[ped_patsam][ped_split] < 0) LoopEnd[ped_patsam][ped_split] = 0;
    if(LoopEnd[ped_patsam][ped_split] >= SampleNumSamples[ped_patsam][ped_split])
    {
        LoopEnd[ped_patsam][ped_split] = SampleNumSamples[ped_patsam][ped_split];
    }
    if(LoopStart[ped_patsam][ped_split] == LoopEnd[ped_patsam][ped_split])
    {
        LoopStart[ped_patsam][ped_split] = 0;
        LoopEnd[ped_patsam][ped_split] = SampleNumSamples[ped_patsam][ped_split];
    }
}
