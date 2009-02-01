// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef NOCODEC

// ------------------------------------------------------
// Includes
#ifdef __WIN32__
#include <windows.h>
#if defined(MINGW)
#include "../../release/distrib/replay/lib/include/mmreg.h"
#include "../../release/distrib/replay/lib/include/msacm.h"
#else
#include <mmreg.h>
#include <msacm.h>
#endif
#endif

#include "include/samples_pack.h"
#include "../../Release/Distrib/Replay/Lib/include/Samples_Unpack.h"

// ------------------------------------------------------
// Variables
WAVEFORMATEX Wave_Format;
MPEGLAYER3WAVEFORMAT MP3_Format;
GSM610WAVEFORMAT GSM_Format;
TRUESPEECHWAVEFORMAT TrueSpeech_Format;
ACMSTREAMHEADER Pack_Stream_Head;
HACMSTREAM Pack_Stream;

int ToGSM(short *Source, short *Dest, int Size)
{
    int Src_size;
    int Dest_Size;

    Wave_Format.wFormatTag = WAVE_FORMAT_PCM;
    Wave_Format.nChannels = 1;
    Wave_Format.cbSize = 0;
    Wave_Format.wBitsPerSample = 16;
    Wave_Format.nSamplesPerSec = 44100;
    Wave_Format.nBlockAlign = Wave_Format.nChannels * Wave_Format.wBitsPerSample / 8;
    Wave_Format.nAvgBytesPerSec = Wave_Format.nSamplesPerSec * Wave_Format.nBlockAlign;

    GSM_Format.wfx.wFormatTag = WAVE_FORMAT_GSM610;
    acmFormatSuggest(NULL, (LPWAVEFORMATEX) &Wave_Format, (LPWAVEFORMATEX) &GSM_Format, sizeof(GSM_Format), ACM_FORMATSUGGESTF_WFORMATTAG);
    acmStreamOpen(&Pack_Stream, NULL, (LPWAVEFORMATEX) &Wave_Format, (LPWAVEFORMATEX) &GSM_Format, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME);

    Src_size = Size;
    unsigned long rawbufsize = 0;
    acmStreamSize(Pack_Stream, Src_size, &rawbufsize, ACM_STREAMSIZEF_SOURCE);
    Uint8 *Pack_Buf = (Uint8 *) malloc(Src_size);
    memset(Pack_Buf, 0, Src_size);
    Uint8 *rawbuf = (Uint8 *) malloc(rawbufsize);
    memset(rawbuf, 0, rawbufsize);

    ACMSTREAMHEADER Pack_Stream_Head;
    ZeroMemory(&Pack_Stream_Head, sizeof(ACMSTREAMHEADER));
    Pack_Stream_Head.cbStruct = sizeof(ACMSTREAMHEADER);
    Pack_Stream_Head.pbSrc = (Uint8 *) Pack_Buf;
    Pack_Stream_Head.cbSrcLength = Src_size;
    Pack_Stream_Head.pbDst = rawbuf;
    Pack_Stream_Head.cbDstLength = rawbufsize;
    acmStreamPrepareHeader(Pack_Stream, &Pack_Stream_Head, 0);

    memcpy(Pack_Buf, Source, Src_size);

    acmStreamConvert(Pack_Stream, &Pack_Stream_Head, 0);
    Dest_Size = Pack_Stream_Head.cbDstLengthUsed;
    if(Dest_Size < Src_size)
    {
        memcpy(Dest, rawbuf, Dest_Size);
    }
    else
    {
        Dest_Size = 0;
    }

    acmStreamUnprepareHeader(Pack_Stream, &Pack_Stream_Head, 0);
    if(rawbuf) free(rawbuf);
    if(Pack_Buf) free(Pack_Buf);
    acmStreamClose(Pack_Stream, 0);

    return(Dest_Size);
}

