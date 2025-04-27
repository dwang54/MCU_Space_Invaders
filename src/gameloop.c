// please just do this in main, the platform.io seems to work this way

/*
#include "defines.h"
#include "adc.c"
#include "dac.c"
#include "LedMatrix.c"
#include "SDcard.c"
*/

/*
Peripheral: All integrated together
Usage: Holds the game loop and any helper functions needed
Person: Everyone ideally
Needed by any files: No; needs all other peripherals though
*/

/*
void loop();
void begin_game();
void go_leaderboard();
void end_game();

// game loop running each frame
void loop() {
    for (;;) {

    }
}
*/

/*
1. Display main menu screen outlaying the option for the user
2. Wait for Inputs to decide what to do:
    A: Start Game
    B: Look at Leaderboard
3. Begin game loop by calling loop()
*/

/*
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
*/