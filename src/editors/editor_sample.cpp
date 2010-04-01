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

#define LARGE_SMP_VIEW 512

#define SMPED_CUT 20
#define SMPED_COPY (SMPED_CUT + 1)
#define SMPED_PASTE (SMPED_COPY + 1)
#define SMPED_DC_ADJUST (SMPED_PASTE + 1)
#define SMPED_MAXIMIZING (SMPED_DC_ADJUST + 1)
#define SMPED_FADEIN (SMPED_MAXIMIZING + 1)
#define SMPED_FADEOUT (SMPED_FADEIN + 1)
#define SMPED_HALF (SMPED_FADEOUT + 1)
#define SMPED_BUF1 (SMPED_HALF + 1)
#define SMPED_BUF2 (SMPED_BUF1 + 1)
#define SMPED_BUF3 (SMPED_BUF2 + 1)
#define SMPED_BUF4 (SMPED_BUF3 + 1)
#define SMPED_ZAP (SMPED_BUF4 + 1)
#define SMPED_CROP (SMPED_ZAP + 1)
#define SMPED_REVERSE (SMPED_CROP + 1)
#define SMPED_ROTATE_LEFT_1 (SMPED_REVERSE + 1)
#define SMPED_ROTATE_LEFT_X (SMPED_ROTATE_LEFT_1 + 1)
#define SMPED_ROTATE_RIGHT_1 (SMPED_ROTATE_LEFT_X + 1)
#define SMPED_ROTATE_RIGHT_X (SMPED_ROTATE_RIGHT_1 + 1)

// ------------------------------------------------------
// Variables
extern s_access sp_Position[MAX_TRACKS][MAX_POLYPHONY];

int32 axswave = 0;

int32 sed_display_start = 0;
int32 sed_display_length = 0;
int32 sed_range_start = 0;
int32 sed_range_end = 0;
char sed_range_mode = FALSE;

int cur_smp_buffer[] =
{
    BUTTON_PUSHED,
    BUTTON_NORMAL,
    BUTTON_NORMAL,
    BUTTON_NORMAL
};

// ------------------------------------------------------
// Functions
void Draw_Wave_PlayBack_Pos(void);
void Draw_Wave_Data3(void);
void Refresh_Sample(int clear_sel);
void Display_Sample_Buffers(int Allow);
void Zoom_In_Sel();
void Zoom_Out_Sel();

