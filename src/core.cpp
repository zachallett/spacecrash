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
void CORE_RenderCenteredSprite(vec2 pos, vec2 size, int texix)
{
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
