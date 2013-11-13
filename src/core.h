#ifndef _CORE_H_
#define _CORE_H_

// Bitmap/texture functions
int   CORE_LoadBmp(const char filename[], bool wrap);
ivec2 CORE_GetBmpSize(int texix);
void  CORE_UnloadBmp(int texix);
void  CORE_RenderCenteredSprite(vec2 pos, vec2 size, int texix);

#endif
