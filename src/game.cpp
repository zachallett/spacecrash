#include "stdafx.h"
#include "base.h"
#include "sys.h"
#include "core.h"

// Game constants
#define SPRITE_SCALE 8.f
#define SHADOW_OFFSET 80.f
#define SHADOW_SCALE 0.7f

#define MAINSHIP_ENTITY 0
#define MAINSHIP_RADIUS 100.f
#define ROCK_RADIUS 100.f
#define JUICE_RADIUS 100.f
#define ROCKET_RADIUS 80.f
#define SHIP_W 250.f
#define SHIP_H 270.f
#define CRASH_VEL 20.f
#define ROCK_CRASH_ENERGY_LOSS 30.f
#define MINE_CRASH_ENERGY_LOSS 80.f
#define MAX_ENERGY 100.f
#define MAX_FUEL 100.f
#define ROCKET_SPEED 15.f
#define MIN_TIME_BETWEEN_ROCKETS 1.f
#define MIN_FUEL_FOR_HEAL MAX_FUEL/2.f
#define FUEL_HEAL_PER_FRAME .2f
#define ENERGY_HEAL_PER_FRAME .1f
#define JUICE_FUEL 30.f

#define MAIN_SHIP g_entities[MAINSHIP_ENTITY]

#define ENERGY_BAR_W 60.f
#define ENERGY_BAR_H 1500.f
#define FUEL_BAR_W 60.f
#define FUEL_BAR_H 1500.f
#define CHUNK_W 40.f
#define CHUNK_H 40.f
#define MAX_CHUNKS 30

#define START_ROCK_CHANCE_PER_PIXEL 1.f/1000.f
#define EXTRA_ROCK_CHANCE_PER_PIXEL 0.f//1.f/2500000.f
#define JUICE_CHANCE_PER_PIXEL 1.f/10000.f

#define SHIP_MAX_SPEED 25.f
#define SHIP_MIN_SPEED 15.f
#define SHIP_CRUISE_SPEED 25.f
#define SHIP_START_SPEED 1.f
#define SHIP_INC_SPEED .1f
#define SHIP_TILT_INC .2f;
#define SHIP_TILT_FRICTION .1f
#define SHIP_MAX_TILT 1.5f
#define SHIP_HVEL_FRICTION .1f
#define TILT_FUEL_COST .03f
#define FRAME_FUEL_COST .01f

#define GEN_IN_ADVANCE 400.f

#define FIRST_CHALLENGE 3000.f

#define STARTING_TIME 1.f
#define DYING_TIME 2.f
#define VICTORY_TIME 8.f

#define FPS 60.f
#define FRAMETIME (1.f/FPS)

#define SND_DEFAULT_VOL .7f
#define SHIP_ENGINE_SOUND_CHANNEL 1

#define MENU_BKG_SCROLL_SPEED 10.f
#define MAIN_MENU_TILESET T_TILES_R_ON_L

// Global options
bool  g_opt_music = true;
bool  g_opt_sound_fx = true;

// Trigger-only keypresses
bool   g_just_pressed_up    = false;
bool   g_just_pressed_down  = false;
bool   g_just_pressed_enter = false;
bool   g_just_pressed_esc   = false;
bool   g_just_pressed_space = false;
bool   g_was_pressed_up    = false;
bool   g_was_pressed_down  = false;
bool   g_was_pressed_enter = false;
bool   g_was_pressed_esc   = false;
bool   g_was_pressed_space = false;

//=============================================================================
// Textures management
enum TexId
{
  T_FONT,
  T_PARTICLE,
  T_SHIP_LL,
  T_SHIP_L,
  T_SHIP_C,
  T_SHIP_R,
  T_SHIP_RR,
  T_BKG0,
  T_ROCK0,
  T_ROCK1,
  T_ROCK2,
  T_ROCK3,
  T_ROCK4,
  T_PEARL,
  T_ENERGY,
  T_FUEL,
  T_STAR,
  T_JUICE,
  T_ROCKET,
  T_MINE,
  T_DRONE0,
  T_DRONE1,
  T_DRONE2,
  
  // Tiles
  T_TILES_G_ON_S,
  T_SSSS   = T_TILES_G_ON_S, T_SSSG, T_SSGS, T_SSGG,
  T_SGSS,                    T_SGSG, T_SGGS, T_SGGG,
  T_GSSS,                    T_GSSG, T_GSGS, T_GSGG,
  T_GGSS,                    T_GGSG, T_GGGS, T_GGGG,
  
  T_TILES_D_ON_S,
  T_SSSS_2 = T_TILES_D_ON_S, T_SSSD, T_SSDS, T_SSDD,
  T_SDSS,                    T_SDSD, T_SDDS, T_SDDD,
  T_DSSS,                    T_DSSD, T_DSDS, T_DSDD,
  T_DDSS,                    T_DDSD, T_DDDS, T_DDDD,
  
  T_TILES_E_ON_D,
  T_DDDD_2 = T_TILES_E_ON_D, T_DDDE, T_DDED, T_DDEE,
  T_DEDD,                    T_DEDE, T_DEED, T_DEEE,
  T_EDDD,                    T_EDDE, T_EDED, T_EDEE,
  T_EEDD,                    T_EEDE, T_EEED, T_EEEE,
  
  T_TILES_P_ON_S,
  T_SSSS_3 = T_TILES_P_ON_S, T_SSSP, T_SSPS, T_SSPP,
  T_SPSS,                    T_SPSP, T_SPPS, T_SPPP,
  T_PSSS,                    T_PSSP, T_PSPS, T_PSPP,
  T_PPSS,                    T_PPSP, T_PPPS, T_PPPP,
  
  T_TILES_I_ON_W,
  T_WWWW   = T_TILES_I_ON_W, T_WWWI, T_WWIW, T_WWII,
  T_WIWW,                    T_WIWI, T_WIIW, T_WIII,
  T_IWWW,                    T_IWWI, T_IWIW, T_IWII,
  T_IIWW,                    T_IIWI, T_IIIW, T_IIII,
  
  T_TILES_R_ON_L,
  T_LLLL   = T_TILES_R_ON_L, T_LLLR, T_LLRL, T_LLRR,
  T_LRLL,                    T_LRLR, T_LRRL, T_LRRR,
  T_RLLL,                    T_RLLR, T_RLRL, T_RLRR,
  T_RRLL,                    T_RRLR, T_RRRL, T_RRRR,
  
  T_TILES_Y_ON_X,
  T_XXXX   = T_TILES_Y_ON_X, T_XXXY, T_XXYX, T_XXYY,
  T_XYXX,                    T_XYXY, T_XYYX, T_XYYY,
  T_YXXX,                    T_YXXY, T_YXYX, T_YXYY,
  T_YYXX,                    T_YYXY, T_YYYX, T_YYYY,
  
  T_TILES_V_ON_K,
  T_KKKK_2 = T_TILES_V_ON_K, T_KKKV, T_KKVK, T_KKVV,
  T_KVKK,                    T_KVKV, T_KVVK, T_KVVV,
  T_VKKK,                    T_VKKV, T_VKVK, T_VKVV,
  T_VVKK,                    T_VVKV, T_VVVK, T_VVVV,
  
  T_TILES_B_ON_K,
  T_KKKK   = T_TILES_B_ON_K, T_KKKB, T_KKBK, T_KKBB,
  T_KBKK,                    T_KBKB, T_KBBK, T_KBBB,
  T_BKKK,                    T_BKKB, T_BKBK, T_BKBB,
  T_BBKK,                    T_BBKB, T_BBBK, T_BBBB
};

struct Texture
{
  char   name[100];
  bool   wrap;
  GLuint tex;
};

