/*==========================================================================

     riff.cpp  -  Don Cross, April 1993.

     Contains code for reading/writing RIFF files, including .WAV files!

     See also:
         ..\include\riff.h
         ..\include\ddc.h

     Revision history:

1993 April 12 [Don Cross]
     Started writing this code.

1994 October 6 [Don Cross]
     Added WriteData, ReadData.
     Added const modifiers to some function parameters.

1997 September 14 [Don Cross]
     Fixed a bug in WaveFile::Close.  It was calling Backpatch
     and getting confused when the the file had been opened for read.
     (Backpatch returns an error in that case, which prevented 
     WaveFile::Close from calling RiffFile::Close.)

2008 April 19 [Franck Charlet]
     Added a function to seek a chunk
     Added handling of 32 bits samples
     Corrected the samples calculation
     Handling for loop informations

==========================================================================*/

// Standard includes
#if defined(__WIN32__)
#include <io.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DDCLIB includes
#include "include/riff.h"


unsigned long FourCC ( const char *ChunkName )
{
   long retbuf = 0x20202020;   // four spaces (padding)
   char *p = ((char *) &retbuf);

   // Remember, this is Intel format!
   // The first character goes in the LSB

   for ( int i=0; i<4 && ChunkName[i]; i++ )
   {
      *p++ = ChunkName[i];
   }

   return retbuf;
}


//----------------------------------------------------------------------


RiffFile::RiffFile()
{
   file = 0;
   fmode = RFM_UNKNOWN;

   riff_header.ckID = FourCC("RIFF");
   riff_header.ckSize = 0;
}


RiffFile::~RiffFile()
{
   if ( fmode != RFM_UNKNOWN )
   {
      Close();
   }
}


DDCRET RiffFile::Open ( const char *Filename, RiffFileMode NewMode )
{
   DDCRET retcode = DDC_SUCCESS;

   if ( fmode != RFM_UNKNOWN )
   {
      retcode = Close();
   }

   if ( retcode == DDC_SUCCESS )
   {
      switch ( NewMode )
      {
         case RFM_WRITE:
              file = fopen ( Filename, "wb" );
              if ( file )
              {
                 // Write the RIFF header...
                 // We will have to come back later and patch it!

                 if ( fwrite ( &riff_header, sizeof(riff_header), 1, file ) != 1 )
                 {
                    fclose(file);
                    //unlink(Filename);
                    fmode = RFM_UNKNOWN;
                    file = 0;
                 }
                 else
                 {
                    fmode = RFM_WRITE;
                 }
              }
              else
              {
                 fmode = RFM_UNKNOWN;
                 retcode = DDC_FILE_ERROR;
              }
              break;

         case RFM_READ:
              file = fopen ( Filename, "rb" );
              if ( file )
              {
                 // Try to read the RIFF header...

                 if ( fread ( &riff_header, sizeof(riff_header), 1, file ) != 1 )
                 {
                    fclose(file);
                    fmode = RFM_UNKNOWN;
                    file = 0;
                 }
                 else
                 {
                    fmode = RFM_READ;
                 }
              }
              else
              {
                 fmode = RFM_UNKNOWN;
                 retcode = DDC_FILE_ERROR;
              }
              break;

         default:
              retcode = DDC_INVALID_CALL;
      }
   }

   return retcode;
}


DDCRET RiffFile::Write ( const void *Data, unsigned NumBytes )
{
   if ( fmode != RFM_WRITE )
   {
      return DDC_INVALID_CALL;
   }

   if ( fwrite ( Data, NumBytes, 1, file ) != 1 )
   {
      return DDC_FILE_ERROR;
   }

   riff_header.ckSize += NumBytes;

   return DDC_SUCCESS;
}


DDCRET RiffFile::Close()
{
   DDCRET retcode = DDC_SUCCESS;

   switch ( fmode )
   {
      case RFM_WRITE:
           if ( fflush(file) ||
                fseek(file,0,SEEK_SET) ||
                fwrite ( &riff_header, sizeof(riff_header), 1, file ) != 1 ||
                fclose(file) )
           {
              retcode = DDC_FILE_ERROR;
           }
           break;

      case RFM_READ:
           fclose(file);
           break;
   }

   file = 0;
   fmode = RFM_UNKNOWN;

   return retcode;
}


