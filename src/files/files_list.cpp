// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/files_list.h"
#include "../include/ptk.h"

#if defined(__WIN32__)
#include <shlwapi.h>
#else
#include <ftw.h>
#endif

// ------------------------------------------------------
// Variables
int lt_ykar = 200;
int lt_items = 0;
int lt_index = 0;
int lt_curr = 0;
int list_counter;

char SMPT_LIST[2048][64];

unsigned FILETYPE[2048];
extern int display_title;

char Dir_Act[MAX_PATH];
char Dir_Mods[MAX_PATH];
char Dir_Instrs[MAX_PATH];
char Dir_Presets[MAX_PATH];
char *cur_dir;

void Clear_Files_List(void)
{
    for(unsigned int listcleaner = 0; listcleaner < 2048; listcleaner++)
    {
        for(int listcleaner2 = 0; listcleaner2 < 64; listcleaner2++)
        {
            SMPT_LIST[listcleaner][listcleaner2] = 0;
        }
    }
}

char *Get_Current_FileName(void)
{
   return(SMPT_LIST[lt_curr]);
}

unsigned int Get_Current_FileType(void)
{
   return(FILETYPE[lt_curr]);
}

void Set_Current_Dir(void)
{
    switch(Scopish)
    {
        case SCOPE_ZONE_MOD_DIR:
            CHDIR(Get_Current_FileName());
            GETCWD(Dir_Mods, MAX_PATH);
            break;
        case SCOPE_ZONE_INSTR_DIR:
            CHDIR(Get_Current_FileName());
            GETCWD(Dir_Instrs, MAX_PATH);
            break;
        case SCOPE_ZONE_PRESET_DIR:
            CHDIR(Get_Current_FileName());
            GETCWD(Dir_Presets, MAX_PATH);
            break;
    }
}

#if defined(__LINUX__) || defined(__MACOSX__)
int list_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int i;
    int len_name;

    if(ftwbuf->level == 1)
    {
        if(strcmp(fpath, Dir_Act))
        {
            switch(typeflag)
            {
                case FTW_D:
                    len_name = strlen(fpath);
                    while(len_name--)
                    {
                        if(fpath[len_name] == '/')
                        {
                            len_name++;
                            break;
                        }
                    }
                    sprintf(SMPT_LIST[list_counter], &fpath[len_name]);
                    FILETYPE[list_counter] = _A_SUBDIR;
                    lt_items++;
                    list_counter++;
                    break;

                case FTW_F:
                    len_name = strlen(fpath);
                    while(len_name--)
                    {
                        if(fpath[len_name] == '/')
                        {
                            len_name++;
                            break;
                        }
                    }
                    sprintf(SMPT_LIST[list_counter], &fpath[len_name]);
                    FILETYPE[list_counter] = 0;
                    lt_items++;
                    list_counter++;
                    break;
            }
        }
    }
    return 0;
}
#endif

void Read_SMPT(void)
{
    lt_ykar = 0;
    lt_items = 0;  
    list_counter = 0;
    lt_curr = 0;
    lt_index = 0;

#if defined(__WIN32__)
    struct _finddata_t c_file;
#endif

    long hFile;

    switch(Scopish)
    {
        case SCOPE_ZONE_MOD_DIR:
            cur_dir = Dir_Mods;
            break;
        case SCOPE_ZONE_INSTR_DIR:
            cur_dir = Dir_Instrs;
            break;
        case SCOPE_ZONE_PRESET_DIR:
            cur_dir = Dir_Presets;
            break;
    }
    CHDIR(cur_dir);

    // Find first .c file in current directory
    strcpy(Dir_Act, cur_dir);

#if defined(__WIN32__)
    strcat(Dir_Act, "\\*.*");
    if((hFile = _findfirst(Dir_Act, &c_file)) == -1L)
    {
        sprintf(SMPT_LIST[0], "No files in current dir.");
        FILETYPE[0] = 0;
        lt_items = 1;
    }
    else
    {
        // The first file
        sprintf(SMPT_LIST[list_counter], c_file.name);
        FILETYPE[list_counter] = c_file.attrib & _A_SUBDIR;
        lt_items++;
        list_counter++;

        // Find the rest of the files (directories)

        while(_findnext(hFile, &c_file) == 0)
        {
            if(c_file.attrib & _A_SUBDIR)
            {
                sprintf(SMPT_LIST[list_counter], c_file.name);
                FILETYPE[list_counter] = _A_SUBDIR;
                lt_items++;
                list_counter++;
            }
        }
        // End dir
        _findclose(hFile);
    }

    if((hFile = _findfirst(Dir_Act, &c_file)) == -1L)
    {
        sprintf(SMPT_LIST[0], "No files in current dir.");
        FILETYPE[0] = 0;
        lt_items = 1;
    }
    else
    {
        // The first file (files)
        if(!(c_file.attrib & _A_SUBDIR))
        {
            sprintf(SMPT_LIST[list_counter], c_file.name);
            FILETYPE[list_counter] = 0;
            lt_items++;
            list_counter++;
        }
        // Find the rest of the files (files)
        while(_findnext(hFile, &c_file) == 0)
        {
            if(!(c_file.attrib & _A_SUBDIR))
            {
                sprintf(SMPT_LIST[list_counter], c_file.name);
                FILETYPE[list_counter] = 0;
                lt_items++;
                list_counter++;
            }
        } // while      
        _findclose(hFile);
    }
#else

    sprintf(SMPT_LIST[list_counter], "./");
    FILETYPE[list_counter] = _A_SUBDIR;
    lt_items++;
    list_counter++;

    sprintf(SMPT_LIST[list_counter], "../");
    FILETYPE[list_counter] = _A_SUBDIR;
    lt_items++;
    list_counter++;

    nftw(Dir_Act, &list_file, FTW_PHYS, 0);

#endif

}