Texture textures[] =
{
  { "data/Kromasky.bmp"   , false, 0 },
  { "data/Particle.bmp"   , false, 0 },
  { "data/ShipLL.bmp"     , false, 0 },
  { "data/ShipL.bmp"      , false, 0 },
  { "data/ShipC.bmp"      , false, 0 },
  { "data/ShipR.bmp"      , false, 0 },
  { "data/ShipRR.bmp"     , false, 0 },
  { "data/bkg0.bmp"       , false, 0 },
  { "data/Rock0.bmp"      , false, 0 },
  { "data/Rock1.bmp"      , false, 0 },
  { "data/Rock2.bmp"      , false, 0 },
  { "data/Rock3.bmp"      , false, 0 },
  { "data/Rock4.bmp"      , false, 0 },
  { "data/Pearl.bmp"      , false, 0 },
  { "data/Energy.bmp"     , false, 0 },
  { "data/Fuel.bmp"       , false, 0 },
  { "data/Star.bmp"       , false, 0 },
  { "data/Juice.bmp"      , false, 0 },
  { "data/Rocket.bmp"     , false, 0 },
  { "data/Mine.bmp"       , false, 0 },
  { "data/Drone0.bmp"     , false, 0 },
  { "data/Drone1.bmp"     , false, 0 },
  { "data/Drone2.bmp"     , false, 0 },
  
  // Terrain tiles
  // Sand-grass
  { "data/tiles/sg/ssss.bmp"  , false, 0 },
  { "data/tiles/sg/sssg.bmp"  , false, 0 },
  { "data/tiles/sg/ssgs.bmp"  , false, 0 },
  { "data/tiles/sg/ssgg.bmp"  , false, 0 },
  { "data/tiles/sg/sgss.bmp"  , false, 0 },
  { "data/tiles/sg/sgsg.bmp"  , false, 0 },
  { "data/tiles/sg/sggs.bmp"  , false, 0 },
  { "data/tiles/sg/sggg.bmp"  , false, 0 },
  { "data/tiles/sg/gsss.bmp"  , false, 0 },
  { "data/tiles/sg/gssg.bmp"  , false, 0 },
  { "data/tiles/sg/gsgs.bmp"  , false, 0 },
  { "data/tiles/sg/gsgg.bmp"  , false, 0 },
  { "data/tiles/sg/ggss.bmp"  , false, 0 },
  { "data/tiles/sg/ggsg.bmp"  , false, 0 },
  { "data/tiles/sg/gggs.bmp"  , false, 0 },
  { "data/tiles/sg/gggg1.bmp" , false, 0 },
  
  // Sand-dark
  { "data/tiles/sd/ssss.bmp"  , false, 0 },
  { "data/tiles/sd/sssd.bmp"  , false, 0 },
  { "data/tiles/sd/ssds.bmp"  , false, 0 },
  { "data/tiles/sd/ssdd.bmp"  , false, 0 },
  { "data/tiles/sd/sdss.bmp"  , false, 0 },
  { "data/tiles/sd/sdsd.bmp"  , false, 0 },
  { "data/tiles/sd/sdds.bmp"  , false, 0 },
  { "data/tiles/sd/sddd.bmp"  , false, 0 },
  { "data/tiles/sd/dsss.bmp"  , false, 0 },
  { "data/tiles/sd/dssd.bmp"  , false, 0 },
  { "data/tiles/sd/dsds.bmp"  , false, 0 },
  { "data/tiles/sd/dsdd.bmp"  , false, 0 },
  { "data/tiles/sd/ddss.bmp"  , false, 0 },
  { "data/tiles/sd/ddsd.bmp"  , false, 0 },
  { "data/tiles/sd/ddds.bmp"  , false, 0 },
  { "data/tiles/sd/dddd.bmp"  , false, 0 },
  
  // Elevated-dark
  { "data/tiles/de/dddd.bmp"  , false, 0 },
  { "data/tiles/de/ddde.bmp"  , false, 0 },
  { "data/tiles/de/dded.bmp"  , false, 0 },
  { "data/tiles/de/ddee.bmp"  , false, 0 },
  { "data/tiles/de/dedd.bmp"  , false, 0 },
  { "data/tiles/de/dede.bmp"  , false, 0 },
  { "data/tiles/de/deed.bmp"  , false, 0 },
  { "data/tiles/de/deee.bmp"  , false, 0 },
  { "data/tiles/de/eddd.bmp"  , false, 0 },
  { "data/tiles/de/edde.bmp"  , false, 0 },
  { "data/tiles/de/eded.bmp"  , false, 0 },
  { "data/tiles/de/edee.bmp"  , false, 0 },
  { "data/tiles/de/eedd.bmp"  , false, 0 },
  { "data/tiles/de/eede.bmp"  , false, 0 },
  { "data/tiles/de/eeed.bmp"  , false, 0 },
  { "data/tiles/de/eeee.bmp"  , false, 0 },
  
  // Sand-palm
  { "data/tiles/sp/ssss.bmp"  , false, 0 },
  { "data/tiles/sp/sssp.bmp"  , false, 0 },
  { "data/tiles/sp/ssps.bmp"  , false, 0 },
  { "data/tiles/sp/sspp.bmp"  , false, 0 },
  { "data/tiles/sp/spss.bmp"  , false, 0 },
  { "data/tiles/sp/spsp.bmp"  , false, 0 },
  { "data/tiles/sp/spps.bmp"  , false, 0 },
  { "data/tiles/sp/sppp.bmp"  , false, 0 },
  { "data/tiles/sp/psss.bmp"  , false, 0 },
  { "data/tiles/sp/pssp.bmp"  , false, 0 },
  { "data/tiles/sp/psps.bmp"  , false, 0 },
  { "data/tiles/sp/pspp.bmp"  , false, 0 },
  { "data/tiles/sp/ppss.bmp"  , false, 0 },
  { "data/tiles/sp/ppsp.bmp"  , false, 0 },
  { "data/tiles/sp/ppps.bmp"  , false, 0 },
  { "data/tiles/sp/pppp.bmp"  , false, 0 },
  
  // Water-island
  { "data/tiles/wi/wwww.bmp"  , false, 0 },
  { "data/tiles/wi/wwwi.bmp"  , false, 0 },
  { "data/tiles/wi/wwiw.bmp"  , false, 0 },
  { "data/tiles/wi/wwii.bmp"  , false, 0 },
  { "data/tiles/wi/wiww.bmp"  , false, 0 },
  { "data/tiles/wi/wiwi.bmp"  , false, 0 },
  { "data/tiles/wi/wiiw.bmp"  , false, 0 },
  { "data/tiles/wi/wiii.bmp"  , false, 0 },
  { "data/tiles/wi/iwww.bmp"  , false, 0 },
  { "data/tiles/wi/iwwi.bmp"  , false, 0 },
  { "data/tiles/wi/iwiw.bmp"  , false, 0 },
  { "data/tiles/wi/iwii.bmp"  , false, 0 },
  { "data/tiles/wi/iiww.bmp"  , false, 0 },
  { "data/tiles/wi/iiwi.bmp"  , false, 0 },
  { "data/tiles/wi/iiiw.bmp"  , false, 0 },
  { "data/tiles/wi/iiii.bmp"  , false, 0 },
  
  // L-R
  { "data/tiles/lr/llll.bmp"  , false, 0 },
  { "data/tiles/lr/lllr.bmp"  , false, 0 },
  { "data/tiles/lr/llrl.bmp"  , false, 0 },
  { "data/tiles/lr/llrr.bmp"  , false, 0 },
  { "data/tiles/lr/lrll.bmp"  , false, 0 },
  { "data/tiles/lr/lrlr.bmp"  , false, 0 },
  { "data/tiles/lr/lrrl.bmp"  , false, 0 },
  { "data/tiles/lr/lrrr.bmp"  , false, 0 },
  { "data/tiles/lr/rlll.bmp"  , false, 0 },
  { "data/tiles/lr/rllr.bmp"  , false, 0 },
  { "data/tiles/lr/rlrl.bmp"  , false, 0 },
  { "data/tiles/lr/rlrr.bmp"  , false, 0 },
  { "data/tiles/lr/rrll.bmp"  , false, 0 },
  { "data/tiles/lr/rrlr.bmp"  , false, 0 },
  { "data/tiles/lr/rrrl.bmp"  , false, 0 },
  { "data/tiles/lr/rrrr.bmp"  , false, 0 },
  
  // X-Y
  { "data/tiles/xy/xxxx.bmp"  , false, 0 },
  { "data/tiles/xy/xxxy.bmp"  , false, 0 },
  { "data/tiles/xy/xxyx.bmp"  , false, 0 },
  { "data/tiles/xy/xxyy.bmp"  , false, 0 },
  { "data/tiles/xy/xyxx.bmp"  , false, 0 },
  { "data/tiles/xy/xyxy.bmp"  , false, 0 },
  { "data/tiles/xy/xyyx.bmp"  , false, 0 },
  { "data/tiles/xy/xyyy.bmp"  , false, 0 },
  { "data/tiles/xy/yxxx.bmp"  , false, 0 },
  { "data/tiles/xy/yxxy.bmp"  , false, 0 },
  { "data/tiles/xy/yxyx.bmp"  , false, 0 },
  { "data/tiles/xy/yxyy.bmp"  , false, 0 },
  { "data/tiles/xy/yyxx.bmp"  , false, 0 },
  { "data/tiles/xy/yyxy.bmp"  , false, 0 },
  { "data/tiles/xy/yyyx.bmp"  , false, 0 },
  { "data/tiles/xy/yyyy.bmp"  , false, 0 },
  
  // K-V
  { "data/tiles/kv/kkkk.bmp"  , false, 0 },
  { "data/tiles/kv/kkkv.bmp"  , false, 0 },
  { "data/tiles/kv/kkvk.bmp"  , false, 0 },
  { "data/tiles/kv/kkvv.bmp"  , false, 0 },
  { "data/tiles/kv/kvkk.bmp"  , false, 0 },
  { "data/tiles/kv/kvkv.bmp"  , false, 0 },
  { "data/tiles/kv/kvvk.bmp"  , false, 0 },
  { "data/tiles/kv/kvvv.bmp"  , false, 0 },
  { "data/tiles/kv/vkkk.bmp"  , false, 0 },
  { "data/tiles/kv/vkkv.bmp"  , false, 0 },
  { "data/tiles/kv/vkvk.bmp"  , false, 0 },
  { "data/tiles/kv/vkvv.bmp"  , false, 0 },
  { "data/tiles/kv/vvkk.bmp"  , false, 0 },
  { "data/tiles/kv/vvkv.bmp"  , false, 0 },
  { "data/tiles/kv/vvvk.bmp"  , false, 0 },
  { "data/tiles/kv/vvvv1.bmp" , false, 0 },
  
  // K-B
  { "data/tiles/kb/kkkk.bmp"  , false, 0 },
  { "data/tiles/kb/kkkb.bmp"  , false, 0 },
  { "data/tiles/kb/kkbk.bmp"  , false, 0 },
  { "data/tiles/kb/kkbb.bmp"  , false, 0 },
  { "data/tiles/kb/kbkk.bmp"  , false, 0 },
  { "data/tiles/kb/kbkb.bmp"  , false, 0 },
  { "data/tiles/kb/kbbk.bmp"  , false, 0 },
  { "data/tiles/kb/kbbb.bmp"  , false, 0 },
  { "data/tiles/kb/bkkk.bmp"  , false, 0 },
  { "data/tiles/kb/bkkb.bmp"  , false, 0 },
  { "data/tiles/kb/bkbk.bmp"  , false, 0 },
  { "data/tiles/kb/bkbb.bmp"  , false, 0 },
  { "data/tiles/kb/bbkk.bmp"  , false, 0 },
  { "data/tiles/kb/bbkb.bmp"  , false, 0 },
  { "data/tiles/kb/bbbk.bmp"  , false, 0 },
  { "data/tiles/kb/bbbb.bmp"  , false, 0 },
};