long RiffFile::CurrentFilePosition() const
{
   return ftell ( file );
}


DDCRET RiffFile::Seek ( long offset )
{
   fflush ( file );

   DDCRET rc;

   if ( fseek ( file, offset, SEEK_SET ) )
   {
      rc = DDC_FILE_ERROR;
   }
   else
   {
      rc = DDC_SUCCESS;
   }

   return rc;
}


DDCRET RiffFile::Backpatch ( long FileOffset,
                             const void *Data,
                             unsigned NumBytes )
{
   if ( !file )
   {
      return DDC_INVALID_CALL;
   }

   if ( fflush(file) ||
        fseek ( file, FileOffset, SEEK_SET ) )
   {
      return DDC_FILE_ERROR;
   }

   return Write ( Data, NumBytes );
}


DDCRET RiffFile::Expect ( const void *Data, unsigned NumBytes )
{
   char *p = (char *)Data;

   while ( NumBytes-- )
   {
      if ( fgetc(file) != *p++ )
      {
         return DDC_FILE_ERROR;
      }
   }

   return DDC_SUCCESS;
}


DDCRET RiffFile::Read ( void *Data, unsigned NumBytes )
{
   DDCRET retcode = DDC_SUCCESS;

   if ( fread(Data,NumBytes,1,file) != 1 )
   {
      retcode = DDC_FILE_ERROR;
   }

   return retcode;
}

int RiffFile::SeekChunk ( const char *ChunkName) {
   int Chunk;
   int Chunk_To_Find = FourCC(ChunkName);
   int i;
   int data_size;
   
   i = 0;
   while(!feof(file)) {
      Chunk = 0;
      Seek(i);
      Read(&Chunk, 4);
      if(Chunk == Chunk_To_Find) {
         if(Chunk_To_Find == 'atad') {
            Seek(i + 4);
            return(0);
         }
         Seek(i);
         return(1);
      }
      // Skip the datas
      if(Chunk == 'atad') {
         Read(&data_size, 4);
         i += data_size + 4 + 4 - 1;
      }
      i++;
   }
   return(0);
}


//-----------------------------------------------------------------------

WaveFile::WaveFile()
{
   pcm_data.ckID = FourCC("data");
   pcm_data.ckSize = 0;
   num_samples = 0;
}

DDCRET WaveFile::OpenForRead ( const char *Filename )
{
   UINT32 data_length;

   // Verify filename parameter as best we can...
   if ( !Filename )
   {
      return DDC_INVALID_CALL;
   }

   DDCRET retcode = Open ( Filename, RFM_READ );

   if ( retcode == DDC_SUCCESS )
   {
      retcode = Expect ( "WAVE", 4 );

      if ( retcode == DDC_SUCCESS )
      {
         if(!SeekChunk("fmt ")) {
            retcode = DDC_FILE_ERROR;
            return retcode;
         }
         retcode = Read ( &wave_format, sizeof(wave_format) );

         if ( retcode == DDC_SUCCESS &&
              !wave_format.VerifyValidity() )
         {
            // This isn't standard PCM, so we don't know what it is!

            retcode = DDC_FILE_ERROR;
            return(retcode);
         }

         if ( retcode == DDC_SUCCESS )
         {
            pcm_data_offset = CurrentFilePosition();

            // Figure out number of samples from
            // file size, current file position, and
            // WAVE header.

            retcode = Read ( &pcm_data, sizeof(pcm_data) );
         }

         if(SeekChunk("smpl")) {
            retcode = Read ( &wave_Smpl, sizeof(wave_Smpl) );
         }
         Seek(0);
         SeekChunk("data");
         retcode = Read ( &data_length, sizeof(data_length) );
         
         num_samples = data_length;
         num_samples /= NumChannels();
         num_samples /= (BitsPerSample() / 8);
      }
   }

   return retcode;
}


