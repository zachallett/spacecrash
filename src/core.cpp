#include "stdafx.h"
#include "base.h"
#include "core.h"

//========================================================================================
// Loading textures (from BMP files)

#define MAX_TEXTURES 32
struct Texture
{
  bool used;
  float w, h; // In 0..1 terms
  int pixw, pixh; // In pixels
  GLuint tex;
} g_textures[MAX_TEXTURES] = { 0 };

int    CORE_LoadBmp   (const char filename[], bool wrap);
void   CORE_UnloadBmp (int texid);

//-----------------------------------------------------------------------------
struct CORE_BMPFileHeader
{
  byte  mark[2];
  byte  filesize[4];
  byte  reserved[4];
  byte  pixdataoffset[4];

  byte  hdrsize[4];
  byte  width[4];
  byte  height[4];
  byte  colorplanes[2];
  byte  bpp[2];
  byte  compression[4];
  byte  pixdatasize[4];
  byte  hres[4];
  byte  vres[4];
  byte  numcol[4];
  byte  numimportantcolors[4];
};

//-----------------------------------------------------------------------------
#define READ_LE_WORD(a)  (a[0] + a[1]*0x100)
#define READ_LE_DWORD(a) (a[0] + a[1]*0x100 + a[2]*0x10000 + a[3]*0x1000000)

//-----------------------------------------------------------------------------
// compute the next highest power of 2 of 32-bit v
dword hp2(dword v) { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }

//-----------------------------------------------------------------------------
static byte pixloadbuffer[2048 * 2048 * 4];
int CORE_LoadBmp(const char filename[], bool wrap)
{
  GLint              retval = -1;
  struct CORE_BMPFileHeader hdr;
  
  int fd = open(filename, O_RDONLY);
  if (fd != -1)
  {
    read(fd, &hdr, sizeof(hdr));

    if (hdr.mark[0] == 'B' && hdr.mark[1] == 'M')
    {
      // Find an empty texture entry
      for (int i = 0; i < MAX_TEXTURES; i++)
      {
        if (!g_textures[i].used)
        {
          retval = i;
          break;
        }
      }
    
      if (retval == -1)
        return retval;
  
      dword  width  = READ_LE_DWORD(hdr.width);
      sdword height = READ_LE_DWORD(hdr.height);
      dword  offset = READ_LE_DWORD(hdr.pixdataoffset);
        
      dword pixdatasize = READ_LE_DWORD(hdr.pixdatasize);
      if (!pixdatasize)
        pixdatasize = (width * abs(height) * READ_LE_WORD(hdr.bpp) / 8);

      lseek(fd, offset, SEEK_SET);
      if (height > 0)
        read(fd, pixloadbuffer, pixdatasize);
      else
      {
        // Reverse while loading
        int nrows = -height;
        for (int i = 0; i < nrows; i++)
          read(fd, pixloadbuffer + (nrows-i-1) * width * 4, (pixdatasize / nrows));
      }

      GLuint texid = 1;

      glGenTextures( 1, &texid );
      glBindTexture( GL_TEXTURE_2D, texid );
      //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); // GL_LINEAR_MIPMAP_NEAREST
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // GL_LINEAR);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

      //gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA8, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixloadbuffer );
      
      height = abs((int)height);
      dword width_pow2 = hp2(width);
      dword height_pow2 = hp2(height);
      
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width_pow2, height_pow2, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixloadbuffer);

      g_textures[retval].used = true;
      g_textures[retval].tex = texid;
      g_textures[retval].pixw = width;
      g_textures[retval].pixh = height;
      g_textures[retval].w = width/(float)width_pow2;
      g_textures[retval].h = height/(float)height_pow2;
    }
    close(fd);
  }

  return retval;
}

//-----------------------------------------------------------------------------
void CORE_UnloadBmp(int texix)
{
  glDeleteTextures( 1, &g_textures[texix].tex );
  g_textures[texix].used = false;
}

//-----------------------------------------------------------------------------
ivec2 CORE_GetBmpSize(int texix)
{
  ivec2 v;
  v.x = g_textures[texix].pixw;
  v.y = g_textures[texix].pixh;
  return v;
}