void Draw_Sample_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_SAMPLE_EDIT);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("");

    Gui_Draw_Button_Box(650, 522, 60, 16, "Range", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(712, 522, 60, 16, "View", BUTTON_NORMAL | BUTTON_DISABLED);
        
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
            Realslider_Horiz(WAVE_LEFT + 18, 559, sed_display_start, sed_display_length,
                             SampleNumSamples[Current_Sample][Current_Sample_Split], LARGE_SMP_VIEW + 2 - (18 * 2), TRUE);

            // Layout around the waveform
            SetColor(COL_BACKGROUND);
            Fillrect(WAVE_LEFT, 450, LARGE_SMP_VIEW + WAVE_LEFT + 1, SAMPLE_HEIGHT);
            DrawHLine(450, WAVE_LEFT, LARGE_SMP_VIEW + WAVE_LEFT + 1, COL_BLACK);
            DrawHLine(SAMPLE_HEIGHT - 1, WAVE_LEFT, LARGE_SMP_VIEW + WAVE_LEFT + 1, COL_BLACK);
            DrawVLine(LARGE_SMP_VIEW + WAVE_LEFT + 1, 450, SAMPLE_HEIGHT - 1, COL_BLACK);
            DrawVLine(WAVE_LEFT, 450, SAMPLE_HEIGHT - 1, COL_BLACK);

            if(SampleType[Current_Sample][Current_Sample_Split] > 0)
            {   
                // Any Sample Out There?
                int strober = SampleChannels[Current_Sample][Current_Sample_Split] * 2;
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
                end_rect = LARGE_SMP_VIEW - 1;
                set_start_rect = 0;
                for(s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                {
                    s_offset = (s_ex * sed_display_length) / LARGE_SMP_VIEW + sed_display_start;

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
                    if(end_rect == start_rect) end_rect++;
                    Fillrect(start_rect + WAVE_LEFT + 1, 450 + 1, end_rect + WAVE_LEFT + 1, SAMPLE_HEIGHT - 1);
                }

                // Now draw the sample data
                switch(SampleChannels[Current_Sample][Current_Sample_Split])
                {
                    case 1:
                        // Mono sample
                        for(s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                        {
                            s_offset = (s_ex * sed_display_length) / LARGE_SMP_VIEW + sed_display_start;
                            int h = *(RawSamples[Current_Sample][0][Current_Sample_Split] + s_offset) / rs_coef;
                            if(h > s_size) h = s_size;
                            if(h < -s_size) h = -s_size;

                            int s_y = s_ey - h;
                            if(s_y > SAMPLE_HEIGHT - 2) s_y = SAMPLE_HEIGHT - 2;

                            // Look for selection
                            if(sed_range_mode &&
                               s_offset >= sed_real_range_start &&
                               s_offset < sed_real_range_end)
                            {

                                // Sample datas
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_BACKGROUND);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_BACKGROUND);
                            }
                            else
                            {
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_SCOPESSAMPLES);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_SCOPESSAMPLES);
                            }
                        }
                        break;

                    case 2:
                        // Stereo sample
                        for(s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                        {
                            s_offset = (s_ex * sed_display_length) / LARGE_SMP_VIEW + sed_display_start;
                            int h = *(RawSamples[Current_Sample][0][Current_Sample_Split] + s_offset) / rs_coef;
                            int h2 = *(RawSamples[Current_Sample][1][Current_Sample_Split] + s_offset) / rs_coef;
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
                               s_offset < sed_real_range_end)
                            {
                                // Sample datas
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_BACKGROUND);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_y2, COL_BACKGROUND);
                                // Straight line
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_BACKGROUND);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey2, COL_BACKGROUND);
                            }
                            else
                            {
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_SCOPESSAMPLES);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_y2, COL_SCOPESSAMPLES);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_SCOPESSAMPLES);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey2, COL_SCOPESSAMPLES);
                            }
                        }
                        break;
                }

                // Display the loop infos bars
                if(LoopType[Current_Sample][Current_Sample_Split])
                {
                    int32 LSX = ((LoopStart[Current_Sample][Current_Sample_Split] - sed_display_start) * LARGE_SMP_VIEW) / sed_display_length;
                    int32 LEX = ((LoopEnd[Current_Sample][Current_Sample_Split] - sed_display_start) * LARGE_SMP_VIEW) / sed_display_length;

                    if(LSX >= 0 && LSX <= LARGE_SMP_VIEW)
                    {
                        DrawVLine(LSX + WAVE_LEFT + 1, 450 + 1, SAMPLE_HEIGHT - 2, COL_VUMETERPEAK);
                        DrawHLine(450 + 5, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine(450 + 6, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 2, COL_VUMETERPEAK);
                        DrawHLine(450 + 7, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine(450 + 8, LSX + WAVE_LEFT + 1 + 5, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine(450 + 9, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                    }

                    if(LEX >= 0 && LEX <= LARGE_SMP_VIEW)
                    {
                        if(LEX == LARGE_SMP_VIEW) LEX--;
                        DrawVLine(LEX + WAVE_LEFT + 1 + 1, 450 + 1, SAMPLE_HEIGHT - 2, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 9, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 8, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 5, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 7, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 6, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 5, COL_VUMETERPEAK);
                        DrawHLine(SAMPLE_HEIGHT - 5, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
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
    sed_display_length = SampleNumSamples[Current_Sample][Current_Sample_Split];
    sed_range_start = 0;
    sed_range_end = 0;
    sed_range_mode = FALSE;
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
        if(SampleType[Current_Sample][Current_Sample_Split] > 0)
        { 
            // Any Sample Out There ?
            int strober = SampleChannels[Current_Sample][Current_Sample_Split] * 2;
            int s_ey = 450 + 1 + (SAMPLE_LINES_HEIGHT / strober);
            int s_ey2 = s_ey + ((SAMPLE_LINES_HEIGHT / strober) * 2);
            int rcolor3;
            int rcolor4;

            pos_in_sample = 0;
            for(i = 0; i < MAX_POLYPHONY; i++)
            {
                if((int32) sp_Position[Track_Under_Caret][i].half.first > pos_in_sample)
                {
                    if(sp_Stage[Track_Under_Caret][i] == PLAYING_SAMPLE)
                    {
                        pos_in_sample = sp_Position[Track_Under_Caret][i].half.first;
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

            if(SampleChannels[Current_Sample][Current_Sample_Split] == 1)
            {
                for(int32 s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                {
                    int32 s_offset = (s_ex * sed_display_length) / LARGE_SMP_VIEW + sed_display_start;

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
            if(SampleChannels[Current_Sample][Current_Sample_Split] == 2)
            {
                for(int32 s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                {
                    int32 s_offset = (s_ex * sed_display_length) / LARGE_SMP_VIEW + sed_display_start;

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
    int ReadOnly;
    int Allow;

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        Allow = 0;
        if(!SampleType[Current_Sample][Current_Sample_Split]) Allow = BUTTON_DISABLED;

#if !defined(__NO_CODEC__)
        if(SamplesSwap[Current_Sample]) ReadOnly = BUTTON_DISABLED;
        else
#endif
        {
            ReadOnly = 0;
        }

        if(gode == 0)
        {

            // Those commands aren't available when viewing packed samples (which are read only)
            Gui_Draw_Button_Box(520, 450, 29, 16, "Cut", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, 450, 29, 16, "Copy", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, 450, 29, 16, "Paste", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(613, 450, 29, 16, "Crop", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, 468, 29, 16, "Half", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(650, 468, 16, 16, "\05", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Gui_Draw_Button_Box(668, 468, 16, 16, "\03", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(686, 468, 16, 16, "\04", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(704, 468, 16, 16, "\06", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Gui_Draw_Button_Box(722, 468, 50, 16, "Reverse", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            
            Gui_Draw_Button_Box(520, 468, 29, 16, "Zap", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(520, 486, 60, 16, "Maximize", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, 504, 60, 16, "DC Adjust", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, 522, 60, 16, "Fade In", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, 540, 60, 16, "Fade Out", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);

            // Non-modifying operations are allowed
            Gui_Draw_Button_Box(582, 468, 60, 16, "Sel. View", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, 486, 60, 16, "Unselect", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(582, 504, 60, 16, "View All", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, 522, 60, 16, "VZoom In", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, 540, 60, 16, "VZoom Out", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(650, 486, 60, 16, "Zoom In", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(650, 504, 60, 16, "Zoom Out", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(712, 486, 60, 16, "Loop Start", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(712, 504, 60, 16, "Loop End", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(WAVE_LEFT, 559, 16, 16, "\03", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Gui_Draw_Button_Box(WAVE_LEFT + LARGE_SMP_VIEW - (18 * 1) + 3, 559, 16, 16, "\04", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);

            Gui_Draw_Button_Box(722, 450, 50, 16, "Sel. All", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Display_Sample_Buffers(Allow | ReadOnly);
        }

        sed_real_range_start = sed_range_start;
        sed_real_range_end = sed_range_end;

        if(sed_real_range_start > sed_real_range_end)
        {
            int Swap_Range = sed_real_range_start;
            sed_real_range_start = sed_real_range_end;
            sed_real_range_end = Swap_Range;
        }

        if(gode == 5 || gode == 0)
        {
            if(LoopType[Current_Sample][Current_Sample_Split] == SMP_LOOP_NONE)
            {
                Gui_Draw_Button_Box(520, 558, 60, 16, "S: -", BUTTON_NORMAL | BUTTON_DISABLED);
                Gui_Draw_Button_Box(582, 558, 60, 16, "E: -", BUTTON_NORMAL | BUTTON_DISABLED);
            }
            else
            {
                outlong(520, 558, LoopStart[Current_Sample][Current_Sample_Split], 11);
                outlong(582, 558, LoopEnd[Current_Sample][Current_Sample_Split], 12);
            }
        }

        if(SampleType[Current_Sample][Current_Sample_Split])
        {
            if(gode == 3 || gode == 0)
            {
                outlong(712, 540, sed_display_start, 10);
                outlong(712, 558, sed_display_length, 12);
            }

            if(gode == 4 || gode == 5 || gode == 0)
            {
                outlong(650, 540, sed_real_range_start, 10);
            }

            if(gode == 4 || gode == 5 || gode == 0)
            {
                outlong(650, 558, sed_real_range_end, 11);
            }

            // Rotate 1 sample left
            if(gode == SMPED_ROTATE_LEFT_1)
            {
                if(Sample_Rotate_Left(sed_real_range_start, sed_real_range_end, 1))
                {
                    Refresh_Sample(FALSE);
                }
            }

            // Rotate 1 sample left
            if(gode == SMPED_ROTATE_RIGHT_1)
            {
                if(Sample_Rotate_Right(sed_real_range_start, sed_real_range_end, 1))
                {
                    Refresh_Sample(FALSE);
                }
            }

            // Rotate x samples left
            if(gode == SMPED_ROTATE_LEFT_X)
            {
                if(Sample_Rotate_Left(sed_real_range_start, sed_real_range_end, (sed_real_range_end - sed_real_range_start) / 2))
                {
                    Refresh_Sample(FALSE);
                }
            }

            // Rotate x samples right
            if(gode == SMPED_ROTATE_RIGHT_X)
            {
                if(Sample_Rotate_Right(sed_real_range_start, sed_real_range_end, (sed_real_range_end - sed_real_range_start) / 2))
                {
                    Refresh_Sample(FALSE);
                }
            }

            // Cut Sample
            if(gode == SMPED_CUT)
            {
                if(Sample_Cut(sed_real_range_start, sed_real_range_end, TRUE))
                {
                    Refresh_Sample(TRUE);
                }
            }

            // Copy Sample
            if(gode == SMPED_COPY)
            {
                if(Sample_Copy(sed_real_range_start, sed_real_range_end))
                {
                    Display_Sample_Buffers(Allow | ReadOnly);
                }
            }

            // Paste Sample
            if(gode == SMPED_PASTE)
            {
                if(Sample_Paste(sed_real_range_start))
                {
                    Refresh_Sample(TRUE);
                }
            }

            // Crop Sample
            if(gode == SMPED_CROP)
            {
                if(Sample_Crop(sed_real_range_start, sed_real_range_end))
                {
                    Refresh_Sample(TRUE);
                }
            }

            // Reverse Sample
            if(gode == SMPED_REVERSE)
            {
                if(Sample_Reverse(sed_real_range_start, sed_real_range_end))
                {
                    Refresh_Sample(FALSE);
                }
            }

            // Select buffer 1
            if(gode == SMPED_BUF1)
            {
                cur_sample_buffer = 0;
                Display_Sample_Buffers(Allow | ReadOnly);
            }

            // Select buffer 2
            if(gode == SMPED_BUF2)
            {
                cur_sample_buffer = 1;
                Display_Sample_Buffers(Allow | ReadOnly);
            }

            // Select buffer 3
            if(gode == SMPED_BUF3)
            {
                cur_sample_buffer = 2;
                Display_Sample_Buffers(Allow | ReadOnly);
            }

            // Select buffer 4
            if(gode == SMPED_BUF4)
            {
                cur_sample_buffer = 3;
                Display_Sample_Buffers(Allow | ReadOnly);
            }

            // Zap
            if(gode == SMPED_ZAP)
            {
                if(Sample_Cut(sed_real_range_start, sed_real_range_end, FALSE))
                {
                    Refresh_Sample(TRUE);
                }
            }

            // DC adjust
            if(gode == SMPED_DC_ADJUST)
            {
                Sample_DC_Adjust(sed_real_range_start, sed_real_range_end);
            }

            // Maximizing
            if(gode == SMPED_MAXIMIZING)
            {
                Sample_Maximize(sed_real_range_start, sed_real_range_end);
            }

            // Fade In
            if(gode == SMPED_FADEIN)
            {
                Sample_FadeIn(sed_real_range_start, sed_real_range_end);
            }

            // Fade Out
            if(gode == SMPED_FADEOUT)
            {
                Sample_FadeOut(sed_real_range_start, sed_real_range_end);
            }

            // Half
            if(gode == SMPED_HALF)
            {
                Sample_Half(sed_real_range_start, sed_real_range_end);
            }

        }
        else
        {
            Gui_Draw_Button_Box(650, 540, 122, 34, "No Sample Loaded", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
        }
    }
}

// ------------------------------------------------------
// Right mouse buttons events
void Mouse_Right_Sample_Ed(void)
{
    int Allow = TRUE;

#if !defined(__NO_CODEC__)
    if(SamplesSwap[Current_Sample]) Allow = FALSE;
#endif

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(WAVE_LEFT + 1, 450, LARGE_SMP_VIEW, SAMPLE_LINES_HEIGHT))
        {
            if(sed_range_mode)
            {
                // Unselect
                sed_range_start = 0;
                sed_range_end = 0;
                sed_range_mode = FALSE;
                draw_sampled_wave = TRUE;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                teac = 4;
            }
            else
            {
                // Select view
                sed_range_mode = TRUE;
                sed_range_start = sed_display_start;
                sed_range_end = sed_display_start + sed_display_length;
                draw_sampled_wave = TRUE;
                teac = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }
        }

        // Rotate left x
        if(zcheckMouse(650, 468, 16, 16) && sed_range_mode && Allow)
        {
            teac = SMPED_ROTATE_LEFT_X;
            gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

        // Rotate right x
        if(zcheckMouse(704, 468, 16, 16) && sed_range_mode && Allow)
        {
            teac = SMPED_ROTATE_RIGHT_X;
            gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

        // Bottom arrow left
        if(zcheckMouse(WAVE_LEFT, 559, 16, 16))
        {
            if(SampleNumSamples[Current_Sample][Current_Sample_Split])
            {
                sed_display_start -= sed_display_length;
                if((int) sed_display_start < 0) sed_display_start = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                draw_sampled_wave = TRUE;
            }
        }

        // Bottom arrow right
        if(zcheckMouse(WAVE_LEFT + LARGE_SMP_VIEW - (18 * 1) + 3, 559, 16, 16))
        {
            if(SampleNumSamples[Current_Sample][Current_Sample_Split])
            {
                int max_length = SampleNumSamples[Current_Sample][Current_Sample_Split] - sed_display_length;
                sed_display_start += sed_display_length;
                if((int) sed_display_start > max_length) sed_display_start = max_length;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                draw_sampled_wave = TRUE;
            }
        }
    }
}

// ------------------------------------------------------
// Middle mouse buttons events
void Mouse_Middle_Sample_Ed(void)
{
    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(WAVE_LEFT + 1, 450, LARGE_SMP_VIEW, SAMPLE_LINES_HEIGHT))
        {
            if(sed_range_mode && sed_range_start != sed_range_end) Zoom_In_Sel();
            else Zoom_Out_Sel();
        }
    }
}

// ------------------------------------------------------
// Left mouse buttons events
void Mouse_Left_Sample_Ed(void)
{
    int Allow = TRUE;

#if !defined(__NO_CODEC__)
    if(SamplesSwap[Current_Sample]) Allow = FALSE;
#endif

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(SampleType[Current_Sample][Current_Sample_Split])
        {
            // Set Loop Start
            if(zcheckMouse(712, 486, 60, 16) && sed_range_mode)
            {
                if(sed_range_start > sed_range_end)
                {
                    LoopStart[Current_Sample][Current_Sample_Split] = sed_range_end;
                }
                else
                {
                    LoopStart[Current_Sample][Current_Sample_Split] = sed_range_start;
                }

                if(LoopStart[Current_Sample][Current_Sample_Split] > LoopEnd[Current_Sample][Current_Sample_Split])
                {
                    LoopEnd[Current_Sample][Current_Sample_Split] = LoopStart[Current_Sample][Current_Sample_Split];
                }

                // Initialize a new loop
                if(LoopType[Current_Sample][Current_Sample_Split] == SMP_LOOP_NONE)
                {
                    LoopType[Current_Sample][Current_Sample_Split] = SMP_LOOP_FORWARD;
                    LoopEnd[Current_Sample][Current_Sample_Split] = SampleNumSamples[Current_Sample][Current_Sample_Split];
                }

                draw_sampled_wave = TRUE;
                switch(userscreen)
                {
                    case USER_SCREEN_INSTRUMENT_EDIT:
                        gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                        teac = 5;
                        break;

                    case USER_SCREEN_SAMPLE_EDIT:
                        teac = 5;
                        gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                        break;
                }
            }

            // Set Loop End
            if(zcheckMouse(712, 504, 60, 16) && sed_range_mode)
            {
                if(sed_range_start > sed_range_end)
                {
                    LoopEnd[Current_Sample][Current_Sample_Split] = sed_range_start;
                }
                else
                {
                    LoopEnd[Current_Sample][Current_Sample_Split] = sed_range_end;
                }

                if(LoopEnd[Current_Sample][Current_Sample_Split] < LoopStart[Current_Sample][Current_Sample_Split])
                {
                    LoopStart[Current_Sample][Current_Sample_Split] = LoopEnd[Current_Sample][Current_Sample_Split];
                }

                if(LoopType[Current_Sample][Current_Sample_Split] == SMP_LOOP_NONE)
                {
                    LoopType[Current_Sample][Current_Sample_Split] = SMP_LOOP_FORWARD;
                    LoopStart[Current_Sample][Current_Sample_Split] = 0;
                }

                if(LoopEnd[Current_Sample][Current_Sample_Split] == LoopStart[Current_Sample][Current_Sample_Split])
                {
                    LoopType[Current_Sample][Current_Sample_Split] = SMP_LOOP_NONE;
                    LoopStart[Current_Sample][Current_Sample_Split] = 0;
                    LoopEnd[Current_Sample][Current_Sample_Split] = 0;
                }

                draw_sampled_wave = TRUE;

                switch(userscreen)
                {
                    case USER_SCREEN_INSTRUMENT_EDIT:
                        gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                        teac = 5;
                        break;

                    case USER_SCREEN_SAMPLE_EDIT:
                        teac = 5;
                        gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                        break;
                }
            }

            // Rotate left 1
            if(zcheckMouse(668, 468, 16, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_ROTATE_LEFT_1;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Rotate right 1
            if(zcheckMouse(686, 468, 16, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_ROTATE_RIGHT_1;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Cut
            if(zcheckMouse(520, 450, 29, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_CUT;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Copy
            if(zcheckMouse(551, 450, 29, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_COPY;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Paste
            if(zcheckMouse(582, 450, 29, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_PASTE;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Crop
            if(zcheckMouse(613, 450, 29, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_CROP;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Reverse
            if(zcheckMouse(722, 468, 50, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_REVERSE;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 1
            if(zcheckMouse(650, 450, 16, 16) && Allow)
            {
                teac = SMPED_BUF1;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 2
            if(zcheckMouse(668, 450, 16, 16) && Allow)
            {
                teac = SMPED_BUF2;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 3
            if(zcheckMouse(686, 450, 16, 16) && Allow)
            {
                teac = SMPED_BUF3;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 4
            if(zcheckMouse(704, 450, 16, 16) && Allow)
            {
                teac = SMPED_BUF4;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 4
            if(zcheckMouse(520, 468, 16, 16) && Allow)
            {
                teac = SMPED_ZAP;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // DC Adjust
            if(zcheckMouse(520, 504, 60, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_DC_ADJUST;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Maximize
            if(zcheckMouse(520, 486, 60, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_MAXIMIZING;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Fade in
            if(zcheckMouse(520, 522, 60, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_FADEIN;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Fade out
            if(zcheckMouse(520, 540, 60, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_FADEOUT;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Half
            if(zcheckMouse(551, 468, 29, 16) && sed_range_mode && Allow)
            {
                teac = SMPED_HALF;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // View All
            if(zcheckMouse(582, 504, 60, 16))
            {
                rs_coef = 32768;
                sed_display_start = 0;
                sed_display_length = SampleNumSamples[Current_Sample][Current_Sample_Split];
                draw_sampled_wave = TRUE;
                teac = 3;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // VZoom In
            if(zcheckMouse(582, 522, 60, 16))
            {
                rs_coef /= 2;
                draw_sampled_wave = TRUE;
            }

            // VZoom Out
            if(zcheckMouse(582, 540, 60, 16))
            {
                rs_coef *= 2;
                draw_sampled_wave = TRUE;
            }

            // Unselect
            if(zcheckMouse(582, 486, 60, 16))
            {
                sed_range_mode = FALSE;
                sed_range_start = 0;
                sed_range_end = 0;
                draw_sampled_wave = TRUE;
                teac = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select all
            if(zcheckMouse(722, 450, 50, 16))
            {
                sed_range_mode = TRUE;
                sed_range_start = 0;
                sed_range_end = SampleNumSamples[Current_Sample][Current_Sample_Split];
                draw_sampled_wave = TRUE;
                teac = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select view
            if(zcheckMouse(582, 468, 60, 16))
            {
                sed_range_mode = TRUE;
                sed_range_start = sed_display_start;
                sed_range_end = sed_display_start + sed_display_length;
                draw_sampled_wave = TRUE;
                teac = 0;
                gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Zoom in
            if(zcheckMouse(650, 486, 60, 16) && sed_range_mode)
            {
                Zoom_In_Sel();
            }

            // Zoom out
            if(zcheckMouse(650, 504, 60, 16))
            {
                Zoom_Out_Sel();
            }

            // Bottom arrow left
            if(zcheckMouse(WAVE_LEFT, 559, 16, 16))
            {
                if(SampleNumSamples[Current_Sample][Current_Sample_Split])
                {
                    sed_display_start--;
                    if((int) sed_display_start < 0) sed_display_start = 0;
                    gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                    draw_sampled_wave = TRUE;
                }
            }

            // Bottom arrow right
            if(zcheckMouse(WAVE_LEFT + LARGE_SMP_VIEW - (18 * 1) + 3, 559, 16, 16))
            {
                if(SampleNumSamples[Current_Sample][Current_Sample_Split])
                {
                    int max_length = SampleNumSamples[Current_Sample][Current_Sample_Split] - sed_display_length;
                    sed_display_start++;
                    if((int) sed_display_start > max_length) sed_display_start = max_length;
                    gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                    draw_sampled_wave = TRUE;
                }
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
        if(zcheckMouse(WAVE_LEFT + 1, 450, LARGE_SMP_VIEW, 109 + 16))
        {
            int max_length = SampleNumSamples[Current_Sample][Current_Sample_Split];
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
    int Allow = TRUE;

#if !defined(__NO_CODEC__)
    if(SamplesSwap[Current_Sample]) Allow = FALSE;
#endif

    if(userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(WAVE_LEFT, 450, LARGE_SMP_VIEW + 1, 109))
        {
            // Move within the sample
            if(SampleType[Current_Sample][Current_Sample_Split])
            {
                Mouse_Pos = (Mouse.x - WAVE_LEFT) - 1;
                if(Mouse_Pos < 0) Mouse_Pos = 0;
                if(Mouse_Pos > LARGE_SMP_VIEW) Mouse_Pos = LARGE_SMP_VIEW;
                axswave = SampleNumSamples[Current_Sample][Current_Sample_Split];
                sed_range_mode = TRUE;
                test = ((Mouse_Pos) * sed_display_length) / LARGE_SMP_VIEW;
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

        // Bottom slider
        if(zcheckMouse(WAVE_LEFT + 18, 559, LARGE_SMP_VIEW + 2 - (18 * 2), 16))
        {
            if(SampleNumSamples[Current_Sample][Current_Sample_Split])
            {
                int max_length = SampleNumSamples[Current_Sample][Current_Sample_Split];
                int Center = Slider_Get_Center(sed_display_length, max_length, LARGE_SMP_VIEW + 2 - (18 * 2));
                float Pos_Mouse = ((float) ((Mouse.x - (WAVE_LEFT + 18)) - (Center / 2))) / (float) (LARGE_SMP_VIEW + 2 - (18 * 2));
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

        // Rotate left 1
        if(zcheckMouse(650, 468, 16, 16) && sed_range_mode && Allow)
        {
            teac = SMPED_ROTATE_LEFT_1;
            gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

        // Rotate right 1
        if(zcheckMouse(704, 468, 16, 16) && sed_range_mode && Allow)
        {
            teac = SMPED_ROTATE_RIGHT_1;
            gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

    }
}

// ------------------------------------------------------
// Make sure the loop infos are sane
void Check_Loops(void)
{
    if(LoopStart[Current_Sample][Current_Sample_Split] < 0) LoopStart[Current_Sample][Current_Sample_Split] = 0;
    if(LoopStart[Current_Sample][Current_Sample_Split] > SampleNumSamples[Current_Sample][Current_Sample_Split])
    {
        LoopStart[Current_Sample][Current_Sample_Split] = SampleNumSamples[Current_Sample][Current_Sample_Split];
    }
    if(LoopEnd[Current_Sample][Current_Sample_Split] < 0) LoopEnd[Current_Sample][Current_Sample_Split] = 0;
    if(LoopEnd[Current_Sample][Current_Sample_Split] > SampleNumSamples[Current_Sample][Current_Sample_Split])
    {
        LoopEnd[Current_Sample][Current_Sample_Split] = SampleNumSamples[Current_Sample][Current_Sample_Split];
    }
    if(LoopStart[Current_Sample][Current_Sample_Split] == LoopEnd[Current_Sample][Current_Sample_Split])
    {
        LoopStart[Current_Sample][Current_Sample_Split] = 0;
        LoopEnd[Current_Sample][Current_Sample_Split] = SampleNumSamples[Current_Sample][Current_Sample_Split];
    }
}

// ------------------------------------------------------
// Bring the sample editor up to date
void Refresh_Sample(int clear_sel)
{
    int ReadOnly;
    int Allow = 0;
    if(!SampleType[Current_Sample][Current_Sample_Split]) Allow = BUTTON_DISABLED;

#if !defined(__NO_CODEC__)
    if(SamplesSwap[Current_Sample]) ReadOnly = BUTTON_DISABLED;
    else
#endif
    {
        ReadOnly = 0;
    }

    // Adjust after the cut
    if(sed_display_length > (int32) SampleNumSamples[Current_Sample][Current_Sample_Split])
    {
        sed_display_length = SampleNumSamples[Current_Sample][Current_Sample_Split];
    }
    if(sed_display_length + sed_display_start > (int32) SampleNumSamples[Current_Sample][Current_Sample_Split])
    {
        sed_display_start = SampleNumSamples[Current_Sample][Current_Sample_Split] - sed_display_length;
    }
    if(clear_sel)
    {
        sed_range_mode = FALSE;
        sed_range_start = 0;
        sed_range_end = 0;
    }
    draw_sampled_wave = TRUE;
    outlong(712, 540, sed_display_start, 10);
    outlong(712, 558, sed_display_length, 12);
    outlong(650, 540, sed_range_start, 10);
    outlong(650, 558, sed_range_end, 11);
    Check_Loops();
    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT) Actualize_Instrument_Ed(0, 4);
    Display_Sample_Buffers(Allow | ReadOnly);
}

// ------------------------------------------------------
// Notify the user selected buffer visually
void Display_Sample_Buffers(int Allow)
{
    switch(cur_sample_buffer)
    {
        case 0:
            cur_smp_buffer[0] = BUTTON_PUSHED;
            cur_smp_buffer[1] = BUTTON_NORMAL;
            cur_smp_buffer[2] = BUTTON_NORMAL;
            cur_smp_buffer[3] = BUTTON_NORMAL;
            break;
        case 1:
            cur_smp_buffer[0] = BUTTON_NORMAL;
            cur_smp_buffer[1] = BUTTON_PUSHED;
            cur_smp_buffer[2] = BUTTON_NORMAL;
            cur_smp_buffer[3] = BUTTON_NORMAL;
            break;
        case 2:
            cur_smp_buffer[0] = BUTTON_NORMAL;
            cur_smp_buffer[1] = BUTTON_NORMAL;
            cur_smp_buffer[2] = BUTTON_PUSHED;
            cur_smp_buffer[3] = BUTTON_NORMAL;
            break;
        case 3:
            cur_smp_buffer[0] = BUTTON_NORMAL;
            cur_smp_buffer[1] = BUTTON_NORMAL;
            cur_smp_buffer[2] = BUTTON_NORMAL;
            cur_smp_buffer[3] = BUTTON_PUSHED;
            break;
    }

    Gui_Draw_Button_Box(650, 450, 16, 16, "1", Allow | cur_smp_buffer[0] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[0] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(668, 450, 16, 16, "2", Allow | cur_smp_buffer[1] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[1] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(686, 450, 16, 16, "3", Allow | cur_smp_buffer[2] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[2] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(704, 450, 16, 16, "4", Allow | cur_smp_buffer[3] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[3] ? 0 : BUTTON_LOW_FONT));
}

// ------------------------------------------------------
// Display a selection
void Zoom_In_Sel()
{
    int max_length;
    sed_range_mode = FALSE;
    if(sed_range_end != sed_range_start)
    {
        if((int) sed_range_end < (int) sed_range_start)
        {
            int swap_range = sed_range_start;
            sed_range_start = sed_range_end;
            sed_range_end = swap_range;
        }
        sed_display_length = (sed_range_end - sed_range_start);
        sed_display_start = sed_range_start;
        max_length = SampleNumSamples[Current_Sample][Current_Sample_Split] - sed_display_length;
        if((int) sed_display_start > max_length) sed_display_start = max_length;

        draw_sampled_wave = TRUE;
        teac = 3;
        gui_action = GUI_CMD_REFRESH_SAMPLE_ED;

    }
}

// ------------------------------------------------------
// Move from a selection
void Zoom_Out_Sel()
{
    int start_test;
    sed_display_start -= sed_display_length;
    start_test = sed_display_start;
    if(start_test < 0) sed_display_start = 0;
    sed_display_length *= 3;

    if(sed_display_length > (int32) SampleNumSamples[Current_Sample][Current_Sample_Split])
    {
        sed_display_length = SampleNumSamples[Current_Sample][Current_Sample_Split];
    }
    if(sed_display_length + sed_display_start > (int32) SampleNumSamples[Current_Sample][Current_Sample_Split])
    {
        sed_display_start = SampleNumSamples[Current_Sample][Current_Sample_Split] - sed_display_length;
    }
    draw_sampled_wave = TRUE;
    teac = 3;
    gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
}
