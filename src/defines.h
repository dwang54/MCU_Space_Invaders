#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

// TODO - I DO NOT KNOW
#define LCD_WIDTH 1
#define LCD_HEIGHT 1

#define ENEMY_ROWS 5
#define ENEMY_COLS 6

typedef enum _DIRECTION {
    LEFT,
    RIGHT,
    UP,
    DOWN
} DIRECTION;

typedef enum _G_ID { // Graphic ID
    MAINMENU_GID,
    ASTEROID_SID,
    ENEMY_GID,
    LASER_GID,
    PLAYER_GID,
    LEADERBOARD_GID,
    ENDGAME_GID
    // TODO
    // add any more graphics if needed
} G_ID;

typedef enum _E_ID { // Entity ID
    ENEMY_EID,
    LASER_EID,
    PLAYER_EID
} E_ID;

typedef enum _S_ID { // Sound ID - is this even needed? Question for Daniel
    ASTEROID_DESTROY,
    ENEMY_DIE,
    LASER_SHOOT,
    GAME_OVER,
    GAME_BEGIN,
    PLAYER_HIT
    // TODO
    // add any more sounds if needed
} S_ID;

typedef struct _Vec2d {
    int x;
    int y;
} Vec2d;

// graphics array can be 8 bits and RGB ranges to 0-255
// all others can be 8 bits as display is within 255 range
typedef struct _graphic {
    // bits 0-7 hold r, 8-15 g, 16 - 24 hold b
    // pointer to a global array of 
    uint32_t graphic_array[LCD_HEIGHT][LCD_WIDTH];  // x by y array describing the colors of a given space; given const width and height to avoid heap; is a global graphic
    uint8_t w;                                      // width
    uint8_t h;                                      // height
    uint8_t z_level;                                // higher z level lets you get placed over other graphics when overlapping    
    struct _graphic* next;                          // linked list of graphics
} graphic;

typedef struct _sprite {
    graphic* graphic;   // pointer to a globally defined graphic struct
    Vec2d position;     // position of sprite in the world (LCD)
    Vec2d velocity;     // current velocity of the graphic - used to calculate position on the next frame
    E_ID id;            // we have limited number of graphics, so we can track what type of graphic it is with id
} sprite;

// struct to hold the sound wave when loaded in
typedef struct _sound_effect {
    int* sound_wave;
    S_ID id;
} sfx;

typedef struct _player {
    int8_t curr_health;
    time_t last_shot;
    time_t cooldown;
    sprite s;
} player;

typedef struct _enemy {
    int8_t curr_health;
    sprite s;
} enemy;

typedef struct _laser {
  sprite s;
} laser;

#endif
