#include <windows.h>
#include <zlib.h>
#include <stdio.h>

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

// ------------------------------------------------------
// Read data from a file
int Read_Data(void *value, int size, int amount, FILE *handle)
{
    return(fread(value, size, amount, handle));
}

int Read_Mod_Data(void *Datas, int Unit, int Length, FILE *Handle)
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
    return(0);
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
    if(strcmp(extension, "TWNNSNGA"))
    {
        return(FALSE);
    }
    return(TRUE);
}

// ------------------------------------------------------
// Load the data of a synth instrument
// (The new version (v4) use correct data aligment)
void Read_Synth_Params(int (*Read_Function)(void *, int ,int, FILE *),
                       FILE *in,
                       int idx,
                       int read_disto,
                       int read_lfo_adsr,
                       int new_version)
{
    if(!new_version)
    {
        if(read_disto && read_lfo_adsr)
        {
            Read_Function(&PARASynth[idx], sizeof(SynthParameters), 1, in);
        }
        else
        {
            if(read_disto)
            {
                Read_Function(&PARASynth[idx], sizeof(SynthParameters) - 32, 1, in);
            }
            else
            {
                Read_Function(&PARASynth[idx], sizeof(SynthParameters) - 4 - 32, 1, in);
            }
        }

        PARASynth[idx].osc1_pw = PARASynth[idx].osc1_pw;
        PARASynth[idx].osc2_pw = PARASynth[idx].osc2_pw;

        PARASynth[idx].env1_attack = PARASynth[idx].env1_attack;
        PARASynth[idx].env1_decay = PARASynth[idx].env1_decay;
        PARASynth[idx].env1_release = PARASynth[idx].env1_release;
    
        PARASynth[idx].env2_attack = PARASynth[idx].env2_attack;
        PARASynth[idx].env2_decay = PARASynth[idx].env2_decay;
        PARASynth[idx].env2_release = PARASynth[idx].env2_release;
    
        PARASynth[idx].lfo1_period = PARASynth[idx].lfo1_period;
        PARASynth[idx].lfo2_period = PARASynth[idx].lfo2_period;

        if(read_lfo_adsr)
        {
            PARASynth[idx].lfo1_attack = PARASynth[idx].lfo1_attack;
            PARASynth[idx].lfo1_decay = PARASynth[idx].lfo1_decay;
            PARASynth[idx].lfo1_release = PARASynth[idx].lfo1_release;

            PARASynth[idx].lfo2_attack = PARASynth[idx].lfo2_attack;
            PARASynth[idx].lfo2_decay = PARASynth[idx].lfo2_decay;
            PARASynth[idx].lfo2_release = PARASynth[idx].lfo2_release;
        }
    }
    else
    {
        Read_Function(&PARASynth[idx].presetname, sizeof(char), 20, in);
        
        Read_Function(&PARASynth[idx].osc1_waveform, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].osc2_waveform, sizeof(char), 1, in);
        
        Read_Function(&PARASynth[idx].osc1_pw, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].osc2_pw, sizeof(int), 1, in);
        
        Read_Function(&PARASynth[idx].osc2_detune, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].osc2_finetune, sizeof(char), 1, in);
        
        Read_Function(&PARASynth[idx].vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].vcf_resonance, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].vcf_type, sizeof(char), 1, in);
    
        Read_Function(&PARASynth[idx].env1_attack, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].env1_decay, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].env1_sustain, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_release, sizeof(int), 1, in);
    
        Read_Function(&PARASynth[idx].env2_attack, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].env2_decay, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].env2_sustain, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_release, sizeof(int), 1, in);
    
        Read_Function(&PARASynth[idx].lfo1_period, sizeof(int), 1, in);
        Read_Function(&PARASynth[idx].lfo2_period, sizeof(int), 1, in);

        Read_Function(&PARASynth[idx].lfo1_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo1_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].lfo2_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].lfo2_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].env1_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env1_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].env2_osc1_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc2_pw, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc1_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc2_pitch, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc1_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_osc2_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_vcf_cutoff, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].env2_vcf_resonance, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].osc3_volume, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].osc3_switch, sizeof(char), 1, in);

        Read_Function(&PARASynth[idx].ptc_glide, sizeof(char), 1, in);
        Read_Function(&PARASynth[idx].glb_volume, sizeof(char), 1, in);

        if(read_disto)
        {
            Read_Function(&PARASynth[idx].disto, sizeof(char), 1, in);
        }

        if(read_lfo_adsr)
        {
            Read_Function(&PARASynth[idx].lfo1_attack, sizeof(int), 1, in);
            Read_Function(&PARASynth[idx].lfo1_decay, sizeof(int), 1, in);
            Read_Function(&PARASynth[idx].lfo1_sustain, sizeof(char), 1, in);
            Read_Function(&PARASynth[idx].lfo1_release, sizeof(int), 1, in);

            Read_Function(&PARASynth[idx].lfo2_attack, sizeof(int), 1, in);
            Read_Function(&PARASynth[idx].lfo2_decay, sizeof(int), 1, in);
            Read_Function(&PARASynth[idx].lfo2_sustain, sizeof(char), 1, in);
            Read_Function(&PARASynth[idx].lfo2_release, sizeof(int), 1, in);
        }
    }
}

