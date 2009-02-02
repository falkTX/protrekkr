#include <windows.h>
#include <zlib.h>

#define LOAD_READ FALSE
#define LOAD_READMEM TRUE

#include "../../release/distrib/replay/lib/include/synth.h"
#include "../../release/distrib/replay/lib/include/tb_303.h"
#include "../../release/distrib/replay/lib/include/replay.h"

extern SynthParameters PARASynth[128];

extern gear303 tb303engine[2];
extern para303 tb303[2];
extern int patt_highlight;
extern char FullScreen;
extern char AutoSave;
extern int Beveled;
extern int Continuous_Scroll;
extern char Dir_Mods[MAX_PATH];
extern char Dir_Instrs[MAX_PATH];
extern char Dir_Presets[MAX_PATH];
extern char *cur_dir;

int Mod_Length;
int Mod_Simulate;
BYTE *Mod_Memory;
int Mod_Mem_Pos;
int Final_Mod_Length;
extern int wait_AutoSave;

int TRACKMIDICHANNEL[MAX_TRACKS];

char modname[22];
char artist[20];
char style[20];

char Midiprg[128];
char nameins[128][20];
char SampleName[128][16][64];

// ---------------------------------------------------------------------------
// Clear all patterns
// ---------------------------------------------------------------------------
void Clean_Up_Patterns_Pool(void) {
    for(int inicial = 0; inicial < PBLEN; inicial += 6)
    {
        *(RawPatterns + inicial) = 121;     // 121
        *(RawPatterns + inicial + 1) = 255; // 255
        *(RawPatterns + inicial + 2) = 255; // 255
        *(RawPatterns + inicial + 3) = 255; // 255
        *(RawPatterns + inicial + 4) = 0;   // 0
        *(RawPatterns + inicial + 5) = 0;   // 0
    }
}

// ---------------------------------------------------------------------------
// Prepare the tracker interface once a module has been loaded
// ---------------------------------------------------------------------------
void Init_Tracker_Context_After_ModLoad(void)
{
    cPosition = 0;

    Final_Mod_Length = 0;

    Post_Song_Init();

    lchorus_counter = 44100;
    rchorus_counter = 44100;
    lchorus_counter2 = 44100 - lchorus_delay;
    rchorus_counter2 = 44100 - rchorus_delay;
    Initreverb();

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
    Mas_Compressor_Set_Variables(mas_comp_threshold, mas_comp_ratio);
#endif

}

void Read_Mod_Datas(void *Datas, int Unit, int Length, FILE *Handle)
{
    switch(Mod_Simulate)
    {
        case LOAD_READ:
            fread(Datas, Unit, Length, Handle);
            break;

        case LOAD_READMEM:
            memcpy(Datas, Mod_Memory + Mod_Mem_Pos, Unit * Length);
            Mod_Mem_Pos += Unit * Length;
            break;
    }
}

int Get_File_Size(FILE *Handle)
{
    int File_Size;
    int Current_Pos;

    Current_Pos = ftell(Handle);
    fseek(Handle, 0, SEEK_END);
    File_Size = ftell(Handle);
    fseek(Handle, Current_Pos, SEEK_SET);
    return(File_Size);
}

// ---------------------------------------------------------------------------
// Depack a compressed module
// ---------------------------------------------------------------------------
BYTE *Depack_Data(BYTE *Memory, int Size)
{
    z_stream d_stream;

    BYTE *Test_Mem = (BYTE *) malloc(Size * 10);
    if(Test_Mem)
    {
        memset(&d_stream, 0, sizeof(d_stream));
        d_stream.next_in = (BYTE *) Memory;
        d_stream.avail_in = Size;
        d_stream.next_out = Test_Mem;
        d_stream.avail_out = -1;
        inflateInit(&d_stream);
        inflate(&d_stream, Z_FULL_FLUSH);
        inflateEnd(&d_stream);
    }
    return(Test_Mem);
}

int check_file_type(char *extension)
{
    if(strcmp(extension, "TWNNSNG9"))
    {
        return(FALSE);
    }
    return(TRUE);
}

