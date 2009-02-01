// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/editor_diskio.h"

// ------------------------------------------------------
// Variables
extern int Cpu_Trigger;
extern char CpuStr[80];
int allow_save = TRUE;
int song_Seconds;
int song_Minutes;
int song_Hours;

int Tracks_To_Render[MAX_TRACKS];
char *Tracks_Labels[MAX_TRACKS] =
{
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F"
};

typedef struct
{
    int x;
    int y;
} TRACK_POS, *LPTRACK_POS;

TRACK_POS Tracks_Position[MAX_TRACKS] =
{
    { 350 + (0 * 18), 488 },
    { 350 + (1 * 18), 488 },
    { 350 + (2 * 18), 488 },
    { 350 + (3 * 18), 488 },
    { 350 + (4 * 18), 488 },
    { 350 + (5 * 18), 488 },
    { 350 + (6 * 18), 488 },
    { 350 + (7 * 18), 488 },

    { 350 + (0 * 18), 506 },
    { 350 + (1 * 18), 506 },
    { 350 + (2 * 18), 506 },
    { 350 + (3 * 18), 506 },
    { 350 + (4 * 18), 506 },
    { 350 + (5 * 18), 506 },
    { 350 + (6 * 18), 506 },
    { 350 + (7 * 18), 506 },
};

// ------------------------------------------------------
// Functions
void Display_Song_Length(void);
void Calc_Length(void);
void Display_Tracks_To_Render(void);
void Check_Tracks_To_Render(void);
int Is_Track_To_Render_Solo(int nbr);
void Check_Tracks_To_Render_To_Solo(void);

