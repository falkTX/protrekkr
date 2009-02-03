// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#if !defined(__NO_CODEC__)

// ------------------------------------------------------
// Includes
#if defined(__WIN32__)
#include <windows.h>
#if defined(__GCC__)
#include "include/mmreg.h"
#include "include/msacm.h"
#else
#include <mmreg.h>
#include <msacm.h>
#endif
#endif

#include "include/samples_unpack.h"

// ------------------------------------------------------
// Variables
#if defined(__STAND_ALONE__)
WAVEFORMATEX Wave_Format;
#if defined(PTK_GSM)
GSM610WAVEFORMAT GSM_Format;
#endif
#if defined(PTK_TRUESPEECH)
TRUESPEECHWAVEFORMAT TrueSpeech_Format;
#endif
#if defined(PTK_MP3)
MPEGLAYER3WAVEFORMAT MP3_Format;
#endif
#else
extern WAVEFORMATEX Wave_Format;
extern GSM610WAVEFORMAT GSM_Format;
extern TRUESPEECHWAVEFORMAT TrueSpeech_Format;
extern MPEGLAYER3WAVEFORMAT MP3_Format;
#endif
ACMSTREAMHEADER Unpack_Stream_Head;
HACMSTREAM Unpack_Stream;

// ------------------------------------------------------
// Unpack a GSM sample
#if defined(PTK_GSM)
void UnpackGSM(Uint8 *Source, short *Dest, int Src_Size, int Dst_Size)
{
    int i;
    int Real_Size;

    memset(&Wave_Format, 0, sizeof(WAVEFORMATEX));
    Wave_Format.wFormatTag = WAVE_FORMAT_PCM;

    GSM_Format.wfx.wFormatTag = WAVE_FORMAT_GSM610;
    GSM_Format.wfx.nChannels = 1;
    GSM_Format.wfx.nSamplesPerSec = 0xac44;
    GSM_Format.wfx.nAvgBytesPerSec = 0x22fd;
    GSM_Format.wfx.nBlockAlign = 0x41;
    GSM_Format.wfx.wBitsPerSample = 0;
    GSM_Format.wfx.cbSize = 2;
    GSM_Format.wSamplesPerBlock = 0x140;
    acmFormatSuggest(NULL, (LPWAVEFORMATEX) &GSM_Format, (LPWAVEFORMATEX) &Wave_Format, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);
    acmStreamOpen(&Unpack_Stream, NULL, (LPWAVEFORMATEX) &GSM_Format, (LPWAVEFORMATEX) &Wave_Format, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME);

    acmStreamSize(Unpack_Stream, Src_Size, (DWORD *) &Real_Size, ACM_STREAMSIZEF_SOURCE);
    short *dwDest = (short *) malloc((Real_Size * 2));
    memset(dwDest, 0, (Real_Size * 2));
    Unpack_Stream_Head.cbStruct = sizeof(ACMSTREAMHEADER);
    Unpack_Stream_Head.pbSrc = Source;
    Unpack_Stream_Head.cbSrcLength = Src_Size;
    Unpack_Stream_Head.pbDst = (Uint8 *) dwDest;
    Unpack_Stream_Head.cbDstLength = Real_Size;
    acmStreamPrepareHeader(Unpack_Stream, &Unpack_Stream_Head, 0);
    acmStreamConvert(Unpack_Stream, &Unpack_Stream_Head, 0);

    for(i = 0; i < Dst_Size; i++)
    {
        Dest[i] = dwDest[i];
    }

    if(dwDest) free(dwDest);
    acmStreamUnprepareHeader(Unpack_Stream, &Unpack_Stream_Head, 0);
    acmStreamClose(Unpack_Stream, 0);
}
#endif

