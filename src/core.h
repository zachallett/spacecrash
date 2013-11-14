// core.h
#ifndef _CORE_H_
#define _CORE_H_

//-----------------------------------------------------------------------------
inline float    CORE_FRand(float from, float to) { return from + (to-from)*(rand()/(float)RAND_MAX); }
inline unsigned CORE_URand(unsigned from, unsigned to) { return from + rand()%((unsigned)(to-from+1)); }
inline bool     CORE_RandChance(float chance) { return CORE_FRand(0.f, 1.f) < chance; }
inline float    CORE_FSquare(float f) { return f * f; }

//-----------------------------------------------------------------------------
struct rgba { float r, g, b, a; };
inline rgba vmake(float r, float g, float b, float a) { rgba c; c.r = r; c.g = g; c.b = b; c.a = a; return c; }

#define COLOR_WHITE vmake(1.f,1.f,1.f,1.f)

//-----------------------------------------------------------------------------
// Bitmap/texture functions
int   CORE_LoadBmp(const char filename[], bool wrap);
ivec2 CORE_GetBmpSize(int texix);
void  CORE_UnloadBmp(int texix);
void  CORE_RenderCenteredSprite(vec2 pos, vec2 size, int texix, rgba color = COLOR_WHITE, bool additive = false);

#endif