//-----------------------------------------------------------------------------
GLuint CORE_GetBmpOpenGLTex(int texix)
{
  return g_textures[texix].tex;
}

//-----------------------------------------------------------------------------
void CORE_RenderCenteredSprite(vec2 pos, vec2 size, int texix, rgba color, bool additive)
{
  glColor4f(color.r, color.g, color.b, color.a);
  if (additive) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  else          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  vec2 p0 = vsub(pos, vscale(size, .5f));
  vec2 p1 = vadd(pos, vscale(size, .5f));

  glBindTexture( GL_TEXTURE_2D, g_textures[texix].tex );
  glBegin( GL_QUADS );
  glTexCoord2d(0.0,                 0.0);                 glVertex2f(p0.x, p0.y);
  glTexCoord2d(g_textures[texix].w, 0.0);                 glVertex2f(p1.x, p0.y);
  glTexCoord2d(g_textures[texix].w, g_textures[texix].h); glVertex2f(p1.x, p1.y);
  glTexCoord2d(0.0,                 g_textures[texix].h); glVertex2f(p0.x, p1.y);
  glEnd();
}

//=============================================================================
// Sound (OpenAL, WAV files), leave last one for music!
#define SND_MAX_SOURCES 8
#define SND_MAX_LOOP_SOUNDS 2
ALuint SND_Sources[SND_MAX_SOURCES];
int    SND_NextSource = 0;

//-----------------------------------------------------------------------------
bool CORE_InitSound()
{
  ALCcontext *context;
  ALCdevice *device;
 
  device = alcOpenDevice(NULL);
  if (device)
  {
    context = alcCreateContext(device,NULL);
    alcMakeContextCurrent(context);
    alGenSources(SND_MAX_SOURCES, SND_Sources);
    alGetError();
    return true;
  }
  else
    return false;
 
}

//-----------------------------------------------------------------------------
void CORE_EndSound()
{
  ALCdevice *device;
  ALCcontext *context;

  context = alcGetCurrentContext();
  device = alcGetContextsDevice(context);

  alDeleteSources(SND_MAX_SOURCES, SND_Sources);
   
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);
}

//-----------------------------------------------------------------------------
void CORE_PlaySound(ALuint snd, float volume, float pitch)
{
  // Stop and play!
  alSourceStop(SND_Sources[SND_NextSource]);
  alSourcei(SND_Sources[SND_NextSource], AL_BUFFER, snd);
  alSourcef(SND_Sources[SND_NextSource], AL_GAIN,   volume);
  alSourcef(SND_Sources[SND_NextSource], AL_PITCH,  pitch);
  alSourcePlay(SND_Sources[SND_NextSource]);

  SND_NextSource++;
  if (SND_NextSource == SND_MAX_SOURCES-SND_MAX_LOOP_SOUNDS)
    SND_NextSource = 0;
}

//-----------------------------------------------------------------------------
void CORE_PlayLoopSound (unsigned loopchannel, ALuint snd, float volume, float pitch)
{
  if (loopchannel >= SND_MAX_LOOP_SOUNDS)
    return;
  alSourceStop(SND_Sources[SND_MAX_SOURCES-loopchannel]);
  alSourcei(SND_Sources[SND_MAX_SOURCES-loopchannel], AL_BUFFER, snd);
  alSourcef(SND_Sources[SND_MAX_SOURCES-loopchannel], AL_GAIN,   volume);
  alSourcei(SND_Sources[SND_MAX_SOURCES-loopchannel], AL_LOOPING, AL_TRUE);
  alSourcePlay(SND_Sources[SND_MAX_SOURCES-loopchannel]);
}

//-----------------------------------------------------------------------------
void CORE_SetLoopSoundParam (unsigned loopchannel, float volume, float pitch)
{
  if (loopchannel >= SND_MAX_LOOP_SOUNDS)
    return;
  alSourcef(SND_Sources[SND_MAX_SOURCES-loopchannel], AL_GAIN,   volume);
  alSourcef(SND_Sources[SND_MAX_SOURCES-loopchannel], AL_PITCH,  pitch);
}

//-----------------------------------------------------------------------------
void CORE_StopLoopSound(unsigned loopchannel)
{
  alSourceStop(SND_Sources[SND_MAX_SOURCES-loopchannel]);
}

