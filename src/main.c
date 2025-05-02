#include "defines.h"
#include <stddef.h>
#include <time.h>

#define STARTING_X 5
#define STARTING_Y 0
#define STARTING_ENEMY_HEALTH 1
#define STARTING_PLAYER_HEALTH 5
#define LASER_SPEED 5
#define ENEMY_SPEED 5
#define Vec2d_ZERO (Vec2d) { 0, 0 }
#define Z_LEVEL_MASK 0xFF000000
#define Z_LEVEL_POS 24
#define R_COLOR_MASK 0x00FF0000
#define R_COLOR_POS 16
#define G_COLOR_MASK 0x0000FF00
#define G_COLOR_POS 8
#define B_COLOR_MASK 0x000000FF
#define B_COLOR_POS 0

#define LCD_WIDTH 1
#define LCD_HEIGHT 1

void loop();
void begin_game();
void go_leaderboard();
void end_game();
player new_player();
void spawn_wave(enemy* enemy_arr, int row); // do it this way to avoid using the heap
void move_sprite(sprite* s, int* hit_wall_flag);
void player_shoot(player* p, laser* l);

void clear_world();
void add_to_world(sprite* s);
void display_world();

// collision detection
void check_laser_hit(laser* l, enemy enemies[ENEMY_ROWS][ENEMY_COLS]);
void check_enemy_on_wall(enemy enemies[ENEMY_ROWS][ENEMY_COLS]);
void check_player_on_wall(player* p);

int is_wave_beat();
void next_round(player* p, laser* l);
int enemies_reach_bottom();

void draw_pixel(int red, int green, int blue, int length, int depth);

// with the api on caleb's led matrix, might not even need
// I believe I simply draw each sprite on its own
// Would love to draw everything all at once -> will look into that
// World is with 0,0 as the bottom left pixel
uint32_t world_graphic[LCD_HEIGHT][LCD_WIDTH]; // will display on each render
unsigned int curr_hit = 0;
int enemy_velocity_x = ENEMY_SPEED;
int furthest_left = 0;
int furthest_right = 0;
int furthest_bottom = 0;
int move_down_flag = 0;

#define TEST_PLAYER 0
#define TEST_INPUT 0
#define TEST_LASER 0
#define TEST_ENEMIES 0
#define TEST_MAINMENU 0
#define TEST_END_MENU 0

// game loop running each frame
void loop() {
  set_message("Looping");
#if TEST_PLAYER
  // settup
  player p = new_player();

#if TEST_LASER
  laser l;
#endif

  int quit_time = 0;
  int hit_wall_flag;
  extern volatile char last_char_pressed;

  // creating enemies!
#if TEST_ENEMIES
  enemy enemies[ENEMY_ROWS][ENEMY_COLS];
  for (int i = 0; i < ENEMY_ROWS; ++i) {
    spawn_wave(enemies[i], i);
  }
#endif

  // begin game!
#if TEST_INPUT
  for (;;) {
    p.s.velocity.x = 0;
    switch(last_char_pressed) {
      case 'A': // move right!
        p.s.velocity.x = 1;
        break;
      case 'B': // stop movement
        p.s.velocity = Vec2d_ZERO;
        break;
      case 'C': // move left
        p.s.velocity.x = -1;
        break;
#if TEST_LASER
      case 'D': // SHOOT
        if (p.last_shot <= p.cooldown + time(NULL)) {
          player_shoot(&p, &l);
        }
        break;
#endif
      case 'E': // prime the quit
        quit_time = time(NULL);
        break;
      case 'F': // actually quit
        if (quit_time + 5 <= time(NULL)) // within 5 seconds of priming, quit game
          goto end_game_goto;
        else
          quit_time = time(NULL); // reprime
    }
#endif

    // applying all physics to the entites
    move_sprite(&p.s, &hit_wall_flag);

#if TEST_LASER
    move_sprite(&l.s, &hit_wall_flag);
#endif

    // collision checking
    // move laser and check if it hits before even moving enemies

#if TEST_ENEMIES && TEST_LASER
    check_laser_hit(&l, enemies);
#endif
#if TEST_ENEMIES
    check_enemy_on_wall(enemies);
#endif
    check_player_on_wall(&p);

    // moving enemies and adding graphics to world in one loop
#if TEST_ENEMIES
    for (int i = 0; i < ENEMY_ROWS; ++i) {
      for (int j = 0; j < ENEMY_COLS; ++j) {
        move_sprite(&enemies[i][j].s, &hit_wall_flag);
        add_to_world(&enemies[i][j].s);
      }
    }
#endif

    // adding the graphics to the world array
    add_to_world(&p.s);
#if TEST_LASER
    add_to_world(&l.s);
#endif
    
    // displaying the whole world to the tft display
    display_world();


    // if all enemies are defeated, go to next round
#if TEST_ENEMIES && TEST_LASER
    if (is_wave_beat()) {
      next_round(&p, &l);
    } else if (enemies_reach_bottom(enemies)) {
      goto end_game_goto;
      break;
    }
  }
#endif

end_game_goto:
  end_game();
#else
  return;
#endif
}