int ToMP3(short *Source, short *Dest, int Size)
{
    int Src_size;
    int Dest_Size;

    Wave_Format.wFormatTag = WAVE_FORMAT_PCM;
    Wave_Format.nChannels = 1;
    Wave_Format.cbSize = 0;
    Wave_Format.wBitsPerSample = 16;
    Wave_Format.nSamplesPerSec = 44100;
    Wave_Format.nBlockAlign = Wave_Format.nChannels * Wave_Format.wBitsPerSample / 8;
    Wave_Format.nAvgBytesPerSec = Wave_Format.nSamplesPerSec * Wave_Format.nBlockAlign;

    MP3_Format.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
    MP3_Format.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
    MP3_Format.wfx.nChannels = 1;
    MP3_Format.wfx.nSamplesPerSec = 44100;
    MP3_Format.wfx.nAvgBytesPerSec = BITRATE * (1000 / 8);
    MP3_Format.wfx.wBitsPerSample = 0;
    MP3_Format.wfx.nBlockAlign = 1;
    MP3_Format.wID = MPEGLAYER3_ID_MPEG;
    MP3_Format.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
    MP3_Format.nBlockSize = 0;
    MP3_Format.nFramesPerBlock = 0;
    MP3_Format.nCodecDelay = 0;
    acmStreamOpen( &Pack_Stream, NULL, (LPWAVEFORMATEX) &Wave_Format, (LPWAVEFORMATEX) &MP3_Format, NULL, 0, 0, 0);

    Src_size = Size;
    unsigned long rawbufsize = 0;
    acmStreamSize(Pack_Stream, Src_size, &rawbufsize, ACM_STREAMSIZEF_SOURCE);
    rawbufsize += MP3_FRAMES_LAG * 2;
    Uint8 *Pack_Buf = (Uint8 *) malloc(Src_size + (MP3_FRAMES_LAG * 4));
    memset(Pack_Buf, 0, Src_size + (MP3_FRAMES_LAG * 4));
    Uint8 *rawbuf = (Uint8 *) malloc(rawbufsize + (MP3_FRAMES_LAG * 4));
    memset(rawbuf, 0, rawbufsize + (MP3_FRAMES_LAG * 4));

    ACMSTREAMHEADER Pack_Stream_Head;
    ZeroMemory(&Pack_Stream_Head, sizeof(ACMSTREAMHEADER));
    Pack_Stream_Head.cbStruct = sizeof(ACMSTREAMHEADER);
    Pack_Stream_Head.pbSrc = (Uint8 *) Pack_Buf;
    Pack_Stream_Head.cbSrcLength = Src_size + (MP3_FRAMES_LAG * 2);
    Pack_Stream_Head.pbDst = rawbuf;
    Pack_Stream_Head.cbDstLength = rawbufsize;
    acmStreamPrepareHeader(Pack_Stream, &Pack_Stream_Head, 0);

    memcpy(Pack_Buf, Source, Src_size);

    acmStreamConvert(Pack_Stream, &Pack_Stream_Head, 0);
    Dest_Size = Pack_Stream_Head.cbDstLengthUsed;
    if(Dest_Size < Src_size)
    {
        memcpy(Dest, rawbuf, Dest_Size);
    }
    else
    {
        Dest_Size = 0;
    }

    acmStreamUnprepareHeader(Pack_Stream, &Pack_Stream_Head, 0);
    if(rawbuf) free(rawbuf);
    if(Pack_Buf) free(Pack_Buf);
    acmStreamClose(Pack_Stream, 0);

    return(Dest_Size);
}

int ToTrueSpeech(short *Source, short *Dest, int Size)
{
    int Src_size;
    int Dest_Size;

    Wave_Format.wFormatTag = WAVE_FORMAT_PCM;
    Wave_Format.nChannels = 1;
    Wave_Format.cbSize = 0;
    Wave_Format.wBitsPerSample = 16;
    Wave_Format.nSamplesPerSec = 8000;
    Wave_Format.nBlockAlign = Wave_Format.nChannels * Wave_Format.wBitsPerSample / 8;
    Wave_Format.nAvgBytesPerSec = Wave_Format.nSamplesPerSec * Wave_Format.nBlockAlign;

    TrueSpeech_Format.wfx.wFormatTag = WAVE_FORMAT_DSPGROUP_TRUESPEECH;
    acmFormatSuggest(NULL, (LPWAVEFORMATEX) &Wave_Format, (LPWAVEFORMATEX) &TrueSpeech_Format, sizeof(TrueSpeech_Format), ACM_FORMATSUGGESTF_WFORMATTAG);
    acmStreamOpen(&Pack_Stream, NULL, (LPWAVEFORMATEX) &Wave_Format, (LPWAVEFORMATEX) &TrueSpeech_Format, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME);

    Src_size = Size;
    unsigned long rawbufsize = 0;
    acmStreamSize(Pack_Stream, Src_size, &rawbufsize, ACM_STREAMSIZEF_SOURCE);
    Uint8 *Pack_Buf = (Uint8 *) malloc(Src_size);
    memset(Pack_Buf, 0, Src_size);
    Uint8 *rawbuf = (Uint8 *) malloc(rawbufsize);
    memset(rawbuf, 0, rawbufsize);

    ACMSTREAMHEADER Pack_Stream_Head;
    ZeroMemory(&Pack_Stream_Head, sizeof(ACMSTREAMHEADER));
    Pack_Stream_Head.cbStruct = sizeof(ACMSTREAMHEADER);
    Pack_Stream_Head.pbSrc = (Uint8 *) Pack_Buf;
    Pack_Stream_Head.cbSrcLength = Src_size;
    Pack_Stream_Head.pbDst = rawbuf;
    Pack_Stream_Head.cbDstLength = rawbufsize;
    acmStreamPrepareHeader(Pack_Stream, &Pack_Stream_Head, 0);

    memcpy(Pack_Buf, Source, Src_size);

    acmStreamConvert(Pack_Stream, &Pack_Stream_Head, 0);
    Dest_Size = Pack_Stream_Head.cbDstLengthUsed;
    if(Dest_Size < Src_size)
    {
        memcpy(Dest, rawbuf, Dest_Size);
    }
    else
    {
        Dest_Size = 0;
    }

    acmStreamUnprepareHeader(Pack_Stream, &Pack_Stream_Head, 0);
    if(rawbuf) free(rawbuf);
    if(Pack_Buf) free(Pack_Buf);
    acmStreamClose(Pack_Stream, 0);

    return(Dest_Size);
}

#endif
