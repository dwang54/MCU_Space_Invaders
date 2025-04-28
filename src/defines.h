#ifndef DEFINES_H
#define DEFINES_H

// INCLUDES

#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>

// CONSTANTS

#define LED_DEPTH 64 // dimensions of the actual LED matrix 
#define LED_LENGTH 32 // actual LED matrix dimensions

// STRUCTURES:

// struct to hold the sound wave when loaded in for dac.c
// and an additional type
typedef enum _S_ID { // Sound ID - is this even needed? Question for Daniel
    ENEMY_DIE_SID,
    LASER_SHOOT_SID,
    PLAYER_DEATH_SID,
    NO_SID
    // TODO
    // add any more sounds if needed
} S_ID;

typedef struct _sound_effect {
    unsigned char* sound_wave;            // array 
    S_ID id;                              // s_id 
    unsigned int len;            
} sfx;


// of currently unknown usage
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

typedef struct _player {
    int8_t max_health;
    int8_t curr_health;
    int8_t speed;
    int8_t cooldown;
} player;

// FUNCTIONS

// from nano_wait.S
extern void nano_wait(unsigned int n);

// from clock.c
void internal_clock();

// from keypad.c
void init_keypad();
// don't use the ones below
void set_col(int col);
void SysTick_Handler();

// from LedMatrix.c
void init_matrix(); // first invoke init_matrix, then init_display
void init_display();
void draw_pixel(int red, int green, int blue, int length, int depth);
void draw_rectangle(int red, int green, int blue, int length, int depth, int dim_length, int dim_depth);

// from usart.c
void init_computer_feedback();
// // please don't invoke the function below
int __io_putchar(int c);

// from dac.c
void init_sound_effects(); // do this first prior to using audio output!
void init_audio_output();
void TIM6_DAC_IRQHandler();
void play_sfx(S_ID sfx_id);
// don't use these functions/variables!
void setup_dac();
void init_tim6();

#endif