// ---------------------------------------------------------------------------
// Load a module file
// ---------------------------------------------------------------------------
int LoadMod(const char *FileName)
{
    int Phony_Old_Value;
    int New_adsr = FALSE;
    int New_Comp = FALSE;
    int Comp_Flag;

    int result = TRUE;
    int Old_303;
    int Old_Bug = TRUE;
    int new_disto = FALSE;
    int Pack_Scheme = FALSE;
    int tps_pos;
    int tps_trk;
    int twrite;
    int fake_value;
    int Packed_Size;
    unsigned char *Packed_Module = NULL;
   
    Mod_Simulate = LOAD_READ;
    Mod_Mem_Pos = 0;
    Mod_Memory = NULL;

    Ptk_Stop();
    Ptk_InitDriver();

    FILE *in;
    in = fopen(FileName, "rb");
    Old_303 = FALSE;

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        fread(extension, sizeof(char), 9, in);

        if(!check_file_type(extension))
        {
            fclose(in);
            return(FALSE);
        }

        Mod_Simulate = LOAD_READMEM;
        New_Comp = TRUE;
        New_adsr = TRUE;
        Pack_Scheme = TRUE;
        new_disto = TRUE;
        Old_Bug = FALSE;

        Free_Samples();

        mas_comp_threshold = 100.0f;
        mas_comp_ratio = 0.0f;

        Clean_Up_Patterns_Pool();
        init_sample_bank();

        Pre_Song_Init();

        // Load the module into memory and depack it
        if(Mod_Simulate == LOAD_READMEM)
        {
            Packed_Size = Get_File_Size(in) - 9;
            Packed_Module = (unsigned char *) malloc(Packed_Size);
            if(Packed_Module)
            {
                fread(Packed_Module, sizeof(unsigned char), Packed_Size, in);
                Mod_Memory = Depack_Data(Packed_Module, Packed_Size);
                Mod_Mem_Pos = 0;
                free(Packed_Module);
            }
        }

        Read_Mod_Datas(modname, sizeof(char), 20, in);
        Read_Mod_Datas(&nPatterns, sizeof(unsigned char), 1, in);
        Songtracks = MAX_TRACKS;
        Read_Mod_Datas(&sLength, sizeof(unsigned char), 1, in);

        Read_Mod_Datas(pSequence, sizeof(unsigned char), 256, in);

        memset(RawPatterns, 0, (12288 * nPatterns));

        Read_Mod_Datas(patternLines, sizeof(short), 128, in);
        for(int pwrite = 0; pwrite < nPatterns; pwrite++) {
            Read_Mod_Datas(RawPatterns + pwrite * 12288, 12288, 1, in);
        }

        for(int swrite = 0; swrite < 128; swrite++) {
            Read_Mod_Datas(&nameins[swrite], sizeof(char), 20, in);
            Read_Mod_Datas(&Midiprg[swrite], sizeof(char), 1, in);
            Read_Mod_Datas(&Synthprg[swrite], sizeof(unsigned char), 1, in);

            PARASynth[swrite].disto = 0;

            PARASynth[swrite].lfo1_attack = 0;
            PARASynth[swrite].lfo1_decay = 0;
            PARASynth[swrite].lfo1_sustain = 128;
            PARASynth[swrite].lfo1_release = 0x10000;

            PARASynth[swrite].lfo2_attack = 0;
            PARASynth[swrite].lfo2_decay = 0;
            PARASynth[swrite].lfo2_sustain = 128;
            PARASynth[swrite].lfo2_release = 0x10000;

            // BEWARE: struct is aligned on 4 bytes !!!
            if(!new_disto)
            {
                Read_Mod_Datas(&PARASynth[swrite], sizeof(SynthParameters) - 4 - 32, 1, in);
            }
            else
            {
                if(New_adsr)
                {
                    Read_Mod_Datas(&PARASynth[swrite], sizeof(SynthParameters), 1, in);
                }
                else
                {
                    Read_Mod_Datas(&PARASynth[swrite], sizeof(SynthParameters) - 32, 1, in);
                }
            }
            // Compression type
            if(Pack_Scheme) Read_Mod_Datas(&SampleCompression[swrite], sizeof(char), 1, in);

            for(int slwrite = 0; slwrite < 16; slwrite++)
            {
                Read_Mod_Datas(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite] != 0)
                {
                    if(Old_Bug) Read_Mod_Datas(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                    else Read_Mod_Datas(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                    Read_Mod_Datas(&Basenote[swrite][slwrite], sizeof(char), 1, in);
                    Read_Mod_Datas(&LoopStart[swrite][slwrite], sizeof(long), 1, in);
                    Read_Mod_Datas(&LoopEnd[swrite][slwrite], sizeof(long), 1, in);
                    Read_Mod_Datas(&LoopType[swrite][slwrite], sizeof(char), 1, in);
                    Read_Mod_Datas(&SampleNumSamples[swrite][slwrite], sizeof(long), 1, in);
                    Read_Mod_Datas(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                    Read_Mod_Datas(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                    Read_Mod_Datas(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                    RawSamples[swrite][0][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * 2);
                    Read_Mod_Datas(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                    *RawSamples[swrite][0][slwrite] = 0;
                    // Stereo flag
                    Read_Mod_Datas(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * 2);
                        Read_Mod_Datas(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                        *RawSamples[swrite][1][slwrite] = 0;
                    }
                }// Exist Sample
            }
        }

        // Reading Track Properties
        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Datas(&TCut[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&ICut[twrite], sizeof(float), 1, in);
            if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
            if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;

            Read_Mod_Datas(&TPan[twrite], sizeof(float), 1, in);
            ComputeStereo(twrite);
            Read_Mod_Datas(&FType[twrite], sizeof(int), 1, in);
            Read_Mod_Datas(&FRez[twrite], sizeof(int), 1, in);
            Read_Mod_Datas(&DThreshold[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&DClamp[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&DSend[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&CSend[twrite], sizeof(int), 1, in);
        }

        // Reading mod properties
        Read_Mod_Datas(&compressor, sizeof(int), 1, in);
        Read_Mod_Datas(&c_threshold, sizeof(int), 1, in);
        Read_Mod_Datas(&BeatsPerMin, sizeof(int), 1, in);
        Read_Mod_Datas(&TicksPerBeat, sizeof(int),1, in);
        Read_Mod_Datas(&mas_vol, sizeof(float), 1, in);
        if(mas_vol < 0.01f) mas_vol = 0.01f;
        if(mas_vol > 1.0f) mas_vol = 1.0f;

        if(New_Comp)
        {
            Comp_Flag = 0;
            Read_Mod_Datas(&Comp_Flag, sizeof(char), 1, in);
            if(Comp_Flag)
            {
                Read_Mod_Datas(&mas_comp_threshold, sizeof(float), 1, in);
                if(mas_comp_threshold < 0.01f) mas_comp_threshold = 0.01f;
                if(mas_comp_threshold > 100.0f) mas_comp_threshold = 100.0f;
                Read_Mod_Datas(&mas_comp_ratio, sizeof(float), 1, in);
                if(mas_comp_ratio < 0.01f) mas_comp_ratio = 0.01f;
                if(mas_comp_ratio > 100.0f) mas_comp_ratio = 100.0f;
            }
        }

        Read_Mod_Datas(&delay_time, sizeof(int), 1, in);
        Read_Mod_Datas(&Feedback, sizeof(float), 1, in);
        Read_Mod_Datas(&DelayType, sizeof(int), 1, in);
        Read_Mod_Datas(&lchorus_delay, sizeof(int), 1, in);
        Read_Mod_Datas(&rchorus_delay, sizeof(int), 1, in);
        Read_Mod_Datas(&lchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Datas(&rchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Datas(&shuffle, sizeof(int), 1, in);

        // Reading track part sequence

        // Reset'em first
        for(tps_pos = 0; tps_pos < 256; tps_pos++)
        {
            for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
            {
                Read_Mod_Datas(&SACTIVE[tps_pos][tps_trk], sizeof(bool), 1, in);
            }
        }

        for(int spl = 0; spl < Songtracks; spl++)
        {
            CCoef[spl] = float((float) CSend[spl] / 127.0f);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Datas(&TRACKMIDICHANNEL[twrite], sizeof(int), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Datas(&LFO_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Datas(&LFO_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&LFO_AMPL[twrite], sizeof(float), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Datas(&FLANGER_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Datas(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&FLANGER_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
            Read_Mod_Datas(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
            FLANGER_OFFSET[twrite] = 8192;
            foff2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
            foff1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
        }

        Read_Mod_Datas(&FLANGER_DEPHASE, sizeof(float), 1, in);

        for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
        {
            Read_Mod_Datas(&TRACKSTATE[tps_trk], sizeof(int), 1, in);
        }
        Read_Mod_Datas(&Songtracks, sizeof(char), 1, in);
        
        for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
        {
            Read_Mod_Datas(&Disclap[tps_trk], sizeof(bool), 1, in);
            Read_Mod_Datas(&fake_value, sizeof(bool), 1, in);
        }

        Read_Mod_Datas(artist, sizeof(char), 20, in);
        Read_Mod_Datas(style, sizeof(char), 20, in);
        Read_Mod_Datas(&Phony_Old_Value, sizeof(char), 1, in);
        Read_Mod_Datas(beatsync, sizeof(bool), 128, in);
        Read_Mod_Datas(beatlines, sizeof(short), 128, in);
        Read_Mod_Datas(&REVERBFILTER, sizeof(float), 1, in);

        Read_Mod_Datas(CustomVol, sizeof(float), 128, in);
        Read_Mod_Datas(&Phony_Old_Value, sizeof(bool), 1, in);
        
        // Read the 303 datas
        if(Old_303)
        {
            Read_Mod_Datas(&tb303[0], sizeof(para303) - (32 * 20), 1, in);
            Read_Mod_Datas(&tb303[1], sizeof(para303) - (32 * 20), 1, in);
        }
        else
        {
            Read_Mod_Datas(&tb303[0], sizeof(para303), 1, in);
            Read_Mod_Datas(&tb303[1], sizeof(para303), 1, in);
        }  
        Read_Mod_Datas(&tb303engine[0].tbVolume, sizeof(float), 1, in);
        Read_Mod_Datas(&tb303engine[1].tbVolume, sizeof(float), 1, in);
        Read_Mod_Datas(&tb303engine[0].hpf, sizeof(bool), 1, in);
        Read_Mod_Datas(&tb303engine[1].hpf, sizeof(bool), 1, in);

        fclose(in);

        // Init the tracker context
        Init_Tracker_Context_After_ModLoad();
    }
    else
    {
        result = FALSE;
    }
    if(Mod_Memory) free(Mod_Memory);
    return(result);
}

// -------------------------------------
// Calculate the length of the song in hours:minutes:seconds
int calc_length(void)
{
    int i;
    int k;
    int pos_patt;
    int patt_cmd;
    int patt_datas;
    BYTE *Cur_Patt;
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
            if(!PosTicks) {
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

    ilen = (int) (len * 1000);
    return(ilen);
}
