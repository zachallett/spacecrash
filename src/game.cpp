#include "stdafx.h"
#include "base.h"
#include "sys.h"
#include "core.h"

// Useful macros
#define LOG(ALL_ARGS) printf ALL_ARGS
//#define LOG(ALL_ARGS)
#define SAFESUB(ARG_BASE, ARG_CHUNK)          (ARG_BASE - ARG_CHUNK > 0        ? ARG_BASE - ARG_CHUNK : 0)
#define SAFEADD(ARG_BASE, ARG_CHUNK, ARG_MAX) (ARG_BASE + ARG_CHUNK <= ARG_MAX ? ARG_BASE + ARG_CHUNK : ARG_MAX)
#define MAX(ARG_A, ARG_B) ((ARG_A)>(ARG_B)?(ARG_A):(ARG_B))
#define MIN(ARG_A, ARG_B) ((ARG_A)<(ARG_B)?(ARG_A):(ARG_B))

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
#define RACE_END 100000.f

#define STARTING_TIME 1.f
#define DYING_TIME 2.f
#define VICTORY_TIME 8.f

// Tech constants
#define FPS 60.f
#define FRAMETIME (1.f/FPS)

#define SND_DEFAULT_VOL .7f

//=============================================================================
// Game state
enum GameState { GS_PLAYING, GS_DYING, GS_STARTING, GS_VICTORY };
GameState g_gs = GS_STARTING;
float g_gs_timer = 0.f;
float g_current_race_pos = 0.f;
float g_camera_offset = 0.f;
float g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
float g_time_from_last_rocket = 0.f;

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
  T_DRONE2
};

struct Texture
{
  char   name[100];
  bool   wrap;
  GLuint tex;
};

Texture textures[] =
{
  { "data/Kromasky.bmp" , false, 0 },
  { "data/Particle.bmp" , false, 0 },
  { "data/ShipLL.bmp"   , false, 0 },
  { "data/ShipL.bmp"    , false, 0 },
  { "data/ShipC.bmp"    , false, 0 },
  { "data/ShipR.bmp"    , false, 0 },
  { "data/ShipRR.bmp"   , false, 0 },
  { "data/bkg0.bmp"     , false, 0 },
  { "data/Rock0.bmp"    , false, 0 },
  { "data/Rock1.bmp"    , false, 0 },
  { "data/Rock2.bmp"    , false, 0 },
  { "data/Rock3.bmp"    , false, 0 },
  { "data/Rock4.bmp"    , false, 0 },
  { "data/Pearl.bmp"    , false, 0 },
  { "data/Energy.bmp"   , false, 0 },
  { "data/Fuel.bmp"     , false, 0 },
  { "data/Star.bmp"     , false, 0 },
  { "data/Juice.bmp"    , false, 0 },
  { "data/Rocket.bmp"   , false, 0 },
  { "data/Mine.bmp"     , false, 0 },
  { "data/Drone0.bmp"   , false, 0 },
  { "data/Drone1.bmp"   , false, 0 },
  { "data/Drone2.bmp"   , false, 0 }
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
  CORE_PlaySound(sounds[id].bufid, vol, freq);
}

void PlayLoopSound (unsigned loopchannel, SoundId id, float vol, float pitch)
{
  CORE_PlayLoopSound(loopchannel, sounds[id].bufid, vol, pitch);
}

void SetLoopSoundParam (unsigned loopchannel, float vol, float pitch)
{
  CORE_SetLoopSoundParam(loopchannel, vol, pitch);
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
  /* fire  */ { T_PARTICLE,  true ,  8,  60, vmake(0.f, -0.045f),  4.f,  vmake(0.f,+0.25f), .1f,  8.f, 16.f, RGBA(255, 192, 128, 128), RGBA(0, 0, 0, 0) },
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
          float r = color.r;
          float g = color.g;
          float b = color.b;
          float a = color.a;
          
          glColor4f( r, g, b, a);
          
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
#define PATH_TWIST_RATIO .5f // This means 30 degrees maximum
#define PATH_WIDTH (2.f * MAINSHIP_RADIUS)

void GenNextElements()
{
  // Called every game loop, but only does work when we are close to the next "challenge area"
  if (g_current_race_pos + G_HEIGHT > g_next_challenge_area)
  {
    float current_y = g_next_challenge_area;
    LOG(("Current: %f\n", g_next_challenge_area));
    
    // Choose how many layers of rocks
    int nlayers = (int)CORE_URand(1, 20);
    LOG((" nlayers: %d\n", nlayers));
    for (int i = 0; i < nlayers; i++)
    {
      LOG(("  where: %f\n", current_y));
      
      // Choose pass point
      float displace = (current_y - g_last_conditioned.y) * PATH_TWIST_RATIO;
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
      int nrocks = (int)CORE_URand(0, 3);
      LOG(("  nrocks: %d\n", nrocks));
      
      // Gen rocks
      for (int i = 0; i < nrocks; i++)
      {
        // Find a valid pos!
        vec2 rockpos;
        for (int k = 0; k < 10; k++) // 10 attempts maximum, avoid infinite loops!
        {
          rockpos = vmake(CORE_FRand(0.f, G_WIDTH), current_y);
          if (   rockpos.x + ROCK_RADIUS < g_last_conditioned.x - PATH_WIDTH
              || rockpos.x - ROCK_RADIUS > g_last_conditioned.x + PATH_WIDTH)
            break;
        }
        
        // Insert obstacle
        EType t = E_ROCK;
        TexId gfx = T_ROCK1;
        if      (CORE_RandChance(0.1f)) { t = E_MINE;  gfx = T_MINE; }
        else if (CORE_RandChance(0.1f)) { t = E_DRONE; gfx = T_DRONE2; }
        InsertEntity(t,
                     rockpos,
                     vmake(CORE_FRand(-.5f, +.5f), CORE_FRand(-.5f, +.5f)),
                     //vmake(0.f,0.f),
                     ROCK_RADIUS, gfx, true);
      }
      
      current_y += CORE_FRand(300.f, 600.f);
    }
    
    g_next_challenge_area = current_y + CORE_FRand(.5f * G_HEIGHT, 1.5f * G_HEIGHT);
    LOG(("Next: %f\n\n", g_next_challenge_area));
  }
}

//-----------------------------------------------------------------------------
void Render()
{
  glClear( GL_COLOR_BUFFER_BIT );
  
  // Render background, only tiles within the camera view
  int first_tile = (int)floorf(g_camera_offset / G_HEIGHT);
  for (int i = first_tile; i < first_tile + 2; i++)
  {
    CORE_RenderCenteredSprite(
      vsub(vadd(vmake(G_WIDTH/2.0f,G_HEIGHT/2.0f), vmake(0.f, (float)i * G_HEIGHT)), vmake(0.f,g_camera_offset)),
      vmake(G_WIDTH,G_HEIGHT),
      Tex(T_BKG0));
  }
  
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
    int num_chunks = (int)((g_current_race_pos/RACE_END) * MAX_CHUNKS);
    for (int i = 0; i < num_chunks; i++)
      CORE_RenderCenteredSprite(
        vmake(G_WIDTH - 100.f, 50.f + i * 50.f),
        vmake(CHUNK_W, CHUNK_H),
        Tex(T_PEARL));
  }
}