void LoadTextures()
{
  for (int i = 0; i < SIZE_ARRAY(textures); i++)
    textures[i].tex = CORE_LoadBmp(textures[i].name, true);
}

void UnloadTextures()
{
  for (int i = 0; i < SIZE_ARRAY(textures); i++)
    CORE_UnloadBmp(textures[i].tex);
}

GLuint Tex(TexId id)
{
  return textures[id].tex;
}

//=============================================================================
// Font support
#define FONTDEF_ROWS 8
#define FONTDEF_COLS 8
#define MAX_FONT_CHARDEFS 256
#define FONTDEF_CHAR_WIDTH (16.f/128.f)
#define FONTDEF_CHAR_HEIGHT (16.f/128.f)
#define FONT_CHAR_WIDTH 16.f
#define FONT_CHAR_HEIGHT 16.f

char fontdef[FONTDEF_ROWS][FONTDEF_COLS+1] =
{
  { " !\"~*%-'" },
  { "   +, ./" },
  { "01234567" },
  { "89:;aib?" },
  { "*ABCDEFG" },
  { "HIJKLMNO" },
  { "PQRSTUVW" },
  { "XYZ     " },
};

struct FontCharDef
{
  char ch;
  vec2 p0;
};

FontCharDef fontchardefs[MAX_FONT_CHARDEFS] = { 0 };

//-----------------------------------------------------------------------------
void PrepareFont()
{
  for (int i = 0; i < FONTDEF_ROWS; i++) // One iteration per row
  {
    for (int j = 0; j < FONTDEF_COLS; j++) // Inside row
    {
      unsigned char ch = fontdef[i][j];
      fontchardefs[ch].ch = ch;
      fontchardefs[ch].p0 = vmake(j * FONTDEF_CHAR_WIDTH, (FONTDEF_ROWS - i - 1) * FONTDEF_CHAR_HEIGHT);
    }
  }
}

//-----------------------------------------------------------------------------
void DrawChar(vec2 p0, vec2 p1, unsigned char ch, rgba color)
{
  if (ch < MAX_FONT_CHARDEFS && fontchardefs[ch].ch == ch)
  {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f( color.r, color.g, color.b, color.a);
    
    glBindTexture( GL_TEXTURE_2D, CORE_GetBmpOpenGLTex(Tex(T_FONT)) );
    glBegin( GL_QUADS );
    glTexCoord2d(fontchardefs[ch].p0.x                     , fontchardefs[ch].p0.y                      ); glVertex2f(p0.x, p0.y);
    glTexCoord2d(fontchardefs[ch].p0.x + FONTDEF_CHAR_WIDTH, fontchardefs[ch].p0.y                      ); glVertex2f(p1.x, p0.y);
    glTexCoord2d(fontchardefs[ch].p0.x + FONTDEF_CHAR_WIDTH, fontchardefs[ch].p0.y + FONTDEF_CHAR_HEIGHT); glVertex2f(p1.x, p1.y);
    glTexCoord2d(fontchardefs[ch].p0.x                     , fontchardefs[ch].p0.y + FONTDEF_CHAR_HEIGHT); glVertex2f(p0.x, p1.y);
    glEnd();
  }
}

//-----------------------------------------------------------------------------
void DrawString(vec2 p0, const char string[], float charsize, rgba color)
{
  int n = (int)strlen(string);
  for (int i = 0; i < n; i++)
  {
    DrawChar(p0, vadd(p0, vmake(charsize, charsize)), string[i], color);
    p0 = vadd(p0, vmake(charsize, 0.f));
  }
}


//=============================================================================
// Sound engine
enum SoundId
{
  SND_THUMP,
  SND_EXPLOSSION,
  SND_ENGINE,
  SND_SUCCESS
};

struct Sound
{
  char name[100];
  int  bufid;
};

Sound sounds[] =
{
  { "data/410__tictacshutup__thump-1.wav"       , 0 },
  { "data/94185__nbs-dark__explosion.wav"       , 0 },
  { "data/ffff.wav"                             , 0 },
  { "data/171671__fins__success-1.wav"          , 0 }
};

void LoadSounds()
{
  for (int i = 0; i < SIZE_ARRAY(sounds); i++)
    sounds[i].bufid = CORE_LoadWav(sounds[i].name);
}

void UnloadSounds()
{
  for (int i = 0; i < SIZE_ARRAY(sounds); i++)
    CORE_UnloadWav(sounds[i].bufid);
}

void PlaySound(SoundId id, float vol = SND_DEFAULT_VOL, float freq = 1.f)
{
  if (g_opt_sound_fx)
    CORE_PlaySound(sounds[id].bufid, vol, freq);
}

void PlayLoopSound (unsigned loopchannel, SoundId id, float vol, float pitch)
{
  if (g_opt_sound_fx)
    CORE_PlayLoopSound(loopchannel, sounds[id].bufid, vol, pitch);
}

void StopLoopSound(unsigned loopchannel)
{
  CORE_StopLoopSound(loopchannel);
}

void SetLoopSoundParam (unsigned loopchannel, float vol, float pitch)
{
  if (g_opt_sound_fx)
    CORE_SetLoopSoundParam(loopchannel, vol, pitch);
}

void UpdateSoundStatus()
{
  if (!g_opt_sound_fx)
  {
    CORE_SetLoopSoundParam(SHIP_ENGINE_SOUND_CHANNEL, 0.f, 1.f); // Stop engine
  }
  
  // Pending music!
}

//=============================================================================
// Particle systems manager
enum PSType
{
  PST_NULL, PST_WATER, PST_FIRE, PST_SMOKE, PST_DUST, PST_GOLD
};

struct PSDef
{
  TexId  texture;
  bool   additive;
  int    newpartsperframe;
  int    death;
  vec2   force;
  float  startpos_random;
  vec2   startspeed_fixed;
  float  startspeed_random;
  float  startradius_min, startradius_max;
  rgba   startcolor_fixed;
  rgba   startcolor_random;
};

PSDef psdefs[] =
{
  //            GFX          ADD     N  DTH  FORCE               rndPos SPEED           rndSPD  Rmin  Rmax  clr                       clr-rand
  /* null  */ { },
  /* water */ { T_PARTICLE,  true ,  4, 150, vmake(0.f, -0.025f),  4.f,  vmake(0.f, 0.45f), .1f,  8.f, 10.f, RGBA( 64,  64, 255, 128), RGBA(0, 0, 0, 0) },
  /* fire  */ { T_PARTICLE,  true ,  8,  60, vmake(0.f, -0.75f),  4.f,  vmake(0.f,+0.25f), .1f,  8.f, 16.f, RGBA(255, 192, 128, 128), RGBA(0, 0, 0, 0) },
  /* smoke */ { T_PARTICLE,  false,  2, 250, vmake(0.f,  0.05f) ,  4.f,  vmake(0.f, 0.f)  , .4f,  5.f, 12.f, RGBA( 64,  64,  64, 192), RGBA(0, 0, 0, 0) },
  /* dust  */ { T_PARTICLE,  false,  4, 100, vmake(0.f,  0.05f) ,  4.f,  vmake(0.f, 0.f)  , .4f,  3.f,  6.f, RGBA(192, 192, 192, 192), RGBA(0, 0, 0, 0) },
  /* gold  */ { T_PARTICLE,  true ,  1,  50, vmake(0.f,  0.f  ) ,  4.f,  vmake(0.f, 0.f)  , .3f,  3.f,  6.f, RGBA(192, 192,  64, 192), RGBA(0, 0, 0, 0) },
};



//=============================================================================
// Particle system model

#define MAX_PSYSTEMS 64
#define MAX_PARTICLES 512

struct Particle
{
  byte   active;
  byte   padding;
  word   age;
  vec2   pos;
  vec2   vel;
  float  radius;
  rgba   color;
};

struct PSystem
{
  PSType   type;
  vec2     source_pos;
  vec2     source_vel;
  Particle particles[MAX_PARTICLES];
};

PSystem psystems[MAX_PSYSTEMS];

//-----------------------------------------------------------------------------
void ResetPSystems()
{
  for (int i = 0; i < MAX_PSYSTEMS; i++)
    psystems[i].type = PST_NULL;
}

//-----------------------------------------------------------------------------
int CreatePSystem(PSType type, vec2 pos, vec2 vel)
{
  for (int i = 0; i < MAX_PSYSTEMS; i++)
  {
    if (psystems[i].type == PST_NULL)
    {
      psystems[i].type = type;
      psystems[i].source_pos = pos;
      psystems[i].source_vel = vel;
      for (int j = 0; j < MAX_PARTICLES; j++)
        psystems[i].particles[j].active = 0;
      return i;
    }
  }
  
  return -1;
}

//-----------------------------------------------------------------------------
void KillPSystem(int ix)
{
  if (ix >= 0 && ix < MAX_PSYSTEMS)
    psystems[ix].type = PST_NULL;
}

