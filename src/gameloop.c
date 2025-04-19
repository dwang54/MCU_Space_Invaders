#include "defines.h"
#include "adc.c"
#include "dac.c"
#include "LedMatrix.c"
#include "SDcard.c"

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
void sync_player_graphic(player* p);
void move_enemies(enemy** enemies);



// game loop running each frame
void loop() {
    // settup
    player p = new_player();

    // creating enemies!
    enemy enemies[ENEMY_ROWS][ENEMY_ROWS];
    for (int i = 0; i < ENEMY_ROWS; ++i) {
        spawn_wave(enemies[i]);
    }

    p.player_graphic->next = enemies[0][0].enemy_graphic;

    // begin game!

    extern volatile char last_char_pressed;
    for (;;) {
        switch(last_char_pressed) {
            case 'A': // move right!
            case 'B': // move left!
            case 'C': // SHOOT
            case 'D': // prime the quit
            case 'E': // actually quit
            break;
        }

        sync_player_graphic(&p);
        move_enemies(enemies);
        display_entities(&p.player_graphic);
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
    graphic main_menu = load_graphic(MAIN_MENU);
    display_entities(&main_menu);

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

void go_leaderboard() {
    graphic leaderboard_graphic = load_graphic(LEADERBOARD_GID);
    display_entities(&leaderboard_graphic); // Do I need to display everytime, or does LCD clear skin after each frame on its own?
    extern volatile char last_char_pressed;
    for (;;) {
        if (last_char_pressed == 'B') {
            // User exits leaderboard display by pressing 'B' on the keypad
            break;
        }
    }
}

void end_game() {

}

player new_player() {
    return (player) {
        .max_health = 3,
        .curr_health = 3,
        .speed = 5,
        .cooldown = 1,
        .score = 0
    };
}

void spawn_wave(enemy* enemy_arr) {
    for (size_t i = 0; i < ENEMY_ROWS; ++i) {
        enemy_arr[i] = (enemy) {
            .max_health = 3,
            .curr_health = 3,
            .speed = 1,
            .cooldown = 5,
        };
    }
}

// I change thte direction of movement inside the loop() function
// However, I must also link up the graphics struct position in p on each frame
// Basically, I am applying the physics
void sync_player_graphic(player* p) {

}

// Applying the physics to each monster and updating the graphics
void move_enemies(enemy** enemies) {

}