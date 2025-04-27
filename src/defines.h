#ifndef DEFINES_H
#define DEFINES_H

// INCLUDES

#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>

// CONSTANTS

#define LED_DEPTH 64 // dimensions of the actual LED matrix 
#define LED_LENGTH 32 // actual LED matrix dimensions

// for LedMatrix.c
#define CLK_MASK 0x40    // (1 << 6)
#define OE_MASK  0x80    // (1 << 7)
#define LAT_MASK 0x100   // (1 << 8)
#define A_MASK   0x10    // (1 << 4)
#define B_MASK   0x400   // (1 << 10)
#define C_MASK   0x20    // (1 << 5)
#define D_MASK   0x200   // (1 << 9)
#define R1_MASK  0x1     // (1 << 0)
#define B1_MASK  0x2     // (1 << 1)
#define G1_MASK  0x1000   // (1 << 12)
#define R2_MASK  0x4     // (1 << 2)
#define B2_MASK  0x8     // (1 << 3)
#define G2_MASK  0x800  // (1 << 11)
#define NUM_ELEMENTS 4144
// these dimensions are used in mind in regards to the actual number of commands in each one 
// and don't account for the begining and end segments of each column 
#define MATRIX_LENGTH 16 // compressed dimensions for array compared to actual, 32 pixels in a row, but there are 2 RGB's for it
// color code (on clk), color code (off clk), LAT on, LAT off
#define SEGMENT_LENGTH_PER_PIXEL 4
// LENGTH * (DEPTH * SEGMENT_LENGTH_PER_PIXEL + 3) // 3 is present for the commands at the beginning and end
#define NUM_ELEMENTS 4144
// (LED_DEPTH * SEGMENT_LENGTH_PER_PIXEL + 3 end commands)
#define ARRAY_DEPTH 259

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