//-----------------------------------------------------------------------------
void SetPSystemSource(int ix, vec2 pos, vec2 vel)
{
  if (ix >= 0 && ix < MAX_PSYSTEMS)
  {
    psystems[ix].source_pos = pos;
    psystems[ix].source_vel = vel;
  }
}

//-----------------------------------------------------------------------------
void RenderPSystems(vec2 offset)
{
  glEnable(GL_BLEND);
  for (int i = 0; i < MAX_PSYSTEMS; i++)
  {
    if (psystems[i].type != PST_NULL)
    {
      if (psdefs[psystems[i].type].additive)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      else
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          
      glBindTexture( GL_TEXTURE_2D, CORE_GetBmpOpenGLTex(Tex(psdefs[psystems[i].type].texture)) );
      glBegin( GL_QUADS );

      for (int j = 0; j < MAX_PARTICLES; j++)
      {
        if (psystems[i].particles[j].active)
        {
          vec2 pos = psystems[i].particles[j].pos;
          float radius = psystems[i].particles[j].radius;
          vec2 p0 = vsub( pos, vmake(radius, radius) );
          vec2 p1 = vadd( pos, vmake(radius, radius) );
          rgba color = psystems[i].particles[j].color;
          
          glColor4f( color.r, color.g, color.b, color.a);
          
          glTexCoord2d(0.0,0.0);
          glVertex2f(p0.x + offset.x, p0.y + offset.y);
          
          glTexCoord2d(1.0,0.0);
          glVertex2f(p1.x + offset.x, p0.y + offset.y);
          
          glTexCoord2d(1.0,1.0);
          glVertex2f(p1.x + offset.x, p1.y + offset.y);
          
          glTexCoord2d(0.0,1.0);
          glVertex2f(p0.x + offset.x, p1.y + offset.y);
          
        }
      }

      glEnd();
    }
  }

  glColor4f(1.f, 1.f, 1.f, 1.);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//-----------------------------------------------------------------------------
void RunPSystems()
{
  for (int i = 0; i < MAX_PSYSTEMS; i++)
  {
    if (psystems[i].type != PST_NULL)
    {
      // New particles -----------------------------------------------------------------------
      for (int j = 0; j < psdefs[psystems[i].type].newpartsperframe; j++)
      {
        for (int k = 0; k < MAX_PARTICLES; k++)
        {
          if (!psystems[i].particles[k].active)
          {
            psystems[i].particles[k].active = 1;
            psystems[i].particles[k].age = 0;
            psystems[i].particles[k].pos =
              vadd(psystems[i].source_pos,
                   vmake( CORE_FRand(-psdefs[psystems[i].type].startpos_random, +psdefs[psystems[i].type].startpos_random)
                         ,CORE_FRand(-psdefs[psystems[i].type].startpos_random, +psdefs[psystems[i].type].startpos_random)
                   )
              );
            psystems[i].particles[k].vel =
               vadd(psystems[i].source_vel,
                 vmake(
                   psdefs[psystems[i].type].startspeed_fixed.x + CORE_FRand(-psdefs[psystems[i].type].startspeed_random, +psdefs[psystems[i].type].startspeed_random),
                   psdefs[psystems[i].type].startspeed_fixed.y + CORE_FRand(-psdefs[psystems[i].type].startspeed_random, +psdefs[psystems[i].type].startspeed_random)
                 )
               );
            psystems[i].particles[k].radius = CORE_FRand(psdefs[psystems[i].type].startradius_min, psdefs[psystems[i].type].startradius_max);
            psystems[i].particles[k].color  = psdefs[psystems[i].type].startcolor_fixed;
            break; // Added!
          }
        }
      }
    
      // Run particles -----------------------------------------------------------------------
      for (int k = 0; k < MAX_PARTICLES; k++)
      {
        if (psystems[i].particles[k].active)
        {
          psystems[i].particles[k].age++;
          if (psystems[i].particles[k].age > psdefs[psystems[i].type].death)
            psystems[i].particles[k].active = 0;
          else
          {
            // Move
            psystems[i].particles[k].pos = vadd(psystems[i].particles[k].pos, psystems[i].particles[k].vel);
            psystems[i].particles[k].vel = vadd(psystems[i].particles[k].vel, psdefs[psystems[i].type].force);
            
            // Color
            psystems[i].particles[k].color.a = (psdefs[psystems[i].type].death - psystems[i].particles[k].age)/255.f;
          }
        }
      }
    }
  }
}

//=============================================================================
// Game

//-----------------------------------------------------------------------------
// Game state
float g_time      = 0.f;
enum GameState { GS_PLAYING, GS_DYING, GS_STARTING, GS_VICTORY, GS_MAIN_MENU, GS_INGAME_MENU };
GameState g_gs = GS_STARTING;
float g_gs_timer = 0.f;
float g_current_race_pos = 0.f;
float g_camera_offset = 0.f;
float g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
float g_time_from_last_rocket = 0.f;
TexId g_active_tileset = MAIN_MENU_TILESET;
int   g_current_level = 0; // 0 to 8
bool  g_user_exit = false;
int   g_unlocked_level = 0;

//=============================================================================
// Levels
#define NUM_LEVELS 9
struct LevelDesc
{
  TexId tileset;
  float level_length;
  float path_width;
  float path_twist_ratio;
  int   min_layers;
  int   max_layers;
  int   min_rocks_per_layer;
  int   max_rocks_per_layer;
  float chance_mine;
  float chance_drone;
  float min_space_between_layers;
  float max_space_between_layers;
  float min_space_between_challenges;
  float max_space_between_challenges;
  float level_start_chance_alt_terrain;
  float level_end_chance_alt_terrain;
};

LevelDesc LevelDescs[NUM_LEVELS] = {
//
  { T_TILES_P_ON_S,  40000.f, (2.2f * MAINSHIP_RADIUS), 0.5f, 0, 10, 0, 3, 0.1f, 0.0f, 300.f, 600.f, 700.f, 2500.f, 0.2f, 0.9f },
  { T_TILES_G_ON_S,  60000.f, (2.1f * MAINSHIP_RADIUS), 0.5f, 1, 15, 0, 3, 0.1f, 0.0f, 300.f, 600.f, 650.f, 2300.f, 0.2f, 0.9f },
  { T_TILES_D_ON_S,  80000.f, (2.0f * MAINSHIP_RADIUS), 0.5f, 1, 20, 0, 3, 0.2f, 0.0f, 250.f, 600.f, 600.f, 2100.f, 0.2f, 0.9f },
  { T_TILES_E_ON_D, 100000.f, (1.9f * MAINSHIP_RADIUS), 0.6f, 2, 25, 1, 4, 0.3f, 0.0f, 250.f, 500.f, 550.f, 1900.f, 0.2f, 0.9f },
  { T_TILES_I_ON_W, 120000.f, (1.8f * MAINSHIP_RADIUS), 0.7f, 2, 30, 1, 4, 0.4f, 0.1f, 200.f, 500.f, 500.f, 1700.f, 0.1f, 0.9f },
  { T_TILES_Y_ON_X, 140000.f, (1.7f * MAINSHIP_RADIUS), 0.8f, 3, 35, 1, 4, 0.4f, 0.1f, 200.f, 500.f, 450.f, 1500.f, 0.2f, 0.9f },
  { T_TILES_R_ON_L, 160000.f, (1.6f * MAINSHIP_RADIUS), 0.9f, 3, 40, 2, 5, 0.5f, 0.1f, 200.f, 400.f, 400.f, 1300.f, 0.8f, 0.1f },
  { T_TILES_V_ON_K, 180000.f, (1.5f * MAINSHIP_RADIUS), 1.0f, 4, 45, 2, 5, 0.6f, 0.2f, 150.f, 400.f, 350.f, 1100.f, 0.2f, 0.9f },
  { T_TILES_B_ON_K, 200000.f, (1.4f * MAINSHIP_RADIUS), 1.1f, 4, 50, 2, 5, 0.7f, 0.3f, 150.f, 400.f, 300.f,  900.f, 0.2f, 0.9f }
};

//-----------------------------------------------------------------------------
// Background generation
#define TILE_WIDTH 120//96//75
#define TILE_HEIGHT 140//112//58
#define TILES_ACROSS (1+(int)((G_WIDTH +TILE_WIDTH -1)/TILE_WIDTH))
#define TILES_DOWN   (1+(int)((G_HEIGHT+TILE_HEIGHT-1)/TILE_HEIGHT))
#define RUNNING_ROWS (2 * TILES_DOWN)
// Bottom to top!
byte  Terrain[RUNNING_ROWS][TILES_ACROSS];
TexId TileMap[RUNNING_ROWS][TILES_ACROSS];

int g_last_generated = -1;

void GenTerrain(float upto)
{
  int last_required_row = 1 + (int)(upto / TILE_HEIGHT);
  
  // Generate random terrain types
  for (int i = g_last_generated+1; i <= last_required_row; i++)
  {
    float advance = (i * TILE_HEIGHT) / LevelDescs[g_current_level].level_length;
    float chance = LevelDescs[g_current_level].level_start_chance_alt_terrain
                 + advance * (LevelDescs[g_current_level].level_end_chance_alt_terrain
                             - LevelDescs[g_current_level].level_start_chance_alt_terrain);
    
    int mapped_row = UMod(i, RUNNING_ROWS);
    
    for (int j = 0; j < TILES_ACROSS; j++)
      Terrain[mapped_row][j] = (CORE_RandChance(chance) & 1);
  }
  
  // Calculate the tiles
  for (int i = g_last_generated; i <= last_required_row; i++)
  {
    int mapped_row = UMod(i, RUNNING_ROWS);
    
    for (int j = 0; j < TILES_ACROSS; j++)
    {
      unsigned v =   (Terrain[mapped_row                      ][j                      ] << 1)
                   | (Terrain[mapped_row                      ][UMod(j+1, TILES_ACROSS)] << 0)
                   | (Terrain[UMod(mapped_row+1, RUNNING_ROWS)][j                      ] << 3)
                   | (Terrain[UMod(mapped_row+1, RUNNING_ROWS)][UMod(j+1, TILES_ACROSS)] << 2);
      if (v > 15) v = 15;
      TileMap[mapped_row][j] = (TexId)(g_active_tileset + v);
    }
  }
  
  g_last_generated = last_required_row;
}

//-----------------------------------------------------------------------------
void RenderTerrain()
{
  int first_row = (int)(g_camera_offset / TILE_HEIGHT);
  
  for (int i = first_row; i < first_row + TILES_DOWN; i++)
  {
    int mapped_row = UMod(i, RUNNING_ROWS);
    for (int j = 0; j < TILES_ACROSS; j++)
      CORE_RenderCenteredSprite(
        vsub(vmake(j * TILE_WIDTH + .5f * TILE_WIDTH, i * TILE_HEIGHT + .5f * TILE_HEIGHT), vmake(0.f,g_camera_offset)),
        vmake(TILE_WIDTH * 1.01f, TILE_HEIGHT * 1.01f),
        Tex(TileMap[mapped_row][j])
      );
  }
}

//-----------------------------------------------------------------------------
// Entities

enum EType { E_NULL, E_MAIN, E_ROCK, E_STAR, E_JUICE, E_ROCKET, E_MINE, E_DRONE };

#define MAX_ENTITIES 64

struct Entity
{
  EType  type;
  vec2   pos;
  vec2   vel;
  float  radius;
  float  energy;
  float  fuel;
  float  tilt;
  float  gfxscale;
  TexId  gfx;
  bool   gfxadditive;
  rgba   color;
  bool   has_shadow;
  int    psystem;
  vec2   psystem_off;
};
Entity   g_entities[MAX_ENTITIES];

//-----------------------------------------------------------------------------
int InsertEntity(EType type, vec2 pos, vec2 vel, float radius, TexId gfx, bool has_shadow, bool additive = false)
{
  for (int i = 0; i < MAX_ENTITIES; i++)
  {
    if (g_entities[i].type == E_NULL)
    {
      g_entities[i].type         = type;
      g_entities[i].pos          = pos;
      g_entities[i].vel          = vel;
      g_entities[i].radius       = radius;
      g_entities[i].gfx          = gfx;
      g_entities[i].energy       = MAX_ENERGY;
      g_entities[i].fuel         = MAX_FUEL;
      g_entities[i].tilt         = 0.f;
      g_entities[i].gfxscale     = 1.f;
      g_entities[i].gfxadditive  = additive;
      g_entities[i].color        = COLOR_WHITE;
      g_entities[i].has_shadow   = has_shadow;
      g_entities[i].psystem      = -1;
      g_entities[i].psystem_off  = vmake(0.f,0.f);
      return i;
    }
  }
  return -1;
}

//-----------------------------------------------------------------------------
void KillEntity(int ix)
{
  if (ix >= 0 && ix <= MAX_ENTITIES)
  {
    g_entities[ix].type = E_NULL;
    if (g_entities[ix].psystem >= 0)
      KillPSystem(g_entities[ix].psystem);
  }
}


//-----------------------------------------------------------------------------
// Level generation
float g_next_challenge_area = FIRST_CHALLENGE;
vec2 g_last_conditioned = vmake(0.f,0.f);
//#define PATH_TWIST_RATIO .5f // This means about 30 degrees maximum
//#define PATH_WIDTH (2.f * MAINSHIP_RADIUS)

void GenNextElements()
{
  // Called every game loop, but only does work when we are close to the next "challenge area"
  if (g_current_race_pos + G_HEIGHT > g_next_challenge_area)
  {
    // Get params from current level
    float path_width                   = LevelDescs[g_current_level].path_width;
    float path_twist_ratio             = LevelDescs[g_current_level].path_twist_ratio;
    int   min_layers                   = LevelDescs[g_current_level].min_layers;
    int   max_layers                   = LevelDescs[g_current_level].max_layers;
    int   min_rocks_per_layer          = LevelDescs[g_current_level].min_rocks_per_layer;
    int   max_rocks_per_layer          = LevelDescs[g_current_level].max_rocks_per_layer;
    float chance_mine                  = LevelDescs[g_current_level].chance_mine;
    float chance_drone                 = LevelDescs[g_current_level].chance_drone;
    float min_space_between_layers     = LevelDescs[g_current_level].min_space_between_layers;
    float max_space_between_layers     = LevelDescs[g_current_level].max_space_between_layers;
    float min_space_between_challenges = LevelDescs[g_current_level].min_space_between_challenges;
    float max_space_between_challenges = LevelDescs[g_current_level].max_space_between_challenges;
    
    float current_y = g_next_challenge_area;
    LOG(("Current: %f\n", g_next_challenge_area));
    
    // Choose how many layers of rocks
    int nlayers = (int)CORE_URand(min_layers, max_layers);
    LOG((" nlayers: %d\n", nlayers));
    for (int i = 0; i < nlayers; i++)
    {
      LOG(("  where: %f\n", current_y));
      
      // Choose pass point
      float displace = (current_y - g_last_conditioned.y) * path_twist_ratio;
      float bracket_left  = g_last_conditioned.x - displace;
      float bracket_right = g_last_conditioned.x + displace;
      bracket_left  = MAX(bracket_left, 2.f * MAINSHIP_RADIUS);
      bracket_right = MIN(bracket_right, G_WIDTH - 2.f * MAINSHIP_RADIUS);
      g_last_conditioned.y = current_y;
      g_last_conditioned.x = CORE_FRand(bracket_left, bracket_right);
      
      /*InsertEntity(E_JUICE,
                   vmake(g_last_conditioned.x, g_last_conditioned.y),
                   vmake(0.f,0.f),
                   JUICE_RADIUS, T_JUICE, false, true);*/
      
      // Choose how many rocks
      int nrocks = (int)CORE_URand(min_rocks_per_layer, max_rocks_per_layer);
      LOG(("  nrocks: %d\n", nrocks));
      
      // Gen rocks
      for (int i = 0; i < nrocks; i++)
      {
        // Find a valid pos!
        vec2 rockpos;
        for (int k = 0; k < 10; k++) // 10 attempts maximum, avoid infinite loops!
        {
          rockpos = vmake(CORE_FRand(0.f, G_WIDTH), current_y);
          if (   rockpos.x + ROCK_RADIUS < g_last_conditioned.x - path_width
              || rockpos.x - ROCK_RADIUS > g_last_conditioned.x + path_width)
            break;
        }
        
        // Insert obstacle
        EType t = E_ROCK;
        TexId gfx = T_ROCK1;
        if      (CORE_RandChance(chance_mine )) { t = E_MINE;  gfx = T_MINE; }
        else if (CORE_RandChance(chance_drone)) { t = E_DRONE; gfx = T_DRONE2; }
        InsertEntity(t,
                     rockpos,
                     vmake(CORE_FRand(-.5f, +.5f), CORE_FRand(-.5f, +.5f)),
                     //vmake(0.f,0.f),
                     ROCK_RADIUS, gfx, true);
      }
      
      current_y += CORE_FRand(min_space_between_layers, max_space_between_layers);
    }
    
    g_next_challenge_area = current_y + CORE_FRand(min_space_between_challenges, max_space_between_challenges);
    LOG(("Next: %f\n\n", g_next_challenge_area));
  }
}

//-----------------------------------------------------------------------------
void ResetNewGame(int level)
{
  if      (level < 0) level = 0;
  else if (level >= NUM_LEVELS) level = NUM_LEVELS-1;
  g_current_level = level;
  g_active_tileset = LevelDescs[level].tileset;
  
  // Reset everything for a new game...
  g_next_challenge_area = FIRST_CHALLENGE;
  g_last_conditioned = vmake(.5f * G_WIDTH, 0.f);
  g_current_race_pos = 0.f;
  g_camera_offset = 0.f;
  g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
  g_gs = GS_STARTING;
  g_gs_timer = 0.f;
  g_last_generated = -1;

  // Start logic
  for (int i = 0; i < MAX_ENTITIES; i++)
    g_entities[i].type = E_NULL;
  InsertEntity(E_MAIN, vmake(G_WIDTH/2.0, G_HEIGHT/8.f), vmake(0.f, SHIP_START_SPEED), MAINSHIP_RADIUS, T_SHIP_C, true);
  
  PlayLoopSound(SHIP_ENGINE_SOUND_CHANNEL, SND_ENGINE, 0.7f, 0.3f);
  
  ResetPSystems();
  
  MAIN_SHIP.psystem = CreatePSystem(PST_FIRE, MAIN_SHIP.pos, vmake(0.f,0.f));
  MAIN_SHIP.psystem_off = vmake(0.f, -120.f);
}

//-----------------------------------------------------------------------------
void FinishGame()
{
  g_active_tileset = MAIN_MENU_TILESET;
  g_current_race_pos = 0.f;
  g_camera_offset = 0.f;
  g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
  g_gs = GS_MAIN_MENU;
  g_gs_timer = 0.f;
  g_last_generated = -1;
  for (int i = 0; i < MAX_ENTITIES; i++)
    g_entities[i].type = E_NULL;
  ResetPSystems();
  StopLoopSound(SHIP_ENGINE_SOUND_CHANNEL);
}

//=============================================================================
// Menus
#define MENU_CHAR_SIZE 60.f
#define MENU_SPACE_BETWEEN_LINES 20.f
#define MENU_COLOR_UNSELECTED makergba(.8f, .8f, .8f, .8f)
#define MENU_COLOR_SELECTED COLOR_WHITE
enum MenuId
{
  M_NONE,
  M_MAIN, M_LEVELS, M_HELP, M_MAIN_OPTIONS, M_CONFIRM_EXIT,
  M_INGAME, M_INGAME_OPTIONS, M_CONFIRM_CANCEL,
  // Pseudo-menus
  M_PASSIVE, M_ACTION_EXIT, M_ACTION_CANCEL, M_ACTION_PLAY, M_ACTION_RESUME, M_ACTION_TOGGLE_MUSIC, M_ACTION_TOGGLE_SOUND_FX
};
#define MAX_MENU_STRING  100
#define MAX_MENU_ENTRIES 20
struct MenuEntryDef
{
  char   text[MAX_MENU_STRING];
  MenuId target;
  int    target_ix;
};
struct MenuDef
{
  char         title[MAX_MENU_STRING];
  int          num_entries;
  MenuEntryDef entries[MAX_MENU_ENTRIES];
};

MenuDef g_MenuDefs[] = {
  /*M_NONE*/           { "", 0, {}},
  /*M_MAIN*/           { "",             4,
                           {{ "PLAY",             M_LEVELS,       0 },
                            { "HELP",             M_HELP,         0 },
                            { "OPTIONS",          M_MAIN_OPTIONS, 0 },
                            { "EXIT",             M_CONFIRM_EXIT, 1 }}
                       },
  /*M_LEVELS*/         { "CHOOSE WORLD", 10,
                           {{ "JUNGLE",           M_ACTION_PLAY, 0},
                            { "PLAINS",           M_ACTION_PLAY, 1},
                            { "DESERT",           M_ACTION_PLAY, 2},
                            { "CAVES",            M_ACTION_PLAY, 3},
                            { "OCEAN",            M_ACTION_PLAY, 4},
                            { "TECHNO",           M_ACTION_PLAY, 5},
                            { "LAVA",             M_ACTION_PLAY, 6},
                            { "ANEMONA",          M_ACTION_PLAY, 7},
                            { "LIMBO",            M_ACTION_PLAY, 8},
                            { "BACK",             M_MAIN,        0}}
                       },
  /*M_HELP*/           { "", 15,
                           {{ "WELCOME TO",       M_PASSIVE, 0},
                            { "SPACECRASH!",      M_PASSIVE, 0},
                            { "STEER YOUR SHIP,", M_PASSIVE, 0},
                            { "AVOID ROCKS,",     M_PASSIVE, 0},
                            { "MINES, AND",       M_PASSIVE, 0},
                            { "ENEMY DRONES",     M_PASSIVE, 0},
                            { "",                 M_PASSIVE, 0},
                            { "GAME BY JONBHO",   M_PASSIVE, 0},
                            { "GFX BY DAN COOK",  M_PASSIVE, 0},
                            { "MUSIC AND FX BY",  M_PASSIVE, 0},
                            { "(---)",            M_PASSIVE, 0},
                            { "",                 M_PASSIVE, 0},
                            { "WWW.JONBHO.NET",   M_PASSIVE, 0},
                            { "",                 M_PASSIVE, 0},
                            { "BACK",             M_MAIN,    1}}
                       },
  /*M_MAIN_OPTIONS*/   { "OPTIONS", 3,
                           {{ "MUSIC: ON",        M_ACTION_TOGGLE_MUSIC, 0},
                            { "SOUND FX: ON",     M_ACTION_TOGGLE_SOUND_FX, 1},
                            { "BACK",             M_MAIN, 2}},
                       },
  /*M_CONFIRM_EXIT*/   { "ARE YOU SURE?", 2,
                           {{ "EXIT",             M_ACTION_EXIT, 0},
                            { "GO BACK",          M_MAIN, 3}}
                       },
  /*M_INGAME*/         { "",  3,
                           {{ "OPTIONS",          M_INGAME_OPTIONS, 0 },
                            { "EXIT WORLD",       M_CONFIRM_CANCEL, 1 },
                            { "CONTINUE",         M_ACTION_RESUME,  0 }}
                       },
  /*M_INGAME_OPTIONS*/ { "OPTIONS", 3,
                           {{ "MUSIC: ON",        M_ACTION_TOGGLE_MUSIC, 0},
                            { "SOUND FX: ON",     M_ACTION_TOGGLE_SOUND_FX, 1},
                            { "BACK",             M_INGAME, 1}}
                       },
  /*M_CONFIRM_CANCEL*/ { "ARE YOU SURE?", 2,
                           {{ "EXIT",             M_ACTION_CANCEL, 0},
                            { "GO BACK",          M_INGAME, 1}}
                       },
};

//-----------------------------------------------------------------------------
MenuId g_current_menu = M_HELP;
int    g_current_menu_option = 0;

//-----------------------------------------------------------------------------
void DrawCenteredLine(float y, char text[], rgba color)
{
  float w = strlen(text) * MENU_CHAR_SIZE;
  DrawString(vmake(.5f * G_WIDTH - .5f * w, y), text, MENU_CHAR_SIZE, color);
}

//-----------------------------------------------------------------------------
void RenderMenu()
{
  // Make sure current option is valid for current menu
  if (g_current_menu_option < 0)
    g_current_menu_option = 0;
  else if (g_current_menu_option >= g_MenuDefs[g_current_menu].num_entries)
    g_current_menu_option = g_MenuDefs[g_current_menu].num_entries - 1;
  while (g_current_menu_option >= 0
         && g_current_menu_option < g_MenuDefs[g_current_menu].num_entries - 1
         && g_MenuDefs[g_current_menu].entries[g_current_menu_option].target == M_PASSIVE)
    g_current_menu_option++;
  
  // Calculate menu height
  bool has_title = strlen(g_MenuDefs[g_current_menu].title) > 0;
  int nlines = g_MenuDefs[g_current_menu].num_entries + (has_title ? 1 : 0);
  float menu_height = nlines * MENU_CHAR_SIZE + (nlines-1) * MENU_SPACE_BETWEEN_LINES;
  
  float current_y = .5f * G_HEIGHT + .5f * menu_height;
  
  if (has_title)
  {
    DrawCenteredLine(current_y, g_MenuDefs[g_current_menu].title, MENU_COLOR_UNSELECTED);
    current_y -= MENU_CHAR_SIZE + MENU_SPACE_BETWEEN_LINES;
  }
  
  for (int i = 0; i < g_MenuDefs[g_current_menu].num_entries; i++)
  {
    char text[MAX_MENU_STRING];
    
    // Special text
    if (g_MenuDefs[g_current_menu].entries[i].target == M_ACTION_TOGGLE_MUSIC)
    {
      if (g_opt_music)
        strcpy(text, "MUSIC: ON");
      else
        strcpy(text, "MUSIC: OFF");
    }
    else if (g_MenuDefs[g_current_menu].entries[i].target == M_ACTION_TOGGLE_SOUND_FX)
    {
      if (g_opt_sound_fx)
        strcpy(text, "SOUND FX: ON");
      else
        strcpy(text, "SOUND FX: OFF");
    }
    else
      strcpy(text, g_MenuDefs[g_current_menu].entries[i].text);
    
    if (i != g_current_menu_option)
      DrawCenteredLine(current_y, text, MENU_COLOR_UNSELECTED);
    else
      DrawCenteredLine(current_y, text, MENU_COLOR_SELECTED);
    
    current_y -= MENU_CHAR_SIZE + MENU_SPACE_BETWEEN_LINES;
  }
}

bool  g_music = true;
bool  g_sound_fx = true;

//-----------------------------------------------------------------------------
void EnterMenu(GameState gs)
{
  if (gs == GS_MAIN_MENU)
  {
    g_gs = GS_MAIN_MENU;
    g_current_menu = M_MAIN;
    g_current_menu_option = 0;
  }
  else if (gs == GS_INGAME_MENU)
  {
    g_gs = GS_INGAME_MENU;
    g_current_menu = M_INGAME;
    g_current_menu_option = 0;
  }
}

//-----------------------------------------------------------------------------
void DoMenuAction(MenuId action, int ix)
{
  switch (action)
  {
    case M_NONE:
    case M_PASSIVE:
      // Do nothing!
      break;
      
    case M_ACTION_EXIT:
      g_user_exit = true;
      break;
      
    case M_ACTION_CANCEL:
      // Cancel the game
      FinishGame();
      EnterMenu(GS_MAIN_MENU);
      break;
      
    case M_ACTION_PLAY:
      ResetNewGame(ix); // Start new game in given level
      break;
      
    case M_ACTION_RESUME:
      g_gs = GS_PLAYING; // Return to playing
      break;
      
    case M_ACTION_TOGGLE_MUSIC:
      g_opt_music = !g_opt_music;
      UpdateSoundStatus();
      break;
      
    case M_ACTION_TOGGLE_SOUND_FX:
      g_opt_sound_fx = !g_opt_sound_fx;
      UpdateSoundStatus();
      break;
      
    // All other cases: enter menu
    default:
      g_current_menu = action;
      g_current_menu_option = ix;
      break;
  }
}

#define IS_UNSELECTABLE(ENTRY) (ENTRY.target == M_PASSIVE || (ENTRY.target == M_ACTION_PLAY && ENTRY.target_ix > g_unlocked_level))

//-----------------------------------------------------------------------------
void ProcessInputMenu()
{
  if (g_just_pressed_up)
  {
    do {
      g_current_menu_option--;
      if (g_current_menu_option < 0)
        g_current_menu_option = g_MenuDefs[g_current_menu].num_entries - 1;
    } while (IS_UNSELECTABLE(g_MenuDefs[g_current_menu].entries[g_current_menu_option]));
  }
  else if (g_just_pressed_down)
  {
    do {
      g_current_menu_option++;
      if (g_current_menu_option >= g_MenuDefs[g_current_menu].num_entries)
        g_current_menu_option = 0;
    } while (IS_UNSELECTABLE(g_MenuDefs[g_current_menu].entries[g_current_menu_option]));
  }
  else if (g_just_pressed_space || g_just_pressed_enter)
  {
    MenuId action = g_MenuDefs[g_current_menu].entries[g_current_menu_option].target;
    int ix = g_MenuDefs[g_current_menu].entries[g_current_menu_option].target_ix;
    DoMenuAction(action, ix);
  }
  else if (g_just_pressed_esc)
  {
    // Do action for last entry in the menu
    MenuId action = g_MenuDefs[g_current_menu].entries[g_MenuDefs[g_current_menu].num_entries-1].target;
    int ix = g_MenuDefs[g_current_menu].entries[g_MenuDefs[g_current_menu].num_entries-1].target_ix;
    DoMenuAction(action, ix);
  }
}

//=============================================================================
// Main game entry points: Run(), Render(), ProcessInput()
void Render()
{
  glClear( GL_COLOR_BUFFER_BIT );
  
  // Generate terrain as necessary for rendering next!
  GenTerrain(g_camera_offset + G_HEIGHT);
  RenderTerrain();
  
  // Draw entities
  for (int i = MAX_ENTITIES - 1; i >= 0; i--)
  {
    if (g_entities[i].type != E_NULL)
    {
      ivec2 sz = CORE_GetBmpSize(Tex(g_entities[i].gfx));
      vec2 pos = g_entities[i].pos;
      pos.x = (float)((int)pos.x);
      pos.y = (float)((int)pos.y);
      
      if (g_entities[i].has_shadow)
        CORE_RenderCenteredSprite(
          vadd(vsub(pos, vmake(0.f,g_camera_offset)), vmake(0.f, -SHADOW_OFFSET)),
          vmake(sz.x * SPRITE_SCALE * g_entities[i].gfxscale * SHADOW_SCALE, sz.y * SPRITE_SCALE * g_entities[i].gfxscale * SHADOW_SCALE),
          Tex(g_entities[i].gfx), makergba(0.f,0.f,0.f,0.4f), g_entities[i].gfxadditive
        );
      
      CORE_RenderCenteredSprite(
        vsub(pos, vmake(0.f,g_camera_offset)),
        vmake(sz.x * SPRITE_SCALE * g_entities[i].gfxscale, sz.y * SPRITE_SCALE * g_entities[i].gfxscale),
        Tex(g_entities[i].gfx), g_entities[i].color, g_entities[i].gfxadditive
      );
    }
  }
  
  RenderPSystems(vmake(0.f,-g_camera_offset));
  
  if (g_gs != GS_VICTORY)
  {
    // Draw UI
    float energy_ratio = MAIN_SHIP.energy / MAX_ENERGY;
    CORE_RenderCenteredSprite(
      vmake(ENERGY_BAR_W/2.f, energy_ratio * ENERGY_BAR_H / 2.f),
      vmake(ENERGY_BAR_W, ENERGY_BAR_H * energy_ratio),
      Tex(T_ENERGY), COLOR_WHITE, true);
    
    float fuel_ratio = MAIN_SHIP.fuel / MAX_FUEL;
    CORE_RenderCenteredSprite(
      vmake(G_WIDTH - FUEL_BAR_W/2.f, fuel_ratio * FUEL_BAR_H / 2.f),
      vmake(FUEL_BAR_W, FUEL_BAR_H * fuel_ratio),
      Tex(T_FUEL), COLOR_WHITE, true);
  
    // Show your progression in the level
    int num_chunks = (int)((g_current_race_pos/LevelDescs[g_current_level].level_length) * MAX_CHUNKS);
    for (int i = 0; i < num_chunks; i++)
      CORE_RenderCenteredSprite(
        vmake(G_WIDTH - 100.f, 50.f + i * 50.f),
        vmake(CHUNK_W, CHUNK_H),
        Tex(T_PEARL));
  }

  if (g_gs == GS_MAIN_MENU || g_gs == GS_INGAME_MENU)
    RenderMenu();
}

//-----------------------------------------------------------------------------
void Run()
{
  if (g_gs != GS_INGAME_MENU && g_gs != GS_MAIN_MENU)
  {
    // Control main ship
    if (g_gs == GS_VICTORY || g_gs == GS_STARTING)
    {
      if (MAIN_SHIP.vel.y < SHIP_CRUISE_SPEED)
        MAIN_SHIP.vel.y = SAFEADD(MAIN_SHIP.vel.y, SHIP_INC_SPEED, SHIP_CRUISE_SPEED);
      
      MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, FRAME_FUEL_COST);
    }
    
    SetLoopSoundParam(SHIP_ENGINE_SOUND_CHANNEL, 0.7f, 0.4f + 0.2f * (MAIN_SHIP.vel.y - SHIP_START_SPEED)/(SHIP_CRUISE_SPEED - SHIP_START_SPEED));
    
    // Heal main ship
    if (g_gs != GS_DYING)
    {
      if (MAIN_SHIP.energy < MAX_ENERGY && MAIN_SHIP.fuel >= MIN_FUEL_FOR_HEAL)
      {
        MAIN_SHIP.energy = SAFEADD(MAIN_SHIP.energy, ENERGY_HEAL_PER_FRAME, MAX_ENERGY);
        MAIN_SHIP.fuel   = SAFESUB(MAIN_SHIP.fuel, FUEL_HEAL_PER_FRAME);
      }
    }
    
    // Move entities
    for (int i = MAX_ENTITIES - 1; i >= 0; i--)
    {
      if (g_entities[i].type != E_NULL)
      {
        g_entities[i].pos = vadd(g_entities[i].pos, g_entities[i].vel);
        
        // Remove entities that fell off the screen
        if (g_entities[i].pos.y < g_camera_offset - G_HEIGHT)
          KillEntity(i);
        
        if (g_entities[i].psystem != -1)
          SetPSystemSource(g_entities[i].psystem, vadd(g_entities[i].pos, g_entities[i].psystem_off), g_entities[i].vel);
      }
    }
    
    // Advance 'stars'
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
      if (g_entities[i].type == E_STAR)
      {
        g_entities[i].gfxscale *= 1.008f;
      }
    }
    
    // Advance particle systems
    RunPSystems();
    
    // Dont let steering off the screen!
    if (MAIN_SHIP.pos.x < MAINSHIP_RADIUS)
      MAIN_SHIP.pos.x = MAINSHIP_RADIUS;
    if (MAIN_SHIP.pos.x > G_WIDTH - MAINSHIP_RADIUS)
      MAIN_SHIP.pos.x = G_WIDTH - MAINSHIP_RADIUS;
    
    // Check collisions
    if (g_gs == GS_PLAYING)
    {
      // Check everything agains ship
      for (int i = 1; i < MAX_ENTITIES; i++)
      {
        // Should check against ship?
        if (   g_entities[i].type == E_ROCK
            || g_entities[i].type == E_JUICE
            || g_entities[i].type == E_MINE
            || g_entities[i].type == E_DRONE
            )
        {
          if (vlen2(vsub(g_entities[i].pos, MAIN_SHIP.pos)) < CORE_FSquare(g_entities[i].radius+MAIN_SHIP.radius))
          {
            switch (g_entities[i].type)
            {
              case E_ROCK:
                if (g_entities[i].energy > 0)
                {
                  PlaySound(SND_THUMP);
                  MAIN_SHIP.energy = SAFESUB(MAIN_SHIP.energy, ROCK_CRASH_ENERGY_LOSS);
                  MAIN_SHIP.vel.y = SHIP_START_SPEED;
                  g_entities[i].vel = vscale(vunit(vsub(g_entities[i].pos, MAIN_SHIP.pos)), CRASH_VEL);
                  g_entities[i].energy = 0;
                }
                break;
                
              case E_JUICE:
                MAIN_SHIP.fuel = SAFEADD(MAIN_SHIP.fuel, JUICE_FUEL, MAX_FUEL);
                KillEntity(i);
                break;
                
              case E_MINE:
                PlaySound(SND_EXPLOSSION);
                MAIN_SHIP.energy = SAFESUB(MAIN_SHIP.energy, MINE_CRASH_ENERGY_LOSS);
                MAIN_SHIP.vel.y = SHIP_START_SPEED;
                KillEntity(i);
                break;
                
              case E_DRONE:
                MAIN_SHIP.energy = SAFESUB(MAIN_SHIP.energy, MINE_CRASH_ENERGY_LOSS);
                MAIN_SHIP.vel.y = SHIP_START_SPEED;
                KillEntity(i);
                break;
                
              default:
                break;
            }
          }
        }
        else if (g_entities[i].type == E_ROCKET)
        {
          // Check all rocks against this rocket
          for (int j = 1; j < MAX_ENTITIES; j++)
          {
            // Should check against ship?
            if (   g_entities[j].type == E_ROCK
                || g_entities[j].type == E_MINE
                || g_entities[j].type == E_DRONE
                )
            {
              if (vlen2(vsub(g_entities[i].pos, g_entities[j].pos)) < CORE_FSquare(g_entities[i].radius+g_entities[j].radius))
              {
                // Rocket hit the target!
                switch (g_entities[j].type)
                {
                  case E_MINE:
                    PlaySound(SND_EXPLOSSION);
                    break;
                  default:
                    break;
                }
               
                KillEntity(i);
                KillEntity(j);
                
                break; // Stop checking rocks!
              }
            }
          }
        }
      }
    }
    
    // Generate new level elements as we advance
    GenNextElements();
    
    // Possibly insert new rock
    if (g_gs == GS_PLAYING)
    {
      float trench = MAIN_SHIP.pos.y - g_current_race_pos; // How much advanced from previous frame
      if (CORE_RandChance(trench * JUICE_CHANCE_PER_PIXEL))
        InsertEntity(E_JUICE,
                     vmake(CORE_FRand(0.f, G_WIDTH), g_camera_offset + G_HEIGHT + GEN_IN_ADVANCE),
                     vmake(CORE_FRand(-1.f, +1.f), CORE_FRand(-1.f, +1.f)),
                     JUICE_RADIUS, T_JUICE, false, true);
    }
    
    // Set camera to follow the main ship
    g_camera_offset = MAIN_SHIP.pos.y - G_HEIGHT/8.f;
    
    // Check victory
    g_current_race_pos = MAIN_SHIP.pos.y;
    if (g_gs == GS_PLAYING)
    {
      if (g_current_race_pos >= LevelDescs[g_current_level].level_length)
      {
        g_gs = GS_VICTORY;
        g_gs_timer = 0.f;
        MAIN_SHIP.gfxadditive = true;
        PlaySound(SND_SUCCESS);
        g_unlocked_level = g_current_level + 1;
      }
    }
  }
  
  // Advance game mode
  g_gs_timer += FRAMETIME;
  switch (g_gs)
  {
    case GS_STARTING:
      if (g_gs_timer >= STARTING_TIME)
      {
        g_gs = GS_PLAYING;
        g_gs_timer = 0.f;
      }
      break;
      
    case GS_DYING:
      if (g_gs_timer >= DYING_TIME)
      {
        ResetNewGame(g_current_level);
      }
      break;
      
    case GS_PLAYING:
      if (MAIN_SHIP.energy <= 0.f || MAIN_SHIP.fuel <= 0.f)
      {
        g_gs = GS_DYING;
        g_gs_timer = 0.f;
        MAIN_SHIP.gfx = T_SHIP_RR;
      }
      break;
      
    case GS_VICTORY:
      if (CORE_RandChance(1.f/10.f))
        InsertEntity(E_STAR,
          MAIN_SHIP.pos,
          vadd(MAIN_SHIP.vel,vmake(CORE_FRand(-5.f, 5.f), CORE_FRand(-5.f,5.f))),
          0, T_STAR, false, true);
      if (g_gs_timer >= VICTORY_TIME)
        ResetNewGame(g_current_level+1);
      break;
      
    case GS_MAIN_MENU:
      g_camera_offset += MENU_BKG_SCROLL_SPEED;
      break;
      
    case GS_INGAME_MENU:
      // Do nothing
      break;
  }
  
  g_time_from_last_rocket += FRAMETIME;
}