/*
1. Display main menu screen outlaying the option for the user
2. Wait for Inputs to decide what to do:
    A: Start Game
    B: Look at Leaderboard
3. Begin game loop by calling loop()
*/
void begin_game() {
#if TEST_MAINMENU
    set_message("BEGIN");

    sprite main_menu = (sprite) {
      .graphic = load_graphic(MAINMENU_GID),
      .position = Vec2d_ZERO,
      .velocity = Vec2d_ZERO,
      .id = MAINMENU_EID
    };

    add_to_world(&main_menu);
    display_world();

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
#else 
    loop();
#endif
}


void go_leaderboard() {
    sprite leaderboard = (sprite) {
      .graphic = load_graphic(LEADERBOARD_GID),
      .position = Vec2d_ZERO,
      .velocity = Vec2d_ZERO,
      .id = LEADERBOARD_EID
    };

    add_to_world(&leaderboard);
    display_world();

    extern volatile char last_char_pressed;
    for (;;) {
        if (last_char_pressed == 'B') {
            // User exits leaderboard display by pressing 'B' on the keypad
            break;
        }
    }
}

void end_game() {
#if TEST_END_MENU
  sprite endgame = (sprite) {
    .graphic = load_graphic(ENDGAME_GID),
    .position = Vec2d_ZERO,
    .velocity = Vec2d_ZERO,
    .id = ENEMY_EID
  };

  add_to_world(&endgame);
  display_world();

  extern volatile char last_char_pressed;
  char prev = last_char_pressed;

  for (;;) {
      if (last_char_pressed != prev) {
          // User exits enggame display by pressing any character not the same as before
          break;
      }
  }
#else
  return;
#endif
}

player new_player() {
  return (player) {
    .curr_health = 3,
    .last_shot = 0,
    .cooldown = 5,
    .s = (sprite) {
      load_graphic(PLAYER_GID),
      .position = (Vec2d) { .x = STARTING_X, .y= STARTING_Y },
      .velocity = Vec2d_ZERO,
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
        .velocity = Vec2d_ZERO,
        .id = ENEMY_EID
      }
    };
  }
}

// Apply velocity to sprite's position
void move_sprite(sprite* s, int* hit_wall_flag) {
  s->position.x += s->velocity.x;
  s->position.y += s->velocity.y;

  if (s->position.x >= LCD_WIDTH || s->position.x < 0) {
    *hit_wall_flag = 1;
    s->position.x -= s->velocity.x;
  } else if (s->position.y >= LCD_HEIGHT || s->position.y < 0) {
    *hit_wall_flag = 1;
    s->position.y -= s->position.y;
  }
  else 
    *hit_wall_flag = 0;
}


void player_shoot(player* p, laser* l) {
  *l = (laser) {
    .alive = 1,
    .s = (sprite) {
      .graphic = load_graphic(LASER_GID),
      .position = p->s.position,
      .velocity = (Vec2d) { 0, LASER_SPEED },
      .id = LASER_EID
    }
  };
}

