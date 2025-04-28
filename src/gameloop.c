#include "defines.h"
#include "adc.c"
#include "dac.c"
#include "LedMatrix.c"
#include "SDcard.c"

#include <time.c>
#include <time.h>

/*
Peripheral: All integrated together
Usage: Holds the game loop and any helper functions needed
Person: Everyone ideally
Needed by any files: No; needs all other peripherals though
*/

#define STARTING_X 5
#define STARTING_Y 0
#define STARTING_ENEMY_HEALTH 1
#define STARTING_PLAYER_HEALTH 5

void loop();
void begin_game();
void go_leaderboard();
void end_game();
player new_player();
void spawn_wave(enemy* enemy_arr, int row); // do it this way to avoid using the heap
void move_sprite(sprite* s);
void add_to_world(sprite* s);
void player_shoot(laser* l);
void display_entities(graphic* graphic_arr);

// collision detection
void check_laser_hit(laser* l, enemy enemies[ENEMY_ROWS][ENEMY_ROWS]);
void check_enemy_on_wall(enemy enemies[ENEMY_ROWS][ENEMY_ROWS]);
void check_player_on_wall(player* p);

int is_wave_beat();
void next_round(player* p, laser* l);
int enemies_reach_bottom(enemy enemies[ENEMY_ROWS][ENEMY_ROWS]);

// with the api on caleb's led matrix, might not even need
// I believe I simply draw each sprite on its own
// Would love to draw everything all at once -> will look into that
uint8_t world_graphic[LCD_HEIGHT][LCD_WIDTH]; // will display on each render

// game loop running each frame
void loop() {
  // settup
  player p = new_player();
  laser l;
  int quit_time = 0;
  extern volatile char last_char_pressed;

  // creating enemies!
  enemy enemies[ENEMY_ROWS][ENEMY_ROWS];
  for (int i = 0; i < ENEMY_ROWS; ++i) {
    spawn_wave(enemies[i], i);
  }

  // begin game!

  for (;;) {
    p.s.velocity.x = 0;
    switch(last_char_pressed) {
      case 'A': // move right!
        p.s.velocity.x = 1;
      case 'B': // move left!s
        p.s.velocity.x = -1;
      case 'C': // SHOOT
        if (p.last_shot <= p.cooldown + time(NULL)) {
          player_shoot(&l);
        }
      case 'D': // prime the quit
        quit_time = time(NULL);
        break;
      case 'E': // actually quit
        if (quit_time + 5 <= time(NULL)) // within 5 seconds of priming, quit game
          goto end_game_goto;
        else
          quit_time = time(NULL); // reprime
    }

    // applying all physics to the entites
    move_sprite(&p.s);
    move_sprite(&l.s);

    // collision checking
    // move laser and check if it hits before even moving enemies
    check_laser_hit(&l, enemies);
    check_enemy_on_wall(enemies);
    check_player_on_wall(&p);

    // moving enemies and adding graphics to world in one loop
    for (int i = 0; i < ENEMY_ROWS; ++i) {
      for (int j = 0; j < ENEMY_COLS; ++j) {
        move_sprite(&enemies[i][j].s);
        add_to_world(&enemies[i][j].s);
      }
    }

    // adding the graphics to the world array
    add_to_world(&p.s);
    add_to_world(&l.s);
    
    // displaying the whole world to the tft display
    display_world();


    // if all enemies are defeated, go to next round
    if (is_wave_beat()) {
      next_round(&p, &l);
    } else if (enemies_reach_bottom(enemies)) {
      goto end_game_goto;
      break;
    }
  }

end_game_goto:
  end_game();
}

/*
1. Display main menu screen outlaying the option for the user
2. Wait for Inputs to decide what to do:
    A: Start Game
    B: Look at Leaderboard
3. Begin game loop by calling loop()
*/
void begin_game() {
    graphic* main_menu = load_graphic(MAINMENU_GID);
    display_entities(main_menu);
    extern volatile char last_char_pressed;
    for (;;) {
        if (last_char_pressed == 'A') {
            // Start game
            loop();
            break;
        } else if (last_char_pressed == 'B') {
            // Look at leader board
            // need function in SDCard to do that
            go_leaderboard();
            // after leaderboard, restart the loop to allow user to go back to game loop
        }
    }
}


// May be too hard? Unless Taviish can find way to make usernames and integers a graphic to display!
void go_leaderboard() {
    graphic* leaderboard_graphic = load_graphic(LEADERBOARD_GID);
    display_entities(leaderboard_graphic); // Do I need to display everytime, or does LCD clear skin after each frame on its own?
    extern volatile char last_char_pressed;
    for (;;) {
        if (last_char_pressed == 'B') {
            // User exits leaderboard display by pressing 'B' on the keypad
            break;
        }
    }
}

// TODO
void end_game() {
  graphic* endgame_graphic = load_graphic(ENDGAME_GID);
  display_entities(endgame_graphic); // Do I need to display everytime, or does LCD clear skin after each frame on its own?
  extern volatile char last_char_pressed;
  char prev = last_char_pressed;
  for (;;) {
      if (last_char_pressed != prev) {
          // User exits enggame display by pressing any character not the same as before
          break;
      }
  }
}

player new_player() {
  return (player) {
    .curr_health = 3,
    .last_shot = 0,
    .cooldown = 5,
    .s = (sprite) {
      load_graphic(PLAYER_GID),
      .position = (Vec2d) { .x = STARTING_X, .y= STARTING_Y },
      .velocity = (Vec2d) { .x = 0, .y = 0 },
      .id = PLAYER_EID
    }
  };
}

void spawn_wave(enemy* enemy_arr, int row) {
  for (size_t i = 0; i < ENEMY_COLS; ++i) {
    enemy_arr[i] = (enemy) {
      .curr_health = 3,
      .s = (sprite) {
        .graphic = load_graphic(ENEMY_GID),
        .position = (Vec2d) { i, row },
        .velocity = (Vec2d) { 0, 0 },
        .id = ENEMY_EID
      }
    };
  }
}

// Apply velocity to sprite's position
void move_sprite(sprite* s) {
  // TODO

}

void player_shoot(laser* l) {

}

void display_entities(graphic* graphic_arr) {
  
}

void check_laser_hit(laser* l, enemy enemies[ENEMY_ROWS][ENEMY_ROWS]) {

}

int is_wave_beat() {

}

void next_round(player* p, laser* l) {

}

int enemies_reach_bottom(enemy enemies[ENEMY_ROWS][ENEMY_ROWS]) {

}