DDCRET WaveFile::OpenForWrite ( const char  *Filename,
                                UINT32       SamplingRate,
                                UINT16       BitsPerSample,
                                UINT16       NumChannels )
{
   // Verify parameters...

   if ( !Filename ||
        (BitsPerSample != 8 && BitsPerSample != 16) ||
        NumChannels < 1 || NumChannels > 2 )
   {
      return DDC_INVALID_CALL;
   }

   wave_format.data.Config ( SamplingRate, BitsPerSample, NumChannels );

   DDCRET retcode = Open ( Filename, RFM_WRITE );

   if ( retcode == DDC_SUCCESS )
   {
      retcode = Write ( "WAVE", 4 );

      if ( retcode == DDC_SUCCESS )
      {
         retcode = Write ( &wave_format, sizeof(wave_format) );

         if ( retcode == DDC_SUCCESS )
         {
            pcm_data_offset = CurrentFilePosition();
            retcode = Write ( &pcm_data, sizeof(pcm_data) );
         }
      }
   }

   return retcode;
}


DDCRET WaveFile::Close()
{
   DDCRET rc = DDC_SUCCESS;
   
   if ( fmode == RFM_WRITE )
      rc = Backpatch ( pcm_data_offset, &pcm_data, sizeof(pcm_data) );

   if ( rc == DDC_SUCCESS )
      rc = RiffFile::Close();

   return rc;
}


DDCRET WaveFile::WriteSample ( const INT16 Sample [MAX_WAVE_CHANNELS] )
{
   DDCRET retcode = DDC_SUCCESS;

   switch ( wave_format.data.nChannels )
   {
      case 1:
           switch ( wave_format.data.nBitsPerSample )
           {
              case 8:
                   pcm_data.ckSize += 1;
                   retcode = Write ( &Sample[0], 1 );
                   break;

              case 16:
                   pcm_data.ckSize += 2;
                   retcode = Write ( &Sample[0], 2 );
                   break;

              default:
                   retcode = DDC_INVALID_CALL;
           }
           break;

      case 2:
           switch ( wave_format.data.nBitsPerSample )
           {
              case 8:
                   retcode = Write ( &Sample[0], 1 );
                   if ( retcode == DDC_SUCCESS )
                   {
                      retcode = Write ( &Sample[1], 1 );
                      if ( retcode == DDC_SUCCESS )
                      {
                         pcm_data.ckSize += 2;
                      }
                   }
                   break;

              case 16:
                   retcode = Write ( &Sample[0], 2 );
                   if ( retcode == DDC_SUCCESS )
                   {
                      retcode = Write ( &Sample[1], 2 );
                      if ( retcode == DDC_SUCCESS )
                      {
                         pcm_data.ckSize += 4;
                      }
                   }
                   break;

              default:
                   retcode = DDC_INVALID_CALL;
           }
           break;

      default:
           retcode = DDC_INVALID_CALL;
   }

   return retcode;
}


DDCRET WaveFile::WriteMonoSample ( INT16 SampleData )
{
   switch ( wave_format.data.nBitsPerSample )
   {
      case 8:
           pcm_data.ckSize += 1;
           return Write ( &SampleData, 1 );

      case 16:
           pcm_data.ckSize += 2;
           return Write ( &SampleData, 2 );
   }

   return DDC_INVALID_CALL;
}


DDCRET WaveFile::WriteStereoSample ( INT16 LeftSample,
                                     INT16 RightSample )
{
   DDCRET retcode = DDC_SUCCESS;

   switch ( wave_format.data.nBitsPerSample )
   {
      case 8:
           retcode = Write ( &LeftSample, 1 );
           if ( retcode == DDC_SUCCESS )
           {
              retcode = Write ( &RightSample, 1 );
              if ( retcode == DDC_SUCCESS )
              {
                 pcm_data.ckSize += 2;
              }
           }
           break;

      case 16:
           retcode = Write ( &LeftSample, 2 );
           if ( retcode == DDC_SUCCESS )
           {
              retcode = Write ( &RightSample, 2 );
              if ( retcode == DDC_SUCCESS )
              {
                 pcm_data.ckSize += 4;
              }
           }
           break;

      default:
           retcode = DDC_INVALID_CALL;
   }

   return retcode;
}