// ------------------------------------------------------
// Load a 303 pattern
void Load_303_Data(int (*Read_Function)(void *, int ,int, FILE *),
                   FILE *in, int unit, int pattern)
{
    int i;

    Read_Function(&tb303[unit].patternlength[pattern], sizeof(char), 1, in);
    Read_Function(&tb303[unit].tone[pattern], sizeof(char), 16, in);
    for(i = 0; i < 16; i++)
    {
        Read_Function(&tb303[unit].flag[pattern][i], sizeof(struct flag303), 1, in);
    }
    Read_Function(&tb303[unit].pattern_name[pattern], sizeof(char), 20, in);
}


// ------------------------------------------------------
// Load a module file
// TODO: split this one in 2
int LoadMod(char *FileName)
{
    int Ye_Old_Phony_Value;
    int New_adsr = FALSE;
    int New_Comp = FALSE;
    int Portable = FALSE;
    char Comp_Flag;

    int result = TRUE;

    int i;
    int j;
    int k;
    int Old_Ntk;
    //unsigned char *TmpPatterns;
  //  unsigned char *TmpPatterns_Tracks;
//    unsigned char *TmpPatterns_Notes;
    //unsigned char *TmpPatterns_Rows;
    int Old_Bug = TRUE;
    int new_disto = FALSE;
    int Pack_Scheme = FALSE;
    int tps_pos;
    int tps_trk;
//    int nbr_instr;
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
    Old_Ntk = FALSE;

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


        Portable = TRUE;
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
                Read_Data(Packed_Module, sizeof(char), Packed_Size, in);
                Mod_Memory = Depack_Data(Packed_Module, Packed_Size);
                Mod_Mem_Pos = 0;
                free(Packed_Module);
            }
        }

        Read_Mod_Data(FileName, sizeof(char), 20, in);
        Read_Mod_Data(&nPatterns, sizeof(char), 1, in);

        Songtracks = MAX_TRACKS;
        Read_Mod_Data(&sLength, sizeof(char), 1, in);

        Read_Mod_Data(pSequence, sizeof(char), 256, in);

        memset(RawPatterns, 0, (12288 * nPatterns));

        // Load the patterns rows infos
        for(i = 0; i < 128; i++)
        {
            Read_Mod_Data(&patternLines[i], sizeof(short), 1, in);
        }

        // Load the patterns data
        for(int pwrite = 0; pwrite < nPatterns; pwrite++)
        {
            Read_Mod_Data(RawPatterns + pwrite * 12288, sizeof(char), 12288, in);
        }

        for(int swrite = 0; swrite < 128; swrite++)
        {
            Read_Mod_Data(&nameins[swrite], sizeof(char), 20, in);
            Read_Mod_Data(&Midiprg[swrite], sizeof(char), 1, in);
            Read_Mod_Data(&Synthprg[swrite], sizeof(char), 1, in);

            PARASynth[swrite].disto = 0;

            PARASynth[swrite].lfo1_attack = 0;
            PARASynth[swrite].lfo1_decay = 0;
            PARASynth[swrite].lfo1_sustain = 128;
            PARASynth[swrite].lfo1_release = 0x10000;

            PARASynth[swrite].lfo2_attack = 0;
            PARASynth[swrite].lfo2_decay = 0;
            PARASynth[swrite].lfo2_sustain = 128;
            PARASynth[swrite].lfo2_release = 0x10000;

            Read_Synth_Params(Read_Mod_Data, in, swrite, new_disto, New_adsr, Portable);

            // Fix some very old Ntk bugs
            if(PARASynth[swrite].lfo1_period > 128) PARASynth[swrite].lfo1_period = 128;
            if(PARASynth[swrite].lfo2_period > 128) PARASynth[swrite].lfo2_period = 128;
            if(Old_Ntk)
            {
                if(PARASynth[swrite].ptc_glide < 1) PARASynth[swrite].ptc_glide = 64;
                if(PARASynth[swrite].glb_volume < 1) PARASynth[swrite].glb_volume = 64;
            }

            // Compression type
            if(Pack_Scheme) Read_Mod_Data(&SampleCompression[swrite], sizeof(char), 1, in);

            for(int slwrite = 0; slwrite < 16; slwrite++)
            {
                Read_Mod_Data(&SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite] != 0)
                {
                    if(Old_Bug) Read_Mod_Data(&SampleName[swrite][slwrite], sizeof(char), 256, in);
                    else Read_Mod_Data(&SampleName[swrite][slwrite], sizeof(char), 64, in);
                    Read_Mod_Data(&Basenote[swrite][slwrite], sizeof(char), 1, in);

                    Read_Mod_Data(&LoopStart[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data(&LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data(&LoopType[swrite][slwrite], sizeof(char), 1, in);

                    Read_Mod_Data(&SampleNumSamples[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data(&Finetune[swrite][slwrite], sizeof(char), 1, in);
                    Read_Mod_Data(&SampleVol[swrite][slwrite], sizeof(float), 1, in);
                    Read_Mod_Data(&FDecay[swrite][slwrite], sizeof(float), 1, in);

                    RawSamples[swrite][0][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short));
                    Read_Mod_Data(RawSamples[swrite][0][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                    *RawSamples[swrite][0][slwrite] = 0;

                    // Stereo flag
                    Read_Mod_Data(&SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        RawSamples[swrite][1][slwrite] = (short *) malloc(SampleNumSamples[swrite][slwrite] * sizeof(short));
                        Read_Mod_Data(RawSamples[swrite][1][slwrite], sizeof(short), SampleNumSamples[swrite][slwrite], in);
                        *RawSamples[swrite][1][slwrite] = 0;
                    }
                } // Exist Sample
            }
        }

        // Reading Track Properties
        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(&TCut[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&ICut[twrite], sizeof(float), 1, in);
            if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
            if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;

            Read_Mod_Data(&TPan[twrite], sizeof(float), 1, in);
            ComputeStereo(twrite);
            Read_Mod_Data(&FType[twrite], sizeof(int), 1, in);
            Read_Mod_Data(&FRez[twrite], sizeof(int), 1, in);
            Read_Mod_Data(&DThreshold[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&DClamp[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&DSend[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&CSend[twrite], sizeof(int), 1, in);
        }

        // Reading mod properties
        Read_Mod_Data(&compressor, sizeof(int), 1, in);
        Read_Mod_Data(&c_threshold, sizeof(int), 1, in);
        Read_Mod_Data(&BeatsPerMin, sizeof(int), 1, in);
        Read_Mod_Data(&TicksPerBeat, sizeof(int), 1, in);
        Read_Mod_Data(&mas_vol, sizeof(float), 1, in);
        if(mas_vol < 0.01f) mas_vol = 0.01f;
        if(mas_vol > 1.0f) mas_vol = 1.0f;

        if(New_Comp)
        {
            Comp_Flag = 0;
            Read_Mod_Data(&Comp_Flag, sizeof(char), 1, in);
            if(Comp_Flag)
            {
                Read_Mod_Data(&mas_comp_threshold, sizeof(float), 1, in);
                if(mas_comp_threshold < 0.01f) mas_comp_threshold = 0.01f;
                if(mas_comp_threshold > 100.0f) mas_comp_threshold = 100.0f;
            
                Read_Mod_Data(&mas_comp_ratio, sizeof(float), 1, in);
                if(mas_comp_ratio < 0.01f) mas_comp_ratio = 0.01f;
                if(mas_comp_ratio > 100.0f) mas_comp_ratio = 100.0f;
            }
        }

        Read_Mod_Data(&delay_time, sizeof(int), 1, in);
        Read_Mod_Data(&Feedback, sizeof(float), 1, in);
        Read_Mod_Data(&DelayType, sizeof(int), 1, in);
        Read_Mod_Data(&lchorus_delay, sizeof(int), 1, in);
        Read_Mod_Data(&rchorus_delay, sizeof(int), 1, in);
        Read_Mod_Data(&lchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Data(&rchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Data(&shuffle, sizeof(int), 1, in);

        // Reading track part sequence

        for(tps_pos = 0; tps_pos < 256; tps_pos++)
        {
            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Read_Mod_Data(&SACTIVE[tps_pos][tps_trk], sizeof(char), 1, in);
            }
        }

        for(int spl = 0; spl < Songtracks; spl++)
        {
            CCoef[spl] = float((float) CSend[spl] / 127.0f);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(&TRACKMIDICHANNEL[twrite], sizeof(int), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(&LFO_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Data(&LFO_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&LFO_AMPL[twrite], sizeof(float), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(&FLANGER_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Data(&FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&FLANGER_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&FLANGER_AMPL[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
            Read_Mod_Data(&FLANGER_DELAY[twrite], sizeof(int), 1, in);
            FLANGER_OFFSET[twrite] = 8192;
            foff2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
            foff1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
        }

        Read_Mod_Data(&FLANGER_DEPHASE, sizeof(float), 1, in);

        for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
        {
            Read_Mod_Data(&TRACKSTATE[tps_trk], sizeof(int), 1, in);
        }
        Read_Mod_Data(&Songtracks, sizeof(char), 1, in);
        for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
        {
            Read_Mod_Data(&Disclap[tps_trk], sizeof(char), 1, in);
            if(!Portable) Read_Mod_Data(&fake_value, sizeof(char), 1, in);
        }

        Read_Mod_Data(artist, sizeof(char), 20, in);
        Read_Mod_Data(style, sizeof(char), 20, in);

        Read_Mod_Data(beatsync, sizeof(char), 128, in);
        
        for(i = 0; i < 128; i++)
        {
            Read_Mod_Data(&beatlines[i], sizeof(short), 1, in);
        }

        Read_Mod_Data(&REVERBFILTER, sizeof(float), 1, in);

        for(i = 0; i < 128; i++)
        {
            Read_Mod_Data(&CustomVol[i], sizeof(float), 1, in);
        }

        // Read the 303 datas
        for(j = 0; j < 2; j++)
        {
            Read_Mod_Data(&tb303[j].enabled, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].selectedpattern, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].tune, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].cutoff, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].resonance, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].envmod, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].decay, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].accent, sizeof(char), 1, in);
            Read_Mod_Data(&tb303[j].waveform, sizeof(char), 1, in);
            if(Portable)
            {
                for(i = 0; i < 32; i++)
                {
                    Load_303_Data(Read_Mod_Data, in, j, i);
                }
            }
            else
            {
                Read_Mod_Data(&tb303[j].patternlength, sizeof(char), 32, in);
                Read_Mod_Data(&tb303[j].tone, sizeof(char), 32 * 16, in);
                Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);
                Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);
                Read_Mod_Data(&Ye_Old_Phony_Value, sizeof(char), 1, in);
                for(k = 0; k < 32; k++)
                {
                    for(i = 0; i < 16; i++)
                    {
                        Read_Mod_Data(&tb303[j].flag[k][i], sizeof(struct flag303), 1, in);
                    }                            
                }
                Read_Mod_Data(&tb303[j].pattern_name, sizeof(char), 32 * 20, in);
            }
        }
        Read_Mod_Data(&tb303engine[0].tbVolume, sizeof(float), 1, in);
        Read_Mod_Data(&tb303engine[1].tbVolume, sizeof(float), 1, in);
        Read_Mod_Data(&tb303engine[0].hpf, sizeof(char), 1, in);
        Read_Mod_Data(&tb303engine[1].hpf, sizeof(char), 1, in);

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