// ------------------------------------------------------
// Unpack a TrueSpeech sample
#if defined(PTK_TRUESPEECH)
void UnpackTrueSpeech(Uint8 *Source, short *Dest, int Src_Size, int Dst_Size)
{
    int i;
    int Real_Size;

    memset(&Wave_Format, 0, sizeof(WAVEFORMATEX));
    Wave_Format.wFormatTag = WAVE_FORMAT_PCM;

    TrueSpeech_Format.wfx.wFormatTag = WAVE_FORMAT_DSPGROUP_TRUESPEECH;
    TrueSpeech_Format.wfx.nChannels = 1;
    TrueSpeech_Format.wfx.nSamplesPerSec = 0x1f40;
    TrueSpeech_Format.wfx.nAvgBytesPerSec = 0x42b;
    TrueSpeech_Format.wfx.nBlockAlign = 0x20;
    TrueSpeech_Format.wfx.wBitsPerSample = 1;
    TrueSpeech_Format.wfx.cbSize = 0x20;
    TrueSpeech_Format.wRevision = 1;
    TrueSpeech_Format.nSamplesPerBlock = 0xf0;

    acmFormatSuggest(NULL, (LPWAVEFORMATEX) &TrueSpeech_Format, (LPWAVEFORMATEX) &Wave_Format, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);
    acmStreamOpen(&Unpack_Stream, NULL, (LPWAVEFORMATEX) &TrueSpeech_Format, (LPWAVEFORMATEX) &Wave_Format, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME);

    acmStreamSize(Unpack_Stream, Src_Size, (DWORD *) &Real_Size, ACM_STREAMSIZEF_SOURCE);
    short *dwDest = (short *) malloc((Real_Size * 2));
    memset(dwDest, 0, (Real_Size * 2));
    Unpack_Stream_Head.cbStruct = sizeof(ACMSTREAMHEADER);
    Unpack_Stream_Head.pbSrc = Source;
    Unpack_Stream_Head.cbSrcLength = Src_Size;
    Unpack_Stream_Head.pbDst = (Uint8 *) dwDest;
    Unpack_Stream_Head.cbDstLength = Real_Size;
    acmStreamPrepareHeader(Unpack_Stream, &Unpack_Stream_Head, 0);
    acmStreamConvert(Unpack_Stream, &Unpack_Stream_Head, 0);

    for(i = 0; i < Dst_Size; i++)
    {
        Dest[i] = dwDest[i];
    }

    if(dwDest) free(dwDest);
    acmStreamUnprepareHeader(Unpack_Stream, &Unpack_Stream_Head, 0);
    acmStreamClose(Unpack_Stream, 0);
}
#endif

// ------------------------------------------------------
// Unpack a MP3 sample
#if defined(PTK_MP3)
void UnpackMP3(Uint8 *Source, short *Dest, int Src_Size, int Dst_Size)
{
    int i;
    int Real_Size;

    memset(&Wave_Format, 0, sizeof(WAVEFORMATEX));
    Wave_Format.wFormatTag = WAVE_FORMAT_PCM;
    MP3_Format.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
    MP3_Format.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
    MP3_Format.wfx.nChannels = 1;
    MP3_Format.wfx.nSamplesPerSec = 44100;
    MP3_Format.wfx.nAvgBytesPerSec = BITRATE * (1000 / 8);
    MP3_Format.wfx.nBlockAlign = 1;
    MP3_Format.wID = MPEGLAYER3_ID_MPEG;
    MP3_Format.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
    MP3_Format.nBlockSize = 0x68;
    acmFormatSuggest(NULL, (LPWAVEFORMATEX) &MP3_Format, (LPWAVEFORMATEX) &Wave_Format, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);
    acmStreamOpen( &Unpack_Stream, NULL, (LPWAVEFORMATEX) &MP3_Format, (LPWAVEFORMATEX) &Wave_Format, NULL, 0, 0, 0 );

    acmStreamSize(Unpack_Stream, Src_Size, (DWORD *) &Real_Size, ACM_STREAMSIZEF_SOURCE);
    short *dwDest = (short *) malloc((Real_Size * 2) + (MP3_FRAMES_DELAG * 4));
    memset(dwDest, 0, (Real_Size * 2) + (MP3_FRAMES_DELAG * 4));
    Unpack_Stream_Head.cbStruct = sizeof(ACMSTREAMHEADER);
    Unpack_Stream_Head.pbSrc = Source;
    Unpack_Stream_Head.cbSrcLength = Src_Size;
    Unpack_Stream_Head.pbDst = (Uint8 *) dwDest;
    Unpack_Stream_Head.cbDstLength = Real_Size;
    acmStreamPrepareHeader(Unpack_Stream, &Unpack_Stream_Head, 0);
    acmStreamConvert(Unpack_Stream, &Unpack_Stream_Head, 0);

    for(i = 0; i < Dst_Size; i++)
    {
        Dest[i] = dwDest[i + MP3_FRAMES_DELAG];
    }

    if(dwDest) free(dwDest);
    acmStreamUnprepareHeader(Unpack_Stream, &Unpack_Stream_Head, 0);
    acmStreamClose(Unpack_Stream, 0);
}
#endif

#endif