DDCRET WaveFile::ReadMonoSample ( INT16 *Sample )
{
   DDCRET retcode = DDC_SUCCESS;

   switch ( wave_format.data.nBitsPerSample )
   {
      case 8:
           unsigned char x;
           retcode = Read ( &x, 1 );
           *Sample = (INT16(x) << 8) - 32768;
           break;

      case 16:
           retcode = Read ( Sample, 2 );
           break;

      case 32:
           float y;
           retcode = Read ( &y, 4 );
           *Sample = (short) (y * 32767.0f);
           break;

      default:
           retcode = DDC_INVALID_CALL;
   }

   return retcode;
}


DDCRET WaveFile::ReadStereoSample ( INT16 *L, INT16 *R )
{
   DDCRET retcode = DDC_SUCCESS;
   UINT8          x[2];
   INT16          y[2];
   float          z[2];

   switch ( wave_format.data.nBitsPerSample )
   {
      case 8:
           retcode = Read ( x, 2 );
           *L = (INT16 ( x[0] ) << 8) - 32768;
           *R = (INT16 ( x[1] ) << 8) - 32768;
           break;

      case 16:
           retcode = Read ( y, 4 );
           *L = INT16 ( y[0] );
           *R = INT16 ( y[1] );
           break;

      case 32:
           retcode = Read ( z, 8 );
           *L = (short) (z[0] * 32767.0f);
           *R = (short) (z[1] * 32767.0f);
           break;

      default:
           retcode = DDC_INVALID_CALL;
   }

   return retcode;
}


DDCRET WaveFile::SeekToSample ( unsigned long SampleIndex )
{
   if ( SampleIndex >= NumSamples() )
   {
      return DDC_INVALID_CALL;
   }

   unsigned SampleSize = (BitsPerSample() + 7) / 8;

   DDCRET rc = Seek ( pcm_data_offset + sizeof(pcm_data) +
                      SampleSize * NumChannels() * SampleIndex );

   return rc;
}


UINT32 WaveFile::SamplingRate() const
{
   return wave_format.data.nSamplesPerSec;
}


UINT16 WaveFile::BitsPerSample() const
{
   return wave_format.data.nBitsPerSample;
}


UINT16 WaveFile::NumChannels() const
{
   return wave_format.data.nChannels;
}


UINT32 WaveFile::NumSamples() const
{
   return num_samples;
}

// Simple or none
UINT32 WaveFile::LoopType() const
{
   return wave_Smpl.data.Num_Sample_Loops != 0 ? 1 : 0;
}

UINT32 WaveFile::LoopStart() const
{
   return wave_Smpl.data.Start;
}

UINT32 WaveFile::LoopEnd() const
{
   return wave_Smpl.data.End;
}


DDCRET WaveFile::WriteData ( const void *data, UINT32 numData )
{
    UINT32 extraBytes = numData;
    return RiffFile::Write ( data, extraBytes );
}

DDCRET WaveFile::WriteData ( const INT32 *data, UINT32 numData )
{
    UINT32 extraBytes = numData * sizeof(INT32);
    pcm_data.ckSize += extraBytes;
    return RiffFile::Write ( data, extraBytes );
}


DDCRET WaveFile::WriteData ( const INT16 *data, UINT32 numData )
{
    UINT32 extraBytes = numData * sizeof(INT16);
    pcm_data.ckSize += extraBytes;
    return RiffFile::Write ( data, extraBytes );
}


DDCRET WaveFile::WriteData ( const UINT8 *data, UINT32 numData )
{
    pcm_data.ckSize += numData;
    return RiffFile::Write ( data, numData );
}


DDCRET WaveFile::ReadData ( INT16 *data, UINT32 numData )
{
    return RiffFile::Read ( data, numData * sizeof(INT16) );
}


DDCRET WaveFile::ReadData ( UINT8 *data, UINT32 numData )
{
    return RiffFile::Read ( data, numData );
}

/*--- end of file riff.cpp ---*/