//-----------------------------------------------------------------------------
void ResetNewGame()
{
  // Reset everything for a new game...
  g_next_challenge_area = FIRST_CHALLENGE;
  g_last_conditioned = vmake(.5f * G_WIDTH, 0.f);
  g_current_race_pos = 0.f;
  g_camera_offset = 0.f;
  g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
  g_gs = GS_STARTING;
  g_gs_timer = 0.f;

  // Start logic
  for (int i = 0; i < MAX_ENTITIES; i++)
    g_entities[i].type = E_NULL;
  InsertEntity(E_MAIN, vmake(G_WIDTH/2.0, G_HEIGHT/8.f), vmake(0.f, SHIP_START_SPEED), MAINSHIP_RADIUS, T_SHIP_C, true);
  
  PlayLoopSound(1, SND_ENGINE, 0.7f, 0.3f);
  
  ResetPSystems();
  
  MAIN_SHIP.psystem = CreatePSystem(PST_FIRE, MAIN_SHIP.pos, vmake(0.f,0.f));
  MAIN_SHIP.psystem_off = vmake(0.f, -120.f);
}

//-----------------------------------------------------------------------------
void RunGame()
{
  // Control main ship
  if (g_gs == GS_VICTORY || g_gs == GS_STARTING)
  {
    if (MAIN_SHIP.vel.y < SHIP_CRUISE_SPEED)
      MAIN_SHIP.vel.y = SAFEADD(MAIN_SHIP.vel.y, SHIP_INC_SPEED, SHIP_CRUISE_SPEED);
    
    MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, FRAME_FUEL_COST);
  }
  
  SetLoopSoundParam(1, 0.7f, 0.4f + 0.2f * (MAIN_SHIP.vel.y - SHIP_START_SPEED)/(SHIP_CRUISE_SPEED - SHIP_START_SPEED));
  
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
  
  g_current_race_pos = MAIN_SHIP.pos.y;
  if (g_gs == GS_PLAYING)
  {
    if (g_current_race_pos >= RACE_END)
    {
      g_gs = GS_VICTORY;
      g_gs_timer = 0.f;
      MAIN_SHIP.gfxadditive = true;
      PlaySound(SND_SUCCESS);
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
        ResetNewGame();
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
        ResetNewGame();
      break;
  }
  
  g_time_from_last_rocket += FRAMETIME;
}

//-----------------------------------------------------------------------------
void ProcessInput()
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
}

//-----------------------------------------------------------------------------
// Game state (apart from entities & other stand-alone modules)
float g_time      = 0.f;

//-----------------------------------------------------------------------------
// Main
int Main(void)
{
  // Start things up & load resources ---------------------------------------------------
  CORE_InitSound();
  LoadTextures();
  LoadSounds();
  ResetNewGame();
  
  // Set up rendering ---------------------------------------------------------------------
  glViewport(0, 0, SYS_WIDTH, SYS_HEIGHT);
  glClearColor( 0.0f, 0.1f, 0.3f, 0.0f );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( 0.0, G_WIDTH, 0.0, G_HEIGHT, 0.0, 1.0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  // Main game loop! ======================================================================
  while (!SYS_GottaQuit())
  {
    Render();
    SYS_Show();
    ProcessInput();
    RunGame();
    SYS_Pump();
    SYS_Sleep(16);
    g_time += FRAMETIME;
  }
  
  UnloadSounds();
  UnloadTextures();
  CORE_EndSound();

  return 0;
}
