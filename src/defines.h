#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>
#include <stddef.h>

// TODO - I DO NOT KNOW
#define LCD_WIDTH 1
#define LCD_HEIGHT 1

typedef enum _DIRECTION {
    LEFT,
    RIGHT,
    UP,
    DOWN
} DIRECTION;

typedef enum _G_ID { // Graphic ID
    MAIN_MENU,
    ASTEROID,
    ENEMY,
    LASER,
    PLAYER,
    LEADERBOARD_GID,
    // TODO
    // add any more graphics if needed
} G_ID;

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
    uint8_t graphic_array[10][10];               // x by y array describing the colors of a given space; given const width and height to avoid heap
    uint8_t w;                                      // width
    uint8_t h;                                      // height
    uint8_t z_level;                                // higher z level lets you get placed over other graphics when overlapping
    Vec2d position;                                 // position in the world
    Vec2d velocity;                                 // current velocity in the world
    G_ID id;                                        // we have limited number of graphics, so we can track what type of graphic it is with id
    struct _graphic* next;                          // linked list of graphics
} graphic;


// struct to hold the sound wave when loaded in
typedef struct _sound_effect {
    int* sound_wave;
    S_ID id;
} sfx;

typedef struct _player {
    int8_t max_health;
    int8_t curr_health;
    int8_t speed;
    int8_t cooldown;
} player;

#endif