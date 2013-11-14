#ifndef _BASE_H_
#define _BASE_H_

//-----------------------------------------------------------------------------
// Basic types
typedef unsigned       char byte;
typedef unsigned short int  word;
typedef unsigned       int  dword;
typedef                int  sdword;

typedef unsigned       int  uint;

//-----------------------------------------------------------------------------
// Useful macros
#define SIZE_ARRAY(__a__) (sizeof(__a__)/sizeof(__a__[0]))
#define LOG(ALL_ARGS) printf ALL_ARGS
//#define LOG(ALL_ARGS)
#define SAFESUB(ARG_BASE, ARG_CHUNK)          (ARG_BASE - ARG_CHUNK > 0        ? ARG_BASE - ARG_CHUNK : 0)
#define SAFEADD(ARG_BASE, ARG_CHUNK, ARG_MAX) (ARG_BASE + ARG_CHUNK <= ARG_MAX ? ARG_BASE + ARG_CHUNK : ARG_MAX)
#define MAX(ARG_A, ARG_B) ((ARG_A)>(ARG_B)?(ARG_A):(ARG_B))
#define MIN(ARG_A, ARG_B) ((ARG_A)<(ARG_B)?(ARG_A):(ARG_B))

//-----------------------------------------------------------------------------
struct ivec2 { int x, y; };

//-----------------------------------------------------------------------------
struct vec2 { float x, y; };
inline vec2  vmake (float x, float y) { vec2 r; r.x = x; r.y = y; return r; }
inline vec2  vadd  (vec2 v1, vec2 v2) { return vmake(v1.x + v2.x, v1.y + v2.y); }
inline vec2  vsub  (vec2 v1, vec2 v2) { return vmake(v1.x - v2.x, v1.y - v2.y); }
inline vec2  vscale(vec2 v, float f)  { return vmake(v.x * f, v.y * f); }
inline float vlen2 (vec2 v)           { return v.x * v.x + v.y * v.y; }
inline float vlen  (vec2 v)           { return (float)sqrt(vlen2(v)); }
inline float vdot  (vec2 v1, vec2 v2) { return v1.x * v2.x + v1.y * v2.y; }
inline vec2  vunit (float angle)      { return vmake((float)cos(angle), (float)sin(angle)); }
inline vec2  vunit (vec2 v)           { return vscale(v, 1.f/vlen(v)); }

#endif