void Draw_DiskIO_Ed(void)
{
    Draw_Editors_Bar(USER_SCREEN_DISKIO_EDIT);

    Gui_Draw_Button_Box(0, 447, fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Disk Operations / Module Credits");

    Gui_Draw_Button_Box(254, 488, 80, 16, "Calc Final", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    outlong(254, 506, Final_Mod_Length, 7);

    Gui_Draw_Button_Box(254, 524, 80, 16, "Calc Length", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Display_Song_Length();

    Gui_Draw_Button_Box(8, 506, 80, 16, "Title", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, 524, 80, 16, "Produced By", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, 542, 80, 16, "Style", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, 470, 80, 16, "New Module", BUTTON_NORMAL);
    Gui_Draw_Button_Box(90, 488, 80, 16, "WAV Render", BUTTON_NORMAL);
    Gui_Draw_Button_Box(90, 470, 80, 16, "Show Info", BUTTON_NORMAL);

    PrintXY(350, 472, USE_FONT, "Tracks To Render As WAV:");
    Display_Tracks_To_Render();

    Cpu_Trigger = TRUE;
}

void Actualize_DiskIO_Ed(void)
{
    if(userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        char tname[32];

        if(allow_save)
        {
            Gui_Draw_Button_Box(8, 488, 80, 16, "Save Module", BUTTON_NORMAL);
            Gui_Draw_Button_Box(254, 470, 80, 16, "Save Final", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }
        else
        {
            Gui_Draw_Button_Box(8, 488, 80, 16, "Save Module", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(254, 470, 80, 16, "Save Final", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
        }

        if(snamesel == 1)
        {
            sprintf(tname, "%s_", name);
            Gui_Draw_Button_Box(90, 506, 162, 16, tname, BUTTON_PUSHED);
        }
        else
        {
            sprintf(tname, "%s", name);
            Gui_Draw_Button_Box(90, 506, 162, 16, tname, BUTTON_NORMAL);
        }

        if(snamesel == 4)
        {
            sprintf(tname, "%s_", artist);
            Gui_Draw_Button_Box(90, 524, 162, 16, tname, BUTTON_PUSHED);
        }
        else
        {
            sprintf(tname, "%s", artist);
            Gui_Draw_Button_Box(90, 524, 162, 16, tname, BUTTON_NORMAL);
        }

        if(snamesel == 5)
        {
            sprintf(tname, "%s_", style);
            Gui_Draw_Button_Box(90, 542, 162, 16, tname, BUTTON_PUSHED);
        }
        else
        {
            sprintf(tname, "%s", style);
            Gui_Draw_Button_Box(90, 542, 162, 16, tname, BUTTON_NORMAL);
        }
    }
}

void Mouse_Right_DiskIO_Ed(void)
{
    if(userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        Check_Tracks_To_Render_To_Solo();
    }
}

void Mouse_Left_DiskIO_Ed(void)
{
    if(userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        // Save song
        if(zcheckMouse(8, 488, 80, 16))
        {
            gui_action = GUI_CMD_SAVE_MODULE;
        }
        // Save final
        if(zcheckMouse(254, 470, 80, 16))
        {
            gui_action = GUI_CMD_SAVE_FINAL;
        }
        // Calc final
        if(zcheckMouse(254, 488, 80, 16))
        {
            gui_action = GUI_CMD_CALC_FINAL;
        }
        // Calc length
        if(zcheckMouse(254, 524, 80, 16))
        {
            Calc_Length();
        }

        if(zcheckMouse(90, 470, 80, 16))
        {
            gui_action = GUI_CMD_MODULE_INFOS;
        }

        if(zcheckMouse(90, 506, 162, 16) && snamesel == 0)
        {
            strcpy(cur_input_name, name);
            sprintf(name, "");
            namesize = 0;
            snamesel = 1;
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }
        if(zcheckMouse(90, 524, 162, 16) && snamesel == 0)
        {
            strcpy(cur_input_name, artist);
            sprintf(artist, "");
            namesize = 0;
            snamesel = 4;
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }
        if(zcheckMouse(90, 542, 162, 16) && snamesel == 0)
        {
            strcpy(cur_input_name, style);
            sprintf(style, "");
            namesize = 0;
            snamesel = 5;
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        if(zcheckMouse(8, 470, 80, 16))
        {
            gui_action = GUI_CMD_NEW_MODULE;
        }
        if(zcheckMouse(90, 488, 80, 16))
        {
            rawrender = true;
            gui_action = GUI_CMD_RENDER_WAV;
        }

        Check_Tracks_To_Render();
    }
}

// -------------------------------------
// Calculate the length of the song in hours:minutes:seconds
void Calc_Length(void)
{
    int i;
    int k;
    int pos_patt;
    int patt_cmd;
    int patt_datas;
    Uint8 *Cur_Patt;
    float Ticks = (float) TicksPerBeat;
    float BPM = (float) BeatsPerMin;
    int rep_pos = 0;
    int rep_counter = -1;
    int have_break = 255;
    int PosTicks;
    int shuffle_switch;
    int shuffle_stp = shuffle;
    double len;
    int nbr_ticks;
    int Samples;
    int ilen;

    shuffle_switch = -1;
    Samples = (int) ((60 * 44100) / (BeatsPerMin * TicksPerBeat));
    if(shuffle_switch == 1) shuffle_stp = -((Samples * shuffle) / 200);
    else shuffle_stp = (Samples * shuffle) / 200;

    PosTicks = 0;
    nbr_ticks = 0;
    len = 0;
    for(i = 0; i < sLength; i++)
    {
        if(have_break < 128) pos_patt = have_break;
        else pos_patt = 0;
        have_break = 255;
        while(pos_patt < patternLines[pSequence[i]])
        {
            Cur_Patt = RawPatterns + (pSequence[i] * 12288) + (pos_patt * 6 * MAX_TRACKS);
            if(!PosTicks)
            {
                for(k = 0; k < Songtracks; k++)
                {
                    // Check if there's a pattern loop command
                    // or a change in the tempo/ticks
                    patt_cmd = Cur_Patt[4];
                    patt_datas = Cur_Patt[5];
                    switch(patt_cmd)
                    {
                        case 0x6:
                            if(!patt_datas)
                            {
                                rep_counter = -1;
                                rep_pos = pos_patt;
                            }
                            else
                            {
                                if(rep_counter == -1)
                                {
                                    rep_counter = (int) patt_datas;
                                    pos_patt = rep_pos;
                                }
                                else
                                {
                                    // count
                                    rep_counter--;
                                    if(rep_counter)
                                    {
                                        pos_patt = rep_pos;
                                    }
                                    else
                                    {
                                        rep_counter = -1;
                                        rep_pos = 0;
                                    }
                                }
                            }
                            break;

                        case 0xd:
                            if(patt_datas < 128) have_break = patt_datas;
                            break;
                        
                        case 0xf:
                            Ticks = (float) patt_datas;
                            break;
    
                        case 0xf0:
                            BPM = (float) patt_datas;
                            break;
                    }
                    Cur_Patt += 6;
                }
            }
            Samples = (int) ((60 * 44100) / (BPM * Ticks));

            PosTicks++;
            if(PosTicks > Samples + shuffle_stp)
            {

                shuffle_switch = -shuffle_switch;

                if(shuffle_switch == 1)
                {
                    shuffle_stp = -((Samples * shuffle) / 200);
                }
                else
                {
                    shuffle_stp = (Samples * shuffle) / 200;
                }
                len += PosTicks - 1;

                nbr_ticks++;
                PosTicks = 0;
                if(have_break > 127)
                {
                    pos_patt++;
                }
                else
                {
                    // End the pattern here
                    pos_patt = patternLines[pSequence[i]];
                    rep_counter = -1;
                    rep_pos = 0;
                }
            }
        }
    }
    len /= 44100;

    ilen = (int) len;
    song_Seconds = (int) ilen;
    song_Seconds %= 60;
    song_Minutes = (ilen / 60);
    song_Hours = ilen / 60 / 24;

    Display_Song_Length();
}

void Reset_Song_Length(void)
{
    song_Hours = 0;
    song_Minutes = 0;
    song_Seconds = 0;
}

void Display_Song_Length(void)
{
    char ms[64];

    if(userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        sprintf(ms, "%.2d:%.2d:%.2d", song_Hours, song_Minutes, song_Seconds);
        Gui_Draw_Button_Box(254, 542, 80, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
        PrintXY(274, 544, USE_FONT, ms);
    }
}

// Check if a track to render has been soloed already
int Is_Track_To_Render_Solo(int nbr)
{
    int i;
    int found_tracks = FALSE;

    for(i = 0; i < Songtracks; i++)
    {
        if(i != nbr)
        {
            if(!Tracks_To_Render[i]) found_tracks = TRUE;
        }
    }
    // Yes: unmute all
    if(found_tracks == FALSE) return(TRUE);
    // No: solo it
    return(FALSE);
}

void Display_1_Track_To_Render(int nbr)
{
    if(Tracks_To_Render[nbr])
    {
        if(nbr < Songtracks)
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }
        else
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
        }
    }
    else
    {
        if(nbr < Songtracks)
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
        }
        else
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
        }
    }
}

void Display_Tracks_To_Render(void)
{
    int i;

    if(userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        for(i = 0; i < MAX_TRACKS; i++)
        {
            Display_1_Track_To_Render(i);
        }
    }
}

void Reset_Tracks_To_Render(void)
{
    int i;
    for(i = 0; i < Songtracks; i++)
    {
        Tracks_To_Render[i] = FALSE;
    }
    Display_Tracks_To_Render();
}

void Check_Tracks_To_Render_To_Solo(void)
{
    int i;
    int j;

    for(i = 0; i < Songtracks; i++)
    {
        if(zcheckMouse(Tracks_Position[i].x, Tracks_Position[i].y, 17, 16))
        {
            if(Is_Track_To_Render_Solo(i))
            {
                for(j = 0; j < Songtracks; j++)
                {
                    Tracks_To_Render[j] = FALSE;
                }
            }
            else
            {
                for(j = 0; j < Songtracks; j++)
                {
                    Tracks_To_Render[j] = TRUE;
                }
                Tracks_To_Render[i] = FALSE;
            }
        }
    }
    Display_Tracks_To_Render();
}

void Check_Tracks_To_Render(void)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(i < Songtracks)
        {
            if(zcheckMouse(Tracks_Position[i].x, Tracks_Position[i].y, 17, 16))
            {
                Tracks_To_Render[i] ^= TRUE;
                Display_1_Track_To_Render(i);
            }
        }
    }
}
