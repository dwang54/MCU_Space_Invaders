#ifndef DEFINES_H
#define DEFINES_H

// INCLUDES

#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

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
    PLAYER_EID,
    MAINMENU_EID,
    LEADERBOARD_EID,
    ENDGAME_EID
} E_ID;

typedef struct _Vec2d {
    int x;
    int y;
} Vec2d;

// graphics array can be 8 bits and RGB ranges to 0-255
// all others can be 8 bits as display is within 255 range
typedef struct _graphic {
    // bits 0-7 hold r, 8-15 g, 16 - 24 hold b
    // pointer to a global array of 
    uint32_t graphic_array[LED_DEPTH][LED_LENGTH];  // x by y array describing the colors of a given space; given const width and height to avoid heap; is a global graphic
    uint8_t w;                                      // width
    uint8_t h;                                      // height
    uint8_t z_level;                                // higher z level lets you get placed over other graphics when overlapping    
} graphic;

typedef struct _sprite {
    graphic* graphic;   // pointer to a globally defined graphic struct
    Vec2d position;     // position of sprite in the world (LCD) of the bottom left pixel
    Vec2d velocity;     // current velocity of the graphic - used to calculate position on the next frame
    E_ID id;            // we have limited number of graphics, so we can track what type of graphic it is with id
} sprite;

typedef struct _player {
    int8_t curr_health;
    time_t last_shot;
    time_t cooldown;
    sprite s;
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

// from commands.c
void command_shell(void);
void mount(int argc, char *argv[]);
void f_write_wrapper(unsigned char shoot_raw[1480], char * FILENAME);
void load_wav_files(int len, unsigned char shoot_raw[1480], unsigned char invaderkilled_raw[1225], char explosion_raw[2376], char* FILENAME);


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
void set_volume(uint32_t new_volume);
void play_sfx(S_ID sfx_id);
// don't use these functions/variables!
void setup_dac();
void init_tim6();

// from adc.c
void setup_adc(); // invoke both prior to using getting readings
void init_tim2();
uint32_t get_volume();
// don't invoke this function
void TIM2_IRQHandler();

// from spidisplay.c 
void init_7_segment_display();
void set_message(char* msg);

// from SDcard.c
graphic* load_graphic(G_ID id);
void pre_init_audio();

// do not call these:
void init_spi1();
void spi1_setup_dma();

typedef struct _enemy {
    int8_t curr_health;
    sprite s;
} enemy;

typedef struct _laser {
    int alive;
    sprite s;
} laser;
#endif