//-----------------------------------------------------------------------------
struct CORE_RIFFHeader
{
  byte  chunkID[4];   // 'RIFF'
  byte  chunkSize[4];
  byte  format[4];    // 'WAVE'
};

struct CORE_RIFFChunkHeader
{
  byte  subChunkID[4];
  byte  subChunkSize[4];
};

struct CORE_WAVEFormatChunk
{
  byte  audioFormat[2];
  byte  numChannels[2];
  byte  sampleRate[4];
  byte  byteRate[4];
  byte  blockAlignp[2];
  byte  bitsPerSample[2];
};

//-----------------------------------------------------------------------------
#define MAX_WAV_SIZE 32*1024*1024 // Max 32Mb sound!
static byte soundloadbuffer[MAX_WAV_SIZE];
ALuint CORE_LoadWav(const char filename[])
{
  ALuint          retval = UINT_MAX;
  CORE_RIFFHeader hdr;
  
  int fd = open(filename, O_RDONLY);
  if (fd != -1)
  {
    read(fd, &hdr, sizeof(hdr));

    if (  hdr.chunkID[0] == 'R' && hdr.chunkID[1] == 'I' && hdr.chunkID[2] == 'F' && hdr.chunkID[3] == 'F'
       && hdr.format [0] == 'W' && hdr.format [1] == 'A' && hdr.format [2] == 'V' && hdr.format [3] == 'E' )
    {
      CORE_WAVEFormatChunk fmt;
      memset(&fmt, 0, sizeof(fmt));

      for (;;)
      {
        CORE_RIFFChunkHeader chunkhdr;
        if (read(fd, &chunkhdr, sizeof(chunkhdr)) < sizeof(chunkhdr))
          break;

        dword chunkdatasize = READ_LE_DWORD(chunkhdr.subChunkSize);

        if (chunkhdr.subChunkID[0] == 'f' && chunkhdr.subChunkID[1] == 'm' && chunkhdr.subChunkID[2] == 't' && chunkhdr.subChunkID[3] == ' ')
        {
          read(fd, &fmt, sizeof(fmt));
          lseek(fd, ((1 + chunkdatasize) & -2) - sizeof(fmt), SEEK_CUR); // Skip to next chunk
        }
        else if (chunkhdr.subChunkID[0] == 'd' && chunkhdr.subChunkID[1] == 'a' && chunkhdr.subChunkID[2] == 't' && chunkhdr.subChunkID[3] == 'a')
        {
          dword contentsize = chunkdatasize;
          if (contentsize > sizeof(soundloadbuffer))
            contentsize = sizeof(soundloadbuffer);

          read(fd, soundloadbuffer, contentsize);

          bool valid = true;
          ALsizei al_size = contentsize;
          ALsizei al_frequency = READ_LE_DWORD(fmt.sampleRate);
          ALenum al_format = (ALenum)-1;
          if (READ_LE_WORD(fmt.numChannels) == 1)
          {
            if      (READ_LE_WORD(fmt.bitsPerSample) == 8 ) al_format = AL_FORMAT_MONO8;
            else if (READ_LE_WORD(fmt.bitsPerSample) == 16) al_format = AL_FORMAT_MONO16;
            else                                            valid = false;
          }
          else if (READ_LE_WORD(fmt.numChannels) == 2)
          {
            if      (READ_LE_WORD(fmt.bitsPerSample) == 8 ) al_format = AL_FORMAT_STEREO8;
            else if (READ_LE_WORD(fmt.bitsPerSample) == 16) al_format = AL_FORMAT_STEREO16;
            else                                            valid = false;
          }

          if (valid)
          {
            ALuint al_buffer = UINT_MAX;
            alGenBuffers(1, &al_buffer);

            alBufferData(al_buffer, al_format, soundloadbuffer, al_size, al_frequency);
            retval = al_buffer;
          }
          break;
        }
        else
          lseek(fd, ((1 + READ_LE_DWORD(chunkhdr.subChunkSize)) & -2), SEEK_CUR); // Skip to next chunk
      }
    }
    close(fd);
  }

  return retval;
}

//-----------------------------------------------------------------------------
void CORE_UnloadWav(ALuint snd)
{
  alDeleteBuffers(1, &snd);
}