//-----------------------------------------------------------------------------
void ProcessInputInGame()
{
  if (g_gs == GS_PLAYING)
  {
    if (SYS_KeyPressed(' ') && g_time_from_last_rocket > MIN_TIME_BETWEEN_ROCKETS)
    {
      int e = InsertEntity(E_ROCKET, MAIN_SHIP.pos, vadd(MAIN_SHIP.vel, vmake(0.f, ROCKET_SPEED)),
                   ROCKET_RADIUS, T_ROCKET, true);
      g_time_from_last_rocket = 0;
      
      g_entities[e].psystem = CreatePSystem(PST_FIRE, MAIN_SHIP.pos, vmake(0.f,0.f));
      g_entities[e].psystem_off = vmake(0.f, -120.f);
    }
  
    bool up    = SYS_KeyPressed(SYS_KEY_UP);
    bool down  = SYS_KeyPressed(SYS_KEY_DOWN);
    bool left  = SYS_KeyPressed(SYS_KEY_LEFT);
    bool right = SYS_KeyPressed(SYS_KEY_RIGHT);
  
    // Left-right movement
    if (left && !right)
    {
      MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, TILT_FUEL_COST);
      MAIN_SHIP.tilt -= SHIP_TILT_INC;
    }
    if (right && !left)
    {
      MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, TILT_FUEL_COST);
      MAIN_SHIP.tilt += SHIP_TILT_INC;
    }
    if (!left && !right)
      MAIN_SHIP.tilt *= (1.f - SHIP_TILT_FRICTION);
  
    if (MAIN_SHIP.tilt <= -SHIP_MAX_TILT) MAIN_SHIP.tilt = -SHIP_MAX_TILT;
    if (MAIN_SHIP.tilt >=  SHIP_MAX_TILT) MAIN_SHIP.tilt =  SHIP_MAX_TILT;
  
    MAIN_SHIP.vel.x += MAIN_SHIP.tilt;
    MAIN_SHIP.vel.x *= (1.f - SHIP_HVEL_FRICTION);
  
    // Accelerate/slowdown
    if (up   && !down) MAIN_SHIP.vel.y += SHIP_INC_SPEED;
    if (down && !up)   MAIN_SHIP.vel.y -= SHIP_INC_SPEED;
    if (MAIN_SHIP.vel.y > SHIP_MAX_SPEED) MAIN_SHIP.vel.y = SHIP_MAX_SPEED;
    if (MAIN_SHIP.vel.y < SHIP_MIN_SPEED) MAIN_SHIP.vel.y = SHIP_MIN_SPEED;
  
    float tilt = MAIN_SHIP.tilt;
    if      (tilt < -.6f * SHIP_MAX_TILT) MAIN_SHIP.gfx = T_SHIP_LL;
    else if (tilt < -.2f * SHIP_MAX_TILT) MAIN_SHIP.gfx = T_SHIP_L;
    else if (tilt < +.2f * SHIP_MAX_TILT) MAIN_SHIP.gfx = T_SHIP_C;
    else if (tilt < +.6f * SHIP_MAX_TILT) MAIN_SHIP.gfx = T_SHIP_R;
    else                                  MAIN_SHIP.gfx = T_SHIP_RR;
  }
  
  // Remove this before releasing the game!
  if      (SYS_KeyPressed('1')) ResetNewGame(0);
  else if (SYS_KeyPressed('2')) ResetNewGame(1);
  else if (SYS_KeyPressed('3')) ResetNewGame(2);
  else if (SYS_KeyPressed('4')) ResetNewGame(3);
  else if (SYS_KeyPressed('5')) ResetNewGame(4);
  else if (SYS_KeyPressed('6')) ResetNewGame(5);
  else if (SYS_KeyPressed('7')) ResetNewGame(6);
  else if (SYS_KeyPressed('8')) ResetNewGame(7);
  else if (SYS_KeyPressed('9')) ResetNewGame(8);
  
  if (g_just_pressed_esc)
    EnterMenu(GS_INGAME_MENU);
}

