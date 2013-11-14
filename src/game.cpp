#include "stdafx.h"
#include "base.h"
#include "sys.h"
#include "core.h"

#define SAFESUB(a,b) (a-b > 0 ? a-b : 0)

#define SPRITE_SCALE 8.f
#define SHADOW_OFFSET 80.f
#define SHADOW_SCALE 0.9f

#define MAINSHIP_ENTITY 0
#define MAINSHIP_RADIUS 100.f
#define ROCK_RADIUS 100.f
#define SHIP_W 250.f
#define SHIP_H 270.f
#define CRASH_VEL 20.f
#define CRASH_ENERGY_LOSS 30.f
#define MAX_ENERGY 100.f
#define MAX_FUEL 100.f

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

#define SHIP_CRUISE_SPEED 25.f
#define SHIP_START_SPEED 5.f
#define SHIP_INC_SPEED .5f
#define HORIZONTAL_SHIP_VEL 10.f
#define SHIP_TILT_INC .2f;
#define SHIP_TILT_FRICTION .1f
#define SHIP_MAX_TILT 1.f
#define SHIP_HVEL_FRICTION .1f
#define TILT_FUEL_COST .03f
#define FRAME_FUEL_COST .01f

#define RACE_END 100000.f

#define FPS 60.f
#define FRAMETIME (1.f/FPS)

#define STARTING_TIME 2.f
#define DYING_TIME 2.f
#define VICTORY_TIME 8.f

float g_current_race_pos = 0.f;
float g_camera_offset = 0.f;
float g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;

//=============================================================================
// Game state
enum GameState { GS_PLAYING, GS_DYING, GS_STARTING, GS_VICTORY };
GameState g_gs = GS_STARTING;
float g_gs_timer = 0.f;

//=============================================================================
// Textures
int g_ship_LL, g_ship_L, g_ship_C, g_ship_R, g_ship_RR;
int g_bkg, g_pearl, g_energy, g_fuel, g_star;
int g_rock[5];

//=============================================================================
// Entities

enum EType { E_NULL, E_MAIN, E_ROCK, E_STAR };

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
    int    gfx;
    bool   gfxadditive;
    rgba   color;
    bool   has_shadow;
};
Entity   g_entities[MAX_ENTITIES];

//-----------------------------------------------------------------------------
void InsertEntity(EType type, vec2 pos, vec2 vel, float radius, int gfx, bool has_shadow, bool additive = false)
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
            g_entities[i].color        = vmake(1.f,1.f,1.f,1.f);
            g_entities[i].has_shadow   = has_shadow;
            break;
        }
    }
}

//-----------------------------------------------------------------------------
void LoadResources()
{
    // Resources
    g_ship_LL = CORE_LoadBmp("data/ShipLL.bmp", false);
    g_ship_L  = CORE_LoadBmp("data/ShipL.bmp" , false);
    g_ship_C  = CORE_LoadBmp("data/ShipC.bmp" , false);
    g_ship_R  = CORE_LoadBmp("data/ShipR.bmp" , false);
    g_ship_RR = CORE_LoadBmp("data/ShipRR.bmp", false);
    g_bkg     = CORE_LoadBmp("data/bkg0.bmp"  , false);
    g_rock[0] = CORE_LoadBmp("data/Rock0.bmp" , false);
    g_rock[1] = CORE_LoadBmp("data/Rock1.bmp" , false);
    g_rock[2] = CORE_LoadBmp("data/Rock2.bmp" , false);
    g_rock[3] = CORE_LoadBmp("data/Rock3.bmp" , false);
    g_rock[4] = CORE_LoadBmp("data/Rock4.bmp" , false);
    g_pearl   = CORE_LoadBmp("data/Pearl.bmp" , false);
    g_energy  = CORE_LoadBmp("data/Energy.bmp", false);
    g_fuel    = CORE_LoadBmp("data/Fuel.bmp"  , false);
    g_star    = CORE_LoadBmp("data/Star.bmp"  , false);
}