void clear_world() {
  for (size_t i = 0; i < LCD_HEIGHT; ++i) {
    for (size_t j = 0; j < LCD_WIDTH; ++j) {
      world_graphic[i][j] = 0;
    }
  }
}

void add_to_world(sprite* s) {
  graphic* g = s->graphic;
  Vec2d pos = s->position;
  for (size_t i = 0; i < g->h; ++i) {
    for (size_t j = 0; j < g->w; ++j) {
      uint8_t z_level = (world_graphic[i+pos.y][i+pos.x] & Z_LEVEL_MASK) >> 24;
      if (z_level >= g->z_level)
        continue;
      uint32_t color = (g->z_level << Z_LEVEL_POS) + g->graphic_array[i][j];
      world_graphic[i+pos.y][i+pos.x] = color;
    }
  }
}

void display_world() {
  for (size_t i = 0; i < LCD_HEIGHT; ++i) {
    for (size_t j = 0; j < LCD_WIDTH; ++j) {
      draw_pixel(
        (world_graphic[i][j] & R_COLOR_MASK) >> R_COLOR_POS,
        (world_graphic[i][j] & G_COLOR_MASK) >> G_COLOR_POS,
        (world_graphic[i][j] & B_COLOR_MASK) >> B_COLOR_POS,
        j, i
      );
    }
  }

  // after displaying, clear world for next display
  // for better optimizations, would ideally keep the same display from pervious cycle and simply manipulate it
  clear_world();
}

void check_laser_hit(laser* l, enemy enemies[ENEMY_ROWS][ENEMY_COLS]) {
  if (!l->alive)
    return;

  for (size_t i = 0; i < ENEMY_ROWS; ++i) {
    for (size_t j = 0; j < ENEMY_ROWS; ++j) {
      enemy* curr_enemy = &enemies[i][j];
      if (curr_enemy->curr_health <= 0) 
        continue;
      // check colission again
      if ((curr_enemy->s.position.x >= l->s.position.x && curr_enemy->s.position.x <= l->s.position.x + curr_enemy->s.graphic->w) &&
      (curr_enemy->s.position.y >= l->s.position.y && curr_enemy->s.position.y <= l->s.position.y + curr_enemy->s.graphic->h)) {
        curr_enemy->curr_health--;
        if (curr_enemy->curr_health == 0)
          curr_hit++;
        l->alive = 0;
        return;
      }
    }
  }
}

void check_enemy_on_wall(enemy enemies[ENEMY_ROWS][ENEMY_COLS]) {
  if (furthest_left <= 10) {
    move_down_flag = 1;
    enemy_velocity_x = ENEMY_SPEED;  // make direction to the right
  } else if (furthest_right >= LCD_WIDTH - 10) {
    move_down_flag = 1;
    enemy_velocity_x = -ENEMY_SPEED; // make direction to the left
  } else {
    move_down_flag = 0;
  }
}

int is_wave_beat() {
  if (curr_hit == ENEMY_ROWS * ENEMY_COLS) return 1;
  return 0;
}

void next_round(player* p, laser* l) {

}

int enemies_reach_bottom() {
  if (furthest_bottom <= 5) return 1;
  return 0;
}


int main()
{
    internal_clock();
    init_keypad();
    init_computer_feedback();

    init_usart5();
    enable_tty_interrupt();

    setbuf(stdin,0); 
    setbuf(stdout,0);
    setbuf(stderr,0);
    mount(0, NULL);
    
    pre_init_audio();
    
    // command_shell();

    // init_matrix();
    // init_display();

    // // begin_game();

    // draw_rectangle(1, 1, 1, 16, 32, 8, 8);
    // draw_pixel(1, 0, 0, 0, 0);

    // setup_adc();
    // init_tim2();

    // init_7_segment_display();
    // set_message("Initiatalzing");
    
    // init_sound_effects();
    // init_audio_output();

    // begin_game();




    // let interrupts run the rest of the program
    for(;;) 
    {
        asm("wfi");
    }
}