//-----------------------------------------------------------------------------
void ProcessInput()
{
  g_just_pressed_up    = SYS_KeyPressed(SYS_KEY_UP)    && !g_was_pressed_up;
  g_just_pressed_down  = SYS_KeyPressed(SYS_KEY_DOWN)  && !g_was_pressed_down;
  g_just_pressed_enter = SYS_KeyPressed(SYS_KEY_ENTER) && !g_was_pressed_enter;
  g_just_pressed_esc   = SYS_KeyPressed(SYS_KEY_ESC)   && !g_was_pressed_esc;
  g_just_pressed_space = SYS_KeyPressed(SYS_KEY_SPACE) && !g_was_pressed_space;
  
  if (g_gs == GS_MAIN_MENU || g_gs == GS_INGAME_MENU)
    ProcessInputMenu();
  else
    ProcessInputInGame();
  
  g_was_pressed_up    = SYS_KeyPressed(SYS_KEY_UP);
  g_was_pressed_down  = SYS_KeyPressed(SYS_KEY_DOWN);
  g_was_pressed_enter = SYS_KeyPressed(SYS_KEY_ENTER);
  g_was_pressed_esc   = SYS_KeyPressed(SYS_KEY_ESC);
  g_was_pressed_space = SYS_KeyPressed(SYS_KEY_SPACE);
}


//=============================================================================
// Main
int Main(void)
{
  // Start things up & load resources ---------------------------------------------------
  CORE_InitSound();
  LoadTextures();
  LoadSounds();
  PrepareFont();
  
  //ResetNewGame(0);
  EnterMenu(GS_MAIN_MENU);
  
  // Set up rendering ---------------------------------------------------------------------
  glViewport(0, 0, SYS_WIDTH, SYS_HEIGHT);
  glClearColor( 0.0f, 0.1f, 0.3f, 0.0f );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( 0.0, G_WIDTH, 0.0, G_HEIGHT, 0.0, 1.0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  // Main game loop!
  while (!SYS_GottaQuit() && !g_user_exit)
  {
    Render();
    SYS_Show();
    ProcessInput();
    Run();
    SYS_Pump();
    SYS_Sleep(16);
    g_time += FRAMETIME;
  }
  
  UnloadSounds();
  UnloadTextures();
  CORE_EndSound();

  return 0;
}