void DumpList(int xr, int yr, int y)
{
    if(Scopish != SCOPE_ZONE_SCOPE)
    {
        SetColor(COL_BACKGROUND);
        bjbox(xr - 1, yr + 1, 227 + restx, 135);

        // Current dir background
        Gui_Draw_Button_Box(454, 24, 112 + restx, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);

        switch(Scopish)
        {
            case SCOPE_ZONE_MOD_DIR:
                PrintXY(458, 26, USE_FONT, Dir_Mods);
                break;
            case SCOPE_ZONE_INSTR_DIR:
                PrintXY(458, 26, USE_FONT, Dir_Instrs);
                break;
            case SCOPE_ZONE_PRESET_DIR:
                PrintXY(458, 26, USE_FONT, Dir_Presets);
                break;
        }

        for(int counter = 0; counter < 11; counter++)
        {
            int rel_val = y + counter;

            if(y + counter < lt_items)
            {
                // Highlight bar in files requester.
                if(y + counter == lt_curr)
                {
                    SetColor(COL_PUSHED_MED);
                    bjbox(xr - 1, yr + (counter * 12) + 2, 227 + restx, 12);
                }

                if(FILETYPE[rel_val] == _A_SUBDIR)
                {
                    PrintXY(xr, yr + (counter * 12), USE_FONT_LOW, SMPT_LIST[rel_val]);
                }
                else
                {
                    PrintXY(xr, yr + (counter * 12), USE_FONT, SMPT_LIST[rel_val]);
                }
            }

        }
    }
}

// ------------------------------------------------------
// Redraw the files list
void ltActualize(int modeac)
{
    int const brolim = lt_items - 11;

    switch(display_title)
    {
        case 0:
            break;

        case 1:
            display_title = 2;
            break;

        case 2:
            display_title = 3;
            Refresh_Palette();
            // No break on purpose to display the files list slider

        case 3:

            if(Scopish != SCOPE_ZONE_SCOPE)
            {

                if(modeac == 0)
                {
                    if(lt_ykar > 70) lt_ykar = 70;
                    if(lt_ykar < 0) lt_ykar = 0;
                    lt_index = (lt_ykar * brolim) / 70;
                }

                if(lt_index > brolim) lt_index = brolim;
                if(lt_index < 0) lt_index = 0;
                if(modeac != 0)
                {
                    if(brolim)
                    {
                        lt_ykar = (lt_index * 70) / brolim;
                    }
                    else
                    {
                        lt_ykar = (lt_index * 70);
                    }
                }

                SetColor(COL_SLIDER_LO);
                bjbox(395 - 1, 59 - 1, 15 + 2, 101 + 2);
                SetColor(COL_SLIDER_HI);
                bjbox(395, 59, 15 + 1, 101 + 1);
                SetColor(COL_SLIDER_MED);
                bjbox(395, 59, 15, 101);
                Gui_Draw_Button_Box(394 + 1, 58 + lt_ykar + 1, 16 - 2, 32 - 2, "", BUTTON_NORMAL);
                if(last_index != lt_index)
                {
                    DumpList(413, 41, lt_index);
                    last_index = lt_index;
                }
            }
            break;
    }
}
