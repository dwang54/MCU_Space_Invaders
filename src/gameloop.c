#include "defines.h"
#include "adc.c"
#include "dac.c"
#include "LedMatrix.c"
#include "SDcard.c"

#include <time.c>

/*
Peripheral: All integrated together
Usage: Holds the game loop and any helper functions needed
Person: Everyone ideally
Needed by any files: No; needs all other peripherals though
*/

void loop();
void begin_game();
void go_leaderboard();
void end_game();
player new_player();
void spawn_wave(enemy* enemy_arr); // do it this way to avoid using the heap
void move_player(player* p);
void move_enemies(enemy enemies[ENEMY_ROWS][ENEMY_ROWS]);
void move_laser(laser* l);
void player_shoot(laser* l);
void display_entities(graphic* graphic_arr);

void check_laser_hit(laser* l, enemy enemies[ENEMY_ROWS][ENEMY_ROWS]);
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

  // creating enemies!
  enemy enemies[ENEMY_ROWS][ENEMY_ROWS];
  for (int i = 0; i < ENEMY_ROWS; ++i) {
    spawn_wave(enemies[i]);
  }


  p.player_graphic->next = enemies[0][0].enemy_graphic;
  display_entities(p.player_graphic);

  // begin game!

  extern volatile char last_char_pressed;
  for (;;) {
    p.velocity = 0;
    switch(last_char_pressed) {
      case 'A': // move right!
        p.velocity = 1;
      case 'B': // move left!
        p.velocity = -1;
      case 'C': // SHOOT
        if (p.last_shot <= p.cooldown + time(NULL)) {
          player_shoot(&l);
        }
      case 'D': // prime the quit
      case 'E': // actually quit
        break;
    }

    // applying all physics to the entites
    move_player(&p);
    move_enemies(enemies);
    move_laser(&l);

    // adding the graphics to the world array
    add_to_world(&p);
    add_to_world(&l);
    for (int i = 0; i < ENEMY_ROWS; ++i) {
      for (int j = 0; j < ENEMY_COLS; ++j) {
        add_to_world(&enemies[i][j]);
      }
    }
    
    // displaying the whole world to the tft display
    display_world();

    // collision checking
    check_laser_hit(&l, enemies);

    // if all enemies are defeated, go to next round
    if (is_wave_beat()) {
      next_round(&p, &l);
    } else if (enemies_reach_bottom()) {
      break;
    }
  }

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
    .max_health = 3,
    .curr_health = 3,
    .speed = 5,
    .cooldown = 1,
    .score = 0,
    .player_graphic = load_graphic(PLAYER_GID),
    .velocity = 0,
    .last_shot = 0
  };
}

void spawn_wave(enemy* enemy_arr) {
  for (size_t i = 0; i < ENEMY_ROWS; ++i) {
    enemy_arr[i] = (enemy) {
      .max_health = 3,
      .curr_health = 3,
      .speed = 1,
      .cooldown = 5,
      .enemy_graphic = load_graphic(ENEMY_GID)
    };
  }
}

// I change thte direction of movement inside the loop() function
// However, I must also link up the graphics struct position in p on each frame
// Basically, I am applying the physics
void move_player(player* p) {
  // TODO

}

// Applying the physics to each monster and updating the graphics
void move_enemies(enemy enemies[ENEMY_ROWS][ENEMY_ROWS]) {
  // TODO

}

void move_laser(laser* l) {

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