//-----------------------------------------------------------------------------
void UnloadResources()
{
    CORE_UnloadBmp(g_ship_LL);
    CORE_UnloadBmp(g_ship_L);
    CORE_UnloadBmp(g_ship_C);
    CORE_UnloadBmp(g_ship_R);
    CORE_UnloadBmp(g_ship_RR);
    CORE_UnloadBmp(g_bkg);
    CORE_UnloadBmp(g_rock[0]);
    CORE_UnloadBmp(g_rock[1]);
    CORE_UnloadBmp(g_rock[2]);
    CORE_UnloadBmp(g_rock[3]);
    CORE_UnloadBmp(g_rock[4]);
    CORE_UnloadBmp(g_pearl);
    CORE_UnloadBmp(g_energy);
    CORE_UnloadBmp(g_fuel);
    CORE_UnloadBmp(g_star);
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
                                  g_bkg);
    }
    
    // Draw entities
    for (int i = MAX_ENTITIES - 1; i >= 0; i--)
    {
        if (g_entities[i].type != E_NULL)
        {
            ivec2 sz = CORE_GetBmpSize(g_entities[i].gfx);
            vec2 pos = g_entities[i].pos;
            pos.x = (float)((int)pos.x);
            pos.y = (float)((int)pos.y);
            
            if (g_entities[i].has_shadow)
                CORE_RenderCenteredSprite(
                                          vadd(vsub(pos, vmake(0.f,g_camera_offset)), vmake(0.f, -SHADOW_OFFSET)),
                                          vmake(sz.x * SPRITE_SCALE * g_entities[i].gfxscale * SHADOW_SCALE, sz.y * SPRITE_SCALE * g_entities[i].gfxscale * SHADOW_SCALE),
                                          g_entities[i].gfx, vmake(0.f,0.f,0.f,0.4f), g_entities[i].gfxadditive
                                          );
            
            CORE_RenderCenteredSprite(
                                      vsub(pos, vmake(0.f,g_camera_offset)),
                                      vmake(sz.x * SPRITE_SCALE * g_entities[i].gfxscale, sz.y * SPRITE_SCALE * g_entities[i].gfxscale),
                                      g_entities[i].gfx, g_entities[i].color, g_entities[i].gfxadditive
                                      );
        }
    }
    
    if (g_gs != GS_VICTORY)
    {
        // Draw UI
        float energy_ratio = MAIN_SHIP.energy / MAX_ENERGY;
        CORE_RenderCenteredSprite(
                                  vmake(ENERGY_BAR_W/2.f, energy_ratio * ENERGY_BAR_H / 2.f),
                                  vmake(ENERGY_BAR_W, ENERGY_BAR_H * energy_ratio),
                                  g_energy, COLOR_WHITE, true);
        
        float fuel_ratio = MAIN_SHIP.fuel / MAX_FUEL;
        CORE_RenderCenteredSprite(
                                  vmake(G_WIDTH - FUEL_BAR_W/2.f, fuel_ratio * FUEL_BAR_H / 2.f),
                                  vmake(FUEL_BAR_W, FUEL_BAR_H * fuel_ratio),
                                  g_fuel, COLOR_WHITE, true);
        
        // Draw how long you have lasted
        int num_chunks = (int)((g_current_race_pos/RACE_END) * MAX_CHUNKS);
        for (int i = 0; i < num_chunks; i++)
            CORE_RenderCenteredSprite(
                                      vmake(G_WIDTH - 100.f, 50.f + i * 50.f),
                                      vmake(CHUNK_W, CHUNK_H),
                                      g_pearl);
    }
}

//-----------------------------------------------------------------------------
void ResetNewGame()
{
    // Reset everything for a new game...
    g_current_race_pos = 0.f;
    g_camera_offset = 0.f;
    g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
    g_gs = GS_STARTING;
    g_gs_timer = 0.f;
    
    // Start logic
    for (int i = 0; i < MAX_ENTITIES; i++)
        g_entities[i].type = E_NULL;
    InsertEntity(E_MAIN, vmake(G_WIDTH/2.0, G_HEIGHT/8.f), vmake(0.f, SHIP_START_SPEED), MAINSHIP_RADIUS, g_ship_C, true);
}

//-----------------------------------------------------------------------------
void RunGame()
{
    // Control main ship
    if (g_gs == GS_PLAYING || g_gs == GS_VICTORY)
    {
        if (MAIN_SHIP.vel.y < SHIP_CRUISE_SPEED)
        {
            MAIN_SHIP.vel.y += SHIP_INC_SPEED;
            if (MAIN_SHIP.vel.y > SHIP_CRUISE_SPEED)
                MAIN_SHIP.vel.y = SHIP_CRUISE_SPEED;
        }
        MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, FRAME_FUEL_COST);
    }
    
    // Move entities
    for (int i = MAX_ENTITIES - 1; i >= 0; i--)
    {
        if (g_entities[i].type != E_NULL)
        {
            g_entities[i].pos = vadd(g_entities[i].pos, g_entities[i].vel);
            
            // Remove entities that fell off the screen
            if (g_entities[i].pos.y < g_camera_offset - G_HEIGHT)
                g_entities[i].type = E_NULL;
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
    
    // Dont let steering off the screen!
    if (MAIN_SHIP.pos.x < MAINSHIP_RADIUS)
        MAIN_SHIP.pos.x = MAINSHIP_RADIUS;
    if (MAIN_SHIP.pos.x > G_WIDTH - MAINSHIP_RADIUS)
        MAIN_SHIP.pos.x = G_WIDTH - MAINSHIP_RADIUS;
    
    // Check collisions between main ship and rocks
    if (g_gs == GS_PLAYING)
    {
        for (int i = 1; i < MAX_ENTITIES; i++)
        {
            if (g_entities[i].type == E_ROCK)
            {
                if (vlen2(vsub(g_entities[i].pos, MAIN_SHIP.pos)) < CORE_FSquare(g_entities[i].radius+MAIN_SHIP.radius))
                {
                    MAIN_SHIP.energy = SAFESUB(MAIN_SHIP.energy, CRASH_ENERGY_LOSS);
                    MAIN_SHIP.vel.y = SHIP_START_SPEED;
                    g_entities[i].vel = vscale(vunit(vsub(g_entities[i].pos, MAIN_SHIP.pos)), CRASH_VEL);
                }
            }
        }
    }
    
    // Possibly insert new rock
    if (g_gs == GS_PLAYING)
    {
        float trench = MAIN_SHIP.pos.y - g_current_race_pos; // How much advanced from previous frame
        if (CORE_RandChance(trench * g_rock_chance))
        {
            InsertEntity(E_ROCK,
                         vmake(CORE_FRand(0.f, G_WIDTH), g_camera_offset + G_HEIGHT + 400.f),
                         vmake(CORE_FRand(-1.f, +1.f), CORE_FRand(-1.f, +1.f)),
                         ROCK_RADIUS, g_rock[CORE_URand(0,4)], true);
        }
        
        // Advance difficulty in level
        g_rock_chance += (trench * EXTRA_ROCK_CHANCE_PER_PIXEL);
    }
    
    // Set camera to follow the main ship
    g_camera_offset = MAIN_SHIP.pos.y - G_HEIGHT/8.f;
    
    
    if (g_gs == GS_PLAYING)
    {
        g_current_race_pos = MAIN_SHIP.pos.y;
        if (g_current_race_pos >= RACE_END)
        {
            g_gs = GS_VICTORY;
            g_gs_timer = 0.f;
            MAIN_SHIP.gfxadditive = true;
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
            }
            break;
            
        case GS_VICTORY:
            if (CORE_RandChance(1.f/10.f))
                InsertEntity(E_STAR,
                             MAIN_SHIP.pos,
                             vadd(MAIN_SHIP.vel,vmake(CORE_FRand(-5.f, 5.f), CORE_FRand(-5.f,5.f))),
                             0, g_star, false, true);
            if (g_gs_timer >= VICTORY_TIME)
                ResetNewGame();
            break;
    }
}

//-----------------------------------------------------------------------------
void ProcessInput()
{
    /* OLD
     if (SYS_KeyPressed(SYS_KEY_LEFT))
     MAIN_SHIP.vel.x = -HORIZONTAL_SHIP_VEL;
     else if (SYS_KeyPressed(SYS_KEY_RIGHT))
     MAIN_SHIP.vel.x = +HORIZONTAL_SHIP_VEL;
     else
     MAIN_SHIP.vel.x = 0.f;
     */
    
    bool left = SYS_KeyPressed(SYS_KEY_LEFT);
    bool right = SYS_KeyPressed(SYS_KEY_RIGHT);
    if (left && !right)
    {
        MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, TILT_FUEL_COST);
        MAIN_SHIP.tilt -= SHIP_TILT_INC;
    }
    if (right && !left)
    {
        MAIN_SHIP.fuel -= TILT_FUEL_COST;
        MAIN_SHIP.tilt += SHIP_TILT_INC;
    }
    if (!left && !right)
        MAIN_SHIP.tilt *= (1.f - SHIP_TILT_FRICTION);
    
    if (MAIN_SHIP.tilt <= -SHIP_MAX_TILT) MAIN_SHIP.tilt = -SHIP_MAX_TILT;
    if (MAIN_SHIP.tilt >=  SHIP_MAX_TILT) MAIN_SHIP.tilt =  SHIP_MAX_TILT;
    
    MAIN_SHIP.vel.x += MAIN_SHIP.tilt;
    MAIN_SHIP.vel.x *= (1.f - SHIP_HVEL_FRICTION);
    
    float tilt = MAIN_SHIP.tilt;
    if      (tilt < -.6f * SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_LL;
    else if (tilt < -.2f * SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_L;
    else if (tilt < +.2f * SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_C;
    else if (tilt < +.6f * SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_R;
    else                                  MAIN_SHIP.gfx = g_ship_RR;
}

//-----------------------------------------------------------------------------
// Game state (apart from entities & other stand-alone modules)
float g_time      = 0.f;

//-----------------------------------------------------------------------------
// Main
int Main(void)
{
    // Start things up & load resources ---------------------------------------------------
    LoadResources();
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
    UnloadResources();
    
    return 0;
}